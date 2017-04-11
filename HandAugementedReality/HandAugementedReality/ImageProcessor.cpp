#include "ImageProcessor.h"


ImageProcessor::ImageProcessor(const Mat& template_img, const vector<Point>& points) : template_img(template_img), roi_points(points)
{
	this->backgroud_substractor = createBackgroundSubtractorMOG2(500, 16, false);
	this->fingers = vector<Finger>(5);
}

void ImageProcessor::process(const Mat & frame)
{
	this->result = frame.clone();
	flip(this->result, this->result, 1);
	this->frame = this->result.clone();

	extract_foreground();

	// convert to HSL color space
	cvtColor(this->frame, this->frame, CV_RGB2HLS);

	if (this->is_initialized)
		process_initialized();
	else
		process_not_initialized();
}

void ImageProcessor::init()
{
	for (const Point& p : this->roi_points) {
		rois.push_back(InterestRegion(this->frame, p, this->color_feature_radius));
	}

	this->color_profile = ColorProfile(rois);
	this->fingers[FingerType::THUMB] = Finger(Point2d(165.0f, 250.0f), FingerType::THUMB);
	this->fingers[FingerType::INDEX] = Finger(Point2d(302.0f, 80.0f), FingerType::INDEX);
	this->fingers[FingerType::MIDDLE] = Finger(Point2d(356.0f, 57.0f), FingerType::MIDDLE);
	this->fingers[FingerType::RING] = Finger(Point2d(409.0f, 76.0f), FingerType::RING);
	this->fingers[FingerType::PINKY] = Finger(Point2d(465.0f, 130.0f), FingerType::PINKY);

	blur(this->frame, this->frame, Size(3, 3));
	EdgedMask edged_mask(this->frame, color_profile);
	Hand hand(edged_mask);

	for (Finger& f : this->fingers) {
		f.extract(edged_mask.mask);
	}

	this->is_initialized = true;
}

void ImageProcessor::process_not_initialized()
{
	this->result += template_img;
	for (int i = 0; i < roi_points.size(); i++) {
		rectangle(this->result, 
			Point(roi_points[i].x - this->color_feature_radius, 
				roi_points[i].y - this->color_feature_radius), 
			Point(roi_points[i].x + this->color_feature_radius, 
				roi_points[i].y + this->color_feature_radius), 
			Scalar(255, 255, 255), 2, 2);
	}

	int width, height;
	width = height = 50;
	for (int i = 0; i < this->fingers.size(); i++) {
		Rect rect = Rect(Point(this->fingers[i].point.x - width / 2, this->fingers[i].point.y - height / 4), Size(width, height));
		rectangle(this->result, rect, Scalar(255, 0, 0), 2, 2);
	}
}

void ImageProcessor::extract_foreground()
{
	Mat foreground;
	this->backgroud_substractor->apply(this->frame, foreground);

	Mat nonzero_points;
	medianBlur(foreground, foreground, 7);
	findNonZero(foreground, nonzero_points);
	Rect rect = boundingRect(nonzero_points);

	imshow("FOREGROUND", foreground);
	Mat tmp = Mat::zeros(this->frame.size(), CV_8UC3);
	this->frame(rect).copyTo(tmp(rect));
	this->frame = tmp;
}

