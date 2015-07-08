#include "RasterReadWrite.h"

#include <gdal_priv.h>
#pragma comment(lib,"gdal_i.lib")
using namespace std;

CRasterReadWrite::CRasterReadWrite(void){};
CRasterReadWrite::~CRasterReadWrite(void){};



//读数据
CRasterData* CRasterReadWrite::Read(char* fileName)
{
  GDALAllRegister();     //注册数据类型驱动
  GDALDataset *poDataset = ( GDALDataset* )GDALOpen(fileName, GA_ReadOnly);  //打开数据
  if ( poDataset == NULL )
  {
	GDALDestroyDriverManager();
	return NULL;
  }
  int nCol = poDataset->GetRasterXSize();    //获取列数
  int nRow = poDataset->GetRasterYSize();    //获取行数

  double xTopLeft, yTopLeft;
  double cellSizeX; 
  double cellSizeY;
  double adfGeoTransform[6];
  if ( poDataset->GetGeoTransform( adfGeoTransform ) == CE_None )
  {
	xTopLeft = adfGeoTransform[0];      
	cellSizeX = adfGeoTransform[1];
    //adfGeoTransform[2] /* rotation, 0 if image is "north up" */
	yTopLeft = adfGeoTransform[3];
    cellSizeY=adfGeoTransform[5];
   // adfGeoTransform[4] /* rotation, 0 if image is "north up" */
  }
  else
  {
	GDALClose(poDataset);
	GDALDestroyDriverManager();
	return NULL;
  }

  GDALRasterBand *poBand = poDataset->GetRasterBand(1);	    //获取第一波段数据
  float* pafScanline = (float*)CPLMalloc(sizeof(float)*nCol*nRow);
  poBand->RasterIO( GF_Read, 0,0,nCol,nRow,pafScanline, nCol, nRow, GDT_Float32, 0, 0 );

  float noDataVaue = (float)poBand->GetNoDataValue();	  
  CRasterData *raster = new CRasterData(xTopLeft, yTopLeft, cellSizeX,cellSizeY,nRow, nCol, noDataVaue );   //结构体初始化
  
  for (int row = 0; row < nRow; row++)
  {
	for ( int col = 0; col < nCol; col++ )
	{
	  float v = pafScanline[row*nCol+col];
	  raster->SetValue( row, col, v );
	}
  }

  CPLFree(pafScanline);
  pafScanline=NULL;
  GDALClose(poDataset);
  poDataset=NULL;
  GDALDestroyDriverManager();
  return raster;
}

//写数据
bool CRasterReadWrite::Write(CRasterData* result,char *resultFileName)
{
  GDALAllRegister();     //注册数据类型驱动
  GDALDataset *poDataset = ( GDALDataset* )GDALOpen(resultFileName, GA_Update);  //打开数据
  if ( poDataset == NULL )
  {
	  //创建结果
	  CreateFile(result,resultFileName,(char*)"GTiff");
	  poDataset = ( GDALDataset* )GDALOpen(resultFileName, GA_Update);  //打开数据
  }
  GDALRasterBand *poBand = poDataset->GetRasterBand(1);	    //获取第一波段数据
  poBand->SetNoDataValue(result->NoDataValue());
  float* pafScanline = (float*)CPLMalloc(sizeof(float)* result->NRow()*result->NCol());
  for (int row = 0; row < result->NRow(); row++)
  {
	for ( int col = 0; col < result->NCol(); col++ )
	{
	  float v = result->GetValue(row,col);
	   pafScanline[row*result->NCol()+col]=v;
	}
  }

  poBand->RasterIO( GF_Write, 0,0,result->NCol(),result->NRow(),pafScanline, result->NCol(),result->NRow(), GDT_Float32, 0, 0 );
  poBand->SetNoDataValue(result->NoDataValue());
  CPLFree(pafScanline);
  GDALClose(poDataset);
  poDataset=NULL;
  GDALDestroyDriverManager();
  return true;
}

// 创建文件操作
bool CRasterReadWrite::CreateFile(CRasterData* result,char * resultFileName, char *FileFormat)
{
	const char *pszFormat = (char*)"GTiff";//GTiff
	GDALDriver *poDriver;
	char **papszMetadata;
	GDALAllRegister() ;
	poDriver = GetGDALDriverManager()->GetDriverByName(pszFormat);

	if( poDriver == NULL )
	{
	  GDALDestroyDriverManager() ;
	  return false;
	}
	  
	papszMetadata = poDriver->GetMetadata();
	   
	char **papszOptions = NULL;
	GDALDataset *poDstDS  = poDriver->Create( resultFileName, result->NCol(), result->NRow(), 1, GDT_Float32, papszOptions );

	double adfGeoTransform[6] = {result->XTopLeft(), result->XCellSize(), 0, result->YTopLeft(), 0,result->YCellSize() };
	poDstDS->SetGeoTransform(adfGeoTransform);

	GDALClose(poDstDS);
	poDstDS=NULL;
	GDALDestroyDriverManager;
	return true;
}
