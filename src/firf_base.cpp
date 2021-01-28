#include "firf_base.h"

firf_base::firf_base() :
	_samplerate(0.0), _error_max(0.0), _win_pow(0.0), _freq_min(0.0),
	_delay_frac(0.0), _causal_taps_max(0), _total_taps_max(0) {}

long firf_base::Get_Group_Delay_Samples() const
{
	if (!Valid_Firf_Base())
	{
		throw config_error("Invalid filter configuration");
	}
	long group_delay_samples = _total_taps_max - _causal_taps_max;

	return group_delay_samples;
}

void firf_base::Set_Base_Configs(double samplerate, double error_max,
	double freq_min, double win_pow, double delay_frac)
{
	Set_Samplerate(samplerate);
	Set_Error_Max(error_max);
	Set_Freq_Min(freq_min);
	Set_Win_Pow(win_pow);
	Set_Causal_Frac(delay_frac);
	Set_Tap_Totals(samplerate, freq_min, delay_frac);
	Test_Ring_Buffer(_total_taps_max, _causal_taps_max);

	return;
}

void firf_base::Test_Ring_Buffer(long total_samples, long causal_samples)
{
	ring_buffer test(total_samples, causal_samples);
	
	return;
}

void firf_base::Normalize_Abs_Kahan(std::vector<double>& vtr_to_norm)
{
	// Req: vtr_to_norm must be non-trivial
	// Prom: Uses Kahan summation for normalization factor
	std::vector<double> abs_sorted(vtr_to_norm);
	for (auto& element : abs_sorted) { element = std::abs(element); }
	std::sort(abs_sorted.begin(), abs_sorted.end());
	
	// Kahan Sum and normalize
	double sum = 0.0;
	double c = 0.0;
	for (auto& element : abs_sorted)
	{
		double y = element - c;
		double t = sum + y;
		c = (t - sum) - y;
		sum = t;
	}
	for (auto& element : vtr_to_norm) { element /= sum; };

	return;
}

double firf_base::Error_Imp_Resp(double total_taps_max, double error_max)
{
	// Prom: the correct error to construct imp_resp
	// Prom: accounts for Kahan normalization, tap * data, and summation
	// ((error_from_imp_resp + 2e)n^2 + 7en + 2e < _error_max
	// n = buffer_size, e = machine_epsilon, error = for imp_resp
	double a = (4.0 * DBL_EPSILON * total_taps_max) + (2.0 * DBL_EPSILON);
	double error_imp_resp = error_max - a;

	return error_imp_resp;
}

std::vector<double> firf_base::Get_Full_Imp_Resp(
	std::vector<double> const& causal_resp, double delay_frac)
{
	// causal_resp.size() [0, _causal_taps_max]
	long causal_taps = static_cast<long>(causal_resp.size());
	long non_causal_taps = static_cast<long>((causal_taps - 1) * _delay_frac);
	long total_taps = causal_taps + non_causal_taps;
	std::vector<double> full_imp_resp(total_taps, 0.0);
	long index = 0;
	while (index < non_causal_taps)
	{
		long tap = non_causal_taps - index;
		full_imp_resp.at(index) = causal_resp.at(tap);
		index++;
	}
	while (index < full_imp_resp.size())
	{
		long tap = index - non_causal_taps;
		full_imp_resp.at(index) = causal_resp.at(tap);
		index++;
	}

	return full_imp_resp;
}

std::vector<double>::size_type firf_base::Filtered_Signal_Size(
	std::vector<double>::size_type signal_size,
	std::vector<double> const& filtered_signal_container)
{
	std::vector<double>::size_type f_sig_size =
		signal_size + _total_taps_max - 1;
	if (f_sig_size < signal_size
		|| f_sig_size > LONG_MAX
		|| f_sig_size > pow(FLT_RADIX, DBL_MANT_DIG)
		|| f_sig_size > filtered_signal_container.max_size())
	{
		throw parameter_error(
			"Invalid filtered signal size, reduce size of input signal");
	}

	return f_sig_size;
}

void firf_base::Set_Error_Max(double error_max)
{
	if (!Valid_Error_Max(error_max))
	{
		throw config_error("Invalid error_max, correct range (0.0, 1.0]");
	}
	_error_max = error_max;

	return;
}

void firf_base::Set_Samplerate(double samplerate)
{
	if (!Valid_Samplerate(samplerate))
	{
		throw config_error(
			"Invalid samplerate, range (0.0, pow(FLT_RADIX, DBL_MANT_DIG))");
	}
	_samplerate = samplerate;

	return;
}

