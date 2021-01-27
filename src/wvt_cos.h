#pragma once

#include <cmath>	// std::cos()
#include <memory>	// std::unique_ptr
#include <vector>

#include "errors_custom.h"
#include "pi_fir.h"
#include "wvt_base.h"

class wvt_cos : public wvt_base
{
public:
	wvt_cos() noexcept;
	wvt_cos(double samplerate, double error_max, long output_samples_max);

public:
	void Configure(double samplerate, double error_max,
		long output_samples_max);
	std::vector<double> Get_Causal_Cos(double freq,
		std::vector<double>::size_type total_samples);

private:
	double Determine_Samples_To_Allocate() const override;
	void Set_Accu_Samples(double table_samples) override;
	void Create_Wvt(double table_samples, long output_samples_max) override;
};