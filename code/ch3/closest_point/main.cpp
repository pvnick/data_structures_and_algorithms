#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <vector>
#include <string>
#include <stdexcept>


typedef double FP; //must be unsigned for distance function
struct Cartesian_Point 
{
	FP x;
	FP y;
};

using Multidimensional_Point = std::vector<FP>;

uint closest_point_quadratic_time(const uint N, const FP distance);
uint closest_point_quadratic_time_d_dimensions(const uint d, const uint N, const FP max_distance); 
const double euclidean_distance(const Cartesian_Point&, const Cartesian_Point&);
const double distance_d_dimensions(const Multidimensional_Point& point1, const Multidimensional_Point& point2);

FP random_float()
{
	return 1.0 * rand() / RAND_MAX;
}

int main() 
{
	FP distance;
	uint number_of_experiments = 100;
	while(1) {
		std::cout << "enter distance: ";
		std::cin >> distance;
		if (distance && 0 < distance) {
			uint cnt = closest_point_quadratic_time_d_dimensions(2, number_of_experiments, distance);
			//closest_point_quadratic_time(number_of_experiments, distance, cnt);
			//closest_point_quadratic_time(number_of_experiments, distance, cnt);
			std::cout << cnt << std::endl;
		} else {
			if (std::cin.bad()) {
				throw std::runtime_error("corrupted stream");
			} else {	
				std::cerr << "bad input" << std::endl;
				if (std::cin.fail()) {
					std::cin.clear();
					std::string nullstring;
					getline(std::cin, nullstring);
				}
			}
		}	
	}
}

const double distance_d_dimensions(const Multidimensional_Point& point1, const Multidimensional_Point& point2) {
	double sum = 0;
	if (point1.size() != point2.size()) {
		throw std::runtime_error("points have same dimensions");
	}
	for (uint i = 0; i < point1.size(); ++i) {
		sum += pow(point1[i] - point2[i], 2);
	}
	return sqrt(sum);
}


const double euclidean_distance(const Cartesian_Point& point1, const Cartesian_Point& point2) {
	return sqrt(pow(point1.x - point2.x, 2) + pow(point1.y - point2.y, 2)); 
}

uint closest_point_quadratic_time(const uint N, const FP max_distance) 
{
	uint cnt_within_distance;
	std::vector<Cartesian_Point> points(N);
	for (uint i = 0; i < N; ++i) {
		Cartesian_Point random_point{random_float(), random_float()};
		points[i] = random_point;
	}
	for (uint i = 0; i < N; ++i) {
		Cartesian_Point& point1 = points[i];
		for (uint j = i + 1; j < N; ++j) {
			Cartesian_Point& point2 = points[j];
			if (euclidean_distance(point1, point2) <= max_distance) {
				++cnt_within_distance;
			}
		}
	}
	return cnt_within_distance;
}


uint closest_point_quadratic_time_d_dimensions(const uint d, const uint N, const FP max_distance) 
{
	uint cnt_within_distance = 0;
	std::vector<Multidimensional_Point> points(N, Multidimensional_Point(d));
	for (auto& point: points) {
		for (uint i = 0; i < d; ++i) {
			point[i] = random_float();
		}
	}
	for (uint i = 0; i < N; ++i) {
		Multidimensional_Point point1 = points[i];
		for (uint j = i + 1; j < N; j++) {
			Multidimensional_Point point2 = points[j];
			double distance = distance_d_dimensions(point1, point2);
			if (distance <= max_distance) {
				++cnt_within_distance;
			}
		}
	}
	return cnt_within_distance;
}