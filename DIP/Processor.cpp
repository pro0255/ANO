#include "stdafx.h"
#include "Processor.h"

Processor::Processor(int number_of_classes)
{
	for (int i = 0; i < number_of_classes; i++)
	{
		this->colors.insert(pair<int, cv::Vec3f>(i, Calculator::create_color()));
	}

	this->colors.insert(pair<int, cv::Vec3f>(-99, cv::Vec3f()));
}

cv::Mat Processor::paint_objects(cv::Mat indexed_img, vector<Object*> objects)
{

	cv::Mat output = cv::Mat::zeros(indexed_img.size(), CV_32FC3);

	for (int y = 0; y < indexed_img.rows; y++) {
		for (int x = 0; x < indexed_img.cols; x++) {


			auto pixel_value = indexed_img.at<uchar>(y, x);
			auto ref_to_vector = pixel_value - 1;

			if (ref_to_vector < 0) {
				output.at<cv::Vec3f>(y, x) = this->colors.at(-99);
			}
			else {
				output.at<cv::Vec3f>(y, x) = this->colors.at(objects.at(ref_to_vector)->label);
			}

		}
	}
	return output;
}

