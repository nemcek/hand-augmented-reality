#include "EdgedMask.h"


EdgedMask::EdgedMask()
{
	return;
}

EdgedMask::EdgedMask(const Mat& frame, const ColorProfile& color_profile)
{
	create_edges(frame, color_profile);
	create_corners(frame);
}

void EdgedMask::create_edges(const Mat& frame, const ColorProfile& color_profile)
{
	mask = Mat::ones(frame.size(), CV_8UC1);
	vector<std::string> names = vector<std::string>();
	int i = 0;
	names.push_back(std::string("thresh 1"));
	names.push_back(std::string("thresh 2"));
	names.push_back(std::string("thresh 3"));
	names.push_back(std::string("thresh 4"));
	names.push_back(std::string("thresh 5"));
	names.push_back(std::string("thresh 6"));
	names.push_back(std::string("thresh 7"));
	names.push_back(std::string("thresh 8"));
	names.push_back(std::string("thresh 9"));
	names.push_back(std::string("thresh 10"));

	for (const ColorFeature& color_feature : color_profile.color_features) {
		Mat tmp;

		int h = (int)color_feature.min_value[0] == 0 ? 10 : (int)color_feature.min_value[0];
		int s = (int)color_feature.min_value[1] == 0 ? 10 : (int)color_feature.min_value[1];
		int l = (int)color_feature.min_value[2] == 0 ? 10 : (int)color_feature.min_value[2];
		inRange(frame, Scalar(h, s, l),
			Scalar((int)color_feature.max_value[0] + h, (int)color_feature.max_value[1] + h, (int)color_feature.max_value[2] +h), tmp);
		mask += tmp;
		//imshow(names[i++], tmp);
	}

	medianBlur(mask, mask, 7);

	Mat structElem = getStructuringElement(MorphShapes::MORPH_RECT, Size(11, 11));
	dilate(mask, mask, structElem);
	erode(mask, mask, structElem);
	imshow("threshold", mask);
	Canny(mask, this->edges, 80, 80 * 2, 3);
}

void EdgedMask::create_corners(const Mat & frame)
{
	Mat gray = Mat::zeros(frame.size(), CV_8UC1);
	Mat tmp = Mat::zeros(frame.size(), CV_8UC3);
	Mat dst = Mat::zeros(frame.size(), CV_32FC1);
	Mat dst_norm, dst_norm_scaled;
	cvtColor(frame, tmp, CV_HLS2RGB);
	cvtColor(tmp, gray, CV_RGB2GRAY);

	cornerHarris(gray, dst, 2, 3, 0.04, BORDER_DEFAULT);

	/// Normalizing
	normalize(dst, dst_norm, 0, 255, NORM_MINMAX, CV_32FC1, Mat());
	convertScaleAbs(dst_norm, dst_norm_scaled);

	//imshow("normalized", dst_norm);
	//imshow("normalized scaled", dst_norm_scaled);
	//imshow("tmp", tmp);
	//imshow("gray", gray);
	/// Drawing a circle around corners
	/*for (int j = 0; j < dst_norm.rows; j++)
	{
		for (int i = 0; i < dst_norm.cols; i++)
		{
			if (dst_norm.at<float>(j, i) > 200.0f)
			{
				circle(dst_norm_scaled, Point(i, j), 5, Scalar(0), 2, 8, 0);
			}
		}
	}*/
	//imshow("normalized scaled points", dst_norm_scaled);

}

EdgedMask::~EdgedMask()
{
}
