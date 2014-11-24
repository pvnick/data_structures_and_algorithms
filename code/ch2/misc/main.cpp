#include <iostream>
#include <time.h>
#include <string>
#include <vector>
#include <math.h>
#include <queue>

double lg(double);
uint F_ver1(uint);
uint N_factorial(uint);
uint N_factorial_digits(uint);
double lgN_factorial_approximation_sterling(uint);
double N_factorial_sterling(uint);
double H_ver1(uint);
double H_ver2(uint);
uint num_bits_required_to_store_int_ver1(uint);
uint num_bits_required_to_store_int_ver2(uint);
uint num_bits_required_to_store_int_ver3(uint);
uint ceil_lglg_est(uint);
double lglg(uint);

static double e = M_E;
static double eulers_constant = 0.57721;
static double pi = M_PI;

int main() {
	for (int N = 1000; N <= 1000000000; N *= 10) {
		std::cout << num_bits_required_to_store_int_ver1(N) << std::endl;
	}
	/*std::cout << "Number of bits required to store " << N << " (ver1):" << num_bits_required_to_store_int_ver1(N) << std::endl;
	std::cout << "Number of bits required to store " << N << " (ver2):" << num_bits_required_to_store_int_ver2(N) << std::endl;
	std::cout << "Number of bits required to store " << N << " (ver3):" << num_bits_required_to_store_int_ver3(N) << std::endl;
	std::cout << N << "th harmonic number (ver1): " << H_ver1(N) << std::endl;
	std::cout << N << "th harmonic number (ver2): " << H_ver2(N) << std::endl;
	std::cout << N << "th Fibonnaci number (ver1): " << F_ver1(N) << std::endl;
	std::cout << N << "! (ver1): " << N_factorial(N) << std::endl; */
	/*std::cout << N << "! sterling: " << N_factorial_sterling(N) << std::endl;
	std::cout << N << "! number of digits: " << N_factorial_digits(N) << std::endl;
	std::cout << N << "! sterling: " << log10(2) << std::endl;
	for (int i = 1; i < 100; i++) {
		double H = H_ver1(i);
		double H_approx = log(i) + eulers_constant;
		std::cout << i << " - " << H << " - " << H_approx << std::endl;			
	}
*/
	return 0;
}

uint N_factorial_digits(uint N) {
	//exercise 2.14
	double logN_factorial_approximation = lgN_factorial_approximation_sterling(N) * log10(2);
	return floor(logN_factorial_approximation + 1);
}

double H_ver1(uint N) {
	double Hn = 0;
	for (double term = 1; term <= N; Hn += 1L/term, ++term);
	return Hn;
}

double H_ver2(uint N) {
	return log(N) + eulers_constant + 1L / (12L * static_cast<double>(N));
}

double lg(double N) {
	return log10(N) / log10(2);
}

uint ceil_lglg_est(uint N) {
	//exercise 2.13
	auto ceil_lg_nplus1 = [](uint N) {
		uint lgNplus1 = 0;
		for (; N > 0; ++lgNplus1, N /= 2);
		return lgNplus1;
	};
	return ceil_lg_nplus1(ceil_lg_nplus1(N-1)-1);
}

double lglg(uint N) {
	return lg(lg(N));
}

uint F_ver1(uint N) {
	double Fn;
	std::queue<uint> Fq;
	Fq.push(0);
	Fq.push(1);
	for (uint term = 1; term <= N; ++term) {
		Fn += Fq.front();
		Fq.pop(); 
		Fq.push(Fn);
	}
	return Fn;
}

uint N_factorial(uint N) {
	uint result = 1;
	for (uint term = 2; term <= N; result *= term, term++);
	return result;
}

double lgN_factorial_approximation_sterling(uint N) {
	double N_dbl = static_cast<double>(N);
	return N_dbl * lg(N) - N_dbl * lg(e) + lg(sqrt(2L * pi * N_dbl));
}

double N_factorial_sterling(uint N) {
	double lgN_factorial_approximation = lgN_factorial_approximation_sterling(N);
	double factorial_approximation = pow(2, lgN_factorial_approximation);
	return factorial_approximation;
}

uint num_bits_required_to_store_int_ver1(uint N) {
	uint bits_required;
	for (bits_required = 0; N > 0; ++bits_required, N /= 2);
	return bits_required;
}

uint num_bits_required_to_store_int_ver2(uint N) {
	uint bits_required, t;
	//want t-1 < N <= t
	for (t = 1, bits_required = 0; t < N; ++bits_required, t *= 2);
	return bits_required;
}

uint num_bits_required_to_store_int_ver3(uint N) {
	return ceil(lg(N + 1));
	//I like this version better:
	//return floor(lg(N)) + 1;
}

double ln(double N) {
	return log(N) / log(e);
}
