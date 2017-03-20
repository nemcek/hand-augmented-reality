#pragma once

#include <opencv2\opencv.hpp>

using namespace cv;
using namespace std;

class ColorProfile
{
public:
	vector<int> color_values;

	ColorProfile();
	ColorProfile(const Mat& frame, const vector<Point>& points);
	~ColorProfile();

private:
	void extract_color_profile(const Mat& frame, const vector<Point>& points);
};

