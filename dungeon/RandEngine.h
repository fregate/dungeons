
#pragma once

#include <random>
#include <chrono>

struct RandomEngineF
{
	std::uniform_real_distribution<double> dist;
	std::default_random_engine re;
	RandomEngineF(uint32_t numSeed = 0)
	{
		if (numSeed == 0)
			re.seed(static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count()));
		else
			re.seed(numSeed);
	}

	double operator () ()
	{
		return get();
	}

	double get()
	{
		return dist(re);
	}

	template<class T>
	T range(const T& bnd1, const T& bnd2)
	{
		return static_cast<T>(bnd1 + (bnd2 - bnd1) * get());
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

template<>
bool RandomEngineF::range<bool>(const bool&, const bool&)
{
	return static_cast<int>(get() + .5) == 1;
}
