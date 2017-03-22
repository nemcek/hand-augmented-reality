#pragma once

#include <opencv2\opencv.hpp>

using namespace cv;
using namespace std;

class InterestRegion
{
public:
	//vector<Vec3b> pixels;
	Mat matrix;
	const int radius;

	InterestRegion(const Mat& frame, const Point& center, int radius);
	~InterestRegion();

private:
	const Mat& frame;
	const Point& center;
	Point upper_left_corner;
	Point lower_right_corner;
};

