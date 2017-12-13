
#pragma once

#include <list>
#include <set>
#include <map>
#include <assert.h>

#include "Vec.h"
#include "Direction.h"
#include "Field.h"
#include "Rect.h"

#include "RandEngine.h"

#include "Tiles.h"
#include "genoptions.h"

struct Dungeon // extends StageBuilder
{
	Dungeon(int nTries, uint32_t nDS = 0)
		: numRoomTries(nTries)
		, re(nDS)
	{}

	Field<unsigned short> dungeon_field;

	CRect bounds; // = > tiles.bounds;

	RandomEngineF re;

	//	int get numRoomTries;
	int numRoomTries;

	// The inverse chance of adding a connector between two regions that have
	// already been joined. Increasing this leads to more loosely connected
	// dungeons.
	//	int get extraConnectorChance = > 20;
	int extraConnectorChance = 50;

	// Increasing this allows rooms to be larger.
	//	int get roomExtraSize = > 0;
	int roomExtraSize = 0;

	//	int get windingPercent = > 0;
	int windingChance = 5;

	//	var _rooms = <Rect>[];
	std::vector<CRect> _rooms;

	// For each open position in the dungeon, the index of the connected region
	// that that position is a part of.
	//	Array2D<int> _regions;
	Field<int> _regions;

	// The index of the current region being carved.
	int _currentRegion = -1;

	void setTile(Vec pos, unsigned short t)
	{
		dungeon_field.set(pos, t);
	}

	unsigned short getTile(Vec pos) const
	{
		return dungeon_field.get(pos);
	}

	void generate(const genopts& gp)
	{
		if (gp.width % 2 == 0 || gp.height % 2 == 0)
		{
			throw std::exception("The stage must be odd-sized.");
		}

		bounds.SetRect(0, 0, gp.width, gp.height);

		dungeon_field.InitField(gp.width, gp.height, Tiles::twall);

		//		bindStage(stage);

		//		fill(Tiles.wall);
		//		_regions.reserve() = new Array2D(stage.width, stage.height);
		_regions.InitField(gp.width, gp.height, -1);

		_addRooms(gp.upperFloorPos, gp.belowFloorPos);
		// 		dungeon_field._print([](unsigned short v) -> char {
		// 			return v == Tiles::twall ? '#' : ' ';
		// 		});
		// 		_regions._print([](int v) -> char {
		// 			return v < 0 ? ' ' - 1 : ' ' + v;
		// 		});

		// Fill in all of the empty space with mazes.
		for (int y = 1; y < bounds.Height(); y += 2)
		{
			for (int x = 1; x < bounds.Width(); x += 2)
			{
				Vec pos(x, y);
				if (getTile(pos) != Tiles::twall)
					continue;

				_growMaze(pos);
			}
		}

		// 		dungeon_field._print([](unsigned short v) -> char {
		// 			return v == Tiles::twall ? '#' : ' ';
		// 		});
		// 		_regions._print([](int v) -> char {
		// 			return v < 0 ? ' ' - 1 : ' ' + v;
		// 		});

		_connectRegions();

		if (gp.enterance)
			_insertDungeonEnterance();

		_removeDeadEnds();

		// 		dungeon_field._print([](unsigned short v) -> char {
		// 			return v == Tiles::twall ? '#' : ' ';
		// 		});


		_insertLadders(gp.upperFloorPos.x == 0, gp.belowFloorPos.x == 0);

		//		_rooms.forEach(onDecorateRoom);
	}

	// set chests traps and so on
	void onDecorateRoom(CRect room)
	{}

	// Implementation of the "growing tree" algorithm from here:
	// http://www.astrolog.org/labyrnth/algrithm.htm.
	void _growMaze(Vec start)
	{
		std::list<Vec> cells;
		Direction lastDir;

		_startRegion();
		_carve(start);

		cells.push_back(start);
		while (!cells.empty())
		{
			Vec cell = cells.back();

			// See which adjacent cells are open.
			std::vector<Direction> unmadeCells;

			for (size_t idx = 0; idx < 4; idx++)
			{
				if (_canCarve(cell, Direction::CARDINAL[idx]))
					unmadeCells.push_back(Direction::CARDINAL[idx]);
			}

			if (!unmadeCells.empty())
			{
				// Based on how "windy" passages are, try to prefer carving in the
				// same direction.
				Direction dir;
				if (find(unmadeCells.begin(), unmadeCells.end(), lastDir) != unmadeCells.end() && !re.oneIn(windingChance))
				{
					dir = lastDir;
				}
				else
				{
					dir = unmadeCells[re.range<decltype(unmadeCells)::size_type>(0U, unmadeCells.size() - 1)];
				}

				_carve(cell + dir);
				_carve(cell + dir * 2);

				cells.push_back(cell + dir * 2);
				lastDir = dir;
			}
			else
			{
				// No adjacent uncarved cells.
				cells.pop_back();

				// This path has ended.
				lastDir = Direction::NONE;
			}
		}
	}

