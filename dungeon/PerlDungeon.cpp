
#include "stdafx.h"

#include <assert.h>
#include <set>

#include "PerlDungeon.h"

namespace perl {

PerlDungeon::PerlDungeon(const BuildOptions& opts)
{
	assert(opts.cols % 2 != 0 && opts.rows % 2 != 0);
	assert(opts.minRoomSize < opts.maxRoomSize);
	assert(opts.rows * opts.cols < std::numeric_limits<int32_t>::max());

	seed = opts.seed;

	halfr = opts.rows / 2;
	halfc = opts.cols / 2;
	rows = halfr * 2;
	cols = halfc * 2;
	maxRow = rows - 1;
	maxCol = cols - 1;
	roomsCount = 0;

	roomMax = opts.maxRoomSize;
	roomMin = opts.minRoomSize;
	roomBase = (roomMin + 1) / 2;
	roomRadix = (roomMax - roomMin) / 2 + 1;

	stairsUp = opts.stairUp;
	stairsDown = opts.stairDown;

	dungeonLayout = opts.dungeonLayout;
	roomsLayout = opts.roomLayout;
	corridorLayout = opts.corrLayout;

	cells.resize(opts.rows * opts.cols, CellType::NOTHING);

	re.init(seed);
}

void PerlDungeon::PrintDungeon() const
{
	std::cout << "----- dungeon ------" << std::endl;
	uint32_t cc = 0;
	std::cout << "|";
	for (auto c : cells) {
		char cch = ' ';
		if (c == CellType::NOTHING)
			cch = '#';
		else if ((c & CellType::BLOCKED) != 0)
			cch = '#';
		else if ((c & CellType::PERIMETER) != 0)
			cch = '#';
		else if ((c & CellType::STAIR_DN) != 0)
			cch = 'D';
		else if ((c & CellType::STAIR_UP) != 0)
			cch = 'U';
		else if ((c & CellType::ROOM) != 0) {
//			uint32_t id = (c & CellType::ROOM_ID) >> 6;
//			cch = '0' + id;
			cch = ' ';
		}
		else if ((c & CellType::DOOR) != 0)
			cch = '=';
		if (cc == cols + 1) {
			cc = 0;
			std::cout << "|" << std::endl << "|";
		}
		cc += 1;
		std::cout << cch;
	}
	std::cout << "|" << std::endl;
	std::cout << "----- dungeon ------" << std::endl;
}

void PerlDungeon::EmplaceRoom(RoomMap& proto)
{
	SetRoom(proto);

	int32_t r1 = proto["i"] * 2 + 1;
	int32_t c1 = proto["j"] * 2 + 1;
	int32_t r2 = (proto["i"] + proto["height"]) * 2 - 1;
	int32_t c2 = (proto["j"] + proto["width"]) * 2 - 1;

	if (r1 < 1 || r2 > maxRow)
		return;
	if (c1 < 1 || c2 > maxCol)
		return;

	auto hits = SoundRoom(r1, c1, r2, c2);
	if (hits == 0) {
		++roomsCount;
	}
	else {
		return;
	}

	printf("%d: %d, %d, %d, %d\n", roomsCount, r1, c1, r2, c2);

	for (auto r = r1; r <= r2; ++r) {
		for (auto c = c1; c <= c2; ++c) {
			auto& cc = cell(r, c);
			if ((cc & CellType::ENTRANCE) == CellType::ENTRANCE) {
				cc &= ~CellType::ESPACE;
			}
			else if ((cc & CellType::PERIMETER) == CellType::PERIMETER) {
				cc &= ~CellType::PERIMETER;
			}
			cc |= CellType::ROOM | (roomsCount << 6);
		}
	}

	int32_t height = ((r2 - r1) + 1) * 10;
	int32_t width = ((c2 - c1) + 1) * 10;

	rooms[roomsCount] = RoomData{ roomsCount, r1, c1, r1, r2, c1, c2, width, height, width * height };

	for (auto r = r1 - 1; r <= r2 + 1; ++r) {
		if ((cell(r, c1 - 1) & (CellType::ROOM | CellType::ENTRANCE)) == 0) {
			cell(r, c1 - 1) |= CellType::PERIMETER;
		}

		if ((cell(r, c2 + 1) & (CellType::ROOM | CellType::ENTRANCE)) == 0) {
			cell(r, c2 + 1) |= CellType::PERIMETER;
		}
	}

	for (auto c = c1; c <= c2; ++c) {
		if ((cell(r1 - 1, c) & (CellType::ROOM | CellType::ENTRANCE)) == 0) {
			cell(r1 - 1, c) |= CellType::PERIMETER;
		}

		if ((cell(r2 + 1, c) & (CellType::ROOM | CellType::ENTRANCE)) == 0) {
			cell(r2 + 1, c) |= CellType::PERIMETER;
		}
	}
}

void PerlDungeon::SetRoom(RoomMap& proto)
{
	if (!contains(proto, "height")) {
		auto r{ roomRadix };
		if (contains(proto, "i")) {
			int32_t a;
			if ((halfr - roomBase - proto["i"]) >= 0)
				a = halfr - roomBase - proto["i"];
			else
				a = 0;
			r = (a < roomRadix) ? a : roomRadix;
		}
		proto["height"] = re.range(0, r) + roomBase;
	}

	if (!contains(proto, "width")) {
		auto r{ roomRadix };
		if (contains(proto, "j")) {
			int32_t a;
			if ((halfc - roomBase - proto["j"]) >= 0)
				a = halfc - roomBase - proto["j"];
			else
				a = 0;
			r = (a < roomRadix) ? a : roomRadix;
		}
		proto["width"] = re.range(0, r) + roomBase;
	}

	if (!contains(proto, "i")) {
		proto["i"] = re.range(0, halfr - proto["height"]);
	}

	if (!contains(proto, "j")) {
		proto["j"] = re.range(0, halfc - proto["width"]);
	}
}

int32_t PerlDungeon::SoundRoom(int32_t r1, int32_t c1, int32_t r2, int32_t c2)
{
	std::set<uint32_t> roomHit;
	for (auto r = r1; r <= r2; ++r) {
		for (auto c = c1; c <= c2; ++c) {
			const auto ct = cell(r, c);
			if ((ct & CellType::BLOCKED) == CellType::BLOCKED) {
				return -1;
			}
			if ((ct & CellType::ROOM) == CellType::ROOM) {
				roomHit.insert((ct & CellType::ROOM_ID) >> 6);
			}
		}
	}

	return static_cast<int32_t>(roomHit.size());
}

void PerlDungeon::OpenRooms()
{
	std::unordered_map<uint64_t, uint32_t> connections;
	for (auto& r : rooms) {
		OpenRoom(r.second, connections);
	}
}

std::vector<PerlDungeon::Sill> PerlDungeon::DoorSills(const RoomData& room)
{
	std::vector<Sill> sills;
	if (room.north >= 3)
	{
		for (auto c = room.west; c <= room.east; c += 2)
		{
			Sill s;
			if (CheckSill(room, room.north, c, Direction::dNorth, s)) {
				sills.push_back(s);
			}
		}
	}
	// if ($room->{'north'} >= 3) {
	//     my $c; for ($c = $room->{'west'}; $c <= $room->{'east'}; $c += 2) {
	//     my $sill = &check_sill($cell,$room,$room->{'north'},$c,'north');
	//     push(@list,$sill) if ($sill);
	//     }
	// }

	if (room.south <= rows - 3)
	{
		for (auto c = room.west; c <= room.east; c += 2)
		{
			Sill s;
			if (CheckSill(room, room.south, c, Direction::dSouth, s)) {
				sills.push_back(s);
			}
		}
	}
	// if ($room->{'south'} <= ($dungeon->{'n_rows'} - 3)) {
	//     my $c; for ($c = $room->{'west'}; $c <= $room->{'east'}; $c += 2) {
	//     my $sill = &check_sill($cell,$room,$room->{'south'},$c,'south');
	//     push(@list,$sill) if ($sill);
	//     }
	// }

	if (room.west >= 3)
	{
		for (auto r = room.north; r <= room.south; r += 2)
		{
			Sill s;
			if (CheckSill(room, r, room.west, Direction::dWest, s)) {
				sills.push_back(s);
			}
		}
	}
	// if ($room->{'west'} >= 3) {
	//     my $r; for ($r = $room->{'north'}; $r <= $room->{'south'}; $r += 2) {
	//     my $sill = &check_sill($cell,$room,$r,$room->{'west'},'west');
	//     push(@list,$sill) if ($sill);
	//     }
	// }

	if (room.east <= cols - 3)
	{
		for (auto r = room.north; r <= room.south; r += 2)
		{
			Sill s;
			if (CheckSill(room, r, room.east, Direction::dEast, s)) {
				sills.push_back(s);
			}
		}
	}
	// if ($room->{'east'} <= ($dungeon->{'n_cols'} - 3)) {
	//     my $r; for ($r = $room->{'north'}; $r <= $room->{'south'}; $r += 2) {
	//     my $sill = &check_sill($cell,$room,$r,$room->{'east'},'east');
	//     push(@list,$sill) if ($sill);
	//     }
	// }
	// return &shuffle(@list);
	std::shuffle(sills.begin(), sills.end(), re.re);
	return sills;
}

bool PerlDungeon::CheckSill(const RoomData& room, int32_t sillR, int32_t sillC, Direction dir, Sill& s)
{
	s.doorR = sillR + di.at(dir);
	s.doorC = sillC + dj.at(dir);
	const auto doorCell = cell(s.doorR, s.doorC);
	if ((doorCell & CellType::PERIMETER) == 0)
		return false;
	if ((doorCell & CellType::BLOCK_DOOR) != 0)
		return false;

	auto outR = s.doorR + di.at(dir);
	auto outC = s.doorC + dj.at(dir);
	auto outCell = cell(outR, outC);
	if ((outCell & BLOCKED) != 0)
		return false;

	uint32_t outId{ 0 };
	if ((outCell & CellType::ROOM) != 0) {
		outId = (outCell & CellType::ROOM_ID) >> 6;
		if (outId == room.id)
			return false;
	}

	s.dir = dir;
	s.sillR = sillR;
	s.sillC = sillC;
	s.outId = outId;

	return true;
}

uint32_t PerlDungeon::AllocOpens(const RoomData & room)
{
	auto roomH = (room.south - room.north) / 2 + 1;
	auto roomW = (room.east - room.west) / 2 + 1;
	uint32_t flumph = std::sqrt(roomH * roomW);
	return flumph + re.range(0U, flumph);
}

void PerlDungeon::OpenRoom(RoomData& room, std::unordered_map<uint64_t, uint32_t>& connections)
{
	auto lsills = DoorSills(room);
	if (lsills.empty())
		return;

	auto opens = AllocOpens(room);

	auto makeHash = [](uint32_t n1, uint32_t n2) -> uint64_t {
		if (n1 > n2) {
			return uint64_t(n2) << 11 | n1;
		} else {
			return uint64_t(n1) << 11 | n2;
		}
	};

	for (uint32_t i = 0; i < opens; ++i) {
		auto it = lsills.begin() + re.range(0ULL, lsills.size());
		Sill s = *it;
		lsills.erase(it);
		if (lsills.empty())
			break;

		auto doorR = s.doorR;
		auto doorC = s.doorC;
		auto doorCell = cell(doorR, doorC);
		if ((doorCell & CellType::DOORSPACE) != 0)
			continue;

		uint32_t outId = s.outId;
		if (outId != 0) {
			connections[makeHash(outId, room.id)]++;
		}

		int32_t openR = s.sillR;
		int32_t openC = s.sillC;
		auto openDir = s.dir;

		for(int32_t x = 0; x < 3; ++x) {
			int32_t r = openR + (di.at(openDir) * x);
			int32_t c = openC + (dj.at(openDir) * x);

			cell(r, c) &= ~CellType::PERIMETER;
			cell(r, c) |= CellType::ENTRANCE;
		}

		// uint32_t doorType = CellType::DOOR; // random doors types in original
		cell(doorR, doorC) |= CellType::DOOR;
		room.doors[idx(doorR, doorC)] = DoorData{openDir, outId};
	}
}

void PerlDungeon::Corridors()
{
	for (int32_t i = 1; i < halfr; ++i) {
		auto r = i * 2 + 1;
		for (int32_t j = 1; j < halfc; ++j) {
			auto c = j * 2 + 1;

			if ((cell(r, c) & CellType::CORRIDOR) != 0)
				continue;

			Tunnel(r, c, Direction::dNone);
		}
	}
}

void PerlDungeon::Tunnel(int32_t r, int32_t c, Direction last)
{
	auto dirs = TunnelDirs(last);
	for (auto d : dirs) {
		if (OpenTunnel(r, c, d)) {
			auto i = r + di.at(d);
			auto j = c + dj.at(d);
			Tunnel(i, j, d);
		}
	}
}

std::vector<Direction> PerlDungeon::TunnelDirs(Direction last)
{
	std::vector<Direction> dirs = djDirs;
	std::shuffle(dirs.begin(), dirs.end(), re.re);

	if (last != Direction::dNone) {
		if (re.range(0U, 100U) < corridorLayout) {
			dirs.insert(dirs.begin(), last);
		}
	}

	return dirs;
}

bool PerlDungeon::OpenTunnel(int32_t r, int32_t c, Direction dir)
{
	auto thisR = r * 2 + 1;
	auto thisC = c * 2 + 1;
	int32_t nextR = (r + di.at(dir)) * 2 + 1;
	int32_t nextC = (c + dj.at(dir)) * 2 + 1;
	int32_t midR = (thisR + nextR) / 2;
	int32_t midC = (thisC + nextC) / 2;

	if (SoundTunnel(midR, midC, nextR, nextC)) {
		return DelveTunnel(thisR, thisC, nextR, nextC);
	}

	return false;
}

bool PerlDungeon::SoundTunnel(int32_t midR, int32_t midC, int32_t nextR, int32_t nextC)
{
	if (nextR < 0 || nextR > rows)
		return 0;
	if (nextC < 0 || nextC > cols)
		return 0;

	int32_t r1, r2;
	r1 = std::min(midR, nextR);
	r2 = std::max(midR, nextR);

	int32_t c1, c2;
	c1 = std::min(midC, nextC);
	c2 = std::max(midC, nextC);

	for (auto r = r1; r <= r2; r++)
	{
		for (auto c = c1; c <= c2; c++)
		{
			if (cell(r, c) & CellType::BLOCK_CORR)
				return false;
		}
	}

 	return true;
}

bool PerlDungeon::DelveTunnel(int32_t r, int32_t c, int32_t nextR, int32_t nextC)
{
	int32_t r1, r2;
	r1 = std::min(r, nextR);
	r2 = std::max(r, nextR);

	int32_t c1, c2;
	c1 = std::min(c, nextC);
	c2 = std::max(c, nextC);

	for (r = r1; r <= r2; r++)
	{
		for (c = c1; c <= c2; c++)
		{
			cell(r, c) &= ~CellType::ENTRANCE;
			cell(r, c) |= CellType::CORRIDOR;
		}
	}

	return true;
}

bool PerlDungeon::EmplaceStairs()
{
	uint32_t count{ 2 };
	bool stairType = re.range(true, false);
	if (stairsUp && !stairsDown) {
		count = 1;
		stairType = true;
	}

	if (stairsDown && !stairsUp) {
		count = 1;
		stairType = false;
	}

	if (count == 1) {
		PutStairInRoom(rooms.at(re.range(1U, roomsCount + 1)), stairType, count);
	}
	else {
		const auto stairsCount{ count };
		auto it = std::begin(rooms);
		while (count == stairsCount) {
			PutStairInRoom(it->second, stairType, count);
			++it;
		}

		for (auto rit = std::rbegin(rooms); rit != std::rend(rooms) && count; ++rit) {
			PutStairInRoom(rit->second, !stairType, count);
		}
	}

	if (count > 0)
		std::cout << "can't place plenty enough stairs" << std::endl;

	return count == 0;
}

void PerlDungeon::PutStairInRoom(const RoomData& room, bool isStairUp, uint32_t& count)
{
	if (room.width < 3 * 10 && room.height < 3 * 10)
		return;

	for (auto r = room.north; r < room.south; ++r) {
		for (auto c = room.west; c < room.east; ++c) {
			if ((cell(r, c) & CellType::STAIR_UP) != 0 || (cell(r, c) & CellType::STAIR_DN))
				return;
		}
	}

	// put stairs
	CellType stair = isStairUp ? CellType::STAIR_UP : CellType::STAIR_DN;
	uint32_t r = re.range(room.north, room.south);
	uint32_t c = re.range(room.west, room.east);

	cell(r, c) |= stair;
	cell(r, c + 1) |= stair;
	cell(r + 1, c) |= stair;
	cell(r + 1, c + 1) |= stair;

	// set all overlapped doors to perimeter (wall)
	auto checkRM = r + di.at(Direction::dNorth);
	auto checkRP = r + 1 + di.at(Direction::dSouth);
	auto checkCM = c + dj.at(Direction::dWest);
	auto checkCP = c + 1 + dj.at(Direction::dEast);

	try {
		if ((cell(checkRM, c) & CellType::DOOR) != 0) {
			cell(checkRM, c) &= ~CellType::DOOR;
			cell(checkRM, c) |= CellType::PERIMETER;
		}
	} catch(...) {}

	try {
		if ((cell(checkRP, c) & CellType::DOOR) != 0) {
			cell(checkRP, c) &= ~CellType::DOOR;
			cell(checkRP, c) |= CellType::PERIMETER;
		}
	}
	catch (...) {}

	try {
		if ((cell(checkRM, c + 1) & CellType::DOOR) != 0) {
			cell(checkRM, c + 1) &= ~CellType::DOOR;
			cell(checkRM, c + 1) |= CellType::PERIMETER;
		}
	}
	catch (...) {}

	try {
		if ((cell(checkRP, c + 1) & CellType::DOOR) != 0) {
			cell(checkRP, c + 1) &= ~CellType::DOOR;
			cell(checkRP, c + 1) |= CellType::PERIMETER;
		}
	}
	catch (...) {}

	try {
		if ((cell(r, checkCM) & CellType::DOOR) != 0) {
			cell(r, checkCM) &= ~CellType::DOOR;
			cell(r, checkCM) |= CellType::PERIMETER;
		}
	}
	catch (...) {}

	try {
		if ((cell(r, checkCP) & CellType::DOOR) != 0) {
			cell(r, checkCP) &= ~CellType::DOOR;
			cell(r, checkCP) |= CellType::PERIMETER;
		}
	}
	catch (...) {}

	try {
		if ((cell(r + 1, checkCM) & CellType::DOOR) != 0) {
			cell(r + 1, checkCM) &= ~CellType::DOOR;
			cell(r + 1, checkCM) |= CellType::PERIMETER;
		}
	}
	catch (...) {}

	try {
		if ((cell(r + 1, checkCP) & CellType::DOOR) != 0) {
			cell(r + 1, checkCP) &= ~CellType::DOOR;
			cell(r + 1, checkCP) |= CellType::PERIMETER;
		}
	}
	catch (...) {}

	--count;
}

void PerlDungeon::CleanDungeon()
{
	FixDoors();
	EmptyBlocks();
}

void PerlDungeon::FixDoors()
{
	std::set<int32_t> fixed;
	for (auto& roomit : rooms) {
		// north doors
		std::map<int32_t, DoorData> shiny;
		for (auto doorit : roomit.second.doors) {
			if ((cells.at(doorit.first) & CellType::OPENSPACE) == 0)
				continue;
			
			if (contains(fixed, doorit.first)) {
				shiny.insert(doorit);
			}
			else {
				auto outId = doorit.second.toRoom;
				if (outId > 0) {
					rooms[outId].doors.insert(doorit);
				}
				shiny.insert(doorit);
				fixed.insert(doorit.first);
			}
		}
		roomit.second.doors = shiny;
	}
}

void PerlDungeon::EmptyBlocks()
{
	for (auto& c: cells) {
		if ((c & CellType::BLOCKED) != 0)
		{
			c = CellType::NOTHING;
		}
	}
}

}
