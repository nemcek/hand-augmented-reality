#include "Utils.h"

/// Calculates euclidean distance between two points
double Utils::euclidean_distance(Point a, Point b)
{
	return sqrt(pow((a.x - b.x), 2) + pow((a.y - b.y), 2));
}

/// Calculates median of vector of integers
int Utils::median(vector<int> vals)
{
	size_t size = vals.size();

	sort(vals.begin(), vals.end());

	if (size % 2 == 1)
		return vals[size / 2];
	else
		return vals[size / 2 - 1];
}

/// Gets the lowes integer value of unsorted vector of integers
int Utils::lowest(vector<int> vals)
{
	sort(vals.begin(), vals.end());

	return vals[0];
}

/// Calculate angle between two points
/// Args:
/// a - starting point
/// b - middle point where angle is calculated
/// c - end point
double Utils::angle(Point a, Point b, Point c)
{
	float dist1 = euclidean_distance(b, a);
	float dist2 = euclidean_distance(b, c);
	float dot_prod = (a.x - b.x)*(c.x - b.x) + (a.y - b.y)*(c.y - b.y);
	float angle = acos(dot_prod / (dist1 * dist2));

	angle = angle * 180 / M_PI;

	return angle;
}

/// Checks whether point is inside rectangle defined by two points with some tolerance
/// Args:
/// upper - upper corner point of rectangle
/// lower - lower corner point of rectangle
/// x - point to determine if is contained inside the rectangle
/// tolerance - tolerance for distance outside of rectangle
bool Utils::contains(Point upper, Point lower, Point x, int tolerance)
{
	return (x.x >= (upper.x - tolerance) && x.x <= (lower.x + tolerance)
		&& x.y >= (upper.y - tolerance) && x.y <= (upper.y + tolerance));
}

/// Finds the intersection of two lines
/// Returns false if two line do not intersect each other
bool Utils::intersection(const Point2f & o1, const Point2f & p1, const Point2f & o2, const Point2f & p2, Point2f &r)
{
	Point2f x = o2 - o1;
	Point2f d1 = p1 - o1;
	Point2f d2 = p2 - o2;
	
	float cross = d1.x*d2.y - d1.y*d2.x;
	if (abs(cross) < /*EPS*/1e-8)
		return false;

	double t1 = (x.x * d2.y - x.y * d2.x) / cross;
	r = o1 + d1 * t1;
	return true;
}