#pragma once

#include <algorithm>
#include <cmath>		// std::pow, std::ceil, std::fmod
#include <cfloat>
#include <climits>
#include <memory>		// std::unique_ptr
#include <tuple>
#include <vector>

#include "errors_custom.h"
#include "firf_base.h"
#include "imp_resp_be.h"

class firf_be_tmp : public firf_base
{
private:
	std::vector<double> const* _freq_center;
	std::vector<double> const* _freq_bw;
	std::vector<double> const* _atten;
	imp_resp_be _imp_resp;

public:
	firf_be_tmp();
	firf_be_tmp(double samplerate, double error_max, double freq_bw_min,
		double win_pow, double delay_frac);

public:
	void Set_Parameters(std::vector<double> const* freq_center,
		std::vector<double> const* freq_bw,
		std::vector<double> const* atten);
	void Configure(double samplerate, double error_max, double freq_bw_min,
		double win_pow, double delay_frac,
		std::vector<double> const* freq_center,
		std::vector<double> const* freq_bw,
		std::vector<double> const* atten);
	std::vector<double> Filter(std::vector<double> const& signal) override;

private:
	bool Valid_Freq_Parameters(std::vector<double> const* freq_center,
		std::vector<double> const* freq_bw, double samplerate) const;
	bool Valid_atten_Parameter(std::vector<double> const* atten) const;
	void Set_Imp_Resp();
	std::tuple<double, double, double> Get_Parameters(long curr_sample,
		long load_samples, std::vector<double>::size_type signal_size) const;
};