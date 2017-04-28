#pragma once

#include <opencv2\opencv.hpp>

using namespace cv;

class ColorFeature
{
public:
	Vec3b min_value;
	Vec3b max_value;
	Vec3b min_value_portion;
	Vec3b max_value_portion;

	ColorFeature(Vec3b min_val, Vec3b max_val);
	~ColorFeature();
};

