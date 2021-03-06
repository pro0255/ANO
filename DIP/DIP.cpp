// DIP.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Thresholder.h"
#include "FloodFill.h"
#include <string>
#include <iostream>
#include <map>
#include <random>
#include <vector>
#include "Shape.h"
#include <map>
#include "Object.h"
#include "Calculator.h"
#include "Model.h"
#include "Processor.h"
#include <algorithm>
#include "ns_test.h"
#include "NeuralNetwork.h"
#include "SLIC.h"


using namespace std;


//Need as input indexed picture
float calculate_moment(cv::Mat &input, int p, int q, int object_id) {
	float moment = 0;
	for (int y = 0; y < input.rows; y++)
	{
		for (int x = 0; x < input.cols; x++)
		{
			const auto value = input.at<uchar>(y, x);
			if (value == object_id) {
				moment += pow(x, p) * pow(y, q);
			}
		}
	}
	return moment;
}

float calculate_area_of_object(cv::Mat &input, int object_id) {
	return calculate_moment(input, 0, 0, object_id);
}

vector<float> calculate_center_of_mass(cv::Mat &input, int object_id) {
	float x_t = calculate_moment(input, 1, 0, object_id) / (float)calculate_moment(input, 0, 0, object_id);
	float y_t = calculate_moment(input, 0, 1, object_id) / (float)calculate_moment(input, 0, 0, object_id);
	vector<float> res = { x_t, y_t };
	return res;
}

float moment_related_to_center_of_mass(cv::Mat &input, int p, int q, int object_id) {
	auto vec = calculate_center_of_mass(input, object_id);
	auto x_t = vec.at(0);
	auto y_t = vec.at(1);


	float moment = 0;
	for (int y = 0; y < input.rows; y++)
	{
		for (int x = 0; x < input.cols; x++)
		{
			const auto value = input.at<uchar>(y, x);
			if (value == object_id) {
				moment += pow(x - x_t, p) * pow(y - y_t, q);
			}
		}
	}
	return moment;
}

vector<float> calculate_u_min_max(cv::Mat &input, int object_id) {
	float u_max = 0;
	float u_min = 0;

	auto u_20 = moment_related_to_center_of_mass(input, 2, 0, object_id);
	auto u_02 = moment_related_to_center_of_mass(input, 0, 2, object_id);
	auto u_11 = moment_related_to_center_of_mass(input, 1, 1, object_id);

	float common = (1 / (float)2) * (u_20 + u_02);
	float under_sqrt = 4 * pow(u_11, 2) + pow(u_20 - u_02, 2);
	float plus = (1 / (float)2) * sqrt(under_sqrt);

	u_max = common + plus;
	u_min = common - plus;
	vector<float> res = { u_max ,u_min };
	return res;
}

float calculate_F2(cv::Mat &input, int object_id) {
	auto res = calculate_u_min_max(input, object_id);
	auto calculated = res.at(1) / (float)res.at(0);
	return calculated;
}

float calculate_F1(float circumference, float area) {
	auto res = pow(circumference, 2) / float(100 * area);
	return res;
}

bool check_circumference_surroundings(cv::Mat &input, int object_id, int x, int y) {
	cv::Point lookForMatrix[]{
		cv::Point(1, 0), //down
		cv::Point(0, 1), //right
		cv::Point(0, -1), //left
		cv::Point(-1, 0) //up
	};

	auto central = input.at<uchar>(y, x);



	int lookForMatrixSize = sizeof(lookForMatrix) / sizeof(cv::Point);
	for (int i = 0; i < lookForMatrixSize; i++)
	{
		auto point_y = y + lookForMatrix[i].y;
		auto point_x = x + lookForMatrix[i].x;

		if ((point_y >= 0 && point_y < input.rows) && (point_x >= 0 && point_x < input.cols)) {
			auto point_value = input.at<uchar>(point_y, point_x);
			auto res = central == point_value;

			if (!res) {
				return false;
			}
		}
	}
	return true;
}

float calculate_circumference(cv::Mat &input, int object_id) {

	float circumference = 0;
	for (int y = 0; y < input.rows; y++)
	{
		for (int x = 0; x < input.cols; x++)
		{
			const auto value = input.at<uchar>(y, x);
			if (value == object_id && !check_circumference_surroundings(input, object_id, x, y)) {
				circumference++;
			}
		}
	}
	return circumference;
}




