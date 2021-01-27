#pragma once

#include <memory>
#include <vector>

#include "errors_custom.h"
#include "wvt_win.h"

class ring_buffer
{
private:
	long _curr_start;
	long _curr_zeroth;
	long _curr_end;
	long _index_max;
	long _total_elements;
	long _non_causal_elements;
	std::vector<double> _ring_buffer;

public:
	ring_buffer();
	ring_buffer(long total_elements, long causal_elements);

public:
	void Configure(long total_elements, long causal_elements);
	void Insert(double value);
	void Clear();
	long Size() const;
	long Max_Size() const;
	bool Valid_Ring_Buffer() const;
	long Load_Buffer(long non_causal_elements,
		std::vector<double> const& data);
	double Process_Cycle(std::vector<double> const& full,
		std::vector<double> const& causal);

private:
	void Decrement_Curr_Indices();
	bool Valid_Total_Elements(long total_elements);
	double Buffer_At(long index);
};

