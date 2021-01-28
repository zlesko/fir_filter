#include "firf_hp_tmp.h"

firf_hp_tmp::firf_hp_tmp() :
	firf_base(), _freq_cutoff(nullptr), _atten(nullptr) {}

firf_hp_tmp::firf_hp_tmp(double samplerate, double error_max,
	double freq_min, double win_pow, double delay_frac) :
	firf_hp_tmp()
{
	auto temp_firf = std::make_unique<firf_hp_tmp>();
	temp_firf->Set_Base_Configs(samplerate, error_max, freq_min,
		win_pow, delay_frac);
	temp_firf->Set_Imp_Resp();
	*this = *(temp_firf.get());

	return;
}

void firf_hp_tmp::Configure(double samplerate, double error_max,
	double freq_min, double win_pow, double delay_frac,
	std::vector<double> const* freq_cutoff,
	std::vector<double> const* atten)
{
	auto temp_firf = std::make_unique<firf_hp_tmp>();
	temp_firf->Set_Base_Configs(samplerate, error_max,
		freq_min, win_pow, delay_frac);
	temp_firf->Set_Imp_Resp();
	temp_firf->Set_Parameters(freq_cutoff, atten);
	*this = *(temp_firf.get());

	return;
}

void firf_hp_tmp::Set_Parameters(std::vector<double> const* freq_cutoff,
	std::vector<double> const* atten)
{
	if (!Valid_Samplerate(_samplerate))
	{
		throw config_error("Invalid filter configuration");
	}
	if (!Valid_Freq_Cutoff_Parameter(freq_cutoff, _samplerate)
		|| !Valid_atten_Parameter(atten))
	{
		throw parameter_error("Invalid filter parameters");
	}
	_freq_cutoff = freq_cutoff;
	_atten = atten;

	return;
}

std::vector<double> firf_hp_tmp::Filter(std::vector<double> const& signal)
{
	if (!Valid_Firf_Base())
	{
		throw config_error("Invalid filter configuration");
	}
	if (_freq_cutoff == nullptr || _atten == nullptr)
	{
		throw parameter_error("Invalid filter parameter(s)");
	}
	std::vector<double> filt_sig;
	auto filt_sig_size = Filtered_Signal_Size(signal.size(), filt_sig);
	filt_sig = std::vector<double>(filt_sig_size, 0.0);
	double freq_cutoff = 0.0;
	double atten = 0.0;
	std::tie(freq_cutoff, atten) = Get_Parameters(0, 0, signal.size());
	std::vector<double> imp_resp_causal = _imp_resp.Get_Causal_Imp_Resp(
		freq_cutoff, atten);
	std::vector<double> imp_resp = Get_Full_Imp_Resp(imp_resp_causal,
		_delay_frac);
	Normalize_Abs_Kahan(imp_resp);
	ring_buffer buffer(_total_taps_max, _causal_taps_max);
	long sample = 0;
	long load_samples = (_total_taps_max - _causal_taps_max)
		- (static_cast<long>(imp_resp.size())
		- static_cast<long>(imp_resp_causal.size()));
	while (sample < load_samples && sample < signal.size()
		&& sample < filt_sig.size())
	{
		buffer.Insert(signal.at(sample));
		double processed_sample = buffer.Process_Cycle(imp_resp,
			imp_resp_causal);
		filt_sig.at(sample) = processed_sample;
		sample++;
	}
	while (sample < load_samples)
	{
		buffer.Insert(0.0);
		double processed_sample = buffer.Process_Cycle(imp_resp,
			imp_resp_causal);
		filt_sig.at(sample) = processed_sample;
		sample++;
	}
	while (sample < signal.size() && sample < filt_sig.size())
	{
		std::tie(freq_cutoff, atten) =
			Get_Parameters(sample, load_samples, signal.size());
		imp_resp_causal = _imp_resp.Get_Causal_Imp_Resp(freq_cutoff, atten);
		imp_resp = Get_Full_Imp_Resp(imp_resp_causal, _delay_frac);
		Normalize_Abs_Kahan(imp_resp);
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

bool firf_hp_tmp::Valid_Freq_Cutoff_Parameter(
	std::vector<double> const* freq_cutoff, double samplerate) const
{
	if (freq_cutoff->size() > pow(FLT_RADIX, DBL_MANT_DIG)
		|| freq_cutoff->size() > LONG_MAX)
	{
		return false;
	}
	double freq_min =
		*std::min_element(freq_cutoff->begin(), freq_cutoff->end());
	double freq_max =
		*std::max_element(freq_cutoff->begin(), freq_cutoff->end());
	if (freq_min >= 0.0 && freq_min < samplerate / 2.0) { return true; }
	if (freq_max >= 0.0 && freq_max < samplerate / 2.0) { return true; }
	return false;
}

bool firf_hp_tmp::Valid_atten_Parameter(
	std::vector<double> const* atten) const
{
	if (atten->size() > pow(FLT_RADIX, DBL_MANT_DIG)
		|| atten->size() > LONG_MAX)
	{
		return false;
	}
	double atten_min =
		*std::min_element(atten->begin(), atten->end());
	double atten_max =
		*std::max_element(atten->begin(), atten->end());
	if (atten_min >= 0.0 && atten_min <= 1.0) { return true; }
	if (atten_max >= 0.0 && atten_max <= 1.0) { return true; }
	return false;
}

void firf_hp_tmp::Set_Imp_Resp()
{
	// Req: Valid_Firf_Base()
	double error_max_imp_resp = Error_Imp_Resp(_total_taps_max, _error_max);
	_imp_resp.Configure(_samplerate, error_max_imp_resp, _win_pow,
		_causal_taps_max);

	return;
}

std::tuple<double, double> firf_hp_tmp::Get_Parameters(long curr_sample,
	long load_samples, std::vector<double>::size_type signal_size) const
{
	// Req: Valid_Freq_Cutoff_Parameter(_freq_cutoff, _samplerate)
	// Req: Filtered_Signal_Size(signal_size, ...)
	long rel_sample = curr_sample - load_samples;
	double rel_sample_fp = static_cast<double>(rel_sample);
	double signal_size_fp = static_cast<double>(signal_size);
	double temporal_frac = rel_sample_fp / signal_size_fp;
	double freq_c_size_fp = static_cast<double>(_freq_cutoff->size());
	double atten_size_fp = static_cast<double>(_atten->size());
	long freq_c_index = static_cast<long>(temporal_frac * freq_c_size_fp);
	long atten_index = static_cast<long>(temporal_frac * atten_size_fp);

	return std::make_tuple(_freq_cutoff->at(freq_c_index),
		_atten->at(atten_index));
}
