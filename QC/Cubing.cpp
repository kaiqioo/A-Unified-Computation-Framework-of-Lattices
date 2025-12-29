#include "Cubing.h"

// Cubing类的构造函数
Cubing::Cubing()
{
    // 初始化维度数量为0
	dimsNum = 0;
    // 初始化度量数量为0
	msrsNum = 0;
    // 初始化元组数量为0
	tuplesNum = 0;
    // 初始化数据指针为NULL
	data = NULL;
    // 初始化度量数据指针为NULL
	msrdata = NULL;

    // 初始化分隔符为一个空格
	delimiter = " ";
    // 初始化数据立方体目录为空字符串
	cubeDir = "";
    // 初始化时间戳起始值为0
	tstart = 0;

    // 杨卓荦更正调用不规范
    // 将聚合函数库的前五个位置分别初始化为sumFun、countFun、avgFun、maxFun和minFun函数的地址
	aggFunLib[0] = &Cubing::sumFun;
	aggFunLib[1] = &Cubing::countFun;
	aggFunLib[2] = &Cubing::avgFun;
	aggFunLib[3] = &Cubing::maxFun;
	aggFunLib[4] = &Cubing::minFun;
}

// Cubing类的析构函数
Cubing::~Cubing()
{
}


/*
  load data from a data file. using array instead of vector will decrease 
  sharply the amount of time because dynamically allocating memory will 
  cause the many data movement. e.g. time consuming drops from 13s to 4.6s.
*/
void Cubing::loadData(string dataFileName)
{
	//get cube dir
	// 获取数据文件名中".txt"的位置
	string::size_type pos = dataFileName.find(".txt");
	// 如果找到".txt"，则提取文件名中".txt"之前的部分作为立方体目录 
	if(pos != string::npos)
		cubeDir = dataFileName.substr(0, pos);
	mkdir(cubeDir.c_str());
	/*
	if(mkdir(cubeDir.c_str()) != 0)  //if exist the file or folder having the same name
	{
		cout<<"can't create the directiory";
		exit(-1);
	}
	*/

	// open data file
	DFHandle df;
	if (!df.Open(dataFileName.c_str())) //判断文件是否open
	{
		cout<<"Error: cannot open file "<<dataFileName<<"."<<endl;
		exit(-1);
	}


	//TID tid;
	FID fid;
	char field[MAXTUPLELEN];
	TupleHandle tHandle(delimiter);

    // 获取文件中的第一个元组
	df.GetNextTuple(tHandle);
    // 遍历元组中的每个字段
	while (tHandle.GetNextField(field, sizeof(field), fid))
	{
		string tmp = field;
        // 查找字段中是否包含"measures."
		int pos = tmp.find("measures.");
		if (pos < 0)
		{
            // 如果不包含"measures."，则认为是维度字段，维度数量加1
			dimsNum++;
		}
		else
		{
            // 如果包含"measures."，则认为是度量字段
			int pos1 = tmp.rfind(",");
			string aggFunType = tmp.substr(pos1 + 1);
            // 根据度量字段的类型确定聚合函数的顺序
			if (aggFunType == "sum")
				aggFunOrder.push_back(0);
			else if (aggFunType == "count")
				aggFunOrder.push_back(1);
			else if (aggFunType == "avg")
				aggFunOrder.push_back(2);
			else if (aggFunType == "max")
				aggFunOrder.push_back(3);
			else if (aggFunType == "min")
				aggFunOrder.push_back(4);
			else
			{
                // 如果度量字段类型未知，默认使用"sum"聚合函数
				aggFunOrder.push_back(0);
			}
            // 度量数量加1
			msrsNum++;
		}
	}


	//count tuples
	//df.ReWind();
	// 计算元组数量
	while (df.GetNextTuple(tHandle))
	{
		tuplesNum++;
	}

	// 定义循环变量
	int i, j, k;

    // 分配数据矩阵的内存，用于存储维度数据
	data = new int* [tuplesNum];
	for (i = 0; i < tuplesNum; i++)
	{
		data[i] = new int[dimsNum]; // S data: [[1,1,1],[1,2,1],[2,1,2]]的矩阵
	}

    // 分配度量数据矩阵的内存，用于存储度量数据
	msrdata = new float* [tuplesNum];
	for (i = 0; i < tuplesNum; i++)
	{
		msrdata[i] = new float[msrsNum];
	}

	#ifdef MAPPER

    // 分配Mapper对象的内存，用于处理维度数据的映射
	Mapper **mapper = new Mapper* [dimsNum]; // 我认为这个地方可以改为Mapper一维数组或者Mapper向量
	for (i = 0; i < dimsNum; i++)
	{
		mapper[i] = new Mapper;
	}

    // 分配card数组的内存，用于存储每个维度的唯一值数量
	int *card = new int[dimsNum]; // 我认为这个地方应该使用memset或者 vector<int> card(dimsNum, 0) 以提高效率
	for (i = 0; i < dimsNum; i++)
	{
		card[i] = 0;
	}
	
	//open map file
	// 分配文件指针数组的内存，用于处理映射文件
	FILE **fpsMap = new FILE* [dimsNum];
	for(i = 0; i < dimsNum; i ++)
	{
		string mapFileName = cubeDir + "\\map" + itos(i) + ".txt";
		fpsMap[i] = fopen(mapFileName.c_str(), "w");
		if(fpsMap[i] == NULL)
		{
			cout<<"can't open map files."<<endl;
			exit(-1);
		}
	}
	
	i = 0; // 初始化数据行索引
	df.ReWind(); // 重置数据文件读取位置到文件开头
	df.GetNextTuple(tHandle);  // 跳过文件的首行（通常是表头行）

	// 逐行读取数据文件中的元组
	while(df.GetNextTuple(tHandle))
	{
		j = 0; // 初始化维度字段索引
		k = 0; // 初始化度量字段索引

		// 逐个字段读取元组中的数据
		while(tHandle.GetNextField(field, sizeof(field), fid)) // 得到了一个字段
		{
			if(j < dimsNum) // 如果当前字段是维度字段
			{
				// 在映射表中查找字段的下界
				Mapper::iterator lb = mapper[j]->lower_bound(field); // lower_bound得到某一个键的下界

				// 如果下界存在，并且当前字段等于下界的键
				if(lb != mapper[j]->end() && !(mapper[j]->key_comp()(field, lb->first)))
				{
					data[i][j] = lb->second; // 使用找到的映射值
				}
				else // 如果当前字段没有找到映射
				{
					card[j] ++; // 维度字段的计数器加一
					// 将新的字段及其对应的值插入映射表中
					mapper[j]->insert(lb, Mapper::value_type(field, card[j]));
					data[i][j] = card[j]; // 使用新的映射值

					// 构造要写入映射文件的数据行
					//string mapperData = string(field) + " " + itos(card[j]) + "\n"; // 原始注释掉的代码，可能用于记录键和值
					string mapperData = string(field) + "\n"; // 只记录键
					// 将数据写入对应的映射文件
					fwrite(mapperData.c_str(), strlen(mapperData.c_str()), 1, fpsMap[j]);
				}
				j ++; // 移动到下一个维度字段
			}
			else if(k < msrsNum) // 如果当前字段是度量字段
			{
				// 将度量字段的值转换为浮点数，并存储在msrdata数组中
				msrdata[i][k] = atof(field);
				k ++; // 移动到下一个度量字段
			}
		}
		i ++; // 移动到下一行数据
	}

	// 下面是测试输出，用于验证维度数量、度量数量和元组数量
	cout << "dimsNum=" << dimsNum << endl; // 输出维度数量
	cout << "msrsNum=" << msrsNum << endl; // 输出度量数量
	cout << "tuplesNum=" << tuplesNum << endl; // 输出元组数量


	// 释放Mapper对象的内存
	for (i = 0; i < dimsNum; i++)
	{
		delete mapper[i]; // 删除每个Mapper对象
	}
	delete[] mapper; // 删除Mapper对象数组本身
	delete[] card; // 删除card数组

    // 关闭并释放映射文件的文件指针
	if (fpsMap)
	{
		for (int i = 0; i < dimsNum; i++)
		{
			fclose(fpsMap[i]); // 关闭每个映射文件
		}
		delete[] fpsMap; // 删除文件指针数组本身
	}


	#else // 如果没有定义MAPPER

	i = 0; // 初始化数据行索引
	df.ReWind(); // 重置数据文件读取位置到文件开头
	df.GetNextTuple(tHandle);  // 跳过文件的首行（通常是表头行）

	// 逐行读取数据文件中的元组
	while(df.GetNextTuple(tHandle))
	{
		j = 0; // 初始化维度字段索引
		k = 0; // 初始化度量字段索引

		// 逐个字段读取元组中的数据
		while(tHandle.GetNextField(field, sizeof(field), fid))
		{
			if(j < dimsNum) // 如果当前字段是维度字段
			{
				data[i][j] = atoi(field); // 将字段值转换为整数并存储在data数组中
				j ++; // 移动到下一个维度字段
			}
			else if(k < msrsNum) // 如果当前字段是度量字段
			{
				msrdata[i][k] = atof(field); // 将字段值转换为浮点数并存储在msrdata数组中
				k ++; // 移动到下一个度量字段
			}
		}
		i ++; // 移动到下一行数据
	}

	#endif // 结束条件编译

    // 关闭数据文件
	df.Close();
}


	

