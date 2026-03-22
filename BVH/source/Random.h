#ifndef RANDOM_H
#define RANDOM_H
#include <random>

int RandomGen(size_t minValue, size_t maxValue)
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dist(minValue, maxValue);
	return dist(gen);
}

#endif