#pragma once

#include <opencv2\opencv.hpp>
#include "ColorProfile.h"
#include "EdgedMask.h"
#include "Hand.h"
#include "Finger.h"
#include "Gap.h"

using namespace cv;
using namespace std;

/*
 * Class for processing images
 */
class ImageProcessor
{
public:
	Mat result;
	bool is_initialized = false;

	ImageProcessor(const vector<Point>& points);
	~ImageProcessor();

	void process(const Mat& frame);
	void init();

private:
	int color_feature_radius = 7;
	const vector<Point>& roi_points;
	vector<InterestRegion> rois;
	Mat frame;
	ColorProfile color_profile;
	Ptr<BackgroundSubtractorMOG2> backgroud_substractor;
	vector<Finger> fingers;
	Location palm_loc;
	vector<Gap> gaps;
	bool gaps_initialized;
	Hand hand;

	void process_not_initialized();
	void process_initialized();
	void extract_foreground();
};

