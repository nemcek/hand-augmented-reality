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