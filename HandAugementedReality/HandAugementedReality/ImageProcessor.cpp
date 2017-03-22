#include "ImageProcessor.h"



ImageProcessor::ImageProcessor(const Mat& template_img, const vector<Point>& points) : template_img(template_img), roi_points(points)
{
}

void ImageProcessor::process(const Mat & frame)
{
	this->result = frame.clone();
	flip(this->result, this->result, 1);
	this->frame = this->result.clone();

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
