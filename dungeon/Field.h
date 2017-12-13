
#pragma once

#include <exception>
#include <iostream>
#include <algorithm>
#include <functional>
#include <vector>

#include "Vec.h"

template<class Tv>
class Field
{
	std::vector<Tv> _field;

	long w, h;

protected:
	void validate_pos(const Vec& pos) const
	{
		if (IsEmpty())
			throw std::exception("Field: no dungeon allocated");

		if (pos.x < 0 || pos.x > w || pos.y < 0 || pos.y > h)
			throw std::exception("Field::validate_pos: out of bounds");
	}

public:
	Field()
		: w(0)
		, h(0)
	{};

	void _print(std::function<char(Tv)> _Filler) const
	{
		std::cout << std::endl;

		for (long y = 0; y < h; y++)
		{
			for (long x = 0; x < w; x++)
			{
				std::cout << " " << _Filler(get(Vec(x, y)));
			}

			std::cout << std::endl;
		}

		std::cout << std::endl;
	}

	Field(size_t width, size_t height, Tv filler)
	{
		InitField(width, height, filler);
	};

	~Field()
	{
	};

	size_t width() const
	{
		return w;
	}

	size_t height() const
	{
		return h;
	}

	void InitField(long width, long height, Tv filler)
	{
		if (width == 0 || 0 == height)
			throw std::exception("null field");

		w = width;
		h = height;

		_field.clear();
		_field.resize(w * h, filler);
	}

	bool IsEmpty() const
	{
		return w == 0 || 0 == h || _field.empty();
	}

	Tv operator[] (Vec pos) const
	{
		validate_pos(pos);
		return _field[pos.y * w + pos.x];
	}

	Tv& operator[] (Vec pos)
	{
		validate_pos(pos);
		return _field[pos.y * w + pos.x];
	}

	void set(Vec pos, Tv v)
	{
		validate_pos(pos);
		_field[pos.y * w + pos.x] = v;
	}

	Tv get(Vec pos) const
	{
		return operator[] (pos);
	}
};
