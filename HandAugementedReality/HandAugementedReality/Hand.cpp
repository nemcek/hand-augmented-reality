#include "Hand.h"


Hand::Hand()
{
	this->gaps = vector<Gap>(4);
	this->gaps_initialized = false;
	this->palm = Location();
}

/// Processes the image
/// Finds contours from edges
/// Created convex hull and it's defect points
/// Inits or finds finger gaps
/// creates palm's center point
/// Called for every frame
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

	if (!this->gaps_initialized) {
		init_gaps(defects_points, color_profile);
	}
	else {
		find_gaps(defects_points);
	}

	create_palm(this->defects_points);
	draw();
}

/// Gets the largest contour from all contour
/// Size / length of contour is based on number of points in that contour (size())
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

/// Created bounding rect for vector of points
/// Used to create bounding rectange for convex hull points
void Hand::create_bounding_rect(const vector<Point>& points)
{
	this->bounding_rect = boundingRect(points);
}

/// Creates convex hull for contour
void Hand::create_convex_hull(const vector<Point>& contour)
{
	convexHull(Mat(contour), this->convex_hull_indices, false, false);

	for (int i = 0; i < convex_hull_indices.size(); i++) {
		this->convex_hull_points.push_back(contours[largest_contour_idx][this->convex_hull_indices[i]]);
	}

	approxPolyDP(this->convex_hull_points, this->convex_hull_points_approxied, 18, true);
}

/// Creates finget tip points
void Hand::create_finger_tips(const vector<Vec4i>& defects) {
	vector<Point> fingers = vector<Point>();

	for (int i = 0; i < defects.size(); i++) {
		fingers.push_back(this->contours[this->largest_contour_idx][defects[i][0]]);
		fingers.push_back(this->contours[this->largest_contour_idx][defects[i][1]]);
	}

	create_finger_tips(fingers);
}

/// Creates finget tip points
void Hand::create_finger_tips(const vector<Point>& points)
{
	for (int i = 0; i < points.size(); i++) {

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

/// Created convex hull defect points
void Hand::create_defects(const vector<int>& points_indices)
{
	if (this->contours.size() == 0)
		return;

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

	// Eliminate defects that have higher angle than 80� between their start and end point
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

		Point defect_point = this->contours[this->largest_contour_idx][defs[i][2]];
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

/// Computes center point of palm
void Hand::create_palm(const vector<Point>& defects_points)
{
	if (!this->gaps_initialized)
		return;

	// Direction vector from ring-pinky gap to index-middle gap
	Point2d direction_v = Point2d(gaps[GapPosition::RING_PINKY].location.get().x - gaps[GapPosition::INDEX_MIDDLE].location.get().x,
		gaps[GapPosition::RING_PINKY].location.get().y - gaps[GapPosition::INDEX_MIDDLE].location.get().y);

	// normilize direction vector
	double mag = sqrt(direction_v.x*direction_v.x + direction_v.y*direction_v.y);
	direction_v.x = direction_v.x / mag;
	direction_v.y = direction_v.y / mag;

	// rotate 90 degrees
	double temp = direction_v.x;
	direction_v.x = -direction_v.y;
	direction_v.y = temp;

	// Calculate point which lies on hypotethical line vertical to direction vector and crosses middle-ring gap
	Point vertical = Point(gaps[GapPosition::MIDDLE_RING].location.get().x + direction_v.x * 1000, gaps[GapPosition::MIDDLE_RING].location.get().y + direction_v.y * 1000);

	// Direction vector from ring-pinky gap to index-middle gap
	Point2d direction_p = Point2d(gaps[GapPosition::RING_PINKY].location.get().x - gaps[GapPosition::INDEX_MIDDLE].location.get().x,
		gaps[GapPosition::RING_PINKY].location.get().y - gaps[GapPosition::INDEX_MIDDLE].location.get().y);

	// normilize direction vector
	mag = sqrt(direction_p.x*direction_p.x + direction_p.y*direction_p.y);
	direction_p.x = direction_p.x / mag;
	direction_p.y = direction_p.y / mag;

	// Calculate point which lies on hypotethical line parallel to direction vector and crosses thumb-index gap
	Point parallel = Point(gaps[GapPosition::THUMB_INDEX].location.get().x + direction_p.x * 1000, gaps[GapPosition::THUMB_INDEX].location.get().y + direction_p.y * 1000);

	// get intersection of these two hypotethical lines
	Point2f intersect;
	Utils::intersection(gaps[GapPosition::MIDDLE_RING].location.get(), vertical, gaps[GapPosition::THUMB_INDEX].location.get(), parallel, intersect);

	// move palm point up the vertical line
	double dist = Utils::euclidean_distance(gaps[GapPosition::INDEX_MIDDLE].location.get(), gaps[GapPosition::RING_PINKY].location.get());
	intersect.x -= direction_v.x * (dist / 5.0);
	intersect.y -= direction_v.y * (dist / 5.0);
	this->palm.add((Point)intersect);

	/*line(this->result, gaps[GapPosition::MIDDLE_RING].location.get(), vertical, Scalar::all(255), 2, 2);
	line(this->result, gaps[GapPosition::THUMB_INDEX].location.get(), parallel, Scalar::all(255), 2, 2);*/
}

/// Initializes position of gaps
void Hand::init_gaps(const vector<Point>& defect_points, ColorProfile * color_profile)
{
	// if we fing less gap points adjust color profile
	if (defects_points.size() < 4) {
		color_profile->adjust();
		return;
	}

	// if there are more than 4 possible gaps do not try to initialize the position
	if (defects_points.size() > 4)
		return;

	vector<int> x_values = vector<int>();
	for (const Point & p : defect_points)
		x_values.push_back(p.x);

	// sort the gaps by x axis
	sort(x_values.begin(), x_values.end());

	// assume position of gaps based on x axis
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			if (defects_points[j].x == x_values[i]) {
				this->gaps[i] = Gap(defects_points[j], (GapPosition)i);
			}
		}
	}

	// Calculate direction vector from ring-pinky gap to index-middle gap
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

	// Calculate point which lies on hypotethical line vertical to direction vector and crosses middle-ring gap
	Point p = Point(gaps[GapPosition::MIDDLE_RING].location.get().x + v.x * 100, gaps[GapPosition::MIDDLE_RING].location.get().y + v.y * 100);

	// Calculate intersect point of hypotethical line and line between ring0pinky gap and index-middle gap
	Point2f intersect;
	Utils::intersection(gaps[GapPosition::RING_PINKY].location.get(), gaps[GapPosition::INDEX_MIDDLE].location.get(), gaps[GapPosition::MIDDLE_RING].location.get(), p, intersect);

	// Calculate distance from midle-ring gap to line between ring-pinky and index-middle gaps
	double dist = Utils::euclidean_distance(intersect, gaps[GapPosition::MIDDLE_RING].location.get());

	// Calculate angle between points thumb-index gap, index-middle gap and ring-pinky gap
	double angle = Utils::angle(gaps[GapPosition::THUMB_INDEX].location.get(), gaps[GapPosition::INDEX_MIDDLE].location.get(), gaps[GapPosition::RING_PINKY].location.get());

	// if points are correct gap positions
	if (dist < 15.0 && angle >= 100.0 && angle <= 140.0) {
		this->gaps_3d = vector<Point3d>(this->gaps.size());
		for (Gap & gap : this->gaps) {
			// convert coordines to system where [0, 0] is in the middle of the image - it's position of the hypotethical camera
			double x = gap.location.get().x > 320.0f ? gap.location.get().x - 320.0f : -(320.0f - gap.location.get().x);
			double y = gap.location.get().y > 240.0f ? gap.location.get().y - 240.0f : -(240.0f - gap.location.get().y);
			gaps_3d[gap.position] = Point3d(x, y, 0.0f);
		}

		// add z axis coordinates for gaps
		gaps_3d[GapPosition::MIDDLE_RING].z = 0.0f;
		gaps_3d[GapPosition::INDEX_MIDDLE].z = 20.0f;
		gaps_3d[GapPosition::RING_PINKY].z = 30.0f;
		gaps_3d[GapPosition::THUMB_INDEX].z = 50.0f;

		this->gaps_initialized = true;
	}
}

/// Finds current gaps location based on their previous location
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

			// Calculate distance of last current location of gap to possible new location of gap
			double dist = Utils::euclidean_distance(gaps[i].location.last(), defects_points[j]);
			if (dist < min_dist) {
				min_dist = dist;
				defect_idx = j;
			}
		}

		// if gap's new position is found update position
		if (defect_idx != -1 && min_dist <= 30.0) {
			gaps[i].location.add(defects_points[defect_idx]);
			assigned_defects_indices.push_back(defect_idx);
		}
	}
}

