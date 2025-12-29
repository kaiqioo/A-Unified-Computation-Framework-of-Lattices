#include "Cubing.h"
#include "QuotientCube.h"
//#include "FullCube.h"

int main(int argc, char* argv[]) //S argc：2，argv[]：test_data.txt的数据
{
	if (argc != 2)
	{
		cout << "cubing <datafile>" << endl;
		exit(-1);
	}
	char* dataFileName = argv[1];//S dataFileName = test_data.txt
	QuotientCube qc;
	Cubing* cubing = &qc;
	//FullCube fc;
	//Cubing *cubing = &fc;
	//cout<<"cubedir:"<<qc.cubeDir()<<endl;
	
	cout << "loading data" << endl;
	cubing->beginTimer();
	cubing->loadData(dataFileName); //S 装载数据，没有返回值
	cubing->endTimer(); //S 计算装载时长
	//cout<<"cubedir:"<<qc.cubeDir()<<endl;
	//cout<<"dimsNum = "<<cubing.dimsNum<<endl;
	//cout<<"tuplesNum = "<<cubing.tuplesNum<<endl;
	cout << "computing Cubing" << endl;
	cubing->beginTimer();
	cubing->preCompute();
	cout << "the amount of cls is :" << cubing->getAggsNum() << endl;
	cubing->endTimer();

	return 0;
}