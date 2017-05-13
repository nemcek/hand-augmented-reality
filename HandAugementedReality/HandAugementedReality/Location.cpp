#include "Location.h"



Location::Location()
{
	this->q = deque<Point>();
	this->avg = Point(0.0, 0.0);
	this->last_loc = this->avg;
}

Location::Location(const Point & p)
{
	this->q = deque<Point>();
	push(p);
}

// Add new location
void Location::add(Point & p)
{
	// do not add location which is too far away
	if (Utils::euclidean_distance(this->avg, p) > 50 && this->q.size() > 1) {
		return;
	}

	// if queue is full pop
	if (q.size() == MAX_SIZE)
		pop();

	push(p);
}

// Calculates average location
void Location::calc_avg()
{
	if (USE_WIGHTED_MEAN)
		calc_weighted_mean();
	else
		calc_arithmetic_mean();
}

// Calculates weighted mean
void Location::calc_weighted_mean()
{
	// newest location has heighest weight (4x) others have the same (1x)
	Point sum = std::accumulate(q.begin(), q.end(), Point(0.0, 0.0));
	sum += (q[q.size() - 1] * 3);

	this->avg = Point(sum.x / (q.size() + 3), sum.y / (q.size() + 3));
}

// Calculates arithemetic mean
void Location::calc_arithmetic_mean()
{
	Point sum = std::accumulate(q.begin(), q.end(), Point(0.0, 0.0));
	this->avg = Point(sum.x / q.size(), sum.y / q.size());
}

void Location::push(const Point & p)
{
	this->q.push_back(p);
	this->last_loc = p;
	calc_avg();
}

void Location::pop()
{
	this->q.pop_front();
}

const Point & Location::get()
{
	return this->avg;
}

const Point & Location::last()
{
	return this->last_loc;
}

Location::~Location()
{
}