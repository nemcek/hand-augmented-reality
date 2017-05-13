#include "ColorProfile.h"


ColorProfile::ColorProfile()
{
}

ColorProfile::ColorProfile(const vector<InterestRegion>& interesting_regions)
{
	extract_color_profile(interesting_regions);
}

/// Extracts color profile from picture
/// Positions to extract color from are deefined by regions of interest
void ColorProfile::extract_color_profile(const vector<InterestRegion>& interesting_regions)
{
	this->color_features = vector<ColorFeature>();

	for (const InterestRegion& region : interesting_regions) {
		vector<int> h_channel = vector<int>();
		vector<int> l_channel = vector<int>();
		vector<int> s_channel = vector<int>();
		int min_h = numeric_limits<int>::max();
		int min_l = numeric_limits<int>::max();
		int min_s = numeric_limits<int>::max();
		int max_h = 0;
		int max_l = 0;
		int max_s = 0;

		for (int y = 0; y < region.radius * 2; y++) {
			for (int x = 0; x < region.radius * 2; x++) {
				h_channel.push_back((int)region.matrix.at<Vec3b>(y, x)[0]);
				l_channel.push_back((int)region.matrix.at<Vec3b>(y, x)[1]);
				s_channel.push_back((int)region.matrix.at<Vec3b>(y, x)[2]);

				if ((int)region.matrix.at<Vec3b>(y, x)[0] < min_h)
					min_h = (int)region.matrix.at<Vec3b>(y, x)[0];

				if ((int)region.matrix.at<Vec3b>(y, x)[1] < min_l)
					min_l = (int)region.matrix.at<Vec3b>(y, x)[1];

				if ((int)region.matrix.at<Vec3b>(y, x)[2] < min_s)
					min_s = (int)region.matrix.at<Vec3b>(y, x)[2];

				if ((int)region.matrix.at<Vec3b>(y, x)[0] > max_h)
					max_h = (int)region.matrix.at<Vec3b>(y, x)[0];

				if ((int)region.matrix.at<Vec3b>(y, x)[1] > max_l)
					max_l = (int)region.matrix.at<Vec3b>(y, x)[1];

				if ((int)region.matrix.at<Vec3b>(y, x)[2] > max_s)
					max_s = (int)region.matrix.at<Vec3b>(y, x)[2];
			}
		}

		this->color_features.push_back(ColorFeature(Vec3b(min_h, min_l, min_s), Vec3b(max_h, max_l, max_s)));
	}
}

/// Adjusts color profile
/// Increments each channel value of color profile by it's proportion part
/// Used for auto adjusting when initializing by finding finger gaps
void ColorProfile::adjust()
{
	for (int i = 0; i < this->color_features.size(); i++) {
		for (int j = 0; j < 3; j++) {
			this->color_features[i].min_value[j] += this->color_features[i].min_value_portion[j];
			this->color_features[i].max_value[j] += this->color_features[i].max_value_portion[j];
		}
	}
}

ColorProfile::~ColorProfile()
{
}
