#pragma once

#include <opencv2\opencv.hpp>

using namespace cv;
using namespace std;

class Utils
{
public:
	static double euclidean_distance(Point a, Point b);
	static int median(vector<int> vals);
	static int lowest(vector<int> vals);
};

