#ifndef ___QUOTIENTCUBE_H_INCLUDED___
#define ___QUOTIENTCUBE_H_INCLUDED___

#include "Common.h"
#include "Cubing.h"

//#define AGGREGATE

class QuotientCube : public Cubing
{
private:
	int **data[2];
	float **msrdata[2];
	int dflag;
	float *msrValues;

	int uclsNum;
	int *aggsNumPerLayer;
	FILE **fps;

	int caNum;  //the count ucl of Ca>1

public:
	QuotientCube();
	~QuotientCube();

	void preCompute();
	int getAggsNum();

	int findCard(int bPos, int ePos, int d);  //find cardinality / maximum value of data on dth dimension;
	void Partition(int bPos, int ePos, int d, int card, int *freq);
	void QuotientCube::DFS( const int const cl[], int bPos, int ePos, int d );
	inline int outputTodimDataFile(int layer, int outputDimData[], int ca);
	inline int outputTomsrDataFile(int layer, float outputMsrData[]);
};

#endif