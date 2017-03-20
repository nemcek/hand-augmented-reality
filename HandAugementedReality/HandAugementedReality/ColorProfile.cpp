#include "ColorProfile.h"


ColorProfile::ColorProfile()
{
}

ColorProfile::ColorProfile(const Mat & frame, const vector<Point>& points)
{
	extract_color_profile(frame, points);
}

void ColorProfile::extract_color_profile(const Mat & frame, const vector<Point>& points)
{
	this->color_values = vector<int>();

	for (const Point& p : points) {
		Mat tmp;
		int thresh = (int)frame.at<uchar>(p.y, p.x);

		this->color_values.push_back(thresh);
	}
}

ColorProfile::~ColorProfile()
{
}