bool firf_base::Valid_Firf_Base() const
{
	if(Valid_Samplerate(_samplerate) && Valid_Error_Max(_error_max)
		&& Valid_Win_Pow(_win_pow) && Valid_Freq_Min(_freq_min)
		&& Valid_Causal_Frac(_delay_frac))
	{
		return true;
	}
	return false;
}

void firf_base::Set_Freq_Min(double freq_min)
{
	if (!Valid_Freq_Min(freq_min))
	{
		throw config_error(
			"Invalid freq_min, correct range (0, samplerate / 2.0)");
	}
	_freq_min = freq_min;

	return;
}

void firf_base::Set_Causal_Frac(double causal_frac)
{
	if (!Valid_Causal_Frac(causal_frac))
	{
		throw config_error("Invalid causal fraction, range [0.0, 1.0]");
	}
	_delay_frac = causal_frac;
}

void firf_base::Set_Win_Pow(double win_pow)
{
	if (!Valid_Win_Pow(win_pow))
	{
		throw config_error("Invalid window power(s)");
	}
	_win_pow = win_pow;
	
	return;
}

void firf_base::Set_Tap_Totals(double samplerate, double freq_min,
	double delay_frac)
{
	// Req: Valid_Samplerate(samplerate) && Valid_Freq_Min(freq_min)
	// Req: Valid_Causal_Frac(causal_frac)
	double causal_taps_max = Determine_Causal_Taps(samplerate, freq_min);
	double total_taps_max = Determine_Total_Taps(causal_taps_max, delay_frac);
	if (!Valid_Causal_Taps(causal_taps_max) 
		|| !Valid_Total_Taps(total_taps_max))
	{
		throw config_error(
			"Invalid number of taps resulting from samplerate and freq_min");
	}
	_causal_taps_max = static_cast<long>(causal_taps_max);
	_total_taps_max = static_cast<long>(total_taps_max);

	return;
}

bool firf_base::Valid_Error_Max(double error_max) const
{
	if (error_max > 0.0 && error_max <= 1.0) { return true; }
	return false;
}

bool firf_base::Valid_Samplerate(double samplerate) const
{
	// samplerate must be exactly representable as double for calculations
	if (samplerate > 0 && samplerate < pow(FLT_RADIX, DBL_MANT_DIG)
		&& std::fmod(samplerate, 2.0) == 0.0)
	{
		return true;
	}
	return false;
}

bool firf_base::Valid_Win_Pow(double win_pow) const
{
	if (win_pow == 0.0 || win_pow >= 1.0)
	{
		return true;
	}
	return false;
}

bool firf_base::Valid_Freq_Min(double freq_min) const
{
	// Req: Valid_Samplerate(_samplerate)
	if (freq_min > 0.0 && freq_min < _samplerate / 2.0)
	{
		return true;
	}
	return false;
}

bool firf_base::Valid_Causal_Frac(double causal_frac) const
{
	if (causal_frac >= 0.0 && causal_frac <= 1.0) { return true; }
	return false;
}

double firf_base::Determine_Causal_Taps(double samplerate,
	double freq_min) const
{
	// Prom: provides the size of sinc range [0, 2pi]
	double causal_taps = std::floor(samplerate / freq_min);
	
	return causal_taps;
}

double firf_base::Determine_Total_Taps(double causal_taps,
	double delay_frac) const
{
	// Req: Valid_Causal_Taps(casual_taps)
	// Prom: total taps is odd number
	double non_causal_taps = std::floor((causal_taps - 1.0) * delay_frac);
	double total_taps = causal_taps + non_causal_taps;

	return total_taps;
}

bool firf_base::Valid_Causal_Taps(double causal_taps) const
{
	// Prom: causal_taps exactly represented as double, long, _buffer.size_type
	if (causal_taps > 0.0
		&& causal_taps < std::pow(FLT_RADIX, DBL_MANT_DIG) / 2.0
		&& causal_taps <= LONG_MAX)
	{
		return true;
	}
	return false;
}

bool firf_base::Valid_Total_Taps(double total_taps) const
{
	// Prom: causal_taps exactly represented as double, long, _buffer.size_type
	// Prom: total_samples is odd
	if (Valid_Causal_Taps(total_taps))
	{
		return true;
	}
	return false;
}

