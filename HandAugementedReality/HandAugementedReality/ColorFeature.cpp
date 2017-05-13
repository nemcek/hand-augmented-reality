#include "ColorFeature.h"



ColorFeature::ColorFeature(Vec3b min_val, Vec3b max_val) : min_value(min_val), max_value(max_val)
{
	// Extract each color channel's proportion when proportion is 1 % of the channel
	min_value_portion = Vec3b(min_val[0] / 100.0, min_val[1] / 100.0, min_val[2] / 100.0);
	max_value_portion = Vec3b(max_val[0] / 100.0, max_val[1] / 100.0, max_val[2] / 100.0);
}

ColorFeature::~ColorFeature()
{
}
