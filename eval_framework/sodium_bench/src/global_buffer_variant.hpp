#ifndef GLOBAL_BUFFER_VARIANT_HPP
#define GLOBAL_BUFFER_VARIANT_HPP

// increase boost_mpl limit from default 20
#define BOOST_MPL_CFG_NO_PREPROCESSED_HEADERS
#define BOOST_MPL_LIMIT_LIST_SIZE 40
#define BOOST_MPL_LIMIT_VECTOR_SIZE 40

#include <map>
#include "global_buffers.hpp"

constexpr size_t max_buffer = 2097152;
auto global_buffers = makeBuffers<2,4,8,16,32,64,128,256,512,1024,2048,4096,8192,16384,32768,65536,131072,262144,524288,1048576,2097152>();
constexpr size_t args_num = std::tuple_size<decltype(global_buffers)>::value;


std::map<size_t, size_t> gen_args_map(const size_t args) {
	std::map<size_t, size_t> loc_map;
	for(size_t i =0; i < args; ++i) {
		loc_map.emplace_hint(loc_map.end(), std::make_pair(static_cast<size_t>(2<<i), i));
	}
	return loc_map;
}
// args map is a wrapper from size (powers of 2) to index in global buffers
// index [0] size [2]
// index [1] size [4]
// ...
// index [10] size [2048]
// ... to args_num (index)
std::map<size_t, size_t> args_map = gen_args_map(args_num);

class chunk_visitor : public boost::static_visitor<std::vector<std::vector<char>>> {
public:
	chunk_visitor(size_t threads) : threads_num(threads), splitted(false) {}

	template <size_t SIZE>
	std::vector<std::vector<char>> operator()(std::array<char,SIZE> &arr) {
		std::vector<char> msg_vec; msg_vec.reserve(SIZE);
		msg_vec.assign(arr.begin(),arr.end());
		split_if(msg_vec);
		return splitted_msg;
	}
	void split_if(std::vector<char> msg_vec) {
		std::lock_guard<std::mutex> lock(split_mtx);
		if (!splitted) {
			splitted_msg = split_char_vector(msg_vec, threads_num);
			splitted = true;
		}
	}

private:
	size_t threads_num;
	bool splitted;
	static std::mutex split_mtx;
	std::vector<std::vector<char>> splitted_msg;
};
std::mutex chunk_visitor::split_mtx;

#endif // GLOBAL_BUFFER_VARIANT_HPP
