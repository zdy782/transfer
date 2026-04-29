#ifndef UNIT_TESTS_HPP
#define UNIT_TESTS_HPP

#include <benchmark/benchmark.h>

void test_split_vec01();
void test_split_vec02();
void test_split_vec03();

void run_unit_tests(benchmark::State& state);

#endif // UNIT_TESTS_HPP
