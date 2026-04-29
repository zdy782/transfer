#ifndef GLOBAL_BUFFERS_HPP
#define GLOBAL_BUFFERS_HPP

#include <boost/variant.hpp>
#include <random>
#include <algorithm>
#include <vector>
#include <tuple>
#include <stdexcept>

// simple at start

constexpr size_t buffer_max_size = 1073741824/1024;
std::vector<unsigned char> generate_randombyte_buffer(size_t size);

static std::vector<unsigned char> random_buf = generate_randombyte_buffer(buffer_max_size);

// at compile time

template <size_t... args>
using buffers = std::tuple<typename std::array<char, args>...>;

template <int... Is> struct seq {};
template <int N, int... Is> struct gen_seq : gen_seq<N - 1, N - 1, Is...> {};
template <int... Is> struct gen_seq<0, Is...> : seq<Is...> {};

template <int N, class T, class F> void apply_one(T &p, F func) {
	func(std::get<N>(p));
}

template <class T, class F, int... Is>
void apply(T &p, int index, F func, seq<Is...>) {
	using FT = void(T &, F);
	static constexpr FT *arr[] = {&apply_one<Is, T, F>...};
	arr[index](p, func);
}

template <class T, class F> void apply(T &p, int index, F func) {
	apply(p, index, func, gen_seq<std::tuple_size<T>::value>{});
}

static std::string Charset = "0123456789"
							 "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
							 "abcdefghijklmnopqrstuvwxyz";

struct Func {
	template <class T> void operator()(T&& p) {
		std::random_device
		rd; // Will be used to obtain a seed for the random number engine
		std::mt19937 gen(rd()); // Standard mersenne_twister_engine seeded with rd()
		std::uniform_int_distribution<> dis(0, Charset.size() - 1);

		std::generate(std::begin(p), std::end(p), [&]() { return Charset.at(dis(gen)); });
	}
};

// example of use:
// auto t = makeBuffersr<1, 2, 3, 500>();
// auto& arr1 = std::get<3>(t);
// for (char ch: arr1) {
// std::cout << static_cast<int>(ch) << std::endl;
template <size_t... args> buffers<args...> makeBuffers() {
	buffers<args...> result;

	for (size_t i = 0; i < std::tuple_size<buffers<args...>>::value; i++) {
		apply(result, i, Func{});
	}
	return result;
}

// inspiration from:
// http://stackoverflow.com/questions/8194227/how-to-get-the-i-th-element-from-an-stdtuple-when-i-isnt-know-at-compile-time
template <size_t n, typename... T>
boost::variant<T...> dynamic_get_impl(size_t i, const std::tuple<T...>& tpl) {
	if (i == n)
		return std::get<n>(tpl);
	else if (n == sizeof...(T) - 1)
		throw std::out_of_range("Tuple element out of range.");
	else
	return dynamic_get_impl<(n < sizeof...(T)-1 ? n+1 : 0)>(i, tpl);
}

template <typename... T>
boost::variant<T...> dynamic_get(size_t i, const std::tuple<T...>& tpl) {
	return dynamic_get_impl<0>(i, tpl);
}

#endif // GLOBAL_BUFFERS_HPP
