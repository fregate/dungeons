
#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <unordered_map>
#include <unordered_set>

#include "RandEngine.h"

// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
// # configuration

// my $dungeon_layout = {
//   'Box'         => [[1,1,1],[1,0,1],[1,1,1]],
//   'Cross'       => [[0,1,0],[1,1,1],[0,1,0]],
// };
// my $corridor_layout = {
//   'Labyrinth'   =>   0,
//   'Bent'        =>  50,
//   'Straight'    => 100,
// };
// my $map_style = {
//   'Standard' => {
//     'fill'      => '000000',
//     'open'      => 'FFFFFF',
//     'open_grid' => 'CCCCCC',
//   },
// };

namespace perl {

enum CellType : uint32_t {
    NOTHING = 0x00000000,

    BLOCKED = 0x00000001,
    ROOM = 0x00000002,
    CORRIDOR = 0x00000004,
    // #                 0x00000008;
    PERIMETER = 0x00000010,
    ENTRANCE = 0x00000020,
    ROOM_ID = 0x0000FFC0,

    ARCH = 0x00010000,
    DOOR = 0x00020000,
    LOCKED = 0x00040000,
    TRAPPED = 0x00080000,
    SECRET = 0x00100000,
    PORTC = 0x00200000,
    STAIR_DN = 0x00400000,
    STAIR_UP = 0x00800000,

    LABEL = 0xFF000000,

    OPENSPACE = ROOM | CORRIDOR,
    DOORSPACE = ARCH | DOOR | LOCKED | TRAPPED | SECRET | PORTC,
    ESPACE = ENTRANCE | DOORSPACE | 0xFF000000,
    STAIRS = STAIR_DN | STAIR_UP,

    BLOCK_ROOM = BLOCKED | ROOM,
    BLOCK_CORR = BLOCKED | PERIMETER | CORRIDOR,
    BLOCK_DOOR = BLOCKED | DOORSPACE,
};

typedef std::vector<std::vector<uint8_t>> DungeonLayout;

enum CorridorLayout : uint32_t
{
    clLabyrinth = 0,
    clBent = 50,
    clStraight = 100
};

enum class RoomLayout : uint8_t
{
    rlScattered = 0,
    rlPacked
};

enum class Direction : uint8_t {
	dNone,
	dNorth,
	dEast,
	dSouth,
	dWest
};

struct Style
{
    uint32_t colorFill;
    uint32_t colorOpen;
    uint32_t colorOpenGrid;
};

struct BuildOptions
{
    uint32_t seed;
    int32_t rows; // must be an odd number
    int32_t cols; // must be an odd number
    DungeonLayout dungeonLayout;
    uint32_t minRoomSize;
    uint32_t maxRoomSize;
    RoomLayout roomLayout;
    CorridorLayout corrLayout;
    float removeDeadEnds; // 0..1 [0 - none, 1 - all]
    bool stairUp;
	bool stairDown;
};

struct RenderOptions
{
    Style mapStyle;
    uint32_t cellSize;
};

struct PerlDungeon
{
    uint32_t seed;

    int32_t halfr;      // $dungeon->{'n_i'} = int($dungeon->{'n_rows'} / 2);
    int32_t halfc;      // $dungeon->{'n_j'} = int($dungeon->{'n_cols'} / 2);
    int32_t rows;   // $dungeon->{'n_rows'} = $dungeon->{'n_i'} * 2;
    int32_t cols;   // $dungeon->{'n_cols'} = $dungeon->{'n_j'} * 2;
    int32_t maxRow; // $dungeon->{'max_row'} = $dungeon->{'n_rows'} - 1;
    int32_t maxCol; // $dungeon->{'max_col'} = $dungeon->{'n_cols'} - 1;
    uint32_t roomsCount;  // $dungeon->{'n_rooms'} = 0;

    int32_t roomMax;   // my $max = $dungeon->{'room_max'};
    int32_t roomMin;   // my $min = $dungeon->{'room_min'};
    int32_t roomBase;  // $dungeon->{'room_base'} = int(($min + 1) / 2);
    int32_t roomRadix; // $dungeon->{'room_radix'} = int(($max - $min) / 2) + 1;

	bool stairsDown;
	bool stairsUp;

    DungeonLayout dungeonLayout;
    RoomLayout roomsLayout;
	CorridorLayout corridorLayout;

    std::vector<uint32_t> cells;

    RandomEngineF re;

	struct DoorData {
		Direction dir;
		uint32_t toRoom;
	};
	
