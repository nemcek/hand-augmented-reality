#include <opencv2\opencv.hpp>
#include "Utils.h"
#include "EdgedMask.h"
#include "Hand.h"
#include "ImageProcessor.h"

using namespace cv;
using namespace std;

double distance_euc(Point a, Point b) {
	return sqrt(pow((a.x - b.x), 2) + pow((a.y - b.y), 2));
}

void mouse_call_back(int event, int x, int y, int flags, void* userdata) {
	if (event == EVENT_LBUTTONDOWN) {
		cout << "[" << x << ", " << y << "]" << endl;
	}
}

int main() {
	Mat template_img = imread("C:\\dev\\hand_object_aug_reality\\data\\template.jpg");
	imshow("aug hand", template_img);
	setMouseCallback("aug hand", mouse_call_back, NULL);
	vector<Point> points;
	points.push_back(Point(199, 281));
	points.push_back(Point(302, 115));
	points.push_back(Point(356, 91));
	points.push_back(Point(409, 111));
	points.push_back(Point(462, 169));
	points.push_back(Point(274, 327));
	points.push_back(Point(326, 239));
	points.push_back(Point(423, 271));
	points.push_back(Point(405, 370));
	points.push_back(Point(371, 303));

	VideoCapture webcam;

	if (!webcam.open(2)) {
		return 1;
	}

	ImageProcessor image_proc = ImageProcessor(template_img, points);
	for (;;) {
		Mat img;
		webcam >> img;
		image_proc.process(img);

		if (waitKey(30) == 13) {
			image_proc.init();
		}

		imshow("aug hand", image_proc.result);
	}

	waitKey();
	return 0;
}