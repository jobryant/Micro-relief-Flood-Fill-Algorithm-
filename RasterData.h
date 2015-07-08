#pragma once
#include <cmath>

struct CRasterData
{
public:
  CRasterData(double xTopLeft, double yTopLeft, double xCellSize,double yCellSize, int nRow, int nCol, float noDataValue)
  {
	this->_xTopLeft = xTopLeft;
	this->_yTopLeft = yTopLeft;
	this->_xCellSize = xCellSize;
	this->_yCellSize = yCellSize;
	this->_nRow = nRow;
	this->_nCol = nCol;
	this->_noDataValue = noDataValue;
	this->_dataPtr = new float[nRow * nCol];
  }

  // ��ȡ���Ͻ�X����
  double XTopLeft() const { return _xTopLeft; }

  // ��ȡ���Ͻ�Y����
  double YTopLeft() const { return _yTopLeft; }

  // ��ȡX���������С
  double XCellSize() const { return _xCellSize; }

  //��ȡY���������С
  double YCellSize() const { return _yCellSize;}
  // ��ȡ��Чֵ����
  float NoDataValue() const { return _noDataValue; }	

  // ��ȡ����ָ��
  float* DataPtr() const { return _dataPtr; }

  // ��ȡ����
  int NRow() const { return _nRow; }

  // ��ȡ����
  int NCol() const { return _nCol; }

  // �����±��ȡ�߳�ֵ
  float GetValue(int row, int col)
  {
	if (IsValidIndex(row, col))
	{
	  return _dataPtr[col + NCol() * row];
	}
	return NoDataValue();
  }

  float GetData(int row, int col, float &val)
  {
	  if (IsValidIndex(row, col))
	  {
		  val = _dataPtr[col + NCol() * row];
	  }
	  else
	  {
		  val = NoDataValue();
	  }	  
	  return val;
  }

  // �����±����ø߳�ֵ
  void SetValue(int row, int col, float value)
  {
	if (IsValidIndex(row, col))
	{
	  _dataPtr[col + NCol() * row] = value;
	}
  }

  //�ж��Ƿ���NodataValue
  bool IsNodataValue(int row,int col)
  {
     if (fabs(this->GetValue(row,col)-NoDataValue())>0.0001)
         return false;
	 else
	     return true;
  }

  //�ж��±��Ƿ�Ϊ��Чֵ
  bool IsValidIndex(int row, int col)
  {
	if (row < 0 || row >= NRow() || col < 0 || col >= NCol())
	{
	  return false;
	}
	else
	{
	  return true;
	}
  }

  //�趨���Ͻ�X����
  void SetXTopLeft(double X_TopLeft)	
  {
	_xTopLeft = X_TopLeft; 
  }

  // �趨���Ͻ�Y����
  void SetYTopLeft(double Y_topLeft)  
  {
	_yTopLeft = Y_topLeft;
  }

  // �趨X����դ���С
  void SetXCellSize(double cell_size)
  {
	_xCellSize = cell_size; 
  }

  // �趨Y����դ���С
  void SetYCellSize(double cell_size)
  {
	_yCellSize = cell_size; 
  }

  // �趨��Чֵ����
  void SetNoDataValue(float no_data) 
  {
	_noDataValue = no_data;
  }	

  // �趨����
  void SetNRow(int n_row)
  { 
	_nRow = n_row;
  }

  //�趨����
  void SetNCol(int n_col) 
  { 
	_nCol = n_col;
  }
 public:
	    float* _dataPtr;	   //դ���������ݾ���
private:
  double _xTopLeft;    //��ʼ��X����
  double _yTopLeft;    //��ʼ��Y����
  double _xCellSize;    //x���������С
  double _yCellSize;   //Y���������С
  int _nRow;           //դ����������
  int _nCol;           //դ����������
  float _noDataValue;  //դ��������ֵ������

};

