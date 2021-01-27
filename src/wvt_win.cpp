#include "wvt_win.h"

wvt_win::wvt_win() :
	wvt_base(), _pow(0.0) {}

wvt_win::wvt_win(double samplerate, double error_max, double pow) :
	wvt_base()
{
	Configure(samplerate, error_max, pow);

	return;
}

void wvt_win::Configure(double samplerate, double error_max, double pow)
{
	auto temp_wvt = std::make_unique<wvt_win>();
	temp_wvt->Set_Samplerate(samplerate);
	temp_wvt->Set_Power(pow);
	temp_wvt->Set_Error_Max(error_max, pow);
	double table_samples = temp_wvt->Determine_Samples_To_Allocate();
	temp_wvt->Set_Freq_Base(table_samples);
	temp_wvt->Set_Accu_Samples(table_samples);
	temp_wvt->Create_Wvt(table_samples, static_cast<long>(table_samples));
	*this = *(temp_wvt.get());

	return;
}

std::vector<double> wvt_win::Get_Causal_Window(
	std::vector<double>::size_type total_samples)
{
	// Req: Valid_Wvt() && Valid_Table_Size_(total_samples)
	std::vector<double> window(total_samples, 0.0);
	double N = total_samples - 1.0;
	double wvt_max_index = _wvt.size() - 1.0;
	for (long sample = 0; sample < total_samples; sample++)
	{
		double n = sample;
		long wvt_index = static_cast<long>((n / N) * wvt_max_index);
		window.at(sample) = _wvt.at(wvt_index);
	}
	
	return window;
}

double wvt_win::Determine_Samples_To_Allocate() const
{
	// Req: Valid_Max_Error(_max_error) && Valid_Power(_pow)
	// Prom: cos^(_pow)(n) n[0.0, pi / 2.0] is exactly represented
	// Prom: table size set so Create_Wvt(...) is within _max_error bounds
	// Prom: calculation uses abs max of derivative of Create_Wvt(...)
	// Prom: sizing takes into account a full element drift (so it's x2)
	double table_samples = 1.0;
	if (_pow != 0.0)
	{
		table_samples = std::ceil(2.0 * PI_FIR * _pow / _error_max);
		if (fmod(table_samples, 2.0) != 0.0) { table_samples += 1.0; }
		table_samples *= 0.25;
		table_samples += 1.0;
	}
	if (!Valid_Table_Size_Cast(table_samples))
	{
		throw accuracy_error(
			"Invalid max error, resulting in wavetable that is too large");
	}

	return table_samples;
}

void wvt_win::Set_Accu_Samples(double table_samples)
{
	// Sets _accu_samples to threshold that ensures wavetable element access
	// has not drifted a full element
	_accu_samples = floor(1.0 / DBL_EPSILON);

	return;
}

void wvt_win::Create_Wvt(double table_samples, long accu_check_samples)
{
	// Prom: cos^(_pow)(n) n[0.0, pi / 2.0] within _error_max bounds
	// Prom: n * pi / 2 is exactly represented in table, n[0, 1]
	if (!Valid_Table_Size_Alloc(table_samples)
		|| !Valid_Table_Size_Alloc(accu_check_samples))
	{
		throw accuracy_error(
			"Invalid table size - resulting error greater than _error_max");
	}
	_wvt = std::vector<double>(static_cast<long>(table_samples), 0.0);
	double table_index_max = table_samples - 1.0;
	for (long sample = 0; sample < _wvt.size(); sample++)
	{
		double sample_fp = sample;
		double x = sample_fp / table_index_max;
		double result = std::cos(0.5 * PI_FIR * x);
		_wvt.at(sample) = Power(result, _pow);
	}

	return;
}

void wvt_win::Set_Power(double pow)
{
	if (!Valid_Power(pow))
	{
		throw config_error("Invalid window power");
	}
	_pow = pow;

	return;
}

double wvt_win::Power(double base, double pow) const
{
	if (pow == 0.0) { return 1.0; }
	return std::pow(base, pow);
}

bool wvt_win::Valid_Power(double pow) const
{
	if (pow >= 0.0) { return true; }
	return false;
}

void wvt_win::Set_Error_Max(double error_max, double pow)
{
	if (!Valid_Error_Max(error_max, pow))
	{
		throw config_error("Invalid error_max");
	}
	_error_max = error_max;

	return;
}

bool wvt_win::Valid_Error_Max(double error_max, double pow) const
{
	// Req: Valid_Power(pow)
	if (pow != 0.0) { return wvt_base::Valid_Error_Max(error_max); }
	if (pow == 0.0) { return Valid_Error_Max(error_max, pow); }
	return false;
}
