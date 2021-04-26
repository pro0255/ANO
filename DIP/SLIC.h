#pragma once

#include <iostream>
#include "Calculator.h"
#include <limits>
#include <numeric>

using namespace std;


const int t = 128;
const int k = 15;
const int m = 10;
const int width = 375;
const int height = 300;
const int n = width * height;


struct Point
{
	int r, g, b;
	int x, y;
	int cluster;
	double minDist;

	Point() :
		r(0),
		g(0),
		b(0),
		x(0),
		y(0),
		cluster(-1),
		minDist(DBL_MAX) {}

	Point(int r, int g, int b, int x, int y) :
		r(r),
		g(g),
		b(b),
		x(x),
		y(y),
		cluster(-1),
		minDist(DBL_MAX) {}

	float distance(Point p)
	{
		int s = sqrt(n / k);

		float d_xy = sqrt((p.x - x) + pow(p.y - y, 2));
		float d_rgb = sqrt(pow(p.r - r, 2) + pow(p.g - g, 2) + pow(p.b - b, 2));

		return d_rgb + (m / s)*d_xy;
	}
};

class SLIC
{

public:

	SLIC();
	~SLIC();

	cv::Mat& run(unsigned int K, cv::Mat& input, float threshold, float m, int epochs);
	void run2(cv::Mat image, int epochs, int k);
	void drawCentroids(vector<Point> centroids, cv::Mat& input, int epoch);
	vector<Point> createPoints(cv::Mat& input);
	vector<Point> initCentroids(cv::Mat& input);





	vector<vector<float>> init(int S, int K, cv::Mat& input);
	void drawPoints(vector<vector<float>> centroids, cv::Mat& input);
	float distance(vector<float> a, vector<float> b, float m, float S);
};




