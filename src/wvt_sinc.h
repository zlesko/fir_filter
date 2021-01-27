#pragma once

#include <cmath>	// std::sin
#include <memory>   // std::unique_ptr
#include <vector>

#include "errors_custom.h"
#include "pi_fir.h"
#include "wvt_base.h"

class wvt_sinc : public wvt_base
{
public:
	wvt_sinc() noexcept;
	wvt_sinc(double samplerate, double max_error, long output_samples_max);

public:
	void Configure(double samplerate, double error_max,
		long output_samples_max);
	std::vector<double> Get_Causal_Sinc_Rev(double freq, long reserve_size);

private:
	double Determine_Samples_To_Allocate() const override;
	void Set_Accu_Samples(double table_samples) override;
	void Create_Wvt(double table_samples, long output_samples_max) override;
};