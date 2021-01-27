#include "imp_resp_fshift.h"

bool imp_resp_fshift::Valid_Freq_Input(double freq) const
{
    if (imp_resp_base::Valid_Freq_Input(freq)
        && _cos.Valid_Freq_Input(freq))
    {
        return true;
    }
    return false;
}

bool imp_resp_fshift::Valid_Imp_Resp() const
{
    if (imp_resp_base::Valid_Imp_Resp()
        && _cos.Valid_Wvt())
    {
        return true;
    }
    return false;
}

std::tuple<double, double, double>
    imp_resp_fshift::Error_Distribution(double error_max, double win_pow)
{
    // Determines error distribution for minimum memory usage
    double dist_sinc = 1.0;
    double dist_win = win_pow / 4.0;
    double dist_cos = 1.0;
    double dist_total = dist_sinc + dist_win + dist_cos;
    double error = error_max - (2 * DBL_EPSILON);
    double err_sinc = error * dist_sinc / dist_total;
    double err_win = error * dist_win / dist_total;
    double err_cos = error * dist_cos / dist_total;

    return std::make_tuple(err_sinc, err_win, err_cos);
}
