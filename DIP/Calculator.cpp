#include "stdafx.h"
#include "Calculator.h"

vector<float> Calculator::calculate_ethalon_for_class(vector<vector<float>> feature_points)
{
	vector<float> ethalon;
	const auto NR = feature_points.size();
	const int number_of_features = feature_points.at(0).size();


	for (int i = 0; i < number_of_features; i++) {
		float sum = 0;


		for (int j = 0; j < NR; j++)
		{
			sum += feature_points.at(j).at(i);

		}
		ethalon.push_back(sum / (float)NR);
	}
	return ethalon;
}

vector<float> Calculator::create_feature_point(Object & o)
{
	vector<float> feature_point = { o.F1, o.F2 };
	return feature_point;
}

map<int, vector<float>> Calculator::calculate_ethalons(vector<Object*>& train_objects)
{
	map<int, vector<vector<float>>> class_feature_points;
	for (auto* o : train_objects) // access by reference to avoid copying
	{
		const auto feature_vec = create_feature_point(*o);
		if (class_feature_points.find(o->label) == class_feature_points.end()) {
			vector<vector<float>> parent;
			class_feature_points.insert(pair<int, vector<vector<float>>>(o->label, parent));
		}
		class_feature_points.at(o->label).push_back(Calculator::create_feature_point(*o));
	}

	map<int, vector<float>> label_2_ethalon;
	for (auto const& x : class_feature_points)
	{
		auto res = calculate_ethalon_for_class(class_feature_points.at(x.first));
		label_2_ethalon.insert(pair<int, vector<float>>(x.first, res));
	}

	return label_2_ethalon;
}

cv::Vec3f Calculator::create_color()
{
	std::random_device rd;
	std::mt19937 eng(rd());
	std::uniform_int_distribution<> distr(50, 255);
	return cv::Vec3f(float(distr(eng)) / 255, float(distr(eng)) / 255, float(distr(eng)) / 255);
}

float Calculator::calculate_eucladian_distance(vector<float> a, vector<float> b)
{
	float sum = 0;
	for (unsigned int i = 0; i < a.size(); i++)
	{
		sum += pow(a.at(i) - b.at(i), 2);
	}
	return sqrt(sum);
}
