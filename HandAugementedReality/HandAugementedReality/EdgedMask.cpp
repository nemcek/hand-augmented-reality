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
	Mat mask(frame.size(), CV_8UC1);

	for (int color_value : color_profile.color_values) {
		Mat tmp;
		
		threshold(frame, tmp, color_value, 255, THRESH_BINARY);
		mask += tmp;
	}

	medianBlur((mask), (mask), 7);

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
