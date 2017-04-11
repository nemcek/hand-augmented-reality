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
};

