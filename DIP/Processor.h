#pragma once
#include <vector>
#include <iostream>
#include "Object.h"
#include <map>
#include "Calculator.h"

using namespace std;

class Processor
{
public:
	map<int, cv::Vec3f> colors;
	Processor(int colors_l);
	cv::Mat paint_objects(cv::Mat indexed_img, vector<Object*>);
};