vector<Object*> create_objects(
	vector<float>& object_areas,
	vector<float>& object_circumference,
	vector<float>& object_F2,
	vector<float>& object_F1
) {
	vector<Object*> test_objects;
	int size = object_areas.size();
	for (int i = 1; i < size; i++) //first is background
	{
		test_objects.push_back(new Object(i, object_circumference.at(i), object_areas.at(i), object_F1.at(i), object_F2.at(i)));
	}

	return test_objects;
}



pair<int, cv::Mat> fill_features(
	int T,
	string path,
	vector<float>& object_areas,
	vector<float>& object_circumference,
	vector<float>& object_F2,
	vector<float>& object_F1
) {

	cv::Mat threshold_img;
	cv::Mat train_img = cv::imread(path, CV_BGR2GRAY);
	Thresholder::run(train_img, threshold_img, T);
	cv::Mat indexed_img = threshold_img.clone();
	const int number_of_objects = FloodFill::run(threshold_img, indexed_img);


	//cv::imshow("Indexed", indexed_img);


	for (size_t i = 0; i < number_of_objects; i++)
	{
		//Object areas
		object_areas.push_back(calculate_moment(indexed_img, 0, 0, i));
	}

	for (size_t i = 0; i < number_of_objects; i++)
	{
		object_F2.push_back(calculate_F2(indexed_img, i));
	}

	for (size_t i = 0; i < number_of_objects; i++)
	{
		object_circumference.push_back(calculate_circumference(indexed_img, i));
	}

	for (size_t i = 0; i < number_of_objects; i++)
	{
		object_F1.push_back(calculate_F1(object_circumference.at(i), object_areas.at(i)));
	}

	return pair<int, cv::Mat>(number_of_objects, indexed_img);
}


void cv1() {
	const int SIZE = 300;
	const int T = 50;

	string path = "images/test02.png";
	cv::Mat threshold_img;
	cv::Mat train_img = cv::imread(path, CV_BGR2GRAY);
	cv::resize(train_img, train_img, cv::Size(SIZE, SIZE));
	Thresholder::run(train_img, threshold_img, T);
	cv::imshow("Threshold", train_img);

	cv::Mat indexed_img = threshold_img.clone();
	const int number_of_objects = FloodFill::run(threshold_img, indexed_img);


	cv::Mat output = cv::Mat::zeros(indexed_img.size(), CV_32FC3);
	std::vector<cv::Vec3f> colors;
	colors.push_back(cv::Vec3f());

	for (int i = 0; i < number_of_objects; i++) {
		colors.push_back(Calculator::create_color());
	}

	for (int y = 0; y < indexed_img.rows; y++) {
		for (int x = 0; x < indexed_img.cols; x++) {
			output.at<cv::Vec3f>(y, x) = colors[indexed_img.at<uchar>(y, x)];
		}
	}
	cv::imshow("Indexed", output);
}

void print_cv2(vector<float> object_areas, vector<float> object_F2, vector<float> object_circumference, vector<float> object_F1) {
	const unsigned int LEN = object_areas.size();
	for (int i = 1; i < LEN; i++)
	{
		cout << "Object ID " << i << endl;
		cout << "\tArea: " << object_areas.at(i) << endl;
		cout << "\tCircumference: " << object_circumference.at(i) << endl;
		cout << "\tF1: " << object_F1.at(i) << endl;
		cout << "\tF2: " << object_F2.at(i) << endl;
	}

}

void cv2() {
	const int T = 50;
	string path = "images/train.png";

	vector<float> object_areas;
	vector<float> object_circumference;
	vector<float> object_F2;
	vector<float> object_F1;

	fill_features(T, path, object_areas, object_circumference, object_F2, object_F1);

	print_cv2(object_areas, object_F2, object_circumference, object_F1);
}

