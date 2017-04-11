#pragma once

#include <opencv2\opencv.hpp>
#include <numeric>
#include "Utils.h"

using namespace std;
using namespace cv;

const int MAX_SIZE = 10;

class Location
{
public:
	Location();
	Location(const Point & p);
	void add(Point & p);
	const Point & get();

	~Location();

private:
	deque<Point> q;
	Point avg;

	void calc_avg();
	void push(const Point & p);
	void pop();
};

