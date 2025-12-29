#include "stdafx.h"
#include "inclose.h"
#include <cstdlib>


float runInClose(const dataset_t &D, const row_t &n, const col_t &m, const row_t &minRow, const col_t &minCol)
{
	clock_t clocks = clock();
	g_RW = new row_t[n];
	
	pbic_t bic = new bic_t;
	bic->A = new row_t[n];
	for (row_t i = 0; i < n; ++i)
		bic->A[i] = i;
	bic->sizeA = n;
	bic->B = new bool[m];
	bic->Z = new bool[m];
	bic->C = new bool[m];
	for (col_t i = 0; i < m; ++i)
	{
		bic->B[i] = false;
		bic->Z[i] = false;
		bic->C[i] = false;
	}
	bic->sizeB = 0;
	bic->col = 0;

	InClose(D, n, m, minRow, minCol, bic); // call In-Close

	clocks = clock() - clocks;
	return ((float)clocks) / CLOCKS_PER_SEC;
}



void InClose(const dataset_t &D, const row_t& n, const col_t &m, const row_t &minRow, const col_t &minCol, const pbic_t &bic)
{
	queue<pbic_t> children;

	
	for (col_t j = bic->col; j < m; ++j)//£¨j = y;j < m;j++)
	{
		if (bic->sizeA == n-1)
			break;

		if (!bic->B[j] && !bic->Z[j] && !bic->C[j])
		{
			

			row_t sizeRW = 0;
			row_t valueCol = 0;
			for (row_t i = 0; i < bic->sizeA; ++i)
			{
				
				if (D[bic->A[i]][j]) {
					g_RW[sizeRW++] = bic->A[i];

					valueCol++;
				}
			}
			if (valueCol == 0) {
				bic->C[j] = true;
			}

			// "Main routine"
			if (sizeRW >= 0)
			{
				
				if (sizeRW == bic->sizeA)//C = A
				{
					bic->B[j] = true;
					bic->C[j] = true;
					++bic->sizeB;
				}
				
				else if (IsCanonical(D, j, sizeRW, bic))
				{
					
					pbic_t child = new bic_t;			
					child->sizeA = sizeRW;				
					child->A = new row_t[sizeRW];		
					//vector<short> tmp;
					for (row_t i = 0; i < sizeRW; ++i) {	
						child->A[i] = g_RW[i];
						
					}
					child->col = j + 1;					//y = j + 1
					children.push(child);				
				}
				
			}
			else bic->Z[j] = true;
		}
	}

	
	delete[] bic->A;
	
	
	while (!children.empty())
	{
		pbic_t child = children.front();
		child->B = new bool[m];
		child->Z = new bool[m];
		child->C = new bool[m];
	
		for (col_t j = 0; j < m; ++j)
		{
			child->B[j] = bic->B[j];
			child->Z[j] = bic->Z[j];
			child->C[j] = bic->C[j];
			
		}
		child->B[child->col - 1] = true;
		child->sizeB = bic->sizeB + 1;
		InClose(D, n, m, minRow, minCol, child);
		children.pop();

	}
	
	delete[] bic->B;
	delete[] bic->Z;
	delete[] bic->C;
	delete bic;
}

bool IsCanonical(const dataset_t& D, const col_t& y, const row_t& sizeRW, const pbic_t& bic)

{
	row_t i;
	for (col_t j = 0; j < y; ++j)
	{
		if (!bic->B[j] && !bic->Z[j])
		{
			for (i = 0; i < sizeRW; ++i) {
				if (!D[g_RW[i]][j])
					break;
			}
			if (i == sizeRW) {
				return false;
			}
			
		}
	}
	return true;
}
