#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <vector>
#include <map>

using Number = uint;

const Number rand_num(uint max = RAND_MAX) {
	return static_cast<double>(rand()) / RAND_MAX * max;
}

struct cartesian_point {
	double x;
	double y;
};

struct polar_point {
	double r;
	double theta;
};

void mean_and_stddev();
void convert_to_polar_coordinates(const cartesian_point, polar_point&);
std::vector<uint> generate_N_random_uints(const uint, const uint);
void print_unique_count(const std::vector<Number>);

int main() {
	const cartesian_point a = {32, 53.1};
	polar_point b;
	convert_to_polar_coordinates(a, b);
	std::cout << "r=" << b.r << ", theta=" << b.theta << std::endl;
	//mean_and_stddev();
	auto random_ints = generate_N_random_uints(10000, 10);
	print_unique_count(random_ints);
	return 0;
}

std::vector<Number> generate_N_random_uints(const uint N, const uint max)
{
	std::vector<Number> v(N);
	for (uint i = 0; i < N; ++i){
		v[i] = rand_num(max);
	}
	return v;
}

void print_unique_count(const std::vector<Number> ints) {
	std::map<Number, Number> unique_counts;
	for (auto i: ints) {
		unique_counts[i]++;
	}
	for (auto j: unique_counts) {
		std::cout << j.first << ": " << j.second << std::endl;
	}
}

void convert_to_polar_coordinates(const cartesian_point cartesian, polar_point& polar) {
	polar.r = sqrt(pow(cartesian.x, 2) + pow(cartesian.y, 2));
	polar.theta = atan2(cartesian.y, cartesian.x);
}

void mean_and_stddev() {
	Number N;
	while (1) {
		std::cout << "Enter sample size: ";
		if (!(std::cin >> N)) 
			break; 
		float mean, stddev;
		for (uint i = 0; i != N; ++i) {
			Number x = rand_num();
			mean += static_cast<float>(x) / N;
			stddev += static_cast<float>(pow(x, 2)) / N;
		}
		std::cout << "Avg: " << mean << std::endl;
		std::cout << "Stddev: " << stddev << std::endl;
	}
}