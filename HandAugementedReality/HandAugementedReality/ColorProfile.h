#pragma once

#include <opencv2\opencv.hpp>
#include "InterestRegion.h"
#include "Utils.h"
#include <limits>
#include "ColorFeature.h"

using namespace cv;
using namespace std;

/*
 * Color profile of image
 * Consists of multiple color features extracted by region of interest
 */
class ColorProfile
{
public:
	vector<ColorFeature> color_features;

	ColorProfile();
	ColorProfile(const vector<InterestRegion>& interesting_regions);
	~ColorProfile();
	void adjust();

private:
	void extract_color_profile(const vector<InterestRegion>& interesting_regions);
};

