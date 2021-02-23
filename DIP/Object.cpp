#include "stdafx.h"
#include "Object.h"


Object::Object(int id, float circumference, float area, float F1, float F2, int label)
{
	this->id = id;
	this->area = area;
	this->F1 = F1;
	this->F2 = F2;
	this->label = label;
	this->circumference = circumference;
}

Object::Object(int id, float circumference, float area, float F1, float F2)
{
	this->id = id;
	this->area = area;
	this->F1 = F1;
	this->F2 = F2;
	this->circumference = circumference;
	this->label = -999;
}

void Object::print()
{
	cout << "Object ID " << this->id << endl;
	cout << "\tArea: " << this->area << endl;
	cout << "\tCircumference: " << this->circumference << endl;
	cout << "\tF1: " << this->F1 << endl;
	cout << "\tF2: " << this->F2 << endl;
	cout << "\tClass: " << this->label << endl;
}

void Object::printObjects(vector<Object*>& o)
{
	for (int i = 0; i < o.size(); i++)
	{
		o.at(i)->print();
	}
}



