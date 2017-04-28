#include "Hand.h"


// Image processor musi obsahovat iba jednu instanciu tejto triedy
// Trieda si musi udrziavat stavy medzi spracovanymi snimkami
// Na zaciatku pokus o inicializaciu - najdenie gaps
// Ak sa nenajdu, bude sa postupne adjustovat color profile - napr. +1 k hornej hranici a -1 k dolej hranici
// Ak sa najdu presne styri - overenie ci ide o gaps prstov (uz predtym bude vykonana kontrola na uhol)
// Medzi druhym (ukazov-prostrednik) a stvrtym (prstenik-malicek) bodmi gaps sa prelozi priamka, zisti sa
// ci bod tretej gap (prostrednik-prsetnik) je v blizkosti
// AK ano, vieme, ze tieto tri body by mali zodpovedat trom gaps medzi styrmi prstami
// Ako overit, ci sme nasli spravnu gap pri palci?
// Prelozit priamku cez gap pri palci a gap pri ukazovaku a s prelozenou priamkou medzi hornymi gaps
// musi zvierat ± nejaky uhol
// 
// Tuto kontrolu vykonavat aj po inicializacii na jednoznacne identifikovanie gaps (uz bez adjsutu color profile)
// Ak su podmienky splnene, viem ze som nasiel spravne gaps a podla blizkosti nejakeho zvoleneho okolia zistim, ktory gap to je
// Ak nie su splnene, hladam iba na zaklade vzdialenosti vo zvolenom okoli
//
// Identifikovanie stredu dlane:
// - Prelozit priamku medzi gap pri ukazovaku a gap pri prsteniku
// - Prelozit rovnobeznu priamku s touto priamkou cez gap pri palci
// - Prelozit pravouhlu priamku na prvu priamku cez gap pri prostredniku
// - Pretnutie priamok = stred dlane
Hand::Hand()
{
	this->gaps = vector<Gap>(4);
	this->gaps_initialized = false;
}

void Hand::process(const EdgedMask & edged_mask, ColorProfile * color_profile)
{
	this->result = Mat::zeros(edged_mask.edges.size(), CV_8UC3);
	this->defects_points = vector<Point>();

	this->edged_mask = edged_mask;
	findContours(edged_mask.edges, this->contours, this->hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
	this->largest_contour_idx = get_largest_contour_idx(this->contours);

	create_convex_hull(this->contours[this->largest_contour_idx]);
	create_bounding_rect(this->convex_hull_points);
	create_defects(this->convex_hull_indices);
	//create_finger_tips(this->convex_hull_points_approxied);
	//create_finger_tips(this->defects);
	//create_palm(this->defects_points);

	if (!this->gaps_initialized) {
		init_gaps(defects_points, color_profile);
	}
	else {
		find_gaps(defects_points);
	}

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
		//if (points[i].y <= (this->bounding_rect.y + this->bounding_rect.height)
		//	&& points[i].y >= (this->bounding_rect.y + this->bounding_rect.height - this->bounding_rect.height / 4))
		//	continue;

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
	this->defects_filtered = vector<Vec4i>();

	for (int i = 0; i < defects.size(); i++) {
		Point start = this->contours[this->largest_contour_idx][defects[i][0]];
		Point end = this->contours[this->largest_contour_idx][defects[i][1]];
		Point farthest = this->contours[this->largest_contour_idx][defects[i][2]];
		double angle = Utils::angle(start, farthest, end);
		if (angle > 0 && angle < 80.0) {
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
			this->defects_filtered.push_back(defs[i]);
		}
	}
}

void Hand::create_palm(const vector<Point>& defects_points)
{
	/// TODO
}

void Hand::init_gaps(const vector<Point>& defect_points, ColorProfile * color_profile)
{
	if (defects_points.size() < 4) {
		color_profile->adjust();
		return;
	}

	if (defects_points.size() > 4)
		return;

	vector<int> x_values = vector<int>();
	for (const Point & p : defect_points)
		x_values.push_back(p.x);

	sort(x_values.begin(), x_values.end());

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			if (defects_points[j].x == x_values[i]) {
				this->gaps[i] = Gap(defects_points[j], (GapPosition)i);
			}
		}
	}

	Point2d v = Point2d(gaps[GapPosition::RING_PINKY].location.get().x - gaps[GapPosition::INDEX_MIDDLE].location.get().x,
		gaps[GapPosition::RING_PINKY].location.get().y - gaps[GapPosition::INDEX_MIDDLE].location.get().y);

	// normilize direction vector
	double mag = sqrt(v.x*v.x + v.y*v.y);
	v.x = v.x / mag;
	v.y = v.y / mag;
	
	// rotate 90 degrees
	double temp = v.x;
	v.x = -v.y;
	v.y = temp;

	Point p = Point(gaps[GapPosition::MIDDLE_RING].location.get().x + v.x * 100, gaps[GapPosition::MIDDLE_RING].location.get().y + v.y * 100);
	line(this->result, gaps[GapPosition::MIDDLE_RING].location.get(), p, Scalar::all(255), 2, 2);

	Point2f intersect;
	Utils::intersection(gaps[GapPosition::RING_PINKY].location.get(), gaps[GapPosition::INDEX_MIDDLE].location.get(), gaps[GapPosition::MIDDLE_RING].location.get(), p, intersect);

	double dist = Utils::euclidean_distance(intersect, gaps[GapPosition::MIDDLE_RING].location.get());
	double angle = Utils::angle(gaps[GapPosition::THUMB_INDEX].location.get(), gaps[GapPosition::INDEX_MIDDLE].location.get(), gaps[GapPosition::RING_PINKY].location.get());

	if (dist < 15.0 && angle >= 100.0 && angle <= 140.0) {
		this->gaps_initialized = true;
	}
}

