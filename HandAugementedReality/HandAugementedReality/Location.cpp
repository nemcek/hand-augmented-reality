#include "Location.h"



Location::Location()
{
	this->q = deque<Point>();
	this->avg = Point(0.0, 0.0);
}

void Location::add(Point & p)
{
	if (Utils::euclidean_distance(this->avg, p) > 50)
		return;

	if (q.size() == MAX_SIZE)
		pop();

	push(p);
}

void Location::calc_avg()
{
	Point sum = std::accumulate(q.begin(), q.end(), Point(0.0, 0.0));
	this->avg = Point(sum.x / q.size(), sum.y / q.size());
}

void Location::push(Point & p)
{
	this->q.push_back(p);
	calc_avg();
}

void Location::pop()
{
	this->q.pop_front();
}

Point & Location::get()
{
	return this->avg;
}

Location::~Location()
{
}