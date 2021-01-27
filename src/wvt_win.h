#pragma once

#include <cmath>	// std::cos, std::pow, std::ceil
#include <memory>	// std::unique_ptr
#include <vector>

#include "errors_custom.h"
#include "wvt_base.h"

class wvt_win : public wvt_base
{
protected:
	double _pow;

public:
	wvt_win();
	wvt_win(double samplerate, double error_max, double pow);

public:
	void Configure(double samplerate, double error_max, double pow);
	std::vector<double> Get_Causal_Window(
		std::vector<double>::size_type total_samples);

private:
	double Determine_Samples_To_Allocate() const override;
	void Set_Accu_Samples(double table_samples) override;
	void Create_Wvt(double table_samples, long accu_check_samples) override;

private:
	void Set_Power(double pow);
	double Power(double base, double pow) const;
	bool Valid_Power(double pow) const;
	void Set_Error_Max(double error_max, double pow);
	bool Valid_Error_Max(double error_max, double pow) const;
};

