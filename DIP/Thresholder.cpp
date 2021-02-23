#include "stdafx.h"
#include "Thresholder.h"
#include <iostream>


using namespace std;

void Thresholder::run(cv::Mat &input, cv::Mat &output, float threshold)
{
	output = input.clone();

	for (int y = 0; y < output.rows; y++) {
		for (int x = 0; x < output.cols; x++) {
			if (output.at<uchar>(y, x) > threshold) {
				output.at<uchar>(y, x) = 1;
			}
			else {
				output.at<uchar>(y, x) = 0;
			}
		}
	}
}

