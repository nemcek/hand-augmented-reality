#include "Finger.h"



Finger::Finger()
{
}

Finger::Finger(const Point& finger_tip_point, FingerType type)
{
	this->location = Location(finger_tip_point);
	this->roi = Rect(Point(location.get().x - width / 2, location.get().y - height / 4), Size(width, height));
	this->type = type;
}

void Finger::extract(const Mat & frame)
{
	this->roi_data = frame(this->roi);
}

Finger::~Finger()
{
}
