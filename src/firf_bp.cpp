#include "firf_bp.h"

firf_bp::firf_bp() :
	firf_base(), _freq_center(0.0), _freq_bw(0.0), _atten(0.0) {}

firf_bp::firf_bp(double samplerate, double error_max, double freq_bw_min,
	double win_pow, double delay_frac)
{
	auto temp_firf = std::make_unique<firf_bp>();
	temp_firf->Set_Base_Configs(samplerate, error_max, freq_bw_min / 2.0,
		win_pow, delay_frac);
	temp_firf->Set_Imp_Resp();
	*this = *(temp_firf.get());

	return;
}

void firf_bp::Set_Parameters(double freq_center, double freq_bw,
	double atten)
{
	if (!Valid_Samplerate(_samplerate))
	{
		throw config_error("Invalid filter configuration");
	}
	if (!Valid_Fshift_Parameter(_freq_center, _samplerate)
		|| !Valid_Freq_Bw_Parameter(_freq_bw, _samplerate)
		|| !Valid_atten_Parameter(atten))
	{
		throw parameter_error("Invalid filter parameter");
	}
	_freq_center = freq_center;
	_freq_bw = freq_bw;
	_atten = atten;

	return;
}

void firf_bp::Configure(double samplerate, double error_max, double freq_bw_min,
	double win_pow, double delay_frac, double freq_center, double freq_bw,
	double atten)
{
	auto temp_firf = std::make_unique<firf_bp>();
	temp_firf->Set_Base_Configs(samplerate, error_max, freq_bw_min / 2.0,
		win_pow, delay_frac);
	temp_firf->Set_Imp_Resp();
	temp_firf->Set_Parameters(freq_center, freq_bw, atten);
	*this = *(temp_firf.get());

	return;
}

std::vector<double> firf_bp::Filter(std::vector<double> const& signal)
{
	if (!Valid_Firf_Base())
	{
		throw config_error("Invalid filter configuration");
	}
	if (!Valid_Fshift_Parameter(_freq_center, _samplerate)
		|| !Valid_Freq_Bw_Parameter(_freq_bw, _samplerate)
		|| !Valid_atten_Parameter(_atten))
	{
		throw parameter_error("Invalid filter parameter(s)");
	}
	std::vector<double> filt_sig;
	auto filt_sig_size = Filtered_Signal_Size(signal.size(), filt_sig);
	filt_sig = std::vector<double>(filt_sig_size, 0.0);
	std::vector<double> imp_resp_causal = _imp_resp.Get_Causal_Imp_Resp(
		_freq_center, _freq_bw, _atten);
	std::vector<double> imp_resp = Get_Full_Imp_Resp(imp_resp_causal,
		_delay_frac);
	Normalize_Abs_Kahan(imp_resp);
	ring_buffer buffer(_total_taps_max, _causal_taps_max);
	long sample = 0;
	while (sample < signal.size() && sample < filt_sig.size())
	{
		buffer.Insert(signal.at(sample));
		double processed_sample = buffer.Process_Cycle(imp_resp,
			imp_resp_causal);
		filt_sig.at(sample) = processed_sample;
		sample++;
	}
	while (sample < filt_sig.size())
	{
		buffer.Insert(0.0);
		double processed_sample = buffer.Process_Cycle(imp_resp,
			imp_resp_causal);
		filt_sig.at(sample) = processed_sample;
		sample++;
	}

	return filt_sig;
}

void firf_bp::Set_Imp_Resp()
{
	// Req: Valid_Firf_Base()
	double error_max_imp_resp = Error_Imp_Resp(_total_taps_max, _error_max);
	_imp_resp.Configure(_samplerate, error_max_imp_resp, _win_pow,
		_causal_taps_max);

	return;
}

bool firf_bp::Valid_Freq_Bw_Parameter(double freq_center,
	double samplerate) const
{
	if (freq_center >= 0.0 && freq_center < samplerate / 2.0)
	{ 
		return true;
	}
	return false;
}

bool firf_bp::Valid_Fshift_Parameter(double freq_shift,
	double samplerate) const
{
	if (freq_shift >= 0.0 && freq_shift < samplerate / 2.0)
	{
		return true;
	}
	return false;
}

bool firf_bp::Valid_atten_Parameter(double atten) const
{
	if (atten >= 0.0 && atten <= 1.0) { return true; }
	return false;
}


