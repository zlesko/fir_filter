#pragma once

#include <cmath>	// std::log, std::abs
#include <tuple>
#include <vector>

#include "wvt_sinc.h"
#include "wvt_win.h"

class imp_resp_base
{
protected:
	wvt_sinc _sinc;
	wvt_win _win;
	double _samplerate;
	long _resp_samples_max;

protected:
	imp_resp_base();

public:
	virtual bool Valid_Freq_Input(double freq) const;
	virtual bool Valid_atten_Frac(double atten) const;
	virtual bool Valid_Imp_Resp() const;

protected:
	virtual std::tuple<double, double, double>
		Error_Distribution(double error_max, double win_pow);
	void Negate(std::vector<double>& resp);
	double Impulse(double freq_cutoff);
};