
#pragma once

struct Vec
{
	long x, y;
	Vec(long x_, long y_)
		: x(x_)
		, y(y_)
	{}

	Vec()
		: x(0), y(0)
	{}

	Vec operator + (const Vec& v)
	{
		return Vec(x + v.x, y + v.y);
	}

	Vec operator + (long c)
	{
		return Vec(x + c, y + c);
	}

	Vec operator - (const Vec& v)
	{
		return Vec(x - v.x, y - v.y);
	}

	Vec operator - (long c)
	{
		return operator+(-c);
	}

	Vec operator * (int m)
	{
		return Vec(x * m, y * m);
	}

	size_t lenSquared() const
	{
		return x * x + y * y;
	}

	bool operator > (size_t L) const
	{
		return lenSquared() > L * L;
	}

	bool operator > (const Vec& v) const
	{
		return lenSquared() > v.lenSquared();
	}

	bool operator >= (size_t L) const
	{
		return lenSquared() >= L * L;
	}

	bool operator >= (const Vec& v) const
	{
		return lenSquared() >= v.lenSquared();
	}

	bool operator < (size_t L) const
	{
		return lenSquared() < L * L;
	}

	bool operator < (const Vec& v) const
	{
		return lenSquared() < v.lenSquared();
	}

	bool operator <= (size_t L) const
	{
		return lenSquared() <= L * L;
	}

	bool operator <= (const Vec& v) const
	{
		return lenSquared() <= v.lenSquared();
	}
};
