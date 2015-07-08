#pragma once

#include "RasterData.h"

//class __declspec(dllexport) CRasterReadWrite
class CRasterReadWrite
{
public:	
  CRasterReadWrite(void);
  ~CRasterReadWrite(void);

  // 读取栅格数据
   CRasterData* Read(char* fileName);

  bool Write(CRasterData* result,char *resultFileName);
  
  bool CreateFile(CRasterData* result,char * resultFileName, char *FileFormat);    // 创建文件操作
  // 写栅格数据
  void WriteToTxt(char* fileName, CRasterData* source);
};