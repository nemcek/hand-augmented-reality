#include "Utils.h"

double Utils::euclidean_distance(Point a, Point b)
{
	return sqrt(pow((a.x - b.x), 2) + pow((a.y - b.y), 2));
}