void Cubing::preCompute()
{
}

int Cubing::getAggsNum()
{
	return 0;
}

// 声明了一个名为sumFun的成员函数，用于计算指定范围内的度量值总和
float Cubing::sumFun(float **msrdata, int bPos, int ePos, int msrNum)
{
	// 初始化总和值为0
	float value = 0;
	// 遍历指定范围内的度量数据，将每个度量值累加到总和值中
	for(int i = bPos; i < ePos; i++)
	{
		value += msrdata[i][msrNum]; //度量值总和sum
	}
	// 返回计算得到的总和值
	return value;
}

float Cubing::countFun(float **msrdata, int bPos, int ePos, int msrNum)
{
	return 0;
}

float Cubing::avgFun(float **msrdata, int bPos, int ePos, int msrNum)
{
	return 0;
}

float Cubing::maxFun(float **msrdata, int bPos, int ePos, int msrNum)
{
	/*
	float value = 0;
	for(int i = 0; i < size; i++)
	{
		value > data[i][msrNum] ? value : data[i][msrNum];
	}
	
	return value;
	*/
	return 0;
}

float Cubing::minFun(float **msrdata, int bPos, int ePos, int msrNum)
{
	/*
	float value = 0;
	for(int i = 0; i < size; i++)
	{
		value < data[i][msrNum] ? value : data[i][msrNum];
	}
	
	return value;
	*/
	return 0;
}

// 声明了一个名为beginTimer的成员函数，用于开始计时
void Cubing::beginTimer(string message)
{
	// 获取当前时间并将其存储在tstart中
	tstart = clock();
	cout<<message<<endl;
}

// 声明了一个名为endTimer的成员函数，用于结束计时并输出计时结果
void Cubing::endTimer(string message)
{
	// 计算从开始计时到现在的时间差，并将其转换为秒
	double endTime = (double)((double)clock() - (double)tstart)/
		(double)CLOCKS_PER_SEC;
	cout<<message<<" "<<endTime<<"s."<<endl;
}
