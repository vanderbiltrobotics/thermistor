#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <math.h>
#include <roboticscape.h>
//#include <covariance-tracker/covariance-tracker.h>
//#include <vector>


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
	double vref, v2, denom, numer, rx, log_term, b_term, c_term, d_term, temp_approx;
	
	//double vtotal;
	//double t_avg = 0;  //get an average over the cycles
	
	//CovarianceTracker<double, 1> temp_cov(100);
	//std::Vector<double> vec;
	//count = 0;

	while(cycles) {
		vref = rc_adc_volt(0);
		//vtotal = rc_adc_volt(2);
		//printf("Total voltage is %f \n", vtotal);
		v2   = rc_adc_volt(1);
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
		
		//vec[count] = temp_approx;
		//count++;
		
		//temp_cov.addData(temp_approx);
		usleep(500*1000);

		--cycles;
	}
	//t_avg = t_avg / 200.0; //replace denominator w/ number of cycles. Again, just for direct connect testing.
	//temp_cov.addData(temp_approx);

	return 0;
}
