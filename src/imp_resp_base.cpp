#include "imp_resp_base.h"

imp_resp_base::imp_resp_base() :
    _samplerate(0.0), _resp_samples_max(0) {}

bool imp_resp_base::Valid_Freq_Input(double freq) const
{
    if (_sinc.Valid_Freq_Input(freq) && _win.Valid_Freq_Input(freq))
    {
        return true;
    }
    return false;
}

bool imp_resp_base::Valid_atten_Frac(double atten) const
{
    if (atten >= 0.0 && atten <= 1.0) { return true; }
    return false;
}

bool imp_resp_base::Valid_Imp_Resp() const
{
    if (_sinc.Valid_Wvt() && _win.Valid_Wvt())
    {
        return true;
    }
    return false;
}

std::tuple<double, double, double>
    imp_resp_base::Error_Distribution(double error_max, double win_pow)
{
    // Determines error distribution for minimum memory usage
    double dist_sinc = 1.0;
    double dist_win_resp = win_pow / 4.0;
    double dist_total = dist_sinc + dist_win_resp;
    double error = error_max - (2 * DBL_EPSILON);
    double err_sinc = error * dist_sinc / dist_total;
    double err_win = error * dist_win_resp / dist_total;
    
    return std::make_tuple(err_sinc, err_win, 0.0);
}

void imp_resp_base::Negate(std::vector<double>& resp)
{
    for (auto& sample : resp) { sample *= -1.0; }
    
    return;
}

double imp_resp_base::Impulse(double freq_cutoff)
{
    // Req: Valid_Freq_Input(freq_cutoff)
    return _samplerate / (2.0 * freq_cutoff);
}
