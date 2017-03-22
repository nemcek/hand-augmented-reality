#include "InterestRegion.h"



InterestRegion::InterestRegion(const Mat & frame, const Point & center, int radius) 
	: frame(frame), center(center), radius(radius)
{
	this->upper_left_corner = Point(center.x - radius, center.y - radius);
	this->lower_right_corner = Point(center.x + radius, center.y + radius);

	this->matrix = frame(Rect(upper_left_corner, lower_right_corner));
}

InterestRegion::~InterestRegion()
{
}