void ImageProcessor::process_initialized()
{
	blur(this->frame, this->frame, Size(3, 3));
	EdgedMask edged_mask(this->frame, color_profile);
	Hand hand(edged_mask);
	
	// template matching of fingers
	for (Finger& finger : this->fingers) {
		Mat tmp = Mat::zeros(finger.roi_data.size(), CV_8UC1);
		Rect roi = Rect(Point(finger.point.x - (finger.point.x < 50 ? finger.point.x : 50), finger.point.y - (finger.point.y < 50 ? finger.point.y : 50)),
			Point(finger.point.x + (finger.point.x + 50 > this->result.cols ? this->result.cols - finger.point.x : 50), finger.point.y + (finger.point.y + 50 > this->result.rows ? this->result.rows - finger.point.y : 50)));
		matchTemplate(edged_mask.mask(roi), finger.roi_data, tmp, CV_TM_CCOEFF);
		normalize(tmp, tmp, 0, 1, NORM_MINMAX, -1, Mat());

		double minVal;
		double maxVal;
		Point minPoint;
		Point maxPoint;
		Point matchLoc;

		minMaxLoc(tmp, &minVal, &maxVal, &minPoint, &maxPoint, Mat());
		matchLoc = Point(roi.x + maxPoint.x, roi.y + maxPoint.y);

		Rect rect = Rect(matchLoc, Point(matchLoc.x + finger.roi_data.cols, matchLoc.y + finger.roi_data.rows));
		rectangle(this->result, rect, Scalar::all(0), 2, 2, 0);
		circle(this->result, Point(matchLoc.x + finger.roi_data.cols / 2, matchLoc.y + finger.roi_data.rows / 2), 5, Scalar::all(0), 2, 2, 0);

		finger.roi = rect;
		finger.point = Point(matchLoc.x + finger.roi_data.cols / 2, matchLoc.y + finger.roi_data.rows / 2);
	}
	

	vector<Point3d> points3d = vector<Point3d>();
	points3d.push_back(Point3d(-155.0f, 10.0f, 60.0f));			// thumb
	points3d.push_back(Point3d(-18.0f, -160.0f, 20.0f));		// index
	points3d.push_back(Point3d(36.0f, -183.0, 0.0f));			// middle
	points3d.push_back(Point3d(89.0f, -164.0f, 20.0f));			// ring
	points3d.push_back(Point3d(145.0f, -110.0f, 40.0f));		// pinkie

	// Camera internals
	double focal_length = this->result.cols;
	Point2d center = cv::Point2d(this->result.cols / 2, this->result.rows / 2);
	cv::Mat camera_matrix = (cv::Mat_<double>(3, 3) << focal_length, 0, center.x, 0, focal_length, center.y, 0, 0, 1);
	cv::Mat dist_coeffs = cv::Mat::zeros(4, 1, cv::DataType<double>::type); // Assuming no lens distortion

	// Output rotation and translation
	cv::Mat rotation_vector; // Rotation in axis-angle form
	cv::Mat translation_vector;

	vector<Point2d> fingers_points = vector<Point2d>();
	for (int i = 0; i < this->fingers.size(); i++) {
		//int x = this->fingers[i].point.x > 320 ? this->fingers[i].point.x - 320 : -(320 - this->fingers[i].point.x);
		//int y = this->fingers[i].point.y > 240 ? this->fingers[i].point.y - 240 : -(240 - this->fingers[i].point.y);
		fingers_points.push_back(Point2d((float)this->fingers[i].point.x, (float)this->fingers[i].point.y));
	}

	vector<Point> test = vector<Point>();
	for (Point2d p : fingers_points)
		test.push_back(Point((int)p.x, (int)p.y));
	RotatedRect ellipse = minAreaRect(test);
	// Solve for pose
	cv::solvePnP(points3d, fingers_points, camera_matrix, dist_coeffs, rotation_vector, translation_vector);

	// Project a 3D point (0, 0, 1000.0) onto the image plane.
	// We use this to draw a line sticking out of the nose
	vector<Point3d> palm_end_point3D;
	vector<Point2d> palm_end_point2D;
	palm_end_point3D.push_back(Point3d(0, 0, 100.0));
	projectPoints(palm_end_point3D, rotation_vector, translation_vector, camera_matrix, dist_coeffs, palm_end_point2D);
	cv::line(this->result, Point(this->result.cols/2,this->result.rows/2), palm_end_point2D[0], cv::Scalar(255, 0, 0), 2);
	
	this->result += hand.result;
}

ImageProcessor::~ImageProcessor()
{
}
