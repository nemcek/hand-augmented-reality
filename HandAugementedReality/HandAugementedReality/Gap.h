#pragma once

#include "Location.h"

enum GapPosition {
	THUMB_INDEX = 0,
	INDEX_MIDDLE = 1,
	MIDDLE_RING = 2,
	RING_PINKY = 3
};

/*
 * Gap between fingers
 */
class Gap
{
public:
	Location location;
	GapPosition position;

	Gap();
	Gap(const Point & p, GapPosition position);
	~Gap();
};

