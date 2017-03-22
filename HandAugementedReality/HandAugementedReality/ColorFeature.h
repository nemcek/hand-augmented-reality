#pragma once

#include <opencv2\opencv.hpp>

using namespace cv;

class ColorFeature
{
public:
	Vec3b min_value;
	Vec3b max_value;

	ColorFeature(const Vec3b& min_val, const Vec3b& max_val);
	~ColorFeature();
};

