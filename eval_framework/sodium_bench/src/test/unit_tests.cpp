#include "unit_tests.hpp"
#include "../utils.hpp"

#include <limits>

void test_split_vec01() {

	std::vector<char> test_vec {'a','b','c','d'};
	std::cout << char_vector_tostring(test_vec, false);
	std::vector<std::vector<char>> splitted_vec = split_char_vector(test_vec,1);
	std::cout << ": [" << print_splitted_char_vector(splitted_vec) << "]\n";

	(splitted_vec.at(0) == std::vector<char>{'a','b','c','d'}) ?
		std::cout << "OK\n" :
		std::cout << "FALSE\n";

	splitted_vec.clear();
	splitted_vec = split_char_vector(test_vec,2);
	std::cout << char_vector_tostring(test_vec, false);
	std::cout << ": [" << print_splitted_char_vector(splitted_vec) << "]\n";

	((splitted_vec.at(0) == std::vector<char>{'a','b'}) && (splitted_vec.at(1) == std::vector<char>{'c','d'})) ?
		std::cout << "OK\n" :
		std::cout << "FALSE\n";

	splitted_vec.clear();
	splitted_vec = split_char_vector(test_vec,4);
	std::cout << char_vector_tostring(test_vec, false);
	std::cout << ": [" << print_splitted_char_vector(splitted_vec) << "]\n";
	((splitted_vec.at(0) == std::vector<char>{'a'}) &&
	 (splitted_vec.at(1) == std::vector<char>{'b'}) &&
	 (splitted_vec.at(2) == std::vector<char>{'c'}) &&
	 (splitted_vec.at(3) == std::vector<char>{'d'})) ?
		std::cout << "OK\n" :
		std::cout << "FALSE\n";

}

void test_split_vec02() {

	std::vector<char> test_vec {'a','b','c','d'};
	std::cout << char_vector_tostring(test_vec, false);
	std::vector<std::vector<char>> splitted_vec = split_char_vector(test_vec,3);
	std::cout << ": [" << print_splitted_char_vector(splitted_vec) << "]\n";

	((splitted_vec.at(0) == std::vector<char>{'a'}) &&
	 (splitted_vec.at(1) == std::vector<char>{'b'}) &&
	 (splitted_vec.at(2) == std::vector<char>{'c','d'})) ?
		std::cout << "OK\n" :
		std::cout << "FALSE\n";

	test_vec.clear();
	test_vec = std::vector<char>{'a','b','c','d','e'};
	std::cout << char_vector_tostring(test_vec, false);
	splitted_vec.clear();
	splitted_vec = split_char_vector(test_vec,3);
	std::cout << ": [" << print_splitted_char_vector(splitted_vec) << "]\n";

	((splitted_vec.at(0) == std::vector<char>{'a',}) &&
	 (splitted_vec.at(1) == std::vector<char>{'b','c'}) &&
	 (splitted_vec.at(2) == std::vector<char>{'d','e'})) ?
		std::cout << "OK\n" :
		std::cout << "FALSE\n";

}

// overall test with random data
void test_split_vec03() {

	std::vector<char> test_vec;
	for (unsigned long long i = 1; i < 1000 ; ++i) {
		test_vec = generate_random_char_vector(i);
		std::cout << "msg length = " << i << '\n';
		for (size_t j = 1; j < 1000; ++j) {
			auto splitted_vec = split_char_vector(test_vec, j);

			std::string left = char_vector_tostring(test_vec,false);
			std::string right;
			for(auto &i : splitted_vec) {
				right += char_vector_tostring(i,false);
			}
			if (left != right) {
				std::cout << "FALSE to split i:[" << i << "] j:["<< j <<"]\n";
				std::cout << "left: " << left << "size: " << left.size() << '\n';
				std::cout << "rigt: " << right<< "size: " << right.size() << '\n';
				throw std::runtime_error("fail to split");
			}
		}
		test_vec.clear();
	}

}

void run_unit_tests(benchmark::State &state) {
	//test_split_vec01();
	//test_split_vec02();
	test_split_vec03();
}
