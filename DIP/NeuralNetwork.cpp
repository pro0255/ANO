#include "stdafx.h"
#include "NeuralNetwork.h"

#define VERBOSE false

NeuralNetwork::NeuralNetwork(int n, int h, int o, int iterations, float t = 0.001)
{
	this->nn = createNN(n, h, o);
	this->iterations = iterations;
	this->t = t;
}

void NeuralNetwork::fit(vector<vector<double>> X, vector<vector<int>> y)
{

	int train_size = X.size() - 1;


	int labels_size = nn->n[nn->l - 1];
	int input_size = nn->n[0];

	double error = 1.0;
	int i = 0;
	double ** trainingSet = new double *[train_size];

	for (int i = 0; i < train_size; i++) {
		trainingSet[i] = new double[input_size + labels_size];
		for (int j = 0; j < input_size; j++) {
			trainingSet[i][j] = X.at(i).at(j);
		}

		for (int j = 0; j < labels_size; j++)
		{
			trainingSet[i][input_size + j] = y.at(i).at(j);
		}

	}

	while (true) {
		setInput(nn, trainingSet[i%train_size]);
		feedforward(nn);
		error = backpropagation(nn, &trainingSet[i%train_size][input_size]);

		if (VERBOSE) {
			printf("\nerr=%0.3f", error);
			printf("\n\t generation=%d", i);

		}

		if (error <= this->t) {
			printf("\nEnd learning!\n");
			printf("\nerr=%0.3f", error);

			break;
		}

		if (i >= this->iterations) {
			printf("\nEnd learning!\n");
			printf("\nerr=%0.3f", error);

			break;
		}
		i++;
	}

	for (int i = 0; i < train_size; i++) {
		delete[] trainingSet[i];
	}
	delete[] trainingSet;

}

vector<int> NeuralNetwork::predict(vector<vector<double>> X)
{
	int input_size = nn->n[0];
	double* in = new double[input_size];

	vector<int> y = {};
	int number_of_samples = X.size();
	for (int i = 0; i < number_of_samples; i++)
	{
		auto vec = X.at(i);
		for (int j = 0; j < vec.size(); j++)
		{
			in[j] = vec.at(j);
		}

		setInput(nn, in, true);
		feedforward(nn);
		int output = getOutput(nn, true);
		y.push_back(output);

	}

	return y;
}
