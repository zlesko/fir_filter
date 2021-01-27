#include "ring_buffer.h"

ring_buffer::ring_buffer() :
	_curr_start(0), _curr_zeroth(0), _curr_end(0), _index_max(0),
	_total_elements(0), _non_causal_elements(0) {}

ring_buffer::ring_buffer(long total_elements, long causal_elements) :
	ring_buffer()
{
	Configure(total_elements, causal_elements);

	return;
}

void ring_buffer::Configure(long total_elements, long causal_elements)
{
	if (!Valid_Total_Elements(total_elements)
		|| total_elements < causal_elements)
	{
		throw config_error("Invalid ring_buffer configuration size");
	}
	_ring_buffer = std::vector<double>(total_elements, 0.0);
	_curr_start = 0;
	_curr_zeroth = total_elements - causal_elements;
	_curr_end = total_elements - 1;
	_index_max = total_elements - 1;
	_total_elements = total_elements;
	_non_causal_elements = total_elements - causal_elements;

	return;
}

void ring_buffer::Insert(double value)
{
	Decrement_Curr_Indices();
	_ring_buffer.at(_curr_start) = value;

	return;
}

void ring_buffer::Clear()
{
	for (auto& element : _ring_buffer) { element = 0.0; }
	
	return;
}

long ring_buffer::Size() const
{
	// _ring_buffer.size() is never greater than LONG_MAX
	return static_cast<long>(_ring_buffer.size());
}

long ring_buffer::Max_Size() const
{
	auto rb_max_size = _ring_buffer.max_size();
	if (rb_max_size > LONG_MAX) { return LONG_MAX; }
	return static_cast<long>(rb_max_size);
}

bool ring_buffer::Valid_Ring_Buffer() const
{
	if (_ring_buffer.size() != 0) { return true; }
	return false;
}

long ring_buffer::Load_Buffer(long non_causal_elements,
	std::vector<double> const& data)
{
	if (!Valid_Total_Elements(non_causal_elements)
		|| non_causal_elements > _non_causal_elements)
	{
		throw config_error("Invalid ring_buffer congifuration");
	}
	long samples_to_preload = _non_causal_elements - non_causal_elements;
	long sample = 0;
	while (sample < samples_to_preload && sample < data.size())
	{
		Insert(data.at(sample));
		sample++;
	}
	while (sample < samples_to_preload)
	{
		Insert(0.0);
		sample++;
	}

	return samples_to_preload;
}

double ring_buffer::Process_Cycle(std::vector<double> const& full,
	std::vector<double> const& causal)
{
	// Req: causal.size() <= full.size()
	// Req: full.size() <= _total_elements
	// Req: vectors size is less than LONG_MAX
	// The summation error introduced
	long total_elements = static_cast<long>(full.size());
	long causal_elements = static_cast<long>(causal.size());
	long non_causal_elements = total_elements - causal_elements;
	long causal_dif = _non_causal_elements - non_causal_elements;
	double sum = 0.0;
	for (long element = 0; element < full.size(); element++)
	{
		long buffer_index = element + causal_dif;
		sum += Buffer_At(buffer_index) * full.at(element);
	}

	return sum;
}

void ring_buffer::Decrement_Curr_Indices()
{
	_curr_start--;
	_curr_zeroth--;
	_curr_end--;
	if (_curr_start < 0) { _curr_start += _total_elements; }
	if (_curr_zeroth < 0) { _curr_zeroth += _total_elements; }
	if (_curr_end < 0) { _curr_end += _total_elements; }

	return;
}

bool ring_buffer::Valid_Total_Elements(long total_elements)
{
	if (total_elements > 0 && total_elements < _ring_buffer.max_size())
	{
		return true;
	}
	return false;
}

double ring_buffer::Buffer_At(long index)
{
	// Req: Valid_Ring_Buffer() && index < _index_max
	long translated_index = index + _curr_start;
	if (translated_index > _index_max)
	{ 
		translated_index -= _total_elements;
	}

	return _ring_buffer.at(translated_index);
}
