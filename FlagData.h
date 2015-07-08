#pragma once
#include <math.h>

struct FlagData 
{
public:
	FlagData(int nRow,int nCol)
	{
		this->_nRow = nRow;
		this->_nCol = nCol;
		this->_dataPtr = new char[nRow * nCol];
	}

	// 根据下标获取高程值
	int GetValue(int row, int col)
	{
		return _dataPtr[col + _nCol * row];		
	}

	// 根据下标设置高程值
	void SetValue(int row, int col, float value)
	{
		_dataPtr[col + _nCol * row] = value;		
	}

private:
	int _nRow;           //栅格数据行数
	int _nCol;           //栅格数据列数
	char* _dataPtr;	   //栅格数据数据矩阵
};
