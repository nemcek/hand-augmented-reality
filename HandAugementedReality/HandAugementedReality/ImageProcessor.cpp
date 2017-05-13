#include "ImageProcessor.h"


vector<Point3d> points3d;

ImageProcessor::ImageProcessor(const vector<Point>& points) : roi_points(points)
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

// Inits the image processing
void ImageProcessor::init()
{
	for (const Point& p : this->roi_points) {
		rois.push_back(InterestRegion(this->frame, p, this->color_feature_radius));
	}

	this->color_profile = ColorProfile(rois);
	blur(this->frame, this->frame, Size(3, 3));
	EdgedMask edged_mask(this->frame, color_profile);
	hand = Hand();
	hand.process(edged_mask, &color_profile);

	this->is_initialized = true;
}

// Processes not started processing
void ImageProcessor::process_not_initialized()
{
	stringstream ss;
	putText(this->result, string("Please cover squares with your hand (palm) and press <space>"), Point(10, 25), FONT_HERSHEY_SIMPLEX, 0.6, Scalar(0, 0, 255), 2, 2);
	for (int i = 0; i < roi_points.size(); i++) {
		rectangle(this->result, 
			Point(roi_points[i].x - this->color_feature_radius, 
				roi_points[i].y - this->color_feature_radius), 
			Point(roi_points[i].x + this->color_feature_radius, 
				roi_points[i].y + this->color_feature_radius), 
			Scalar(255, 255, 255), 2, 2);
	}
}

/// Extrracts foreground to eliminate noise in background
void ImageProcessor::extract_foreground()
{
	Mat foreground;
	this->backgroud_substractor->apply(this->frame, foreground);

	// take non zero points and calculate bounding rectangle from it
	Mat nonzero_points;
	medianBlur(foreground, foreground, 7);
	findNonZero(foreground, nonzero_points);
	Rect rect = boundingRect(nonzero_points);

	// extract part of image covered by bounding rectangle
	Mat tmp = Mat::zeros(this->frame.size(), CV_8UC3);
	this->frame(rect).copyTo(tmp(rect));
	this->frame = tmp;
}

// Process started processing
void ImageProcessor::process_initialized()
{
	blur(this->frame, this->frame, Size(3, 3));
	EdgedMask edged_mask(this->frame, color_profile);
	hand.process(edged_mask, &color_profile);
	
	this->result += hand.result;
}

ImageProcessor::~ImageProcessor()
{
}
