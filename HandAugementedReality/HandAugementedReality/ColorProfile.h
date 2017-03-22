#pragma once

#include <opencv2\opencv.hpp>
#include "InterestRegion.h"
#include "Utils.h"
#include <limits>
#include "ColorFeature.h"

using namespace cv;
using namespace std;

class ColorProfile
{
public:
	vector<ColorFeature> color_features;

	ColorProfile();
	ColorProfile(const vector<InterestRegion>& interesting_regions);
	~ColorProfile();

private:
	void extract_color_profile(const vector<InterestRegion>& interesting_regions);
};

