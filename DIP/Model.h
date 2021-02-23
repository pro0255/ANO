#pragma once
#include <iostream>
#include <vector>
#include "Object.h"
#include <map>
#include "Calculator.h"



using namespace std;

class Model
{

public:
	map<int, vector<float>> label_2_ethalon;
	void fit(vector<Object*> train);
	void predict(vector<Object*> test);

private:
	int find_closest(Object* o);
};

