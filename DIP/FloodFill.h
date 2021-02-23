#pragma once
static class FloodFill
{
public:
	static int run(cv::Mat &input, cv::Mat &output);

private:
	static void iteration(cv::Mat &input, int y, int x, int target_color, int color);
};

