#pragma once

#include <opencv2\opencv.hpp>
#include "ColorProfile.h"
#include "EdgedMask.h"
#include "Hand.h"
#include "Finger.h"

using namespace cv;
using namespace std;

class ImageProcessor
{
public:
	Mat result;
	bool is_initialized = false;

	ImageProcessor(const Mat& template_img, const vector<Point>& points);
	~ImageProcessor();

	void process(const Mat& frame);
	void init();

private:
	int color_feature_radius = 7;
	const vector<Point>& roi_points;
	vector<InterestRegion> rois;
	Mat frame;
	const Mat& template_img;
	ColorProfile color_profile;
	Ptr<BackgroundSubtractorMOG2> backgroud_substractor;
	vector<Finger> fingers;

	void process_not_initialized();
	void process_initialized();
	void extract_foreground();
};