void cv3() {
	const int T = 50;
	string train_path = "images/train.png";

	vector<float> object_areas;
	vector<float> object_circumference;
	vector<float> object_F2;
	vector<float> object_F1;

	auto res_train = fill_features(T, train_path, object_areas, object_circumference, object_F2, object_F1);
	int number_of_objects = res_train.first;
	vector<Object*> train_objects;

	for (int i = 1; i < number_of_objects; i++)
	{
		if (i >= 9) {
			train_objects.push_back(new Object(i, object_circumference.at(i), object_areas.at(i), object_F1.at(i), object_F2.at(i), 2));
		}
		else if (i >= 5) {
			train_objects.push_back(new Object(i, object_circumference.at(i), object_areas.at(i), object_F1.at(i), object_F2.at(i), 1));
		}
		else {
			train_objects.push_back(new Object(i, object_circumference.at(i), object_areas.at(i), object_F1.at(i), object_F2.at(i), 0));
		}
	}

	string test_path = "images/test01.png";
	vector<float> test_object_areas;
	vector<float> test_object_circumference;
	vector<float> test_object_F2;
	vector<float> test_object_F1;
	auto res_test = fill_features(T, test_path, test_object_areas, test_object_circumference, test_object_F2, test_object_F1);
	vector<Object*> test_objects = create_objects(test_object_areas, test_object_circumference, test_object_F2, test_object_F1);


	Model *predictor = new Model();
	predictor->fit(train_objects);
	predictor->predict(test_objects);

	cout << "\n\nTrain objects\n\n" << endl;
	Object::printObjects(train_objects);

	cout << "\n\nTest objects\n\n" << endl;
	Object::printObjects(test_objects);

	Processor *p = new Processor(3);
	cv::Mat train_img = p->paint_objects(res_train.second, train_objects);
	cv::Mat test_img = p->paint_objects(res_test.second, test_objects);

	cv::imshow("Train Image", train_img);
	cv::imshow("Test Image", test_img);
}

void cv4() {
	const int T = 50;
	const int K = 3;
	const float THRESHOLD_K_MEANS = 0.001;
	string train_path = "images/train.png";

	vector<float> object_areas;
	vector<float> object_circumference;
	vector<float> object_F2;
	vector<float> object_F1;

	auto res_train = fill_features(T, train_path, object_areas, object_circumference, object_F2, object_F1);
	int number_of_objects = res_train.first;

	vector<Object*> train_objects;
	for (int i = 1; i < number_of_objects; i++)
	{
		train_objects.push_back(new Object(i, object_circumference.at(i), object_areas.at(i), object_F1.at(i), object_F2.at(i), -99));
	}


	string test_path = "images/test01.png";
	vector<float> test_object_areas;
	vector<float> test_object_circumference;
	vector<float> test_object_F2;
	vector<float> test_object_F1;
	auto res_test = fill_features(T, test_path, test_object_areas, test_object_circumference, test_object_F2, test_object_F1);
	vector<Object*> test_objects = create_objects(test_object_areas, test_object_circumference, test_object_F2, test_object_F1);


	Model *predictor = new Model();
	predictor->fit(test_objects, K, THRESHOLD_K_MEANS);
	predictor->predict(test_objects);
	predictor->predict(train_objects);

	for (auto elem : predictor->label_2_ethalon)
	{

		for (int i = 0; i < elem.second.size(); i++)
		{
			cout << elem.second.at(i) + ';';
		}
		cout << endl;
	}


	cout << "\n\nTrain objects\n\n" << endl;
	//Object::printObjects(train_objects);

	cout << "\n\nTest objects\n\n" << endl;
	//Object::printObjects(test_objects);

	Processor *p = new Processor(3);
	cv::Mat train_img = p->paint_objects(res_train.second, train_objects);
	cv::Mat test_img = p->paint_objects(res_test.second, test_objects);

	cv::imshow("Train Image", train_img);
	cv::imshow("Test Image", test_img);
}

vector<vector<double>> create_F_vectors(vector<float> F1, vector<float> F2) {

	vector<vector<double>> res;
	for (int i = 1; i < F1.size(); i++) //first is background
	{
		vector<double> F = { F1.at(i), F2.at(i) };
		res.push_back(F);
	}
	return res;
}



