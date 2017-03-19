#include <opencv2\opencv.hpp>
#include "Utils.h"
#include "EdgedMask.h"
#include "Hand.h"

using namespace cv;
using namespace std;

void show_threshold(Mat* frame, int thresh);
void show_threshold(Mat* frame, int thresh_b, int thresh_g, int thresh_r);
void show_contours();

Mat img;
Mat dst;
int click_count = 0;
bool is_show_contrours = false;
Mat grayscale;

double distance_euc(Point a, Point b) {
	return sqrt(pow((a.x - b.x), 2) + pow((a.y - b.y), 2));
}

void mouse_call_back(int event, int x, int y, int flags, void* userdata) {
	if (event == EVENT_LBUTTONDOWN) {
		Mat* img = (Mat*)userdata;
		cout << "blue: " << (int)(*img).at<uchar>(y, x) << endl;
		/*cout << "green: " << (int)(*img).at<Vec3b>(x, y)[1] << endl;
		cout << "red: " << (int)(*img).at<Vec3b>(x, y)[2] << endl;
*/
		cout << "pos [x: " << x << ", y: " << y << "]" << endl;

		//show_threshold(img, (int)(*img).at<Vec3b>(y, x)[0], (int)(*img).at<Vec3b>(y, x)[1], (int)(*img).at<Vec3b>(y, x)[2]);
		show_threshold(img, (int)(*img).at<uchar>(y, x));

		if (++click_count == 10) {
			show_contours();
		}
	}
}

int main() {
	img = imread("C:\\dev\\hand_object_aug_reality\\data\\resized\\hand_green_wall_flash.jpg");

	cvtColor(img, grayscale, CV_RGB2GRAY);
	dst = Mat::ones(img.size(), CV_8UC1);

	imshow("original", img);
	setMouseCallback("original", mouse_call_back, &grayscale);

	vector<Point> points;
	points.push_back(Point(144, 314));
	points.push_back(Point(211, 244));
	points.push_back(Point(280, 217));
	points.push_back(Point(350, 255));
	points.push_back(Point(477, 447));
	points.push_back(Point(316, 415));
	points.push_back(Point(345, 532));
	points.push_back(Point(225, 544));
	points.push_back(Point(194, 442));
	points.push_back(Point(265, 472));

	EdgedMask edged_mask(grayscale, points);
	imshow("edged_mask", edged_mask.edges);
	
	Hand hand(edged_mask);
	img += hand.result;

	imshow("img + hand.result", img);

	/*VideoCapture webcam;

	if (!webcam.open(1)) {
		return 1;
	}

	for (;;) {
		webcam >> img;
		cvtColor(img, grayscale, CV_RGB2GRAY);

		imshow("original", img);

		if (is_show_contrours)
			show_contours();

		if (waitKey(10) == 27)
			break;
	}*/

	waitKey();
	return 0;
}

