#include "wvt_cos.h"

wvt_cos::wvt_cos() noexcept :
	wvt_base() {}

wvt_cos::wvt_cos(double samplerate, double error_max,
	long output_samples_max) :
	wvt_base()
{
	Configure(samplerate, error_max, output_samples_max);

	return;
}

void wvt_cos::Configure(double samplerate, double error_max,
	long output_samples_max)
{
	auto temp_wvt = std::make_unique<wvt_cos>();
	temp_wvt->Set_Samplerate(samplerate);
	temp_wvt->Set_Error_Max(error_max);
	double table_samples = temp_wvt->Determine_Samples_To_Allocate();
	temp_wvt->Set_Freq_Base(table_samples);
	temp_wvt->Set_Accu_Samples(table_samples);
	temp_wvt->Create_Wvt(table_samples, output_samples_max);
	*this = *(temp_wvt.get());

	return;
}

std::vector<double> wvt_cos::Get_Causal_Cos(double freq,
	std::vector<double>::size_type total_samples)
{
	// Req: Valid_Wvt_Cos() && Valid_Table_Size_Cast(total_samples)
	// Req: Valid_Freq_Input(freq)
	// Req: total_sampes <= _accu_samples
	//
	// Iterative floating point inaccuracy introduced during for loop
	// This inaccuracy is quantified in Set_Accu_Samples()
	std::vector<double> sinusoid(total_samples, 0.0);
	double sample_fp = 0.0;
	double dsample = freq / _freq_base;
	double total_samples_fp = static_cast<double>(total_samples);
	for (long sample = 0; sample < sinusoid.size(); sample++)
	{
		sinusoid.at(sample) = _wvt.at(static_cast<long>(sample_fp));
		sample_fp += dsample;
		if (sample_fp >= _wvt.size()) { sample_fp -= _wvt.size(); }
	}

	return sinusoid;
}

double wvt_cos::Determine_Samples_To_Allocate() const
{
	// Req: Valid_Max_Error(_max_error)
	// Prom: cos(n * pi / 2) is exactly represented in table, n[0, 4)
	// Prom: table size set so Create_Wvt(...) is within _max_error bounds
	// Prom: calculation uses abs max of derivative of Create_Wvt(...)
	// Prom: sizing takes into account a full element drift (so it's x2)
	double table_samples = std::ceil(2.0 * PI_FIR / _error_max);
	if (fmod(table_samples, 2.0) != 0.0) { table_samples += 1.0; }
	if (!Valid_Table_Size_Cast(table_samples))
	{
		throw config_error(
			"Invalid configuration - resulting wavetable too large");
	}

	return table_samples;
}

void wvt_cos::Set_Accu_Samples(double table_samples)
{
	// Sets _accu_samples to threshold that ensures wavetable element access
	// has not drifted a full element
	_accu_samples = std::floor(
		1.0 / ((4.0 * DBL_EPSILON * table_samples) + (2.0 * DBL_EPSILON))
	);

	return;
}

void wvt_cos::Create_Wvt(double table_samples, long output_samples_max)
{
	// Prom: cos revolution [0.0, 2.0 * pi) within _error_max bounds
	// Prom: n * pi / 2 is exactly represented in table, n[0, 4)
	if (!Valid_Table_Size_Alloc(table_samples)
		|| !Valid_Table_Size_Alloc(output_samples_max))
	{
		throw accuracy_error(
			"Invalid table size - resulting error greater than _error_max");
	}
	_wvt = std::vector<double>(static_cast<long>(table_samples), 0.0);
	for (long sample = 0; sample < _wvt.size(); sample++)
	{
		double sample_fp = sample;
		_wvt.at(sample) = std::cos(2.0 * PI_FIR * sample_fp / table_samples);
	}

	return;
}