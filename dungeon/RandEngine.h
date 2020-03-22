
#pragma once

#include <random>
#include <chrono>

struct RandomEngineF
{
	uint32_t seed;
	std::uniform_real_distribution<double> dist;
	std::default_random_engine re;
	RandomEngineF(uint32_t numSeed = 0)
	{
		init(numSeed);
	}

	void init(uint32_t numSeed) {
		if (numSeed == 0) {
			numSeed = static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count());
		}
		seed = numSeed;
		re.seed(seed);
	}

	double get()
	{
		return dist(re);
	}

	template<class T>
	T range(const T& bnd1, const T& bnd2)
	{
		return static_cast<T>(bnd1 + std::floor((bnd2 - bnd1) * get()));
	}

	// 	template<class T>
	// 	T rangeIn(const T& bnd1, const T& bnd2)
	// 	{
	// 		return static_cast<T>(bnd1 + (bnd2 - bnd1) * get());
	// 	}

	template<class T>
	bool oneIn(const T& v)
	{
		return range(0, v) == T(0);
	}
};
