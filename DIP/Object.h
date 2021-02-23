#pragma once
#include "Shape.h"
#include <vector>
#include <iostream>

using namespace std;

class Object
{
public:
	int id;
	float circumference;
	float area;
	float F1;
	float F2;
	int label;
	Object(int id, float circumference, float area, float F1, float F2, int label);
	Object(int id, float circumference, float area, float F1, float F2);
	void print();

	static void printObjects(vector<Object*>& o);
};

