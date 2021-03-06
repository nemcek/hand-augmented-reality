#pragma once

#include <opencv2\opencv.hpp>
#include "ColorProfile.h"

using namespace cv;
using namespace std;

/*
 * Edge mask of image
 */
class EdgedMask
{
public:
	Mat edges;
	Mat mask;

	EdgedMask();
	EdgedMask(const Mat &frame, const ColorProfile& color_profile);
	virtual ~EdgedMask();

private:
	void create_edges(const Mat &frame, const ColorProfile& color_profile);
};