void Hand::find_gaps(const vector<Point>& defect_points)
{
	vector<int> assigned_defects_indices = vector<int>();
	for (int i = 0; i < gaps.size(); i++) {
		double min_dist = std::numeric_limits<double>::max();
		int defect_idx = -1;

		for (int j = 0; j < defects_points.size(); j++) {
			bool is_assigned = false;
			for (int k = 0; k < assigned_defects_indices.size(); k++) {
				if (assigned_defects_indices[k] == j) {
					is_assigned = true;
					break;
				}
			}

			if (is_assigned)
				continue;

			double dist = Utils::euclidean_distance(gaps[i].location.last(), defects_points[j]);
			if (dist < min_dist) {
				min_dist = dist;
				defect_idx = j;
			}
		}

		if (defect_idx != -1 && min_dist <= 30.0) {
			gaps[i].location.add(defects_points[defect_idx]);
			assigned_defects_indices.push_back(defect_idx);
		}
	}

}

void Hand::draw()
{
	// contours
	drawContours(this->result, contours, this->largest_contour_idx, Scalar(0, 0, 255), 2, 2, hierarchy);

	vector<vector<Point>> tmp_convex_hull;
	tmp_convex_hull.push_back(this->convex_hull_points_approxied);	// draw coontours expects two-dimensional array

	if (!this->gaps_initialized) {
		putText(this->result, string("Initializing..."), Point(100, 100), FONT_HERSHEY_SIMPLEX, 1, Scalar::all(255), 2, 2);
		// defect points
		for (int i = 0; i < this->defects_points.size(); i++)
			circle(this->result, this->defects_points[i], 8, Scalar(255, 0, 0), 5);
	}
	else {
		for (Gap & gap : this->gaps) {
			circle(this->result, gap.location.get(), 8, Scalar(255, 0, 0), 5);
			stringstream ss;
			ss << gap.position;
			putText(this->result, ss.str(), gap.location.get(), FONT_HERSHEY_SIMPLEX, 1, Scalar::all(255), 2, 2);
		}
	}
}

Hand::~Hand()
{
}
