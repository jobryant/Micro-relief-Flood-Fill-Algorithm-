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

	// �����±��ȡ�߳�ֵ
	int GetValue(int row, int col)
	{
		return _dataPtr[col + _nCol * row];		
	}

	// �����±����ø߳�ֵ
	void SetValue(int row, int col, float value)
	{
		_dataPtr[col + _nCol * row] = value;		
	}

private:
	int _nRow;           //դ����������
	int _nCol;           //դ����������
	char* _dataPtr;	   //դ���������ݾ���
};
