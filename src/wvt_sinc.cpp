#include "wvt_sinc.h"

wvt_sinc::wvt_sinc() noexcept :
	wvt_base() {}

wvt_sinc::wvt_sinc(double samplerate, double max_error,
	long output_samples_max) :
	wvt_base()
{
	Configure(samplerate, max_error, output_samples_max);

	return;
}

void wvt_sinc::Configure(double samplerate, double error_max,
	long output_samples_max)
{
	auto temp_wvt = std::make_unique<wvt_sinc>();
	temp_wvt->Set_Samplerate(samplerate);
	temp_wvt->Set_Error_Max(error_max);
	double table_samples = temp_wvt->Determine_Samples_To_Allocate();
	temp_wvt->Set_Freq_Base(table_samples);
	temp_wvt->Set_Accu_Samples(table_samples);
	temp_wvt->Create_Wvt(table_samples, output_samples_max);
	*this = *(temp_wvt.get());

	return;
}

std::vector<double> wvt_sinc::Get_Causal_Sinc_Rev(double freq,
	long reserve_size)
{
	// Req: Valid_Wvt_Cos()
	// Req: Valid_Freq_Input(freq)
	// Req: reserve_size (0, _accu_samples]
	//
	// Iterative floating point inaccuracy introduced during for loop
	// This inaccuracy is quantified in Set_Accu_Samples()
	std::vector<double> sinc_rev;
	sinc_rev.reserve(reserve_size);
	double sample_fp = 0.0;
	double dsample = freq / _freq_base;
	while (sample_fp < _wvt.size() && sinc_rev.size() < reserve_size)
	{
		long sample = static_cast<long>(sample_fp);
		sinc_rev.push_back(_wvt.at(sample));
		sample_fp += dsample;
	}

	return sinc_rev;
}

double wvt_sinc::Determine_Samples_To_Allocate() const
{
	// Req: Valid_Max_Error(_max_error) && Valid_Samplerate(_samplerate)
	// Prom: sinc(n * pi / 2) is exactly represented in table, n[0, 4]
	// Prom: table size set so Create_Wvt(...) is within _max_error bounds
	// Prom: calculation uses abs max of derivative of Create_Wvt(...)
	// Prom: table_samples is exactly representable by double, long and size_t
	// Prom: sizing takes into account a full element drift (so it's x2)
	double table_samples = std::ceil(PI_FIR / (2.0 * _error_max));
	if (fmod(table_samples, 2.0) != 0.0) { table_samples += 1.0; }
	table_samples += 1.0;
	if (!Valid_Table_Size_Cast(table_samples))
	{
		throw config_error(
			"Invalid configuration - resulting wavetable too large");
	}

	return table_samples;
}

void wvt_sinc::Set_Accu_Samples(double table_samples)
{
	// Sets _accu_samples to threshold that ensures wavetable element access
	// has not drifted a full element
	_accu_samples = std::floor(
		1.0 / ((2.0 * DBL_EPSILON * table_samples) + (2.0 * DBL_EPSILON))
	);

	return;
}

void wvt_sinc::Create_Wvt(double table_samples, long output_samples_max)
{
	// Prom: cos revolution [0.0, 2.0 * pi) within _error_max bounds
	// Prom: n * pi / 2 is exactly represented in table, n[0, 4]
	if (!Valid_Table_Size_Alloc(table_samples)
		|| !Valid_Table_Size_Alloc(output_samples_max))
	{
		throw accuracy_error(
			"Invalid table size - resulting error greater than _error_max");
	}
	_wvt = std::vector<double>(static_cast<long>(table_samples), 0.0);
	_wvt.at(0) = 1.0;
	double table_samples_fp = table_samples - 1.0;
	for (long sample = 1; sample < _wvt.size(); sample++)
	{
		double sample_fp = static_cast<double>(sample);
		double x = sample_fp / table_samples_fp;
		_wvt.at(sample) = std::sin(2.0 * PI_FIR * x) / (2.0 * PI_FIR * x);
	}

	return;
}