void show_contours() {
	Mat tmp;
	Mat edges;
	Mat contours_img = Mat::zeros(dst.size(), CV_8UC3);

	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;

	medianBlur(dst, tmp, 7);

	Mat structElem = getStructuringElement(MorphShapes::MORPH_RECT, Size(7, 7));
	dilate(tmp, tmp, structElem);
	erode(tmp, tmp, structElem);

	imshow("blured", tmp);

	Canny(tmp, edges, 80, 80 * 2, 3);

	//imshow("edges", edges);

	findContours(edges, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
	
	size_t biggest_countour_size = contours[0].size();
	int biggest_countour_index = 0;
	for (int i = 0; i < contours.size(); i++) {
		if (contours[i].size() > biggest_countour_size) {
			biggest_countour_size = contours[i].size();
			biggest_countour_index = i;
		}
	}

	drawContours(contours_img, contours, biggest_countour_index, Scalar(0, 0, 255), 2, 2, hierarchy);

	//vector<Point> convex_hull;
	//vector<Point> biggest_contour_approxied;
	//approxPolyDP(contours[biggest_countour_index], biggest_contour_approxied, 1, true);
	//convexHull(contours[biggest_countour_index], convex_hull, CV_CLOCKWISE, true);

	//vector<vector<Point>> tmp_biggest_contour_approxied;
	//tmp_biggest_contour_approxied.push_back(biggest_contour_approxied);
	//Mat tmp_biggest_contour_approxied_img = Mat::zeros(img.size(), CV_8UC3);
	//drawContours(tmp_biggest_contour_approxied_img, tmp_biggest_contour_approxied, 0, Scalar(0, 0, 255), 2, 2);
	//imshow("approxied contour", tmp_biggest_contour_approxied_img);
	//contours[biggest_countour_index] = biggest_contour_approxied;

	vector<int> convex_hull_indices;
	convexHull(Mat(contours[biggest_countour_index]), convex_hull_indices, false, false);

	vector<Point> convex_hull_points;
	//vector<Point> convex_hull;
	for (int i = 0; i < convex_hull_indices.size(); i++) {
		convex_hull_points.push_back(contours[biggest_countour_index][convex_hull_indices[i]]);
	}

	vector<Point> convex_hull_points_approxied;
	approxPolyDP(convex_hull_points, convex_hull_points_approxied, 18, true);
	
	//vector<int> convex_hull_approxied_indices;
	//for (int i = 0; i < convex_hull.size(); i++) {
	//	for (int j = 0; j < convex_hull_all.size(); j++) {
	//		if (convex_hull[i].x == convex_hull_all[j].x
	//			&& convex_hull[i].y == convex_hull_all[j].y) {
	//			convex_hull_approxied_indices.push_back(convex_hull_indices[j]);
	//		}
	//	}
	//}

	Mat bounding_rect_img = Mat::zeros(dst.size(), CV_8UC3);
	Rect bounding_rect = boundingRect(convex_hull_points);
	rectangle(bounding_rect_img, bounding_rect, Scalar(255, 198, 75), 3);

	cout << "circles: " << convex_hull_points_approxied.size() << endl;
	Mat convex_hull_points_circles = Mat::zeros(dst.size(), CV_8UC3);
	vector<Point> finger_tip_points;
	for (int i = 0; i < convex_hull_points_approxied.size(); i++) {
		if (convex_hull_points_approxied[i].y >= (bounding_rect.y + bounding_rect.height - 3)
			&& convex_hull_points_approxied[i].y <= (bounding_rect.y + bounding_rect.height + 3))
			continue;

		//bool has_near_neighbour = false;
		//for (int j = 0; j < convex_hull_points_approxied.size(); j++) {
		//	if (i == j)		// same point
		//		continue;

		//	if (distance_euc(convex_hull_points_approxied[i], convex_hull_points_approxied[j]) < 10) {
		//		has_near_neighbour = true;
		//		break;
		//	}
		//}

		//if (!has_near_neighbour) {
			finger_tip_points.push_back(convex_hull_points_approxied[i]);
			circle(convex_hull_points_circles, convex_hull_points_approxied[i], 8, Scalar(0, 0, 255), 5);
		//}
	}

	Mat convex_hull_img = Mat::zeros(dst.size(), CV_8UC3);
	vector<vector<Point>> tmp_convex_hull;
	tmp_convex_hull.push_back(convex_hull_points_approxied);
	drawContours(convex_hull_img, tmp_convex_hull, 0, Scalar(0, 255, 0), 2, 2);
	


	//Mat result = Mat::zeros(dst.size(), CV_8UC3);
	//vector<vector<Point>> tmp_convex_hull2;
	//tmp_convex_hull.push_back(convex_hull_all);
	//drawContours(result, tmp_convex_hull, 0, Scalar(0, 255, 0), 2, 2);

	//vector<vector<Point>> tmp_biggest_contour_img2;
	//tmp_biggest_contour_img2.push_back(contours[biggest_countour_index]);
	//drawContours(result, tmp_biggest_contour_img2, 0, Scalar(0, 0, 255), 2, 2);
	//imshow("result", result);
	/*vector<Point> biggest_contour_approxied;
	approxPolyDP(contours[biggest_countour_index], biggest_contour_approxied, 1, true);*/

	// !!!! NEEDED !!!!
	// for calculation of convexity defects
	std::sort(convex_hull_indices.begin(), convex_hull_indices.end());

	vector<Vec4i> defects;
	convexityDefects(contours[biggest_countour_index], convex_hull_indices, defects);
	cout << "defects: " << defects.size() << endl;

	Mat convexity_defects_img = Mat::zeros(dst.size(), CV_8UC3);
	vector<Point> defect_points;
	vector<Point> defect_points_all;
	for (int i = 0; i < defects.size(); i++) {
		cout << "x: " << contours[biggest_countour_index][defects[i][2]].x << ", y: " << contours[biggest_countour_index][defects[i][2]].y << ", depth: " << defects[i][3] / 256.0 << endl;

		if (defects[i][3] / 256.0 <= 30)
			continue;

		defect_points_all.push_back(contours[biggest_countour_index][defects[i][2]]);

		bool has_near_neighbour = false;
		for (int j = 0; j < defect_points.size(); j++) {
			Point a = contours[biggest_countour_index][defects[i][2]];
			Point b = defect_points[j];
			double dist = distance_euc(a, b);
			if (dist < 10) {
				has_near_neighbour = true;
				break;
			}
		}

		if (!has_near_neighbour) {
			defect_points.push_back(contours[biggest_countour_index][defects[i][2]]);
			circle(convexity_defects_img, contours[biggest_countour_index][defects[i][2]], 8, Scalar(0, 255, 255), 5);
		}

		//circle(convexity_defects_img, contours[biggest_countour_index][defects[i][2]], 8, Scalar(0, 255, 255), 5);
		//defect_points.push_back(contours[biggest_countour_index][defects[i][2]]);
		//cv::String s = "y: " << contours[biggest_countour_index][defects[i][2]].y << ", x: " << contours[biggest_countour_index][defects[i][2]].x;
		//putText(convexity_defects_img, format("y: %d, x: %d, d: %d", contours[biggest_countour_index][defects[i][2]].y, contours[biggest_countour_index][defects[i][2]].x, defects[i][3]), Point(contours[biggest_countour_index][defects[i][2]].x, contours[biggest_countour_index][defects[i][2]].y), FONT_HERSHEY_SIMPLEX, 1.0f, Scalar(255, 255, 255));

	}

	Mat palm_elipse_img = Mat::zeros(img.size(), CV_8UC3);
	if (defect_points.size() > 4) {
		RotatedRect palm_elipse = fitEllipse(defect_points);
		ellipse(palm_elipse_img, palm_elipse, Scalar(128, 0, 128), 4, 2);
		circle(palm_elipse_img, palm_elipse.center, 4, Scalar(128, 0, 128), 4, 2);
	}

	Mat defect_points_all_img = Mat::zeros(img.size(), CV_8UC3);
	for (int i = 0; i < defect_points_all.size(); i++) {
		circle(defect_points_all_img, defect_points_all[i], 4, Scalar(0, 0, 255), 4, 2);
	}
	//imshow("all defects", defect_points_all_img);

	Mat blended_contours = img.clone();
	blended_contours += contours_img;
	blended_contours += convex_hull_img;
	blended_contours += convex_hull_points_circles;
	blended_contours += bounding_rect_img;
	blended_contours += convexity_defects_img;
	blended_contours += palm_elipse_img;

	/*CvFont *font;
	cvInitFont(font, CV_FONT_HERSHEY_SIMPLEX, 1.0f, 1.0f);*/
	//putText(blended_contours, "test text", Point(100, 100), FONT_HERSHEY_SIMPLEX, 1.0f, Scalar(0, 0, 255));

	//addWeighted(img, 0.5, contours_img, 0.5, 0.0, blended_contours);
	imshow("result", blended_contours);
}

void show_threshold(Mat *frame, int thresh) {
	Mat temp;

	threshold(*frame, temp, thresh, 255, THRESH_BINARY);
	dst += temp;

	imshow("threshold", dst);
}

void show_threshold(Mat* frame, int thresh_b, int thresh_g, int thresh_r) {
	//Mat dst;
	Mat final = Mat::zeros((*frame).size(), CV_8UC3);

	vector<Mat> channels_src(3);
	vector<Mat> channels_dst(3);
	split(*frame, channels_src);

	threshold(channels_src[0], channels_dst[0], thresh_b, 255, THRESH_BINARY);
	threshold(channels_src[1], channels_dst[1], thresh_g, 255, THRESH_BINARY);
	threshold(channels_src[2], channels_dst[2], thresh_r, 255, THRESH_BINARY);

	merge(channels_dst, final);
	dst += final;
	/*dst = channels_dst[0].clone();
	for (size_t i = 1; i < channels_src.size(); i++) 
		dst &= channels_src[i];

	dst &= dst;
*/
	imshow("threshold", dst);
}