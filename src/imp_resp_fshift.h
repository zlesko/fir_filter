#pragma once

#include <cmath>	// std::log, std::abs
#include <tuple>
#include <vector>

#include "imp_resp_base.h"
#include "wvt_cos.h"
#include "wvt_sinc.h"
#include "wvt_win.h"

class imp_resp_fshift : public imp_resp_base
{
protected:
	wvt_cos _cos;

public:
	bool Valid_Freq_Input(double freq) const override;
	bool Valid_Imp_Resp() const override;

protected:
	virtual std::tuple<double, double, double>
		Error_Distribution(double error_max, double win_pow) override;
};

