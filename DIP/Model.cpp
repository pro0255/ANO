#include "stdafx.h"
#include "Model.h"

void Model::fit(vector<Object*> train)
{
	this->label_2_ethalon = Calculator::calculate_ethalons(train);
}

void Model::fit(vector<Object*> train_objects, int k, float threshold)
{
	vector<int> selection_index;
	int nelems = k;

	while (selection_index.size() < nelems) {
		int randomIndex = rand() % train_objects.size();
		if (find(selection_index.begin(), selection_index.end(), randomIndex) == selection_index.end()) {
			selection_index.push_back(randomIndex);
		}
	}

	int counter = 0;
	vector<vector<float>> centroids;

	for (int i = 0; i < selection_index.size(); i++)
	{
		centroids.push_back(Calculator::create_feature_point(*train_objects.at(i)));
	}

	while (true) {
		vector<vector<Object*>> centroid_objects;
		for (int i = 0; i < k; i++)
		{
			centroid_objects.push_back(vector<Object*> {});
		}

		for (int i = 0; i < train_objects.size(); i++)
		{
			auto current = train_objects.at(i);
			auto current_feature_point = Calculator::create_feature_point(*current);
			int min_index = -1;
			float min_distance = -1;

			for (int j = 0; j < centroids.size(); j++)
			{
				auto d = Calculator::calculate_eucladian_distance(current_feature_point, centroids.at(j));
				if (min_index == -1 || d < min_distance) {
					min_index = j;
					min_distance = d;
				}
			}
			centroid_objects.at(min_index).push_back(current);
		}

		//Update centroids
		vector<vector<float>> new_centroids;
		for (int i = 0; i < k; i++)
		{
			new_centroids.push_back(vector<float> {});
			for (int j = 0; j < centroids.at(0).size(); j++)
			{
				float sum = 0;
				for (int m = 0; m < centroid_objects.at(i).size(); m++)
				{
					auto v = Calculator::create_feature_point(*centroid_objects.at(i).at(m));
					sum += v.at(j);
				}
				auto mean = sum / float(centroid_objects.at(i).size());
				new_centroids.at(i).push_back(mean);
			}
		}

		//for (int i = 0; i < centroids.size(); i++)
		//{
		//	cout << centroid_objects.at(i).size() << endl;
		//}
		//cout << endl;

		for (int i = 0; i < centroids.size(); i++)
		{
			cout << "Old centroid " << i << endl;
			for (int j = 0; j < centroids.at(i).size(); j++)
			{
				cout << centroids.at(i).at(j) << ';';
			}
			cout << endl;
		}


		for (int i = 0; i < new_centroids.size(); i++)
		{
			cout << "New centroid " << i << endl;
			for (int j = 0; j < new_centroids.at(i).size(); j++)
			{
				cout << new_centroids.at(i).at(j) << ';';
			}
			cout << endl;

		}






		bool isBreaking = false;
		for (int i = 0; i < new_centroids.size(); i++)
		{
			if (new_centroids.at(i) == centroids.at(i) || Calculator::calculate_eucladian_distance(new_centroids.at(i), centroids.at(i)) < threshold) {
				//cout << "same" << endl;
				isBreaking = true;
			}
			else {
				//cout << "different" << endl;
				isBreaking = false;
				break;
			}
		}

		centroids = new_centroids;
		if (isBreaking) {
			break;
		}
		counter++;
		cout << "\nGeneration -> " << counter << endl;

	}


	map<int, vector<float>> label_2_ethalon;
	for (int i = 0; i < centroids.size(); i++)
	{
		label_2_ethalon.insert(pair<int, vector<float>>(i, centroids.at(i)));
	}

	this->label_2_ethalon = label_2_ethalon;
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
