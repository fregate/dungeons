// dungeon.cpp : Defines the entry point for the console application.
//

// http://journal.stuffwithstuff.com/2014/12/21/rooms-and-mazes/

#include "stdafx.h"

#include <iostream>

#include "Dungeon.h"

int main()
{
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


    return 0;
}
