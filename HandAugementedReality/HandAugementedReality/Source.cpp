#include <opencv2\opencv.hpp>
#include "Utils.h"
#include "EdgedMask.h"
#include "Hand.h"

using namespace cv;
using namespace std;

double distance_euc(Point a, Point b) {
	return sqrt(pow((a.x - b.x), 2) + pow((a.y - b.y), 2));
}

void mouse_call_back(int event, int x, int y, int flags, void* userdata) {
	if (event == EVENT_LBUTTONDOWN) {
		Mat* img = (Mat*)userdata;
		cout << "[" << x << ", " << y << "]" << endl;
	}
}

int main() {
	Mat template_img = imread("C:\\dev\\hand_object_aug_reality\\data\\template.jpg");
	Mat grayscale;
	
	vector<Point> points;
	points.push_back(Point(143, 138));
	points.push_back(Point(210, 84));
	points.push_back(Point(277, 67));
	points.push_back(Point(340, 100));
	points.push_back(Point(468, 283));
	points.push_back(Point(193, 286));
	points.push_back(Point(300, 262));
	points.push_back(Point(332, 387));
	points.push_back(Point(210, 398));
	points.push_back(Point(261, 322));

	VideoCapture webcam;

	if (!webcam.open(2)) {
		return 1;
	}

	bool initialized = false;
	ColorProfile color_profile;
	EdgedMask edged_mask;
	int l = 0;
	Mat dst = Mat::zeros(Size(640, 480), CV_8UC1);
	vector<InterestRegion> interest_regions;
	for (;;) {
		Mat img;
		Mat orig;
		webcam >> img;
		orig = img.clone();
		if (!initialized) {
			img += template_img;
			int r = 10;
			int rect_width = 5;
			for (int i = 0; i < points.size(); i++) {
				rectangle(img, Point(points[i].x - r, points[i].y - r), Point(points[i].x + r, points[i].y + r), Scalar(255, 255, 255), 2, 2);
			}
		}
		else {
			cvtColor(orig, orig, CV_RGB2HLS);
			blur(orig, orig, Size(3, 3));
			edged_mask = EdgedMask(orig, color_profile);
			imshow("edges", edged_mask.edges);
			Hand hand(edged_mask);

			img += hand.result;
		}

		if (waitKey(30) == 13) {
			cvtColor(orig, orig, CV_RGB2HLS);

			for (const Point& p : points) {
				interest_regions.push_back(InterestRegion(orig, p, 10));
			}
			color_profile = ColorProfile(interest_regions);
			initialized = true;
		}

		imshow("webcam", img);
	}

	waitKey();
	return 0;
}