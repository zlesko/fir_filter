#include "firf_lp.h"

firf_lp::firf_lp() :
	firf_base(), _freq_cutoff(0.0), _atten(0.0) {}

firf_lp::firf_lp(double samplerate, double error_max,
 double freq_min, double win_pow, double delay_frac) :
	firf_lp()
{
	auto temp_firf = std::make_unique<firf_lp>();
	temp_firf->Set_Base_Configs(samplerate, error_max, freq_min, win_pow,
		delay_frac);
	temp_firf->Set_Imp_Resp();
	*this = *(temp_firf.get());

	return;
}

void firf_lp::Set_Parameters(double freq_cutoff, double atten)
{
	if (!Valid_Samplerate(_samplerate))
	{
		throw config_error("Invalid filter configuration");
	}
	if (!Valid_Freq_Cutoff_Parameter(freq_cutoff, _samplerate)
		|| !Valid_Atten_Parameter(atten))
	{
		throw parameter_error("Invalid filter parameters");
	}
	_freq_cutoff = freq_cutoff;
	_atten = atten;

	return;
}

void firf_lp::Configure(double samplerate, double error_max, double freq_min,
	double win_pow, double delay_frac, double freq_cutoff, double atten)
{
	auto temp_firf = std::make_unique<firf_lp>();
	temp_firf->Set_Base_Configs(samplerate, error_max, freq_min, win_pow,
		delay_frac);
	temp_firf->Set_Imp_Resp();
	temp_firf->Set_Parameters(freq_cutoff, atten);
	*this = *(temp_firf.get());

	return;
}

std::vector<double> firf_lp::Filter(std::vector<double> const& signal)
{
	if (!Valid_Firf_Base())
	{
		throw config_error("Invalid filter configuration");
	}
	if (!Valid_Freq_Cutoff_Parameter(_freq_cutoff, _samplerate)
		|| !Valid_Atten_Parameter(_atten))
	{
		throw parameter_error("Invalid filter parameter(s)");
	}
	std::vector<double> filt_sig;
	auto filt_sig_size = Filtered_Signal_Size(signal.size(), filt_sig);
	filt_sig = std::vector<double>(filt_sig_size, 0.0);
	std::vector<double> imp_resp_causal =_imp_resp.Get_Causal_Imp_Resp(
		_freq_cutoff, _atten);
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

bool firf_lp::Valid_Freq_Cutoff_Parameter(double freq_cutoff,
	double samplerate) const
{
	if (freq_cutoff >= 0.0 && freq_cutoff < samplerate / 2.0) { return true; }
	return false;
}

bool firf_lp::Valid_Atten_Parameter(double atten) const
{
	if (atten >= 0.0 && atten <= 1.0) { return true; }
	return false;
}

void firf_lp::Set_Imp_Resp()
{
	// Req: Valid_Firf_Base()
	double error_max_imp_resp = Error_Imp_Resp(_total_taps_max, _error_max);
	_imp_resp.Configure(_samplerate, error_max_imp_resp, _win_pow,
		_causal_taps_max);
	
	return;
}




