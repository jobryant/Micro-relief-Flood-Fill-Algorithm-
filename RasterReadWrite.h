#pragma once

#include "RasterData.h"

//class __declspec(dllexport) CRasterReadWrite
class CRasterReadWrite
{
public:	
  CRasterReadWrite(void);
  ~CRasterReadWrite(void);

  // ��ȡդ������
   CRasterData* Read(char* fileName);

  bool Write(CRasterData* result,char *resultFileName);
  
  bool CreateFile(CRasterData* result,char * resultFileName, char *FileFormat);    // �����ļ�����
  // дդ������
  void WriteToTxt(char* fileName, CRasterData* source);
};