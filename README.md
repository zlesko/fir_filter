#FIR Filter C++ Library
Finite impulse response filter C++ library which provides the following functionality:

* Low-pass, high-pass, band-pass, band-elimination
* Power of cosine window
* Control of sample rate, maximum error, window power, causality/delay *(constant)*
* Control of cutoff frequency, center frequency, bandwidth, attenuation *(constant or temporal)*

Possible use cases include:

* Filtering signal data represented in a C++ vector of type double
* Hardware FIR filter design
* Experiment/education through manipulation of various parameters 

## Table of Contents
[1. Code Usage Examples ](#1.)

* [1.1a) Low-Pass, Constant Parameters](#1.1a)

* [1.1b) Low-Pass, Change Parameters](#1.1b)

* [1.2) Band-Pass, Temporal Parameters ](#1.2)

[2. Filter Classes ](#2.)

* [2.1) FIR Low-Pass Filters](#2.1)

* [2.2) FIR High-Pass Filters](#2.2)

* [2.3) FIR Band-Pass Filters](#2.3)

* [2.4) FIR Band-Elimination Filters](#2.4)

* [2.5) Exception Safety](#2.5)

* [2.6) Input Definitions](#2.6)

* [2.7) Pitfalls](#2.7)

[3.  Mechanisms](#3.)

* [3.1) Filters](#3.1)

* [3.2) Impulse Responses](#3.2)

* [3.3) Wavetables](#3.3)

* [3.4) Remaining Within Max Error Limit](#3.4)

[4. Update Plans ](#4.)

<a name="1."></a>
##1. Code Usage Examples
<a name="1.1a"></a>
###Ex1a) Low-Pass, Constant Parameters
Low-pass a 200,000 S/s signal at 20kHz with a maximum attenuation, ideal response(full delay), constant parameters and Hann window:

	// error_max is max absolute error for input signal range [-1.0, 1.0]
	// freq_min  = lowest freq_cutoff, needed for internal sizing
	// win_pow = 2.0 for Hann indow
	// delay_frac = 1.0 for completely ideal response

	std::vector<double> signal{/* populated with data */};
	double samplerate = 200'000.0;
	double error_max = 0.01;
	double freq_min = 20000.0;
	double win_pow = 2.0;
	double delay_frac = 1.0;
	double freq_cutoff = 20000.0;
	double atten = 1.0;
	std::vector<double> filtered_data;

	// LPF
	firf_lp lpf(samplerate, error_max, freq_min, win_pow, delay_frac);
	lpf.Set_Paramters(freq_cutoff, atten);
	filtered_data = lpf.Filter(signal);

	// number of non-causal filter taps
	long group_delay_samples = lpf.Get_Delay_Samples();

	
Below is the impulse response and frequency response when the signal is white noise:

<a name="1.1b"></a>
###Ex1b) Low-Pass, Change Parameters

Change the LPF to causal response and attenuation to 50%:

	delay_frac = 0.0;
	atten = 0.5;
	lpf.Configure(samplerate, error_max, freq_min, win_pow,
		delay_frac, freq_cutoff, atten);
	filtered_data = lpf.Filter(signal);


Below is the impulse response and frequency response when the signal is white noise:

<a name="1.2"></a>
###Ex2) Band-Pass, Temporal Parameters
	
Band-pass of an audio signal, with an center frequency of moving from 4kHz to 500Hz, and a bandwidth of 1000Hz, ideal response(full delay) with full attenuation, Hann window.

	// freq_min = minimum bandwidth (Hz)
	std::vector<double> signal{/* data */};
	double samplerate = 44100.0;
	double error_max = 0.01;
	double freq_bw_min = 1000.0;
	double win_pow = 2.0;
	double delay_frac = 1.0;
	std::vector<double> filtered_data;

	// parameters must be in vector form
	std::vector<double> freq_center{/* 4000.0, …, 500.0 */};
	std::vector<double> freq_bw{ 1000.0 };
	std::vector<double> atten{ 1.0 };

	// BPF
	firf_bp_tmp bpf(samplerate, error_max, freq_bw_min, win_pow, delay_frac);
	bpf.Set_Parameters(&freq_center, &freq_bw, &atten);
	filtered_data = bpf.Filter(signal);

Frequency response when signal is white noise:

Audio of filtered data if signal is a ~200Hz square wave:

<a name="2."></a>
##2. Filter Classes

<a name="2.1"></a>
###2.1 FIR Low-Pass Filters

####*firf_lp::*

	firf_lp();

	firf_lp(double samplerate, double error_max, double freq_min,
		double win_pow, double delay_frac);

	void Set_Parameters(double freq_cutoff, double atten);

	void Configure(double samplerate, double error_max, double freq_min,
		double win_pow, double delay_frac, double freq_cutoff, double atten);

	long Get_Group_Delay_Samples() const;

	std::vector<double> Filter(std::vector<double> const& signal);
		
####*firf\_lp_tmp::*

	firf_lp_tmp();

	firf_lp_tmp(double samplerate, double error_max, double freq_min,
		double win_pow, double delay_frac);

	void Set_Parameters(std::vector<double> const* freq_cutoff,
		std::vector<double> const* atten);

	void Configure(double samplerate, double error_max, double freq_bw_min,
		double win_pow, double delay_frac,
		std::vector<double> const* freq_cutoff,
		std::vector<double> const* atten);

	long Get_Group_Delay_Samples() const;	
	
	std::vector<double> Filter(std::vector<double> const& signal);

<a name="2.2"></a>
###2.2 FIR High-Pass Filters

####*firf_hp::*

	firf_hp();

	firf_hp(double samplerate, double error_max, double freq_min,
		double win_pow, double delay_frac);

	void Set_Parameters(double freq_cutoff, double atten);

	void Configure(double samplerate, double error_max,	double freq_min,
		double win_pow, double delay_frac, double freq_cutoff, double atten);

	long Get_Group_Delay_Samples() const;
	
	std::vector<double> Filter(std::vector<double> const& signal);	
	

####*firf\_hp_tmp::*

	firf_hp_tmp();
	
	firf_hp_tmp(double samplerate, double error_max, double freq_min,
		double win_pow, double delay_frac);

	void Set_Parameters(std::vector<double> const* freq_cutoff,
		std::vector<double> const* atten);

	void Configure(double samplerate, double error_max, double freq_min,
		double win_pow, double delay_frac,
		std::vector<double> const* freq_cutoff,
		std::vector<double> const* atten);
		
	long Get_Group_Delay_Samples() const;

	std::vector<double> Filter(std::vector<double> const& signal);

<a name="2.3"></a>
###2.3 FIR Band-Pass Filters

####*firf_bp::*

	firf_bp();

	firf_bp(double samplerate, double error_max, double freq_bw_min,
		double win_pow, double delay_frac);

	void Set_Parameters(double freq_center, double freq_bw, double atten);

	void Configure(double samplerate, double error_max, double freq_bw_min,
		double win_pow, double delay_frac, double freq_cutoff,
		double freq_bw, double atten);

	long Get_Group_Delay_Samples() const;

	std::vector<double> Filter(std::vector<double> const& signal);

####*firf\_bp_tmp::*

	firf_bp_tmp();
	
	firf_bp_tmp(double samplerate, double error_max, double freq_bw_min,
		double win_pow, double delay_frac);
	
	void Set_Parameters(std::vector<double> const* freq_center,
		std::vector<double> const* freq_bw,
		std::vector<double> const* atten);
	
	void Configure(double samplerate, double error_max,	double freq_bw_min,
		double win_pow, double delay_frac,
		std::vector<double> const* freq_center,
		std::vector<double> const* freq_bw,
		std::vector<double> const* atten);
	
	long Get_Group_Delay_Samples() const;

	std::vector<double> Filter(std::vector<double> const& signal);

<a name="2.4"></a>
###2.4 FIR Band-Elimination Filters

####*firf_be::*

	firf_bp();

	firf_bp(double samplerate, double error_max, double freq_bw_min,
		double win_pow, double delay_frac);

	void Set_Parameters(double freq_center, double freq_bw, double atten);

	void Configure(double samplerate, double error_max, double freq_bw_min,
		double win_pow, double delay_frac, double freq_cutoff,
		double freq_bw, double atten);

	long Get_Group_Delay_Samples() const;

	std::vector<double> Filter(std::vector<double> const& signal);

####*firf\_be_tmp::*

	firf_be_tmp();

	firf_be_tmp(double samplerate, double error_max, double freq_bw_min,
		double win_pow, double delay_frac);

	void Set_Parameters(std::vector<double> const* freq_center,
		std::vector<double> const* freq_bw,
		std::vector<double> const* atten);

	void Configure(double samplerate, double error_max, double freq_bw_min,
		double win_pow, double delay_frac,
		std::vector<double> const* freq_center,
		std::vector<double> const* freq_bw,
		std::vector<double> const* atten);

	long Get_Group_Delay_Samples() const;
	
	std::vector<double> Filter(std::vector<double> const& signal);

<a name="2.5"></a>
###2.5 Exception Safety

All filters provide *strong exception safety*.

* *accuracy\_error* derived from *std::runtime_error*

	* Will be thrown when error\_max, samplerate, freq\_min or freq\_bw\_min are incompatible for accuracy. Can be thrown from constructors(…) and Configure(…) member functions.

* *config\_error* derived from *std::runtime_error*

	* Will be thrown when a filter is being configured with out of range inputs or when an a filter is not properly configured for requested operation. Can be thrown from contructors(…), Configure(…), Get_Group_Delay_Samples() and Filter(…) member functions.

* *parameter\_error* derived from *std::runtime_error*

	* Will be thrown when a filter's parameter are being set with out of range inputs or when a filter's parameters have not been set prior to filtering. Can be thrown from Configure(…), Set_Parameters(…), and Filter(…) member functions.

* *std::bad\_alloc*

	* Will be thrown when wavetable or operation vectors needed for accurate computation exceed computer memory. Can be thrown from contructors(…), Configure(…), and Filter(…) member functions.

* *std::out\_of\_range*

	* Can be thrown during Filter(…) member functions in the case of [2.7) Most Dangerous Pitfall](#2.7)
	
<a name="2.6"></a>
###2.6 Input Definitions

####*double samplerate;*

Sample rate of the data to be filtered. Requires range (0.0, environment dependent max] and must be a whole number.

####*double error\_max;*

The maximum allowable relative error relative to filtered data's absolute maximum value.

####*double freq\_min; double freq\_bw_min;*

The minimum frequency that the filter must guarantee results will be within error\_max. In low-pass and high pass filters this is the minimum cutoff frequency. In band-pass and band elimination filters this is the minimum bandwidth. Has a direct effect on number of filter taps.

####*double win\_pow;*

The power of the cosine window. Range [0.0, 0.0.] and [1.0, environment dependent max]. The range of (0.0, 1.0) can be produced but causes *wvt\_win* to no longer guarantee the results will be within the user defined max error.

####*double delay\_frac;*

A fraction that determines the causality of the filter. Range [0.0, 1.0]. 0.0 results in a completely causal computation with no signal delay. 1.0 results in the signal being delayed by *long Get\_Group\_Delay\_Samples() const* filter member function so that non-causal samples can be accessed for computation.

####*double freq\_cutoff; std::vector&lt;double&gt; freq\_cutoff;*

Cutoff frequency (Hz) for low-pass and high-pass filters. Range [0.0, samplerate / 2.0). Cutoff frequency can go below *freq\_min* but the filter will lose accuracy. Affects *wvt\_sinc*. Temporal vector size has an environmental dependent max.

####*double freq\_center; std::vector&lt;double&gt; freq\_center;*

Center frequency (Hz) for band-pass and band-elimination filters. Range [0.0, samplerate / 2.0). Affects *wvt\_cos*. Temporal vector size has an environmental dependent max.

####*double freq\_bw; std::vector&lt;double&gt; freq\_bw;*

Bandwidth (Hz) for band-pass and band-elimination filters. Range [0.0, samplerate / 2.0). Can go below *freq\_bw\_min* but the filter will lose accuracy. Affects *wvt\_sinc*. Temporal vector size has an environmental dependent max.

####*double \_atten; std::vector&lt;double&gt; atten;*

Attenuation of filter. Range [0.0, 1.0]. Temporal vector size has an environmental dependent max.

<a name="2.7"></a>
###2.7 Pitfalls
* A unobvious pitfall is failing to create filters because of an *accuracy\_error*. This happens because a wavetable is unable to meet *error\_max* restrictions. A very high *samplerate* to *freq\_min* ratio can also cause this

* In order to avoid error checking in repeated loops, the parameters are only error checked upon setting and prior to running. This means that a temporal vector can potentially be accessed and modified while a filtering occurs if using in a multi-threaded context. Parameters that are referenced by filters should be thread safe if being used in a multi-threaded context.

<a name="3."></a>
##3. Mechanisms

<a name="3.1"></a>
###3.1 Filters

All filters are derived from *firf\_base*. Each filter type has a *\_imp\_resp* member class derived from *imp\_resp\_base* which provides the impulse response (or filter tap coefficients) to the filter. All filters have a *\_ring\_buffer* member class which acts as the taps the digital signal is passing through.

The filter classes have the responsibility of being the highest level interface. The filter classes are responsible for managing and computing results from from *\_imp\_resp* and *\_ring\_buffer*. The filter classes reflect and normalize the causal impulse response and manage the ring buffer according to the filter's configured *\delay\_frac* and *\_freq\_min*. 

<a name="3.2"></a>
###3.2 Impulse Responses

*imp\_resp\_lp*, *imp\_resp\_hp*, *imp\_resp\_bp*, and *imp\_resp\_be* are derived from *imp\_resp\_base* and are responsible for properly combining results from wavetable member classes and  aggregating error distribution among wavetables when setting filter configurations.

The order of operations is described below:

####*imp\_resp\_lp*

* Retrieves *wvt\_sinc* response (based on cutoff frequency) and multiplies all samples by the attenuation fraction

* Retrieves and applies the power of cosine window

####*imp\_resp\_hp*

* Retrieves *wvt\_sinc* response (based on cutoff frequency) and multiplies all samples by the attenuation fraction

* Negates all samples

* Adds appropriately sized impulse to the zeroth sample

* Retrieves and applies the power of cosine window

####*imp\_resp\_bp*

* Retrieves *wvt\_sinc* response (based on bandwidth) and multiplies all samples by the attenuation fraction

* Retrieves *wvt\_cos* response (based on center frequency) and multiplies the *wvt\_sinc* response to shift the frequency 

* Retrieves and applies the power of cosine window

####*imp\_resp\_be*

* Retrieves *wvt\_sinc* response (based on bandwidth) and multiplies all samples by the attenuation fraction

* Negates all samples

* Adds appropriately sized impulse to the zeroth sample

* Retrieves *wvt\_cos* response (based on center frequency) and multiplies the *wvt\_sinc* response to shift the frequency 

* Retrieves and applies the power of cosine window

<a name="3.3"></a>
###3.3 Wavetables

This library is built on the foundation of generated wavetables to an accuracy within a user specified maximum relative error. These tables are used for fast retrieval of mathematical equations. 

####*wvt\_sinc*

The backbone of the FIR filter response is the sinc function. A sinc response is the time domain response of a low-pass function in the frequency domain. In addition to the low-pass function, this response is manipulated to provide the rest of the filter responses.

####*wvt\_win*

This wavetable gives the response of cos(x)^(*win\_pow*) over the range x[0, pi/4] fitted to the size of the sinc response. It is the response used for the power of cosine windowing.

####*wvt\_cos*

Provides the causal cosine response used for frequency shifting the sinc response

<a name="3.4"></a>
###3.4 Remaining Within Max Error Limit

n = number of taps, e = machine epsilon

<a name="3.4.1"></a>
####*Wavetable Quantization*

Wavetable quantization is accomplished by sizing the wavetable in relation to the maximum of the derivative of the table and sample rate. The table is created with uniform samples in time with the derivative never exceeding the half the max error for that table (this is to compensate for access drift).

<a name="3.4.2"></a>
####*Limiting Impulse Response Size and Wavetable Size*

The max impulse response size is limited to account for access drift for the wavetable. It ensures the access drift never exceeds one full element, and this keeps the wavetable output within max error limits.

####*Summation in firf Classes*

* error from impulse response sum of max wavetable quantization error and the multiplication of those values

* Normalization (using Kahan summation) adds small error with  summation and division

* Multiplying taps and data results in additional 2e

* Error is again propagated during final signal summation (not Kahan summation this time because the absolute value cannot be used). 

* (error_wvt + 2e + e + e )n+ ne + 2e

####*Kahan Summation*

Kahan Summation is used when the filter classes normalize the impulse response. Since the absolute values of the impulse are summed it is an ideal candidate for Kahan summation and reduces the error from 2e+O(ne) to 2e, where n is the number of samples to sum and e is machine epsilon.

	double sum = 0.0;
	double c = 0.0;
	for (auto& element : abs_sorted)
	{
		double y = element - c;
		double t = sum + y;
		c = (t - sum) - y;
		sum = t;
	}

<a name="4."></a>
###4. Update Plans

* Write *imp\_resp* classes for constant parameters that do not require wavetable generation so that near zero error results can be generated.

* Improve attenuation linearity

* Write a set of temporal filter classes which can be controlled with linearized and normalized frequency and bandwidth parameters.

* Create GUI and to display responses
