#pragma once

#include <opencv2\opencv.hpp>
#include "EdgedMask.h"
#include "Utils.h"
#include "Location.h"
#include "Gap.h"

using namespace cv;
using namespace std;

class Hand
{
public:
	EdgedMask edged_mask;
	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;
	vector<int> convex_hull_indices;
	vector<Point> convex_hull_points;
	vector<Point> convex_hull_points_approxied;
	vector<Point> finger_tips;
	vector<Vec4i> defects;
	vector<Vec4i> defects_filtered;
	vector<Point> defects_points;
	vector<Gap> gaps;
	Rect bounding_rect;
	RotatedRect palm;
	Location palm_loc;
	int largest_contour_idx;
	Mat result;

	Hand();
	~Hand();
	void process(const EdgedMask & edged_mask, ColorProfile * color_profile);

private:
	bool gaps_initialized;

	int get_largest_contour_idx(const vector<vector<Point>>& contours);
	void create_bounding_rect(const vector<Point>& points);
	void create_convex_hull(const vector<Point>& contour);
	void create_finger_tips(const vector<Point>& points);
	void create_finger_tips(const vector<Vec4i>& defects);
	void create_defects(const vector<int>& points_indices);
	void create_defects_points(const vector<Vec4i>& defects);
	void create_palm(const vector<Point>& defects_points);
	void draw();
	void find_gaps(const vector<Point> & defect_points);
	void init_gaps(const vector<Point> & defect_points, ColorProfile * color_profile);
};

