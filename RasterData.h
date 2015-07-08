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

  // 获取左上角X坐标
  double XTopLeft() const { return _xTopLeft; }

  // 获取左上角Y坐标
  double YTopLeft() const { return _yTopLeft; }

  // 获取X方向格网大小
  double XCellSize() const { return _xCellSize; }

  //获取Y方向格网大小
  double YCellSize() const { return _yCellSize;}
  // 获取无效值类型
  float NoDataValue() const { return _noDataValue; }	

  // 获取数据指针
  float* DataPtr() const { return _dataPtr; }

  // 获取行数
  int NRow() const { return _nRow; }

  // 获取列数
  int NCol() const { return _nCol; }

  // 根据下标获取高程值
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

  // 根据下标设置高程值
  void SetValue(int row, int col, float value)
  {
	if (IsValidIndex(row, col))
	{
	  _dataPtr[col + NCol() * row] = value;
	}
  }

  //判断是否是NodataValue
  bool IsNodataValue(int row,int col)
  {
     if (fabs(this->GetValue(row,col)-NoDataValue())>0.0001)
         return false;
	 else
	     return true;
  }

  //判断下标是否为有效值
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

  //设定左上角X坐标
  void SetXTopLeft(double X_TopLeft)	
  {
	_xTopLeft = X_TopLeft; 
  }

  // 设定左上角Y坐标
  void SetYTopLeft(double Y_topLeft)  
  {
	_yTopLeft = Y_topLeft;
  }

  // 设定X方向栅格大小
  void SetXCellSize(double cell_size)
  {
	_xCellSize = cell_size; 
  }

  // 设定Y方向栅格大小
  void SetYCellSize(double cell_size)
  {
	_yCellSize = cell_size; 
  }

  // 设定无效值类型
  void SetNoDataValue(float no_data) 
  {
	_noDataValue = no_data;
  }	

  // 设定行数
  void SetNRow(int n_row)
  { 
	_nRow = n_row;
  }

  //设定列数
  void SetNCol(int n_col) 
  { 
	_nCol = n_col;
  }
 public:
	    float* _dataPtr;	   //栅格数据数据矩阵
private:
  double _xTopLeft;    //起始点X坐标
  double _yTopLeft;    //起始点Y坐标
  double _xCellSize;    //x方向格网大小
  double _yCellSize;   //Y方向格网大小
  int _nRow;           //栅格数据行数
  int _nCol;           //栅格数据列数
  float _noDataValue;  //栅格数据无值区数据

};

