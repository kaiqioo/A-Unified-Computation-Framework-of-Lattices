#ifndef ___COMMON_H_INCLUDED___
#define ___COMMON_H_INCLUDED___

#ifdef _MSC_VER
#pragma warning(disable: 4786)
#pragma warning(disable: 4503)
#endif

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <sstream>

using namespace std;

/* convert a string to an integer */
inline int stoi(const string &s)
{
	int result;
	istringstream(s) >> result;
	return result;
}

/* convert an integer to a string */
inline string itos(int n)
{
	ostringstream o;
	o << n;
	return o.str();
}

/* convert a string to an integer */
inline float stof(const string &s)
{
	float result;
	istringstream(s) >> result;
	return result;
}

/* convert an integer to a string */
inline string ftos(float n)
{
	ostringstream o;
	o.setf(ios::fixed, ios::floatfield);
	o.precision(2);
	o << n;
	return o.str();
}

#endif