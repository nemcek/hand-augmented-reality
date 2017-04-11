#pragma once

#include <opencv2\opencv.hpp>
#include <numeric>
#include "Utils.h"

using namespace std;
using namespace cv;

class Location
{
public:
	Location();
	void add(Point & p);

	~Location();

private:
	deque<Point> q;
	Point avg;
	const int MAX_SIZE = 10;

	void calc_avg();
	void push(Point & p);
	void pop();
	Point & get();
};

