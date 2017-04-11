#pragma once

#include <opencv2\opencv.hpp>
#include "Location.h"

using namespace cv;
using namespace std;

enum FingerType {
	THUMB = 0,
	INDEX = 1,
	MIDDLE = 2,
	RING = 3,
	PINKY = 4
};

class Finger
{
public:
	Rect roi;
	Mat roi_data;
	FingerType type;
	Location location;

	Finger();
	Finger(const Point& finger_tip_point, FingerType type);
	~Finger();
	void extract(const Mat& frame);

private:
	int width = 50;
	int height = 50;
};

