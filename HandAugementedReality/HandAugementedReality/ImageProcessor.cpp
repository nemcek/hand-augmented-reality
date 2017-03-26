#include "ImageProcessor.h"



ImageProcessor::ImageProcessor(const Mat& template_img, const vector<Point>& points) : template_img(template_img), roi_points(points)
{
	this->backgroud_substractor = createBackgroundSubtractorMOG2(500, 16, false);
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
}

void ImageProcessor::extract_foreground()
{
	Mat foreground;
	this->backgroud_substractor->apply(this->frame, foreground);

	Mat nonzero_points;
	medianBlur(foreground, foreground, 7);
	findNonZero(foreground, nonzero_points);
	Rect rect = boundingRect(nonzero_points);

	Mat tmp = Mat::zeros(this->frame.size(), CV_8UC3);
	this->frame(rect).copyTo(tmp(rect));
	this->frame = tmp;
}

void ImageProcessor::process_initialized()
{
	blur(this->frame, this->frame, Size(3, 3));
	EdgedMask edged_mask(this->frame, color_profile);
	Hand hand(edged_mask);

	this->result += hand.result;
}

ImageProcessor::~ImageProcessor()
{
}
