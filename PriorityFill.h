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
	void Fill_WL(char* inPutFile, char* outputFile);	//优先级队列
	void Fill_FlatPitOptimal(char* inputFile, char* outputFile);	//平行栈
	void Fill_JudgeSentence(char* inputFile,char* outputFile);	//模式优化——见excel
	void Fill_MFF(char* inputFile, char* outputFile);	//模式优化+平行栈
	void FillMV(char* inputFile, char* outputFile);	//M&V算法填洼

	void GetMeta(char* inpurFile);	//获取DEM元数据
	bool FillCheck(char* correctFile);	//对arcgis计算对比验证正确性

	~Fill(void);

private:
	CRasterData* m_InputData;	//DEM数据
	CRasterData* m_DirData;		// 流向矩阵
	FlagData* m_FlagData;		//数据处理标识	0:未处理 1:已处理 2:已入队
	priority_queue<GPoint> m_PriQueue;		//缓存优先级队列
	queue<GPoint> m_PlainQueue;	//优先处理队列

	void WriteLog(string fileDir,string logInfo );	//写日志函数
	bool m_IsBdrPoint(int row, int col);	//判断您是否为边界点
	bool m_IsUselessPoint(int row, int col, int dir);	//在PriFIllPattern中判断是否为有效节点
	bool m_CanFlowPassby(int row, int col, int faRow, int faCol, int grandRow, int grandCol); //待检测栅格能否绕过父节点流出
	bool m_FindOutlet(int row, int col, int faRow, int faCol);	//待判断点周边是否有绕过父节点的出水口；
	string m_DoubleToStr(double lf_num);	//double转换为string
	string m_IntToStr(int d_num);		//int转换成string
	string m_GetFileName(char* fullName);	//获取文件名
	string m_GetDirectory(char* fullName);	//获取目录名
	void setDirection(int row, int col, int dir);	//设置流向

};