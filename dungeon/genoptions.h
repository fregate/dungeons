
#pragma once

#include "Vec.h"

struct genopts
{
	int width;
	int height;

	bool enterance; // place connection with outside

	Vec belowFloorPos; // place enterance from the below floor (like downladder) exact in this coords
					   // for floors connection. add some room as well
					   // if no need - set to -1,-1
					   // random pos - set to 0,0
	Vec upperFloorPos; // place enterance from the upper floor (like upladder) exact in this coords
					   // for floors connection. add some room as well
					   // if no need - set to -1,-1
					   // random pos - set to 0,0
};
