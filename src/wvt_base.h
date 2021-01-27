#pragma once

#include <cfloat>
#include <climits>
#include <cmath>	// std::ceil, std::floor, std::fmod
#include <memory>	// std::unique_ptr
#include <vector>

#include "errors_custom.h"
#include "pi_fir.h"

class wvt_base
{
protected:
	double _samplerate;
	double _error_max;
	double _freq_base;
	double _accu_samples;
	std::vector<double> _wvt;

protected:
	wvt_base() noexcept;

public:
	bool Valid_Wvt() const;
	bool Valid_Freq_Input(double freq) const;

protected:
	void Set_Samplerate(double samplerate);
	void Set_Error_Max(double error_max);
	virtual double Determine_Samples_To_Allocate() const = 0;
	virtual void Set_Accu_Samples(double table_samples) = 0;
	void Set_Freq_Base(double table_samples);
	virtual void Create_Wvt(double table_samples, long accu_check_samples) = 0;

protected:
	bool Valid_Table_Size_Cast(double table_samples) const;
	bool Valid_Table_Size_Cast(long table_samples) const;
	bool Valid_Table_Size_Alloc(double output_samples_max) const;
	bool Valid_Table_Size_Alloc(long output_samples_max) const;
	bool Valid_Error_Max(double max_error) const;

private:
	bool Valid_Samplerate(double samplerate) const;
	bool Populated_Wvt() const;
};