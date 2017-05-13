#include "Gap.h"



Gap::Gap()
{
	this->location = Location();
}

Gap::Gap(const Point & p, GapPosition position) : position(position)
{
	this->location = Location(p);
}

Gap::~Gap()
{
}
