#include "Utils.h"

double Utils::euclidean_distance(Point a, Point b)
{
	return sqrt(pow((a.x - b.x), 2) + pow((a.y - b.y), 2));
}

int Utils::median(vector<int> vals)
{
	size_t size = vals.size();

	sort(vals.begin(), vals.end());

	if (size % 2 == 1)
		return vals[size / 2];
	else
		return vals[size / 2 - 1];
}

int Utils::lowest(vector<int> vals)
{
	sort(vals.begin(), vals.end());

	return vals[0];
}

double Utils::angle(Point a, Point b, Point c)
{
	float dist1 = euclidean_distance(b, a);
	float dist2 = euclidean_distance(b, c);
	float dot_prod = (a.x - b.x)*(c.x - b.x) + (a.y - b.y)*(c.y - b.y);
	float angle = acos(dot_prod / (dist1 * dist2));

	angle = angle * 180 / M_PI;

	return angle;
}