#include "stdafx.h"
#include "globalsv.h"
#include "inclose.h"
#include "BicsUtils.h"

bool readDataset(const string &dataSetName, dataset_t &matrix, row_t &n, col_t &m);
void printData(const dataset_t &matrix, const row_t &n, const col_t &m);

int main(int argc, char* argv[])
{
	
	dataset_t matrix; 
	row_t n; 
	col_t m; 

	
	float tempo;

	string datasetname;
	unsigned short mr;
	unsigned short mc;
	string outputname;
	cout << "please enter datasetname:" << endl;
	cin >> datasetname;
	cout << "please enter minRow:" << endl;
	cin >> mr;
	cout << "please enter minCol:" << endl;
	cin >> mc;
	cout << "please enter outputname:" << endl;
	cin >> outputname;
	
	if (!readDataset(datasetname, matrix, n, m))
	{
		cout << "\nDataset was not loaded!";
		exit(1);
	}
	printf("\nDataset loaded: %dx%d\n\n", n, m);

	//printData(matrix, n, m);

	// openPrintFile(argv[4]);
	openPrintFile(outputname);
	cout << "\nRunning..." << endl;
	// tempo = runInClose(matrix, n, m, minRow, minCol);
	
	
	tempo = runInClose(matrix, n, m, mr, mc);
	closePrintFile();

	cout << "\nRuntime = " << tempo << endl;
	cout << "Number of biclusters = " << g_cont << endl;
	system("pause");
	return 0;
}

bool readDataset(const string &dataSetName, dataset_t &matrix, row_t &n, col_t &m)
{
	ifstream myStream;
	myStream.open(dataSetName, ifstream::in);

	if (!myStream.is_open())
		return false;

	//Discovering the number of rows
	n = count(istreambuf_iterator<char>(myStream), istreambuf_iterator<char>(), '\n');

	//Discovering the number of columns
	data_t dbltmp;
	string line;
	m = 0;
	myStream.seekg(0);
	getline(myStream, line);
	stringstream stream(line);
	while (stream.good())
	{
		stream >> dbltmp;
		++m;
	}

	//Allocating memory
	matrix = new data_t*[n];
	for (row_t i = 0; i < n; ++i)
		matrix[i] = new data_t[m];

	//Storing the data
	myStream.seekg(0);
	for (row_t i = 0; i < n; ++i)
	{
		for (col_t j = 0; j < m; ++j)
			myStream >> matrix[i][j];
	}

	myStream.close();
	return true;
}

void printData(const dataset_t &matrix, const row_t &n, const col_t &m)
{
	for (row_t i = 0; i < n; ++i)
	{
		for (col_t j = 0; j < m; ++j)
			cout << matrix[i][j] << '\t';
		cout << endl;
	}
}
