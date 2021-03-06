#include "EdgedMask.h"


EdgedMask::EdgedMask()
{
	return;
}

EdgedMask::EdgedMask(const Mat& frame, const ColorProfile& color_profile)
{
	create_edges(frame, color_profile);
}

/// Creates binary mask for eaach color feature of image's color profile
/// Combines binary mask to result binary mask
/// Filters noise by median blur and morphological operations on result binary mask
/// Applies canny edge detector on result mask to obtain edges
void EdgedMask::create_edges(const Mat& frame, const ColorProfile& color_profile)
{
	mask = Mat::ones(frame.size(), CV_8UC1);
	vector<std::string> names = vector<std::string>();
	int i = 0;
	names.push_back(std::string("thresh 1"));
	names.push_back(std::string("thresh 2"));
	names.push_back(std::string("thresh 3"));
	names.push_back(std::string("thresh 4"));
	names.push_back(std::string("thresh 5"));
	names.push_back(std::string("thresh 6"));
	names.push_back(std::string("thresh 7"));
	names.push_back(std::string("thresh 8"));
	names.push_back(std::string("thresh 9"));
	names.push_back(std::string("thresh 10"));

	for (const ColorFeature& color_feature : color_profile.color_features) {
		Mat tmp;

		// Do not do threshold for value 0
		int h = (int)color_feature.min_value[0] == 0 ? 10 : (int)color_feature.min_value[0];
		int s = (int)color_feature.min_value[1] == 0 ? 10 : (int)color_feature.min_value[1];
		int l = (int)color_feature.min_value[2] == 0 ? 10 : (int)color_feature.min_value[2];

		// Threshold for 3 channels
		inRange(frame, Scalar(h, s, l),
			Scalar((int)color_feature.max_value[0] + h, (int)color_feature.max_value[1] + h, (int)color_feature.max_value[2] +h), tmp);

		// Combine binary masks
		mask += tmp;
		//imshow(names[i++], tmp);
	}

	// Filter noise
	medianBlur(mask, mask, 7);

	Mat structElem = getStructuringElement(MorphShapes::MORPH_RECT, Size(11, 11));
	dilate(mask, mask, structElem);
	erode(mask, mask, structElem);
	imshow("threshold", mask);
	Canny(mask, this->edges, 80, 80 * 2, 3);
}

EdgedMask::~EdgedMask()
{
}
