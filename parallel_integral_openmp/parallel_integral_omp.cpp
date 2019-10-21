#include "parallel_integral_omp.hpp"


#include <future>
#include <iostream>
#include <cmath>
#include <vector>

namespace parallel_integral {

	namespace {

		static const double kAccuracy = 0.00001;

		static const size_t t_size = std::thread::hardware_concurrency();

		double Function(const double& arg) {
			return arg;
		}

		struct AccuracyParameters {
			double parts = 2;
			double step;
			const Limits limits;
			AccuracyParameters(std::ifstream& input_file): limits(input_file){
				step = (limits.right - limits.left) / parts;
			}
			void Increment() {
				parts *= 2;
				step = (limits.right - limits.left) / parts;
			}
			operator double() {
				return step;
			}
		};

	} //namespace

	Limits::Limits(std::ifstream& input_file) {
		input_file.open("input.txt");
		std::string l, r;
		input_file >> left >> right;
		input_file.close();
	}

	double ComputeIntegral() {
		std::ifstream input_file;
		AccuracyParameters accuracy_parameters(input_file);
		double previous_result = 0, result = 0;
		do {
			previous_result = result;
			result = 0;
			accuracy_parameters.Increment();
#pragma omp parallel
			for (auto i = accuracy_parameters.limits.left; i < accuracy_parameters.limits.right; i += accuracy_parameters.step){
#pragma omp atomic
				result += accuracy_parameters.step * Function(i + accuracy_parameters.step / 2);
			}

		} while (abs(result - previous_result) >= kAccuracy);
		return result;
	}

}