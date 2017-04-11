#include "Finger.h"



Finger::Finger()
{
}

Finger::Finger(const Point& finger_tip_point, FingerType type)
{
	this->point = finger_tip_point;
	this->roi = Rect(Point(point.x - width / 2, point.y - height / 4), Size(width, height));
}

void Finger::extract(const Mat & frame)
{
	this->roi_data = frame(this->roi);
}

Finger::~Finger()
{
}
