
#pragma once

#include <exception>

#include "Vec.h"

struct Direction : public Vec
{
	static const Direction NONE;
	static const Direction N;
	static const Direction NE;
	static const Direction E;
	static const Direction SE;
	static const Direction S;
	static const Direction SW;
	static const Direction W;
	static const Direction NW;

	// All possible directions
	static const Direction POSSIBLE[];

	// The eight cardinal and intercardinal directions.
	static const Direction ALL[];

	// The four cardinal directions: north, south, east, and west.
	static const Direction CARDINAL[];

	// The four directions between the cardinal ones: northwest, northeast,
	// southwest and southeast.
	static const Direction INTERCARDINAL[];

	Direction(long x_, long y_) : Vec(x_, y_)
	{};

	Direction()
		: Vec(NONE.x, NONE.y)
	{}

	Direction rotateLeft45() const
	{
		if (NONE == *this)
			return NONE;
		if (N == *this)
			return NW;
		if (NE == *this)
			return N;
		if (E == *this)
			return NE;
		if (SE == *this)
			return E;
		if (S == *this)
			return SE;
		if (SW == *this)
			return S;
		if (W == *this)
			return SW;
		if (NW == *this)
			return W;

		throw std::exception("unreachable");
	}

	Direction rotateRight45() const
	{
		if (NONE == *this)
			return NONE;
		if (N == *this)
			return NE;
		if (NE == *this)
			return E;
		if (E == *this)
			return SE;
		if (SE == *this)
			return S;
		if (S == *this)
			return SW;
		if (SW == *this)
			return W;
		if (W == *this)
			return NW;
		if (NW == *this)
			return N;

		throw std::exception("unreachable");
	}

	Direction rotateLeft90() const
	{
		if (NONE == *this)
			return NONE;
		if (N == *this)
			return W;
		if (NE == *this)
			return NW;
		if (E == *this)
			return N;
		if (SE == *this)
			return NE;
		if (S == *this)
			return E;
		if (SW == *this)
			return SE;
		if (W == *this)
			return S;
		if (NW == *this)
			return SW;

		throw std::exception("unreachable");
	}

	Direction rotateRight90() const
	{
		if (NONE == *this)
			return NONE;
		if (N == *this)
			return E;
		if (NE == *this)
			return SE;
		if (E == *this)
			return S;
		if (SE == *this)
			return SW;
		if (S == *this)
			return W;
		if (SW == *this)
			return NW;
		if (W == *this)
			return N;
		if (NW == *this)
			return NE;

		throw std::exception("unreachable");
	}

	Direction rotate180() const
	{
		if (NONE == *this)
			return NONE;
		if (N == *this)
			return S;
		if (NE == *this)
			return SW;
		if (E == *this)
			return W;
		if (SE == *this)
			return NW;
		if (S == *this)
			return N;
		if (SW == *this)
			return NE;
		if (W == *this)
			return E;
		if (NW == *this)
			return SE;

		throw std::exception("unreachable");
	}

	bool operator == (const Direction& d) const
	{
		return x == d.x && y == d.y;
	}

	/*	String toString()
	{
	switch (this)
	{
	case NONE: return "NONE";
	case N: return "N";
	case NE: return "NE";
	case E: return "E";
	case SE: return "SE";
	case S: return "S";
	case SW: return "SW";
	case W: return "W";
	case NW: return "NW";
	}

	throw "unreachable";
	}
	*/
};

