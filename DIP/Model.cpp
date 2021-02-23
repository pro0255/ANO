#include "stdafx.h"
#include "Model.h"

void Model::fit(vector<Object*> train)
{
	this->label_2_ethalon = Calculator::calculate_ethalons(train);
}

void Model::predict(vector<Object*> test)
{
	for (auto &o : test) {
		o->label = this->find_closest(o);
	}
}

int Model::find_closest(Object * o)
{
	auto feature_vector = Calculator::create_feature_point(*o);
	vector<pair<int, float>> distance_vector;

	for (auto const& entry : this->label_2_ethalon)
	{
		auto d = Calculator::calculate_eucladian_distance(feature_vector, entry.second);
		distance_vector.push_back(pair<int, float>(entry.first, d));
	}



	float min_dst;
	int min_index;

	for (int i = 0; i < distance_vector.size(); i++)
	{

		auto current_dst = distance_vector.at(i).second;
		auto current_index = distance_vector.at(i).first;

		if (i == 0) {
			min_dst = current_dst;
			min_index = current_index;
		}
		else {
			if (min_dst > current_dst) {
				min_dst = current_dst;
				min_index = current_index;
			}
		}
	}

	return min_index;




	return 0;
}