	// Places rooms ignoring the existing maze corridors.
	void _addRooms(const Vec& down, const Vec& up)
	{
		for (int i = 0; i < numRoomTries; i++)
		{
			// Pick a random room size. The funny math here does two things:
			// - It makes sure rooms are odd-sized to line up with maze.
			// - It avoids creating rooms that are too rectangular: too tall and
			//   narrow or too wide and flat.
			// TODO: This isn't very flexible or tunable. Do something better here.
			int size = re.range(1, 3 + roomExtraSize) * 2 + 1;
			int rectangularity = re.range(0, 1 + size / 2) * 2;
			int width = size;
			int height = size;

			if (re.range(true, false))
			{
				width += rectangularity;
			}
			else
			{
				height += rectangularity;
			}

			int x = re.range(0L, (bounds.Width() - width) / 2) * 2 + 1;
			int y = re.range(0L, (bounds.Height() - height) / 2) * 2 + 1;

			CRect room(x, y, x + width, y + height);

			CRect rintersect;
			bool overlaps = false;
			for each(auto other in _rooms)
			{
				if (room.Intersect(other, rintersect))
				{
					overlaps = true;
					break;
				}
			}

			if (overlaps)
				continue;

			_rooms.push_back(room);

			_startRegion();

			for (long px = room.left; px < room.right; px++)
			{
				for (long py = room.top; py < room.bottom; py++)
				{
					_carve(Vec(px, py));
				}
			}
		}
	}

	void _connectRegions()
	{
		// Find all of the tiles that can connect two (or more) regions.
		CRect infbounds(bounds);
		infbounds.InflateRect(-1, -1);

		struct Door
		{
			std::set<int> regs;
			Vec pos;
		};

		std::list< Door > connectorRegions;

		for (long px = infbounds.left; px < infbounds.right; px++)
		{
			for (long py = infbounds.top; py < infbounds.bottom; py++)
			{
				Vec pos(px, py);
				// Can't already be part of a region.
				if (getTile(pos) != Tiles::twall)
					continue;

				Door dr;
				for (size_t idx = 0; idx < 4; idx++)
					//				for (var dir in Direction.CARDINAL)
				{
					int region;
					try
					{
						region = _regions[pos + Direction::CARDINAL[idx]];
					}
					catch (std::exception&)
					{
						region = -1;
					}

					if (region != -1)
						dr.regs.insert(region);
				}

				if (dr.regs.size() < 2)
					continue;

				dr.pos = pos;
				connectorRegions.push_back(dr);
			}
		}

		//		var connectors = connectorRegions.keys.toList();

		// Keep track of which regions have been merged. This maps an original
		// region index to the one it has been merged to.
		std::map<int, int> merged;
		std::set<int> openRegions;
		for (int i = 0; i <= _currentRegion; i++)
		{
			merged[i] = i;
			openRegions.insert(i);
		}

		// Keep connecting regions until we're down to one.
		while (openRegions.size() > 1)
		{
			auto itcc = connectorRegions.begin();
			std::advance(itcc, re.range<decltype(connectorRegions)::size_type>(0U, connectorRegions.size()));
			Door connector = *itcc;

			// Carve the connection.
			_addJunction(connector.pos);

			// Merge the connected regions. We'll pick one region (arbitrarily) and
			// map all of the other regions to its index.

			std::set<int> regions;
			std::for_each(connector.regs.begin(), connector.regs.end(), [&](int v) {
				regions.insert(merged[v]);
			});
			//.map((region) = > merged[region]);
			assert(regions.size() == 2);
			int dest = *regions.begin();
			int source = *(--regions.end());

			// Merge all of the affected regions. We have to look at *all* of the
			// regions because other regions may have previously been merged with
			// some of the ones we're merging now.
			for (int i = 0; i <= _currentRegion; i++)
			{
				if (source == merged[i])
				{
					merged[i] = dest;
				}
			}

			// The sources are no longer in use.
			openRegions.erase(source);

			// Remove any connectors that aren't needed anymore.
			auto itt = connectorRegions.begin();
			while (itt != connectorRegions.end())
			{
				Door& d = *itt;
				if (connector.pos - d.pos < 2)
				{
					itt = connectorRegions.erase(itt);
					continue;
				}

				assert(d.regs.size() == 2);
				int ddest = *d.regs.begin();
				int dsrc = *d.regs.rbegin();
				if (merged[ddest] != merged[dsrc])
				{
					itt++;
					continue;
				}

				if (re.oneIn(extraConnectorChance))
					_addJunction(d.pos);

				itt = connectorRegions.erase(itt);
			}
		}
	}

