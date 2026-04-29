#ifndef SORT_TESTS_HPP
#define SORT_TESTS_HPP

#include "utils.hpp"

int bubble_sort(std::vector<std::string> &vec) {
	while (true) {
		bool sorted=true;
		for (size_t i = 0; i < vec.size()-1; ++i) {
		if (vec.at(i) > vec.at(i+1)) {
				//vec.at(i+1) += vec.at(i);
				//vec.at(i) = vec.at(i+1) - vec.at(i);
				//vec.at(i+1) -= vec.at(i);
				std::string tmp = vec.at(i);
				vec.at(i) = vec.at(i+1);
				vec.at(i+1) = tmp;
				sorted=false;
			}
		}
		if (sorted) break;
	}
	return 0;
}

static void bubble_sort(benchmark::State& state) {

	size_t length = state.range(0);
	std::vector<std::string> vec;
	for (size_t i = 0; i < length; ++i) {
		vec.emplace_back(generate_random_string(20));
	}

	while (state.KeepRunning()) {
		benchmark::DoNotOptimize( bubble_sort(vec) );
	}
	state.SetComplexityN(state.range(0));
}

static void std_sort(benchmark::State& state) {

	size_t length = state.range(0);
	std::vector<std::string> vec;
	for (size_t i = 0; i < length; ++i) {
		vec.emplace_back(generate_random_string(20));
	}

	while (state.KeepRunning()) {
		std::sort(vec.begin(),vec.end());
	}
	state.SetComplexityN(state.range(0));
	state.SetBytesProcessed(state.range(0));
}

#endif // SORT_TESTS_HPP
