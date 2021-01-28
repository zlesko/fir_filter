#include "imp_resp_be.h"

imp_resp_be::imp_resp_be() :
	imp_resp_fshift() {}

imp_resp_be::imp_resp_be(double samplerate,
 double error_max, double win_pow, long resp_samples_max) :
	imp_resp_fshift()
{
	Configure(samplerate, error_max, win_pow, resp_samples_max);

	return;
}

void imp_resp_be::Configure(double samplerate,
 double error_max, double win_pow, long resp_samples_max)
{
	double err_sinc(0.0), err_win(0.0), err_cos(0.0);
	std::tie(err_sinc, err_win, err_cos) =
		Error_Distribution(error_max, win_pow);
	auto temp_resp = std::make_unique<imp_resp_be>();
	temp_resp->_sinc.Configure(samplerate, err_sinc, resp_samples_max);
	temp_resp->_win.Configure(samplerate, err_win, win_pow);
	temp_resp->_cos.Configure(samplerate, err_cos, resp_samples_max);
	temp_resp->_samplerate = samplerate;
	temp_resp->_resp_samples_max = resp_samples_max;
	*this = *(temp_resp.get());

	return;
}

std::vector<double> imp_resp_be::Get_Causal_Imp_Resp(double freq_center,
	double freq_bw, double atten_frac)
{
	// Req: Valid_Freq_Input(freq) && Valid_atten_Frac(atten_frac)
	// Req: Valid_Imp_Resp()
	std::vector<double> sinc(_sinc.Get_Causal_Sinc_Rev(freq_bw / 2.0,
		_resp_samples_max));
	std::vector<double> win(_win.Get_Causal_Window(sinc.size()));
	std::vector<double> fshift(_cos.Get_Causal_Cos(freq_center, sinc.size()));
	Negate(sinc);
	sinc.at(0) += Impulse(freq_bw);
	for (long sample = 1; sample < sinc.size(); sample++)
	{
		sinc.at(sample) *= atten_frac;
	}
	std::vector<double> imp_resp(sinc.size(), 0.0);
	for (long sample = 0; sample < imp_resp.size(); sample++)
	{
		imp_resp.at(sample) = sinc.at(sample) *	win.at(sample)
			* fshift.at(sample);
	}

	return imp_resp;
}