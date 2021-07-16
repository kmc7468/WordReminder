#include "Utility.hpp"

#include <cassert>
#include <random>

int GetRandomInteger(int begin, int end) {
	assert(begin >= 0);
	assert(begin < end);

	static std::mt19937 mt(std::random_device{}());
	return mt() % (end - begin) + begin;
}