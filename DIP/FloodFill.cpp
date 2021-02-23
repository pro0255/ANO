#include "stdafx.h"
#include "FloodFill.h"


int FloodFill::run(cv::Mat & input, cv::Mat &output)
{

	output = cv::Mat::zeros(input.size(), input.type());
	std::vector<cv::Point> points;
	cv::Point lookForMatrix[]{
		cv::Point(0, 1), cv::Point(0, -1), cv::Point(1, 0),
		cv::Point(-1,0)
	};
	int lookForMatrixSize = sizeof(lookForMatrix) / sizeof(cv::Point);
	int o_id = 1;
	for (int y = 0; y < input.rows; y++) {
		for (int x = 0; x < input.cols; x++) {
			if (input.at<uchar>(y, x) != 0.0f) {
				bool indexed = false;
				points.push_back(cv::Point(x, y));

				while (!points.empty()) {
					cv::Point point = points[points.size() - 1];
					points.pop_back();
					if (point.x > 0 && point.y > 0 && point.x < input.cols && point.y < input.rows) {
						if (output.at<uchar>(point) != 0.0f) {
							continue;
						}
						for (int i = 0; i < lookForMatrixSize; i++) {
							cv::Point pointToCheck = point + lookForMatrix[i];
							if (input.at<uchar>(pointToCheck) != 0.0f) {
								points.push_back(pointToCheck);
							}
						}
						indexed = true;
					}
					output.at<uchar>(point) = o_id;
				}
				if (indexed) {
					o_id++;
				}
			}
		}
	}
	return o_id;
}

void FloodFill::iteration(cv::Mat & input, int y, int x, int target_color, int color)
{

}
