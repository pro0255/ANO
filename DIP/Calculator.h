#pragma once
#include <iostream>
#include <vector>
#include "Object.h"
#include <map>
#include <random>

using namespace std;

class Calculator
{
public:
	static  vector<float> calculate_ethalon_for_class(vector<vector<float>> feature_points);
	static vector<float> create_feature_point(Object& o);
	static map<int, vector<float>> calculate_ethalons(
		vector<Object*>& train_objects
	);
	static cv::Vec3f create_color();
	static float calculate_eucladian_distance(vector<float> a, vector<float> b);
};

