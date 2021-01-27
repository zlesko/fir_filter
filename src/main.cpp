#include <fstream>
#include <vector>
#include <random>

#include "firf_bp.h"
#include "firf_bp_tmp.h"
#include "firf_be.h"
#include "firf_be_tmp.h"
#include "firf_lp.h"
#include "firf_lp_tmp.h"
#include "firf_hp.h"
#include "firf_hp_tmp.h"
#include "wvt_cos.h"
#include "imp_resp_be.h"
#include "imp_resp_bp.h"
#include "imp_resp_hp.h"
#include "imp_resp_lp.h"
#include "wvt_sinc.h"
#include "wvt_win.h"

int main()
{
	std::mt19937 r(99999);
	std::vector<double> interval{ -1.0, 1.0 };
	std::vector<double> weights{ 1.0 };
	std::piecewise_constant_distribution<double> dist(interval.begin(), interval.end(), weights.begin());
	std::vector<double> signal(200000, 0.0);
	for (auto& elem : signal) { elem = dist(r); }
	//std::vector<double> signal(1, 1.0);
	
	
	/*int counter = 0;
	double p = 0.5;
	for (int sample = 0; sample < signal.size(); sample++)
	{
		if (counter > 110)
		{
			counter -= 110;
			p *= -1.0;
		}
		signal.at(sample) = p;
		counter++;
	}*/

	std::vector<double> fc(100000, 4000.0);
	double dec = (4000.0 - 500.0) / static_cast<double>(fc.size());
	double x = 4000.0;
	for (int sample = 0; sample < fc.size(); sample++)
	{
		fc.at(sample) = x;
		x -= dec;
	}
	std::vector<double> fb{ 1000.0 };
	std::vector<double> ro{ 1.0 };
	firf_bp_tmp filter(44100.0, 0.01, 500.0, 2.0, 1.0);
	//filter.Set_Parameters(&fc, &ro);
	filter.Set_Parameters(&fc, &fb, &ro);
	std::vector<double> result(filter.Filter(signal));



	std::ofstream raw_file("result.raw", std::ofstream::binary | std::ofstream::trunc);
	int data_size = result.size() * (sizeof(double) / sizeof(char));
	char const* data = (char*)result.data();
	raw_file.write(data, data_size);
	raw_file.close();


	return 0;
}