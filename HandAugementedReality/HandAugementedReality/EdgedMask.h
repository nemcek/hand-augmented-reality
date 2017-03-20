#pragma once

#include <opencv2\opencv.hpp>
#include "ColorProfile.h"

using namespace cv;
using namespace std;

class EdgedMask
{
public:
	Mat edges;

	EdgedMask();
	EdgedMask(const Mat &frame, const ColorProfile& color_profile);
	virtual ~EdgedMask();

private:
	void create_edges(const Mat &frame, const ColorProfile& color_profile);
};

