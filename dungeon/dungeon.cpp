// dungeon.cpp : Defines the entry point for the console application.
//

// http://journal.stuffwithstuff.com/2014/12/21/rooms-and-mazes/

#include "stdafx.h"

#include <iostream>

#include "Dungeon.h"
#include "PerlDungeon.h"

int main()
{
	using namespace perl;
/*
	#define TESTSNUM 10
	int test(TESTSNUM);

	RandomEngineF re;

	while (--test)
	{
		Dungeon d(20);
		d.generate({ 45, 45, re.range(true, false), { 0, 0 }, { 0, 0 } });
		d.dungeon_field._print([](unsigned short v) -> char {
			switch (v)
			{
				case Tiles::twall: return '#';
				case Tiles::tladderup: return 'U';
				case Tiles::tladderdown: return 'D';
				default: return ' ';
			}
		});
	}
*/
	DungeonLayout Fill{ { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 } };
	DungeonLayout Box{ { 1, 1, 1 }, { 1, 0, 1 }, { 1, 1, 1 } };
	DungeonLayout Cross{ { 0, 1, 0 }, { 1, 1, 1 }, { 0, 1, 0 } };

	Style mapStandart{ 0, 0xFFFFFF, 0xCCCCCC };

	BuildOptions opts{
		1, // uint32_t seed;
		19, // uint32_t rows; // must be an odd number
		19, // uint32_t cols; // must be an odd number
		Fill, // DungeonLayout dungeonLayout;
		3, // uint32_t minRoomSize;
		7, // uint32_t maxRoomSize;
		RoomLayout::rlPacked, // RoomLayout roomLayout;
		CorridorLayout::clBent, // CorridorLayout corrLayout;
		0.5f, // float removeDeadEnds; // 0..1 [0 - none, 1 - all]
		true, // stairs Up;
		true // stairs Down
	};

	PerlDungeon pd(opts);
	pd.Build();
	pd.PrintDungeon();
    return 0;
}
