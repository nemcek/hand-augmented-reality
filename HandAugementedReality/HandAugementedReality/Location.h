#pragma once

#include <opencv2\opencv.hpp>
#include <numeric>
#include "Utils.h"

using namespace std;
using namespace cv;

const int MAX_SIZE = 10;
const bool USE_WIGHTED_MEAN = true;

/*
 * Location of points
 * Using history
 * History implemented using queue
 */
class Location
{
public:
	Location();
	Location(const Point & p);
	void add(Point & p);
	const Point & get();
	const Point & last();

	~Location();

private:
	deque<Point> q;
	Point avg;
	Point last_loc;

	void calc_avg();
	void calc_weighted_mean();
	void calc_arithmetic_mean();
	void push(const Point & p);
	void pop();
};

