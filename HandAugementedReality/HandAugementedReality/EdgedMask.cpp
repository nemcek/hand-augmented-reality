#include "EdgedMask.h"


EdgedMask::EdgedMask()
{
	return;
}

EdgedMask::EdgedMask(const Mat& frame, const ColorProfile& color_profile)
{
	create_edges(frame, color_profile);
}

void EdgedMask::create_edges(const Mat& frame, const ColorProfile& color_profile)
{
	Mat mask = Mat::ones(frame.size(), CV_8UC1);
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

		int h = 0;
		inRange(frame, Scalar((int)color_feature.min_value[0], (int)color_feature.min_value[1], (int)color_feature.min_value[2]), 
			Scalar((int)color_feature.max_value[0] + h, (int)color_feature.max_value[1] + h, (int)color_feature.max_value[2] +h), tmp);
		mask += tmp;
	}

	medianBlur(mask, mask, 7);

	Mat structElem = getStructuringElement(MorphShapes::MORPH_RECT, Size(7, 7));
	dilate(mask, mask, structElem);
	erode(mask, mask, structElem);
	imshow("threshold", mask);
	Canny(mask, mask, 80, 80 * 2, 3);

	this->edges = mask;
}

EdgedMask::~EdgedMask()
{
}
