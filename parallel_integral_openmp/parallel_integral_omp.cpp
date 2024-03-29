#include "parallel_integral_omp.hpp"


#include <iostream>
#include <cmath>

namespace parallel_integral {

	namespace {

		static const double kAccuracy = 0.00001;


		double Function(const double& arg) {
			return 3 * std::pow(arg, 2) + 2 * arg;
		}

		struct AccuracyParameters {
			int parts = 2;
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
		input_file >> left >> right;
		input_file.close();
	}

	double ComputeIntegral() {
		std::ifstream input_file;
		AccuracyParameters accuracy_parameters(input_file);
		double previous_result = 0, result = 0;
		int i = 0;
		do {
			
			previous_result = result;
			result = 0;
			accuracy_parameters.Increment();
#pragma omp parallel shared(accuracy_parameters) reduction(+:result)
			{
				#pragma omp for
				for (i = 0; i < accuracy_parameters.parts; ++i) {
					auto x = accuracy_parameters.limits.left + i * accuracy_parameters.step;
					result += accuracy_parameters.step * Function(x + accuracy_parameters.step / 2);
				}
			}

		} while (abs(result - previous_result) >= kAccuracy);
		return result;
	}

}