#include "EdgedMask.h"


EdgedMask::EdgedMask()
{
	return;
}

EdgedMask::EdgedMask(const Mat& frame, const vector<Point>& points)
{
	create_edges(frame, points);
}

void EdgedMask::create_edges(const Mat& frame, const vector<Point>& points)
{
	Mat mask(frame.size(), CV_8UC1);

	for (const Point& p : points) {
		Mat tmp;
		int thresh = (int)frame.at<uchar>(p.y, p.x);

		threshold(frame, tmp, thresh, 255, THRESH_BINARY);
		mask += tmp;
	}

	medianBlur((mask), (mask), 7);

	Mat structElem = getStructuringElement(MorphShapes::MORPH_RECT, Size(7, 7));
	dilate(mask, mask, structElem);
	erode(mask, mask, structElem);
	Canny(mask, mask, 80, 80 * 2, 3);

	this->edges = mask;
}

EdgedMask::~EdgedMask()
{
}
