#pragma once

#include <algorithm>	// std::sort
#include <cmath>		// std::pow, std::ceil, std::fmod
#include <cfloat>
#include <climits>
#include <vector>

#include "ring_buffer.h"

class firf_base
{
protected:
	double _samplerate;
	double _error_max;
	double _win_pow;
	double _freq_min;
	double _delay_frac;
	long _causal_taps_max;
	long _total_taps_max;

protected:
	firf_base();

public:
	virtual std::vector<double> Filter(std::vector<double> const& signal) = 0;
	long Get_Group_Delay_Samples() const;

protected:
	void Set_Base_Configs(double samplerate, double error_max,
		double freq_min, double win_pow, double delay_frac);
	void Test_Ring_Buffer(long total_samples, long causal_samples);
	void Normalize_Abs_Kahan(std::vector<double>& vtr_to_norm);
	double Error_Imp_Resp(double total_taps_max, double error_max);
	std::vector<double> Get_Full_Imp_Resp(
		std::vector<double> const& causal_resp, double delay_frac);
	std::vector<double>::size_type Filtered_Signal_Size(
		std::vector<double>::size_type signal_size,
		std::vector<double> const& filtered_signal_container);

private:
	void Set_Samplerate(double samplerate);
	void Set_Error_Max(double error_max);
	void Set_Win_Pow(double win_pow);
	void Set_Freq_Min(double freq_min);
	void Set_Causal_Frac(double causal_frac);
	void Set_Tap_Totals(double samplerate, double freq_min,
		double causal_frac);

protected:
	bool Valid_Samplerate(double samplerate) const;
	bool Valid_Firf_Base() const;

private:
	bool Valid_Error_Max(double error_max) const;
	bool Valid_Win_Pow(double win_pow) const;
	bool Valid_Freq_Min(double freq_min) const;
	bool Valid_Causal_Frac(double causal_frac) const;
	double Determine_Causal_Taps(double samplerate, double freq_min) const;
	double Determine_Total_Taps(double total_taps, double delay_frac) const;
	bool Valid_Causal_Taps(double causal_taps) const;
	bool Valid_Total_Taps(double total_taps) const;
};	