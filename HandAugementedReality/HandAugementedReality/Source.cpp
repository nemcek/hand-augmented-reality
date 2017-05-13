#include <opencv2\opencv.hpp>
#include "Utils.h"
#include "EdgedMask.h"
#include "Hand.h"
#include "ImageProcessor.h"

using namespace cv;
using namespace std;

void mouse_call_back(int event, int x, int y, int flags, void* userdata) {
	if (event == EVENT_LBUTTONDOWN) {
		cout << "[" << x << ", " << y << "]" << endl;
	}
}

int main(int argc, char* argv[]) {
	Mat template_img = imread("C:\\dev\\hand_object_aug_reality\\data\\template.jpg");

	// Region of interest points for color profile extraction
	vector<Point> points;
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

		if (img.empty()) {
			break;
		}

		image_proc.process(img);

		int key = waitKey(30);
		if (key == 32) {
			image_proc.init();
		}
		else if (key == 114) {
			image_proc.is_initialized = false;
		}

		imshow("aug hand", image_proc.result);
	}

	waitKey();
	return 0;
}