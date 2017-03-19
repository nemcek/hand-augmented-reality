#include "Hand.h"


Hand::Hand(const EdgedMask & edged_mask)
{
	this->edged_mask = edged_mask;
	findContours(edged_mask.edges, this->contours, this->hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
	this->largest_contour_idx = get_largest_contour_idx(this->contours);

	create_convex_hull(this->contours[this->largest_contour_idx]);
	create_bounding_rect(this->convex_hull_points);
	create_finger_tips(this->convex_hull_points_approxied);
	create_defects(this->convex_hull_indices);
	create_palm(this->defects_points);
	draw();
}

int Hand::get_largest_contour_idx(const vector<vector<Point>>& contours)
{
	size_t largest_countour_size = contours[0].size();
	int largest_countour_index = 0;

	for (int i = 0; i < contours.size(); i++) {
		if (contours[i].size() > largest_countour_size) {
			largest_countour_size = contours[i].size();
			largest_countour_index = i;
		}
	}

	return largest_countour_index;
}

void Hand::create_bounding_rect(const vector<Point>& points)
{
	this->bounding_rect = boundingRect(points);
}

void Hand::create_convex_hull(const vector<Point>& contour)
{
	convexHull(Mat(contour), this->convex_hull_indices, false, false);

	for (int i = 0; i < convex_hull_indices.size(); i++) {
		this->convex_hull_points.push_back(contours[largest_contour_idx][this->convex_hull_indices[i]]);
	}

	approxPolyDP(this->convex_hull_points, this->convex_hull_points_approxied, 18, true);
}

void Hand::create_finger_tips(const vector<Point>& points)
{
	for (int i = 0; i < points.size(); i++) {
		// eliminate "finger points" that lay on the bottom of the bounding rect
		if (points[i].y >= (this->bounding_rect.y + this->bounding_rect.height - 3)
			&& points[i].y <= (this->bounding_rect.y + this->bounding_rect.height + 3))
			continue;

		bool has_near_neighbour = false;
		for (int j = 0; j < this->finger_tips.size(); j++) {
			Point a = points[i];
			Point b = this->finger_tips[j];
			if (Utils::euclidean_distance(a, b) < 10) {
				has_near_neighbour = true;
				break;
			}
		}

		if (!has_near_neighbour)
			this->finger_tips.push_back(points[i]);
	}
}

void Hand::create_defects(const vector<int>& points_indices)
{
	vector<int> tmp(points_indices);
	// !!!! NEEDED !!!!
	// for calculation of convexity defects
	std::sort(tmp.begin(), tmp.end());

	vector<Vec4i> defects;
	convexityDefects(this->contours[this->largest_contour_idx], tmp, this->defects);
	create_defects_points(this->defects);
}

/// Eliminates defects points that are too close to each other
void Hand::create_defects_points(const vector<Vec4i>& defects)
{
	for (int i = 0; i < defects.size(); i++) {
		// eliminate defects that are too close to convex hull
		if (defects[i][3] / 256.0 <= 30)
			continue;

		bool has_near_neighbour = false;
		for (int j = 0; j < this->defects_points.size(); j++) {
			Point a = this->contours[this->largest_contour_idx][defects[i][2]];
			Point b = defects_points[j];
			double dist = Utils::euclidean_distance(a, b);
			if (dist < 10) {
				has_near_neighbour = true;
				break;
			}
		}

		if (!has_near_neighbour)
			this->defects_points.push_back(contours[largest_contour_idx][defects[i][2]]);
	}
}

void Hand::create_palm(const vector<Point>& defects_points)
{
	if (defects_points.size() > 4) {	// elipse needs at least 4 points
		this->palm = fitEllipse(defects_points);
	}
}

void Hand::draw()
{
	this->result = Mat::zeros(edged_mask.edges.size(), CV_8UC3);

	// contours
	drawContours(this->result, contours, this->largest_contour_idx, Scalar(0, 0, 255), 2, 2, hierarchy);

	// bounding rectangle
	rectangle(this->result, this->bounding_rect, Scalar(255, 198, 75), 3);

	vector<vector<Point>> tmp_convex_hull;
	tmp_convex_hull.push_back(this->convex_hull_points_approxied);	// draw coontours expects two-dimensional array
	drawContours(this->result, tmp_convex_hull, 0, Scalar(0, 255, 0), 2, 2);

	// finger tips
	for (int i = 0; i < this->finger_tips.size(); i++)
		circle(this->result, this->finger_tips[i], 8, Scalar(0, 0, 255), 5);

	// defect points
	for (int i = 0; i < this->defects_points.size(); i++)
		circle(this->result, this->contours[this->largest_contour_idx][this->defects[i][2]], 8, Scalar(0, 255, 255), 5);

	// palm
	ellipse(this->result, this->palm, Scalar(128, 0, 128), 4, 2);
	circle(this->result, this->palm.center, 4, Scalar(128, 0, 128), 4, 2);
}

Hand::~Hand()
{
}
