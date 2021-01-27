#pragma once

#include <memory>		// std::unique_ptr

#include "errors_custom.h"
#include "firf_base.h"
#include "imp_resp_be.h"

class firf_be : public firf_base
{
private:
	double _freq_center;
	double _freq_bw;
	double _atten;
	imp_resp_be _imp_resp;

public:
	firf_be();
	firf_be(double samplerate, double error_max, double freq_bw_min,
		double win_pow, double delay_frac);

public:
	void Set_Parameters(double freq_cutoff, double freq_bw, double atten);
	void Configure(double samplerate, double error_max,	double freq_bw_min,
		double win_pow, double delay_frac, double freq_center, double freq_bw,
		double atten);
	std::vector<double> Filter(std::vector<double> const& signal) override;

private:
	void Set_Imp_Resp();
	bool Valid_Freq_Bw_Parameter(double freq_center,
		double samplerate) const;
	bool Valid_Fshift_Parameter(double freq_shift, double samplerate) const;
	bool Valid_atten_Parameter(double atten) const;
};