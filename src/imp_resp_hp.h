#pragma once

#include <memory>		// std::unique_ptr
#include <tuple>
#include <vector>

#include "imp_resp_base.h"

class imp_resp_hp : public imp_resp_base
{
public:
	imp_resp_hp();
	imp_resp_hp(double samplerate, double error_max, double win_pow,
		long resp_samples_max);

public:
	void Configure(double samplerate, double error_max, double win_pow,
		long resp_samples_max);
	std::vector<double> Get_Causal_Imp_Resp(double freq_cutoff,
		double atten_frac);
};