	void _addJunction(Vec pos)
	{
		// 		if (rng.oneIn(4))
		// 		{
		// 			setTile(pos, rng.oneIn(3) ? Tiles.openDoor : Tiles.floor);
		// 		}
		// 		else
		// 		{
		// 			setTile(pos, Tiles.closedDoor);
		// 		}
		setTile(pos, Tiles::topened_door);
	}

	void _removeDeadEnds()
	{
		bool done = false;

		while (!done)
		{
			done = true;

			CRect infbounds(bounds);
			infbounds.InflateRect(-1, -1);

			for (long px = infbounds.left; px < infbounds.right; px++)
			{
				for (long py = infbounds.top; py < infbounds.bottom; py++)
				{
					Vec pos(px, py);
					if (getTile(pos) == Tiles::twall)
						continue;

					int exits = 0;
					for (size_t idx = 0; idx < 4; idx++)
					{
						Direction dir = Direction::CARDINAL[idx];
						if (getTile(pos + dir) != Tiles::twall)
							exits++;
					}

					if (exits != 1)
						continue;

					done = false;
					setTile(pos, Tiles::twall);
				}
			}
		}
	}

	void _insertDungeonEnterance()
	{
		Direction d = Direction::CARDINAL[re.range(0, 4)];

		while (1)
		{
			Vec pos, check;
			if (Direction::N == d)
			{
				pos.y = 0;
				pos.x = re.range(1L, bounds.right - 1);
			}
			else if (Direction::S == d)
			{
				pos.y = bounds.bottom - 1;
				pos.x = re.range(1L, bounds.right - 1);
			}
			else if (Direction::W == d)
			{
				pos.x = 0;
				pos.y = re.range(1L, bounds.bottom - 1);
			}
			else if (Direction::E == d)
			{
				pos.x = bounds.right - 1;
				pos.y = re.range(1L, bounds.bottom - 1);
			}

			check = pos + d.rotate180();

			if (_regions[check] >= 0)
			{
				_addJunction(pos);
				break;
			}
		}
	}

	void _insertLadders(bool up, bool down)
	{
		if (up)
		{
			CRect r = _rooms[re.range<decltype(_rooms)::size_type>(0U, _rooms.size())];
			r.InflateRect(-1, -1);
			Vec upl(re.range(r.left, r.right), re.range(r.top, r.bottom));
			setTile(upl, Tiles::tladderup);
		}

		// down
		while (down)
		{
			CRect r = _rooms[re.range<decltype(_rooms)::size_type>(0U, _rooms.size())];
			r.InflateRect(-1, -1);
			Vec dnl(re.range(r.left, r.right), re.range(r.top, r.bottom));

			bool bUpLadderNear(false);
			for (int idx = 0; idx < 9; idx++)
			{
				Direction d = Direction::POSSIBLE[idx];
				if (Tiles::tladderup == getTile(dnl + d))
				{
					bUpLadderNear = true;
					break;
				}
			}

			if (!bUpLadderNear)
			{
				setTile(dnl, Tiles::tladderdown);
				break;
			}
		}
	}

	// Gets whether or not an opening can be carved from the given starting
	// [Cell] at [pos] to the adjacent Cell facing [direction]. Returns `true`
	// if the starting Cell is in bounds and the destination Cell is filled
	// (or out of bounds).</returns>
	bool _canCarve(Vec pos, Direction direction)
	{
		// Must end in bounds.
		if (!bounds.InRect(pos + direction * 3))
			return false;

		// Destination must not be open.
		return getTile(pos + direction * 2) == Tiles::twall;
	}

	void _startRegion()
	{
		_currentRegion++;
	}

	void _carve(Vec pos, int type = Tiles::tnone)
	{
		if (Tiles::tnone == type)
			type = Tiles::tfloor;

		setTile(pos, type);
		_regions[pos] = _currentRegion;
	}
};
