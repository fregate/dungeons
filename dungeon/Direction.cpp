
#include "stdafx.h"

#include "Direction.h"

const Direction Direction::NONE = Direction(0, 0);
const Direction Direction::N = Direction(0, -1);
const Direction Direction::NE = Direction(1, -1);
const Direction Direction::E = Direction(1, 0);
const Direction Direction::SE = Direction(1, 1);
const Direction Direction::S = Direction(0, 1);
const Direction Direction::SW = Direction(-1, 1);
const Direction Direction::W = Direction(-1, 0);
const Direction Direction::NW = Direction(-1, -1);

const Direction Direction::POSSIBLE[] = { Direction::NONE, Direction::N, Direction::NE, Direction::E, Direction::SE, Direction::S, Direction::SW, Direction::W, Direction::NW };

// The eight cardinal and intercardinal directions.
const Direction Direction::ALL[] = { Direction::N, Direction::NE, Direction::E, Direction::SE, Direction::S, Direction::SW, Direction::W, Direction::NW };

// The four cardinal directions: north, south, east, and west.
const Direction Direction::CARDINAL[] = { Direction::N, Direction::E, Direction::S, Direction::W };

// The four directions between the cardinal ones: northwest, northeast,
// southwest and southeast.
const Direction Direction::INTERCARDINAL[] = { Direction::NE, Direction::SE, Direction::SW, Direction::NW };

