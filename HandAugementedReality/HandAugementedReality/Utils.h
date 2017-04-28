#pragma once

#define _USE_MATH_DEFINES

#include <opencv2\opencv.hpp>
#include <math.h>

using namespace cv;
using namespace std;

class Utils
{
public:
	static double euclidean_distance(Point a, Point b);
	static int median(vector<int> vals);
	static int lowest(vector<int> vals);
	static double angle(Point a, Point b, Point c);
	static bool contains(Point upper, Point lower, Point x, int tolerance);
	static bool intersection(const Point2f & o1, const Point2f & p1, const Point2f & o2, const Point2f & p2, Point2f &r);
};

