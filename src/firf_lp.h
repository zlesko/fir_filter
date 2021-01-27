#pragma once

#include <memory>		// std::unique_ptr

#include "errors_custom.h"
#include "firf_base.h"
#include "imp_resp_lp.h"

class firf_lp : public firf_base
{
private:
	double _freq_cutoff;
	double _atten;
	imp_resp_lp _imp_resp;

public:
	firf_lp();
	firf_lp(double samplerate, double error_max, double freq_min,
		double win_pow, double delay_frac);

public:
	void Set_Parameters(double freq_cutoff, double atten);
	void Configure(double samplerate, double error_max, double freq_min,
		double win_pow, double delay_frac, double freq_cutoff, double atten);
	std::vector<double> Filter(std::vector<double> const& signal) override;

private:
	bool Valid_Freq_Cutoff_Parameter(double freq_cutoff,
		double samplerate) const;
	bool Valid_Atten_Parameter(double atten) const;
	void Set_Imp_Resp();
};