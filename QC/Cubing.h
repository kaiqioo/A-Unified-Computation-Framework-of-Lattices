#ifndef ___CUBING_H_INCLUDED___
#define ___CUBING_H_INCLUDED___

#include "Common.h"

#include <ctime>
#include <direct.h>
#include <fstream>
#include "TupleHandle.h"
#include "DFHandle.h"

#define MAPPER

const int ALL = 0;

class Cubing;

typedef map<string, int> Mapper;
typedef float (Cubing::* aggFun)(float **msrdata, int bPos, int ePos, int msrNum);

class Cubing
{
protected:
	int dimsNum;
	int msrsNum;
	int tuplesNum;
	int **data;
	float **msrdata;

	char *delimiter;
	string cubeDir;  //the direction of a cube used for containing the aggregated data etc.
	clock_t tstart;  // Time at beginning of timed section

	vector< int > aggFunOrder;
	aggFun aggFunLib[5];  //from sum, count, avg, max, min in turn

	float sumFun(float **msrdata, int bPos, int ePos, int msrNum);
	float countFun(float **msrdata, int bPos, int ePos, int msrNum);
	float avgFun(float **msrdata, int bPos, int ePos, int msrNum);
	float maxFun(float **msrdata, int bPos, int ePos, int msrNum);
	float minFun(float **msrdata, int bPos, int ePos, int msrNum);

public:
	Cubing();
	~Cubing();
	void loadData(string dataFileName);
	virtual void preCompute();
	virtual int getAggsNum();

	void beginTimer(string message = "begin timer...");
	void endTimer(string message = "end timer:");
};

#endif