pair<vector<vector<double>>, vector<vector<int>>> create_X_y(vector<Object*> train_objects, int number_of_labels) {
	pair<vector<vector<double>>, vector<vector<int>>> res;
	vector<vector<double>> F_res;
	vector<vector<int>> label_res;

	for (int i = 0; i < train_objects.size(); i++)
	{
		vector<double> F = { train_objects.at(i)->F1, train_objects.at(i)->F2 };
		F_res.push_back(F);

		vector<int> label;
		for (int j = 0; j < number_of_labels; j++)
		{
			if (train_objects.at(i)->label == j) {
				label.push_back(1);
			}
			else {
				label.push_back(0);
			}
		}
		label_res.push_back(label);

	}

	res.first = F_res;
	res.second = label_res;
	return res;
}


void cv5_test() {
	ns_test();
}

void cv5() {
	//ns_test();


	const int T = 50;
	const int K = 3;
	const float THRESHOLD_K_MEANS = 0.001;


	string train_path = "images/train.png";
	vector<float> object_areas;
	vector<float> object_circumference;
	vector<float> object_F2;
	vector<float> object_F1;

	auto res_train = fill_features(T, train_path, object_areas, object_circumference, object_F2, object_F1);
	auto train_vectors = create_F_vectors(object_F1, object_F2);
	auto number_of_objects = res_train.first;
	vector<Object*> train_objects;


	for (int i = 1; i < number_of_objects; i++)
	{
		if (i >= 9) {
			train_objects.push_back(new Object(i, object_circumference.at(i), object_areas.at(i), object_F1.at(i), object_F2.at(i), 2));
		}
		else if (i >= 5) {
			train_objects.push_back(new Object(i, object_circumference.at(i), object_areas.at(i), object_F1.at(i), object_F2.at(i), 1));
		}
		else {
			train_objects.push_back(new Object(i, object_circumference.at(i), object_areas.at(i), object_F1.at(i), object_F2.at(i), 0));
		}
	}



	string test_path = "images/test01.png";
	vector<float> test_object_areas;
	vector<float> test_object_circumference;
	vector<float> test_object_F2;
	vector<float> test_object_F1;

	auto res_test = fill_features(T, test_path, test_object_areas, test_object_circumference, test_object_F2, test_object_F1);
	auto test_vectors = create_F_vectors(test_object_F1, test_object_F2);
	vector<Object*> test_objects = create_objects(test_object_areas, test_object_circumference, test_object_F2, test_object_F1);



	int NUMBER_OF_LABELS = 3;
	int number_of_iterations = 50000;
	float net_t = 0.001;
	NeuralNetwork* net = new NeuralNetwork(2, 4, NUMBER_OF_LABELS, number_of_iterations, net_t);
	auto X_y = create_X_y(train_objects, NUMBER_OF_LABELS);
	net->fit(X_y.first, X_y.second);
	auto labels = net->predict(test_vectors);



	cout << "\n\nTest objects Before\n\n" << endl;

	Object::printObjects(test_objects);

	for (int i = 0; i < labels.size(); i++)
	{
		test_objects.at(i)->label = labels.at(i);
	}



	//PRINT
	cout << "\n\nTrain objects\n\n" << endl;
	Object::printObjects(train_objects);

	cout << "\n\nTest objects\n\n" << endl;
	Object::printObjects(test_objects);

	Processor *p = new Processor(3);
	cv::Mat train_img = p->paint_objects(res_train.second, train_objects);
	cv::Mat test_img = p->paint_objects(res_test.second, test_objects);

	cv::imshow("Train Image Net", train_img);
	cv::imshow("Test Image Net", test_img);
}


void cv6() {
	const int T = 50;
	const int K = 30;
	const float THRESHOLD_K_MEANS = 0.001;
	float M = 1;
	const int EPOCHS = 10;


	//string IMG_PATH = "images/bear.png";
	string IMG_PATH = "images/my_avatar.png";

	cv::Mat img = cv::imread(IMG_PATH, cv::IMREAD_COLOR);

	cv::namedWindow("Input", cv::WINDOW_NORMAL);
	cv::imshow("Input", img);

	SLIC* slic = new SLIC();
	slic->run(img, 5, 15);
}







int main()
{
	//cv1();
	//cv2();
	//cv3();
	//cv4();
	//cv5();
	//cv5_test();
	cv6(); //SLIC - Simple Linear Iterative Clustering
	printf("Key to close");
	cv::waitKey(0);
	return 0;
}





