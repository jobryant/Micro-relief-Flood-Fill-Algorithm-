#include "RasterData.h"
#include "RasterReadWrite.h"
#include <gdal_priv.h>
#include <queue>
#include <stack>
#include <iostream>
#include <sstream>
#include <time.h>
#include "FlagData.h"

using namespace std;

struct GPoint
{
	int Row;
	int Col;
	float Value;
	// priority queue overwrite operate "<"
	friend bool operator < (GPoint a1, GPoint a2)
	{
		return a1.Value > a2.Value;
	}
};

class Fill
{
public:
	void Fill_WL(char* inPutFile, char* outputFile);	//���ȼ�����
	void Fill_FlatPitOptimal(char* inputFile, char* outputFile);	//ƽ��ջ
	void Fill_JudgeSentence(char* inputFile,char* outputFile);	//ģʽ�Ż�������excel
	void Fill_MFF(char* inputFile, char* outputFile);	//ģʽ�Ż�+ƽ��ջ
	void FillMV(char* inputFile, char* outputFile);	//M&V�㷨����

	void GetMeta(char* inpurFile);	//��ȡDEMԪ����
	bool FillCheck(char* correctFile);	//��arcgis����Ա���֤��ȷ��

	~Fill(void);

private:
	CRasterData* m_InputData;	//DEM����
	CRasterData* m_DirData;		// �������
	FlagData* m_FlagData;		//���ݴ����ʶ	0:δ���� 1:�Ѵ��� 2:�����
	priority_queue<GPoint> m_PriQueue;		//�������ȼ�����
	queue<GPoint> m_PlainQueue;	//���ȴ������

	void WriteLog(string fileDir,string logInfo );	//д��־����
	bool m_IsBdrPoint(int row, int col);	//�ж����Ƿ�Ϊ�߽��
	bool m_IsUselessPoint(int row, int col, int dir);	//��PriFIllPattern���ж��Ƿ�Ϊ��Ч�ڵ�
	bool m_CanFlowPassby(int row, int col, int faRow, int faCol, int grandRow, int grandCol); //�����դ���ܷ��ƹ����ڵ�����
	bool m_FindOutlet(int row, int col, int faRow, int faCol);	//���жϵ��ܱ��Ƿ����ƹ����ڵ�ĳ�ˮ�ڣ�
	string m_DoubleToStr(double lf_num);	//doubleת��Ϊstring
	string m_IntToStr(int d_num);		//intת����string
	string m_GetFileName(char* fullName);	//��ȡ�ļ���
	string m_GetDirectory(char* fullName);	//��ȡĿ¼��
	void setDirection(int row, int col, int dir);	//��������

};