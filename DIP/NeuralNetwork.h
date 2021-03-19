#pragma once
#include <vector>
#include "iostream"
#include "ns_test.h"
#include "backprop.h"


using namespace std;

class NeuralNetwork
{
public:
	NeuralNetwork(int n, int h, int o, int iterations, float t);
	void fit(vector<vector<double>> X, vector<vector<int>> y);
	vector<int > predict(vector<vector<double>> X);

private:
	NN* nn;
	float t;
	int iterations;
};

