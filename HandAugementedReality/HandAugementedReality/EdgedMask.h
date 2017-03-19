#pragma once

#include <opencv2\opencv.hpp>

using namespace cv;
using namespace std;

class EdgedMask
{
public:
	Mat edges;

	EdgedMask();
	EdgedMask(const Mat &frame, const vector<Point> &points);
	virtual ~EdgedMask();

private:
	void create_edges(const Mat &frame, const vector<Point> &points);
};

