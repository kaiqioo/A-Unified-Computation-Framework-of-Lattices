/*============================================================================
  File:    QuotientCube.cpp

  Summary: The implement of quotient cube algorithm

  Copyright(c) 2007 by DBNC Lab, South China University of Technology, Guangzhou, Guangdong, P.R.C. 

------------------------------------------------------------------------------
  Modification history:
  2007.01.24 Version 1.0 by Jinguo You
============================================================================*/

#include "QuotientCube.h"

QuotientCube::QuotientCube()
{
	dflag = 0;
	uclsNum = 0;
	fps = NULL;

	caNum = 0; //容量
}

QuotientCube::~QuotientCube()
{
	if(fps)
	{
		for(int i = 0; i < 2*(dimsNum + 1); i ++)
		{
			fclose(fps[i]);
		}
		delete[] fps;
	}
}

//noting: ePos = bPos + size
int QuotientCube::findCard(int bPos, int ePos, int d) // Cardinality：找到势，bPos：起始位置，ePos：终止位置
{
	int max = 0;
	for(int i = bPos; i < ePos; i ++)//i:行，d：列
	{
		max = data[dflag][i][d] > max ? data[dflag][i][d] : max; //找d列的最大值
	}
	return (max + 1);//+1为了下一步“ < ” ？
}

void QuotientCube::Partition(int bPos, int ePos, int d, int card, int *freq)//S card：该维度的元素种类数
{
	int i;
	int *tmpFreq = new int[card];

	//initialize the frequency; 可以考虑用memset
	for(i = 0; i < card; i++)
	{
		freq[i] = 0;
	}

	//caculate the frequency of input data element occurence. 计算本列中每个值出现的次数
    //Note: assuming the elements begin from 1, so freq[0] should be 0.
	//注：假设元素从1开始，则频率[0]应为0.
	for(i = bPos; i < ePos; i++)
		freq[ data[dflag][i][d] ] ++;

	//caculate the begin of every element. 由于本列已经有序，那么freq中保存的就是每个值的初始位置
	freq[0] = bPos;
	for(i = 1; i < card; i++)
		freq[i] = freq[i] + freq[i - 1];

	//make a copy of freq  可以考虑用memcpy或者vector更快
	for(i = 0; i < card; i++)
	{
		tmpFreq[i] = freq[i];
	}

	for(i = bPos; i < ePos; i++)
	{
		data[!dflag][ tmpFreq[ data[dflag][i][d] ] - 1 ] = data[dflag][i];
		//msrdata[!dflag][ tmpFreq[ data[dflag][i][d] ] - 1 ] = msrdata[dflag][i];
		tmpFreq[ data[dflag][i][d] ] --;
	}

	dflag = !dflag;

	delete[] tmpFreq;
}

void QuotientCube::DFS( const int const cl[], int bPos, int ePos, int d )//S 深度优先遍历
{
	int i = 0;
	int j = 0;
	int k = 0;
	//int *outputDimData = new int[dimsNum];
	//float *outputMsrData = new float[msrsNum];

	int *ucl = new int[dimsNum]; //S 等价类上界ucl为一个整型数组
	for ( i = 0; i < dimsNum; i++ )
	{
		ucl[i] = cl[i];
	} // 可以考虑改用memset或者vector<int>
	

	for(i = 0; i < dimsNum; i ++)
	{
		if(cl[i] == ALL) // 当前cell的第i个维度是ALL
		{
			int diff = 0; //差集 = i维中，每一行值j - 第一行值bPos
			for(j = bPos + 1; j < ePos; j++)
			{
				diff = data[dflag][j][i] - data[dflag][bPos][i];
				if(diff) // 不相等
				{
					break;
				}
			} // 循环结束，找到首个在第i个维度不相等的元组
			if(!diff) // 找到了相等的
			{
				ucl[i] = data[dflag][bPos][i]; // 上界
				//outputDimData[i] = ucl[i];
				k ++;
			}
			//else
			//	outputDimData[i] = 0;
		}
		else // 当前cell的第i个维度不是all
		{
			//outputDimData[i] = cl[i];
			k ++;
		}
	}


	//prune the cells which have been examined.剪枝非上界的数据单元
	for(j = 0; j < d; j ++)
	{
		if(cl[j] == ALL && ucl[j] != ALL)// 该ucl[]并不是上界，剪枝删除
		{
			delete []ucl;
			//delete []outputDimData;
			//delete []outputMsrData;
			return;
		}
	}
/*
	for(i =0; i < dimsNum; i ++)
	{
		cout<<ucl[i]<<" ";
	}
	cout<<endl;
*/

	#ifdef AGGREGATE
	for(i = 0; i < msrsNum; i ++)
	{
		//msrValues[i] = (this->*aggFunLib[aggFunOrder[i]])(msrdata[dflag], bPos, ePos, i);
		outputMsrData[i] = msrValues[i];
	}
	#endif

	int ca = 1;  //the capacity of a class 类的容量
	for (i=0; i < dimsNum; i++) {
		if (cl[i]==ucl[i] && (ePos-bPos)!=1) continue;
		else  ca = -1;
	}
	if (ca == -1) caNum++;

	outputTodimDataFile(k, ucl, ca);//S（层数，上界，容量）
	//outputTomsrDataFile(k, outputMsrData);

	//delete []outputDimData;
	//delete []outputMsrData;

	aggsNumPerLayer[k] ++;
	
	uclsNum ++;
	
	int *tmpucl = new int[dimsNum];
	for ( i = 0; i < dimsNum; i++ )
	{
		tmpucl[i] = ucl[i];
	} // 建议使用memcpy或者vector
	for(i = d; i < dimsNum; i++)
	{
		if(ucl[i] == ALL)
		{
			int card = findCard(bPos, ePos, i); 
			//找到基表中第i列中最大的元素，返回元素值加1，也就是集的势，即集合中所含元素的量的多少O(n)
			//Does it need to judge the card?
			int *freq = new int[card];
			Partition(bPos, ePos, i, card, freq); // freq保存就是每个值的初始位置O(n)

			int tmpdflag = dflag;
			for(j = 0; j < card - 1; j ++)
			{
				if(freq[j + 1] > freq[j])
				{
					ucl[i] = data[dflag][ freq[j] ][i]; // 找到了上界
					DFS(ucl, freq[j], freq[j + 1], i);	// 后i个维度，因此深度优先搜索的深度是d
				}
				dflag = tmpdflag;
			}
			delete[] freq;
		}
		for ( j =0; j < dimsNum; j++)
		{
			ucl[j] = tmpucl[j];
		} // 建议使用memcpy或者vector
	
	}
	delete []tmpucl;
	delete []ucl;
	
}

