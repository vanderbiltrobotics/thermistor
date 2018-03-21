#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <math.h>
extern "C" {
  #include <roboticscape.h>
  #include <rc_usefulincludes.h>
}
#include <covariance-tracker/covariance-tracker.h>
#include <vector>


// R1 = values of the three other resisitors in the Wheatstone Bridge
const double R1 = 10000.0;

// RREF = resistance of the thermistor at 25 degrees Celsius
const double RREF = 10000.0;

// Steinhart-Hart equation parameters, found on the thermistor datasheet
const double A1 = 0.003354016;
const double B1 = 0.0002569850;
const double C1 = 0.000002620131;
const double D1 = 0.00000006383091;


int main(void) {

	rc_initialize();

	int cycles = 200; //used only for the purpose of testing the code while connected directly to the BeagleBone
	double vref, denom, numer, rx, log_term, b_term, c_term, d_term, temp_approx;


	//double vtotal;
	//double t_avg = 0;  //get an average over the cycles
	double intermediateAvg = 0;
	//double count = (double) cycles;
	CovarianceTracker<double, 1> temp_cov(cycles);
	std::vector<double> vec;

	while(cycles) {
		vref = rc_adc_volt(0);
		//vtotal = rc_adc_volt(2);
		//printf("Total voltage is %f \n", vtotal);
		//v2   = rc_adc_volt(1);
		numer = vref * RREF;
		denom = 1.794 - vref;
		rx = numer/denom;

		printf("The voltage at the ref is %f.\n", vref);
		printf("The calculated resistance of the thermistor is %f.\n", rx);

		log_term = log(rx/RREF);
		b_term = B1*log_term;
		c_term = C1*log_term*log_term;
		d_term = D1*log_term*log_term*log_term;

		// temp_approx is the approximate temperature in degrees Celsius
		temp_approx = 1.0 / (A1 + b_term + c_term + d_term) - 273.15;
		printf("The calculated temp is %f.\n\n", temp_approx);
		//t_avg += temp_approx;
		//intermediateAvg += temp_approx;

		double ta_arr[] = {temp_approx};
		temp_cov.addData(ta_arr);
		usleep(500*1000);

		if(cycles%10 == 0) {
			Eigen::Matrix<double, 1, 1> intMean = temp_cov.getMean();
			intermediateAvg = intMean(0,0);
			printf("---------------------------\n");
			printf("Avg of last 10 points: %f\n", intermediateAvg);
			printf("---------------------------\n\n");
			intermediateAvg = 0;
		}
		--cycles;
	}
	//t_avg = t_avg/count;
	Eigen::Matrix<double, 1, 1> cov = temp_cov.getCovariance();
	double var = cov(0,0);
	Eigen::Matrix<double, 1, 1> mean = temp_cov.getMean();
	double t_avg = mean(0,0);
	printf("--------------------------\n");
	printf("Overall mean: %f\n", t_avg);
	printf("Covariance! %f\n", var);
	printf("--------------------------\n");

	return 0;
}