Location palm_endpoint = Location();
void Hand::draw()
{
	if (!this->gaps_initialized) {
		putText(this->result, string("Initializing..."), Point(50, 50), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 255), 2, 2);
		// defect points
		for (int i = 0; i < this->defects_points.size(); i++)
			circle(this->result, this->defects_points[i], 8, Scalar(255, 0, 0), 5);
	}
	else {
		circle(this->result, this->palm.get(), 8, Scalar(255, 0, 0), 2, 2);

		// Calculate normal vector from center of palm as P'n'P problem
		// solution based on head pose estimation from http://www.learnopencv.com/head-pose-estimation-using-opencv-and-dlib/

		// Camera information
		double focal_length = this->result.cols;
		Point2d center = cv::Point2d(this->result.cols / 2, this->result.rows / 2);
		cv::Mat camera_matrix = (cv::Mat_<double>(3, 3) << focal_length, 0, center.x, 0, focal_length, center.y, 0, 0, 1);
		cv::Mat dist_coeffs = cv::Mat::zeros(4, 1, cv::DataType<double>::type); // Assuming no lens distortion
		
		// Rotation and translation matrix
		cv::Mat rotation_vector;
		cv::Mat translation_vector;
		
		// Get 2D coordinates of gaps
		vector<Point2d> gaps_points = vector<Point2d>(this->gaps.size());
		for (Gap & gap : this->gaps) {
			gaps_points[gap.position] = Point2d((float)gap.location.get().x, (float)gap.location.get().y);
		}
		
		// Solve for pose using gaps 2D and 3D coordinates with hypotethical camera
		cv::solvePnP(gaps_3d, gaps_points, camera_matrix, dist_coeffs, rotation_vector, translation_vector, false, CV_EPNP);
		
		// Project a 3D point (0, 0, 100.0) used to draw the normal vector
		vector<Point3d> palm_end_point3D;
		vector<Point2d> palm_end_point2D;
		palm_end_point3D.push_back(Point3d(0, 0, 100.0));
		projectPoints(palm_end_point3D, rotation_vector, translation_vector, camera_matrix, dist_coeffs, palm_end_point2D);

		palm_endpoint.add((Point)palm_end_point2D[0]);
		cv::line(this->result, this->palm.get(), palm_endpoint.get(), cv::Scalar(255, 0, 0), 6);
	}
}

Hand::~Hand()
{
}
