#pragma once

#include <memory>		// std::unique_ptr
#include <tuple>
#include <vector>

#include "imp_resp_fshift.h"

class imp_resp_bp : public imp_resp_fshift
{
public:
	imp_resp_bp();
	imp_resp_bp(double samplerate, double error_max, double win_pow,
		long resp_samples_max);

public:
	void Configure(double samplerate, double error_max, double win_pow,
		long resp_samples_max);
	std::vector<double> Get_Causal_Imp_Resp(double freq_center,
		double freq_bw, double atten_frac);
};