inline int QuotientCube::outputTodimDataFile(int layer, int outputDimData[], int ca)
{
	for (int i=0; i<dimsNum; i++) {
		fprintf(fps[layer], "%d ", outputDimData[i]);
	}
	fprintf(fps[layer], ":(%d)\n", ca);

	return 0;
}

inline int QuotientCube::outputTomsrDataFile(int layer, float outputMsrData[])
{
	if( msrsNum != 0 ) {
		for (int i=0; i<msrsNum; i++) {
			fprintf(fps[dimsNum+1+layer], "%d ", outputMsrData[i]);
		}
		fprintf(fps[layer], "\n");
	}

	return 0;
}

//inline int QuotientCube::outputTodimDataFile(int layer, int outputDimData[])
//{
//	
//	if(fwrite(outputDimData, sizeof(int)*dimsNum, 1, fps[layer]) != 1)
//	{
//		cout<<"write aggregate dimdata error."<<endl;
//		return -1;
//	}
//
//	return 0;
//}
//
//inline int QuotientCube::outputTomsrDataFile(int layer, float outputMsrData[])
//{
//	if( msrsNum != 0 )
//	{
//		if(fwrite(outputMsrData,sizeof(float)*msrsNum,1,fps[dimsNum+1+layer]) != 1)
//		{
//			cout<<"write aggregate msrdata error."<<endl;
//			return -1;
//		}
//	}
//	return 0;
//}

void QuotientCube::preCompute()
{
	int i = 0;
	
	//open aggregate files
	fps = new FILE* [2*(dimsNum + 1)];
	for(i = 0; i < dimsNum + 1; i ++)
	{
		/*string aggDimDataFileName = cubeDir + "\\aggDimData" + itos(i) + ".dat";
		fps[i] = fopen(aggDimDataFileName.c_str(), "wb");*/
		string aggDimDataFileName = cubeDir + "\\aggDimData" + itos(i) + ".txt";
		fps[i] = fopen(aggDimDataFileName.c_str(), "w");
		if(fps[i] == NULL)
		{
			cout<<"can't open aggregate dimData files."<<endl;
			exit(-1);
		}

	}

	for(; i < 2*(dimsNum +1); i++)
	{
		/*string aggMsrDataFileName = cubeDir + "\\aggMsrData" + itos(i-dimsNum-1) + ".dat";
		fps[i] = fopen(aggMsrDataFileName.c_str(),"wb");*/
		string aggMsrDataFileName = cubeDir + "\\aggMsrData" + itos(i-dimsNum-1) + ".txt";
		fps[i] = fopen(aggMsrDataFileName.c_str(),"w");
		if(fps[i] == NULL)
		{
			cout<<"can't open aggregate msrData files."<<endl;
			exit(-1);
		}
	}
	aggsNumPerLayer = new int [dimsNum + 1];
	for(i = 0; i < dimsNum + 1; i ++)
	{
		aggsNumPerLayer[i] = 0;
	}
	
	data[0] = Cubing::data;
	data[1] = new int* [tuplesNum];
	msrdata[0] = Cubing::msrdata;
	//msrdata[1] = new float* [tuplesNum];

	msrValues = new float[msrsNum];

	int *cl = new int[dimsNum];

	for(i = 0; i < dimsNum; i ++)
	{
		cl[i] = ALL;
	}

	DFS(cl, 0, tuplesNum, 0);

	cout<<"the amount of cls of Ca>1 is :"<<caNum<<endl;

	string filename = cubeDir + "\\aggsNumPerLayer.txt";
	FILE *layerFile = fopen( filename.c_str(), "w" );
	if(layerFile == NULL)
	{
		cout<<"can't open aggsNumPerLayer files."<<endl;
		exit(-1);
	}

	for (i=0; i<dimsNum+1; i++) {
		fprintf(layerFile, "%d\n", aggsNumPerLayer[i]);
	}

	/*if(fwrite(aggsNumPerLayer, sizeof(int)*(dimsNum+1), 1, temp) != 1)
	{
		cout<<"write aggsNumPerLayer error."<<endl;
		exit(-1);
	}*/

	fclose(layerFile);
	

	delete[] msrValues;
	delete[] aggsNumPerLayer;
	//delete[] msrdata[1];
	delete[] data[1];
	delete[] cl;

	for(i = 0; i < tuplesNum; i++)
		if(Cubing::data[i]) delete[] Cubing::data[i];
	delete Cubing::data;

	for(i = 0; i < tuplesNum; i++)
		if(Cubing::msrdata[i]) delete[] Cubing::msrdata[i];
	delete Cubing::msrdata;
}

int QuotientCube::getAggsNum()
{
	return uclsNum;
}