	struct RoomData {
        uint32_t id;
        int32_t row, col;
        int32_t north, south, west, east;
        int32_t width, height;
        int64_t area;
		std::map<int32_t, DoorData> doors;
    };

	typedef std::map<Direction, int8_t> Dirs;
	const Dirs di{ { Direction::dNorth, -1 }, { Direction::dSouth, 1 }, { Direction::dWest, 0 }, { Direction::dEast, 0 } };
	const Dirs dj{ { Direction::dNorth, 0 }, { Direction::dSouth, 0 }, { Direction::dWest, -1 }, { Direction::dEast, 1 } };
	const std::vector<Direction> djDirs{ Direction::dNorth, Direction::dEast, Direction::dSouth, Direction::dWest };

	struct Sill {
		int32_t sillR;
		int32_t sillC;
		Direction dir;
		int32_t doorR;
		int32_t doorC;
		uint32_t outId;
	};

    typedef std::map<uint32_t, RoomData> Rooms;
    Rooms rooms;

	PerlDungeon(const BuildOptions& opts);

    void PrintDungeon() const;

    void Build() {
        InitCells();
        EmplaceRooms();
		PrintDungeon();
        OpenRooms();
		Corridors();
		if (stairsDown || stairsUp)
			EmplaceStairs();
		CleanDungeon();
    }

    void InitCells() {
        MaskCells();
    }

    void EmplaceRooms() {
        switch (roomsLayout) {
        case RoomLayout::rlPacked:
            PackedRooms();
            break;
        case RoomLayout::rlScattered:
            ScatteredRooms();
            break;
        }
    }

    void OpenRooms();

	void Corridors();

	bool EmplaceStairs();

	void CleanDungeon();

private:
    typedef std::map<std::string, int32_t> RoomMap;
	template <typename T, typename K>
    inline bool contains(const T& mm, const K& key) const {
        return mm.find(key) != mm.end();
    }

	inline int32_t idx(int32_t x, int32_t y) const {
		return x + y * (cols + 1);
	}

    uint32_t& cell(int32_t x, int32_t y) {
        const auto ss = cells.size();
		const int32_t i = idx(x, y);
		assert(i >= 0 && i < ss);
        if (i < 0 || i >= ss)
            throw;

        return cells[i];
    }

    const uint32_t& cell(int32_t x, int32_t y) const {
		const auto ss = cells.size();
		const int32_t i = idx(x, y);
		assert(i >= 0 && i < ss);
		if (i < 0 || i >= ss)
			throw;

		return cells[i];
	}

	std::vector<Sill> DoorSills(const RoomData& room);
	bool CheckSill(const RoomData& room, int32_t sillR, int32_t sillC, Direction dir, Sill& s);
	uint32_t AllocOpens(const RoomData& room);
    void OpenRoom(RoomData& room, std::unordered_map<uint64_t, uint32_t>& connections);

	void Tunnel(int32_t r, int32_t c, Direction last);
	std::vector<Direction> TunnelDirs(Direction last);
	bool OpenTunnel(int32_t r, int32_t c, Direction dir);
	bool SoundTunnel(int32_t midR, int32_t midC, int32_t nextR, int32_t nextC);
	bool DelveTunnel(int32_t r, int32_t c, int32_t nextR, int32_t nextC);

	void PutStairInRoom(const RoomData& room, bool isStairUp, uint32_t& count);

	void FixDoors();
	void EmptyBlocks();

    void MaskCells() {
        auto rx = dungeonLayout.size() / float(rows + 1);
        auto cx = dungeonLayout[0].size() / float(cols + 1);
        for (int32_t r = 0; r < rows; ++r) {
            for (int32_t c = 0; c < cols; ++c) {
                if (dungeonLayout[r * rx][c * cx] == 0)
                    cell(r, c) = CellType::BLOCKED;
            }
        }
    }

    void PackedRooms() {
        for (int32_t ii = 0; ii < halfr; ii++) {
            int32_t r = ii * 2 + 1;
            for (int32_t jj = 0; jj < halfc; jj++) {
                int32_t c = jj * 2 + 1;
                if ((cell(r, c) & CellType::ROOM) == CellType::ROOM)
                    continue;
                if ((ii == 0 || jj == 0) && re.range<bool>(true, false))
                    continue;

                RoomMap mm;
                mm["i"] = ii;
                mm["j"] = jj;
                EmplaceRoom(mm);
            }
        }
    }

    void ScatteredRooms() {

    }

    void EmplaceRoom(RoomMap& proto);

    void SetRoom(RoomMap& proto);

    int32_t SoundRoom(int32_t r1, int32_t c1, int32_t r2, int32_t c2);
};

}
