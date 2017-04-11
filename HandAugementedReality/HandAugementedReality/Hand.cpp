#include "Hand.h"



Hand::Hand(const EdgedMask & edged_mask)
{
	this->edged_mask = edged_mask;
	findContours(edged_mask.edges, this->contours, this->hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
	this->largest_contour_idx = get_largest_contour_idx(this->contours);

	create_convex_hull(this->contours[this->largest_contour_idx]);
	create_bounding_rect(this->convex_hull_points);
	create_defects(this->convex_hull_indices);
	//create_finger_tips(this->convex_hull_points_approxied);
	create_finger_tips(this->defects);
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

void Hand::create_finger_tips(const vector<Vec4i>& defects) {
	vector<Point> fingers = vector<Point>();

	for (int i = 0; i < defects.size(); i++) {
		// add also first start point
		//if (i == 0) {
			// start index
			fingers.push_back(this->contours[this->largest_contour_idx][defects[i][0]]);
		//}
		// end index
		fingers.push_back(this->contours[this->largest_contour_idx][defects[i][1]]);
	}

	create_finger_tips(fingers);
}

void Hand::create_finger_tips(const vector<Point>& points)
{
	for (int i = 0; i < points.size(); i++) {
		// eliminate "finger points" that lay on the bottom of the bounding rect
		if (points[i].y <= (this->bounding_rect.y + this->bounding_rect.height)
			&& points[i].y >= (this->bounding_rect.y + this->bounding_rect.height - this->bounding_rect.height / 4))
			continue;

		bool has_near_neighbour = false;
		for (int j = 0; j < this->finger_tips.size(); j++) {
			Point a = points[i];
			Point b = this->finger_tips[j];
			if (Utils::euclidean_distance(a, b) < 20) {
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
	vector<Vec4i> defs = vector<Vec4i>();

	for (int i = 0; i < defects.size(); i++) {
		Point start = this->contours[this->largest_contour_idx][defects[i][0]];
		Point end = this->contours[this->largest_contour_idx][defects[i][1]];
		Point farthest = this->contours[this->largest_contour_idx][defects[i][2]];
		double angle = Utils::angle(start, farthest, end);
		if (angle > 0 && angle < 80.0) {
			//cout << "angle: " << angle << endl;
			defs.push_back(defects[i]);
		}
	}

	this->defects = defs;

	for (int i = 0; i < defs.size(); i++) {
		// eliminate defects that are too close to convex hull
		if (defs[i][3] / 256.0 <= 30)
			continue;

		bool is_near_to_finger_tip = false;
		Point defect_point = this->contours[this->largest_contour_idx][defs[i][2]];
		// eliminate defects that are too close to any finger tip
		/*for (int j = 0; j < this->finger_tips.size(); j++) {
			Point b = this->finger_tips[j];
			if (Utils::euclidean_distance(defect_point, b) < 10) {
				is_near_to_finger_tip = true;
				break;
			}
		}

		if (is_near_to_finger_tip)
			continue;*/

		bool has_near_neighbour = false;
		for (int j = 0; j < this->defects_points.size(); j++) {
			Point b = defects_points[j];
			double dist = Utils::euclidean_distance(defect_point, b);
			if (dist < 10) {
				has_near_neighbour = true;
				break;
			}
		}

		if (!has_near_neighbour) {
			this->defects_points.push_back(contours[largest_contour_idx][defs[i][2]]);
		}
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

	vector<vector<Point>> tmp_convex_hull;
	tmp_convex_hull.push_back(this->convex_hull_points_approxied);	// draw coontours expects two-dimensional array
	drawContours(this->result, tmp_convex_hull, 0, Scalar(0, 255, 0), 2, 2);

	// finger tips
	for (int i = 0; i < this->finger_tips.size(); i++) {
		circle(this->result, this->finger_tips[i], 8, Scalar(0, 0, 255), 5);
		stringstream ss;
		ss << i;

		putText(this->result, ss.str(), this->finger_tips[i], FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 0, 0), 2, 2);
	}

	// defect points
	for (int i = 0; i < this->defects_points.size(); i++)
		circle(this->result, this->defects_points[i], 8, Scalar(255, 0, 0), 5);
}

Hand::~Hand()
{
}
