#include "wvt_base.h"

wvt_base::wvt_base() noexcept :
	_samplerate(0.0), _error_max(0.0),
	_freq_base(0.0), _accu_samples(0.0) {}

bool wvt_base::Valid_Freq_Input(double freq) const
{
	// Req: Valid_Samplerate(_samplerate)
	if (freq >= 0.0 && freq < _samplerate / 2.0)
	{
		return true;
	}
	return false;
}

bool wvt_base::Valid_Wvt() const
{
	if (Valid_Samplerate(_samplerate) && Valid_Error_Max(_error_max)
		&& Populated_Wvt())
	{
		return true;
	}
	return false;
}

void wvt_base::Set_Samplerate(double samplerate)
{
	if (!Valid_Samplerate(samplerate))
	{
		throw config_error(
			"Invalid samplerate, range (0.0, pow(FLT_RADIX, DBL_MANT_DIG))");
	}
	_samplerate = samplerate;

	return;
}

void wvt_base::Set_Error_Max(double max_error)
{
	if (!Valid_Error_Max(max_error))
	{
		throw config_error("Invalid max error, range is (0.0, 1.0]");
	}
	_error_max = max_error;

	return;
}

void wvt_base::Set_Freq_Base(double table_samples)
{
	// Req: Valid_Max_Error(_max_error) && Valid_Samplerate(_samplerate)
	// _freq_base is assumed to carry fp imprecision of LSU
	_freq_base = _samplerate / table_samples;

	return;
}

bool wvt_base::Valid_Table_Size_Cast(double table_samples) const
{
	// Prom: _wvt size can safetly be casted to double
	// Prom: _wvt size can be accessed by long int
	if (sizeof(long) * 8 < DBL_MAX_EXP
		&& sizeof(std::vector<double>::size_type) * 8 < DBL_MAX_EXP
		&& table_samples >= 0.0
		&& table_samples < std::pow(FLT_RADIX, DBL_MANT_DIG - 1)
		&& table_samples <= LONG_MAX
		&& table_samples <= _wvt.max_size())
	{
		return true;
	}
	return false;
}

bool wvt_base::Valid_Table_Size_Cast(long table_samples) const
{
	// Prom: _wvt size can safetly be casted to double
	// Prom: _wvt size can be accessed by long int
	if (table_samples >= 0.0
		&& table_samples < std::pow(FLT_RADIX, DBL_MANT_DIG - 1)
		&& table_samples < LONG_MAX
		&& table_samples <= _wvt.max_size())
	{
		return true;
	}
	return false;
}

bool wvt_base::Valid_Table_Size_Alloc(double accu_check_samples) const
{
	// Req: Set_Accu_Samples(...)
	if (Valid_Table_Size_Cast(accu_check_samples)
		&& accu_check_samples <= _accu_samples)
	{
		return true;
	}
	return false;
}

bool wvt_base::Valid_Table_Size_Alloc(long accu_check_samples) const
{
	// Req: Set_Accu_Samples(...)
	if (Valid_Table_Size_Cast(accu_check_samples)
		&& accu_check_samples <= _accu_samples)
	{
		return true;
	}
	return false;
}

bool wvt_base::Valid_Error_Max(double error_max) const
{
	if (error_max >= 0.0 && error_max <= 1.0) { return true; }
	return false;
}

bool wvt_base::Valid_Samplerate(double samplerate) const
{
	// samplerate must be exactly representable as double, long and size_t
	if (samplerate > 0 && samplerate < pow(FLT_RADIX, DBL_MANT_DIG)
		&& std::fmod(samplerate, 1.0) == 0.0)
	{
		return true;
	}
	return false;
}

bool wvt_base::Populated_Wvt() const
{
	if (_wvt.size() != 0)
	{
		for (auto const& sample : _wvt)
		{
			if (sample != 0) { return true; }
		}
	}
	return false;
}