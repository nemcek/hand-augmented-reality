#pragma once

#include <opencv2\opencv.hpp>
#include "ColorProfile.h"
#include "EdgedMask.h"
#include "Hand.h"

using namespace cv;
using namespace std;

class ImageProcessor
{
public:
	Mat result;

	ImageProcessor(const Mat& template_img, const vector<Point>& points);
	~ImageProcessor();

	void process(const Mat& frame);
	void init();

private:
	bool is_initialized = false;
	int color_feature_radius = 10;
	const vector<Point>& roi_points;
	vector<InterestRegion> rois;
	Mat frame;
	const Mat& template_img;
	ColorProfile color_profile;

	void process_not_initialized();
	void process_initialized();
};

