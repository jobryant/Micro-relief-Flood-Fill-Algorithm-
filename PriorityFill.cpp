#include "PriorityFill.h"
#include <iomanip>
void Fill::Fill_WL( char* inputFile, char* outputFile )
{ 
	CRasterReadWrite rw;
	m_InputData = rw.Read(inputFile);
	m_FlagData = new FlagData(m_InputData->NRow(),m_InputData->NCol());
	

	if (m_FlagData == NULL)
	{
		cout<<"�����޷���ȷ��ȡ���ݣ� "<<endl;
	}

	clock_t startTime,endTime, outlineEndTme;
	int nRows = m_InputData->NRow();
	int nCols = m_InputData->NCol();
	float NODATA_C = m_InputData->NoDataValue();
	double totalTime,fillTime;	//��ʱ�䣬����ʱ��
	string strAlgorithm,strTotalTime, strFillTime, strOptiPoints, strValuePoints ;	//�����㷨����ʱ�䣬����ʱ�䣬�Ż�դ��������դ����
	int nOptiCells=0;	//�Ż������
	int nValueCells=0;	//��ֵդ����
	string outputDir=m_GetDirectory(outputFile);	//���Ŀ¼
	string logInfo;	//��־��Ϣ

	startTime=clock(); //��ʼ��ʱ

	//��ʼ����Ǿ��󣬲����߽���� 0��δ���� | 1���Ѵ���
	for (int row = 0; row < nRows; row++)
	{
		for (int col = 0; col < nCols; col++)
		{
			float midValue=m_InputData->GetValue(row,col);
			if (midValue==NODATA_C)	continue;
			nValueCells++;
			if (m_IsBdrPoint(row,col))
			{
				GPoint newPt;
				newPt.Row=row;
				newPt.Col=col;
				newPt.Value=midValue;
				m_PriQueue.push(newPt);
				m_FlagData->SetValue(row,col,1);
			}
			else
			{
				m_FlagData->SetValue(row,col,0);
			}			
		}
	}	//end for for

	outlineEndTme=clock();

	//����
	while (!m_PriQueue.empty())
	{
		GPoint pt= m_PriQueue.top();
		m_PriQueue.pop();

		for (int row=pt.Row-1; row<=pt.Row+1; row++)
		{
			for (int col=pt.Col-1; col<=pt.Col+1; col++)
			{
				float crntValue=m_InputData->GetValue(row,col);
				if (row==pt.Row && col== pt.Col) continue;
				if (crntValue==NODATA_C)	continue;

				// ��δ����դ�����
				if (m_FlagData->GetValue(row,col) == 0)
				{
					float midValue = m_InputData->GetValue(pt.Row, pt.Col);

					// �ݵص��
					if (crntValue<midValue)
					{
						m_InputData->SetValue(row, col, midValue);
					}

					// ��� 
					GPoint newPt;
					newPt.Row=row;
					newPt.Col=col;
					newPt.Value=crntValue;
					m_PriQueue.push(newPt);
					m_FlagData->SetValue(row,col,1);
				}				
			}
		}	// end for for		
	}	// end while

	endTime=clock();	// ������ʱ 
	totalTime=(double)(endTime-startTime)/CLOCKS_PER_SEC;
	fillTime=(double)(endTime-outlineEndTme)/CLOCKS_PER_SEC;

	if (m_FlagData != NULL)
	{
		delete m_FlagData;	//���ٱ�Ǿ���
		m_FlagData=NULL;
	}


	//������ 
	strAlgorithm="PriFill";
	strTotalTime=m_DoubleToStr(totalTime);
	strFillTime=m_DoubleToStr(fillTime);
	strOptiPoints=m_IntToStr(nOptiCells);
	strValuePoints=m_IntToStr(nValueCells);
	logInfo= strAlgorithm+" "+m_GetFileName(inputFile)+" "+strTotalTime+" "+strFillTime+" "+strOptiPoints+" "+strValuePoints;
	cout<<logInfo<<endl;
	WriteLog(outputDir,logInfo);

	if (outputFile[0]!='\0')
	{
		rw.CreateFile(m_InputData,outputFile,(char*)"GTiff");
		rw.Write(m_InputData,outputFile);
	}
}

void Fill::Fill_FlatPitOptimal( char* inputFile, char* outputFile )
{
	CRasterReadWrite rw;
	m_InputData=rw.Read(inputFile);
	m_FlagData = new FlagData(m_InputData->NRow(),m_InputData->NCol());

	if (m_InputData==NULL)
	{
		cout<<"������������"<<endl;
		return;
	}

	const int nRows=m_InputData->NRow();
	const int nCols=m_InputData->NCol();
	const float NODATA_C=m_InputData->NoDataValue();
	clock_t stTime,endTime,endMarkTime;
	double totalTime,fillTime;	//��ʱ�䣬����ʱ��
	string strAlgorithm,strTotalTime, strFillTime, strOptiPoints, strValuePoints ;	//�����㷨����ʱ�䣬����ʱ�䣬�Ż�դ��������դ����
	int nOptiCells=0;	//�Ż������
	int nValueCells=0;	//��ֵդ����
	string outputDir=m_GetDirectory(outputFile);	//���Ŀ¼
	string logInfo;	//��־��Ϣ

	stTime=clock();

	//��ʼ����Ǿ��󣬲����߽����
	for (int row = 0; row < nRows; row++)
	{
		for (int col = 0; col < nCols; col++)
		{
			float midValue=m_InputData->GetValue(row,col);
			if (midValue==NODATA_C)	continue;
			nValueCells++;
			if (m_IsBdrPoint(row,col))
			{
				GPoint newPt;
				newPt.Row=row;
				newPt.Col=col;
				newPt.Value=midValue;

				m_PriQueue.push(newPt);
				m_FlagData->SetValue(row,col,1);
			}
			else
			{
				m_FlagData->SetValue(row,col,0);
			}			
		}
	}	//end for for

	endMarkTime=clock();

	while(true)
	{
		if (!m_PlainQueue.empty())
		{
			GPoint fatherPt=m_PlainQueue.front();
			m_PlainQueue.pop();
			for (int row=fatherPt.Row-1;row<=fatherPt.Row+1;row++)
			{
				for (int col=fatherPt.Col-1;col<=fatherPt.Col+1;col++)
				{
					float crntValue=m_InputData->GetValue(row,col);
					if (m_FlagData->GetValue(row,col)==1)	continue;
					if (crntValue==NODATA_C)	continue;

					GPoint newPt;
					newPt.Row=row;
					newPt.Col=col;
					newPt.Value=crntValue;

					if (crntValue<=fatherPt.Value)
					{	//���� ���������ȴ������

						newPt.Value=fatherPt.Value;
						m_InputData->SetValue(row,col,fatherPt.Value);
						m_FlagData->SetValue(row,col,1);
						m_PlainQueue.push(newPt);
						nOptiCells++;
					}
					else
					{	//ֱ�ӽ����ȼ�����
						m_FlagData->SetValue(row,col,1);
						m_PriQueue.push(newPt);
					}					
				}
			}	// end for for			
		}
		else if (!m_PriQueue.empty())
		{
			GPoint fatherPt=m_PriQueue.top();
			m_PriQueue.pop();
			for (int row=fatherPt.Row-1;row<=fatherPt.Row+1;row++)
			{
				for (int col=fatherPt.Col-1;col<=fatherPt.Col+1;col++)
				{
					float crntValue=m_InputData->GetValue(row,col);
					if (crntValue==NODATA_C)	continue;
					if (m_FlagData->GetValue(row,col)==1)	continue;

					GPoint newPt;
					newPt.Row=row;
					newPt.Col=col;
					newPt.Value=crntValue;

					if (crntValue<=fatherPt.Value)
					{	//���� ���������ȴ������

						newPt.Value=fatherPt.Value;
						m_InputData->SetValue(row,col,fatherPt.Value);
						m_FlagData->SetValue(row,col,1);
						m_PlainQueue.push(newPt);
						nOptiCells++;
					}
					else
					{	//ֱ�ӽ����ȼ�����
						m_FlagData->SetValue(row,col,1);
						m_PriQueue.push(newPt);
					}					
				}
			}	// end for for			
		}
		else
		{
			break;
		}
	}	//end while

	endTime=clock();	// ������ʱ
	//ʱ�����
	fillTime=(double)(endTime-endMarkTime)/CLOCKS_PER_SEC;
	totalTime=(double)(endTime-stTime)/CLOCKS_PER_SEC;

	if (m_FlagData != NULL)
	{
		delete m_FlagData;	//���ٱ�Ǿ���
		m_FlagData = NULL;
	}



	//������ 
	strAlgorithm="FillPlainQueue";
	strTotalTime=m_DoubleToStr(totalTime);
	strFillTime=m_DoubleToStr(fillTime);
	strOptiPoints=m_IntToStr(nOptiCells);
	strValuePoints=m_IntToStr(nValueCells);
	logInfo= strAlgorithm+" "+m_GetFileName(inputFile)+" "+strTotalTime+" "+strFillTime+" "+strOptiPoints+" "+strValuePoints;
	cout<<logInfo<<endl;
	WriteLog(outputDir,logInfo.data());

	if (outputFile[0]!='\0')
	{
		rw.CreateFile(m_InputData,outputFile,(char*)"GTiff");
		rw.Write(m_InputData,outputFile);
	}
}

void Fill::Fill_JudgeSentence( char* inputFile,char* outputFile )
{
	CRasterReadWrite rw;
	m_InputData=rw.Read(inputFile);
	m_FlagData = new FlagData(m_InputData->NRow(),m_InputData->NCol());	

	if (m_InputData==NULL)
	{
		cout<<"�����ļ���ȡ����"<<endl;
		return;
	}

	float NODATA_C=m_InputData->NoDataValue();
	int nRows=m_InputData->NRow();
	int nCols=m_InputData->NCol();
	time_t stTime,endTime,endEdge;	//��ʼʱ�䣬�ܽ���ʱ�䣬�߽���ҽ���ʱ��
	double	totalTime,fillTime;	//��ʱ�䣬����ʱ��
	string strAlgorithm,strTotalTime, strFillTime, strOptiPoints, strValuePoints ;	//�����㷨����ʱ�䣬����ʱ�䣬�Ż�դ��������դ����
	int nOptiCells=0;	//�Ż������
	int nValueCells=0;	//��ֵդ����
	string outputDir=m_GetDirectory(outputFile);	//���Ŀ¼
	string logInfo;	//��־��Ϣ
	stTime=clock();

	//��ʼ����Ǿ��󣬲����߽����
	for (int row = 0; row < nRows; row++)
	{
		for (int col = 0; col < nCols; col++)
		{
			float midValue=m_InputData->GetValue(row,col);
			if (midValue==NODATA_C)	continue;
			nValueCells++;
			if (m_IsBdrPoint(row,col))
			{
				GPoint newPt;
				newPt.Row=row;
				newPt.Col=col;
				newPt.Value=midValue;

				m_PriQueue.push(newPt);
				m_FlagData->SetValue(row,col,1);
			}
			else
			{
				m_FlagData->SetValue(row,col,0);
			}			
		}
	}	//end for for

	endEdge=clock();

	//����
	while(!m_PriQueue.empty())
	{
		GPoint fatherPt=m_PriQueue.top();
		m_PriQueue.pop();
		int dir=0;
		for (int row=fatherPt.Row-1;row<=fatherPt.Row+1;row++)
		{
			for (int col=fatherPt.Col-1;col<=fatherPt.Col+1;col++)
			{
				dir++;
				float crntValue=m_InputData->GetValue(row,col);
				if (crntValue==NODATA_C)	continue;
				if (m_FlagData->GetValue(row,col)==1)	continue;

				GPoint newPt;
				newPt.Row=row;
				newPt.Col=col;
				newPt.Value=crntValue;

				if (crntValue<=fatherPt.Value)
				{	//���� ���������ȴ������
					newPt.Value=fatherPt.Value;
					m_InputData->SetValue(row,col,fatherPt.Value);
					m_FlagData->SetValue(row,col,1);
					m_PriQueue.push(newPt);
					continue;
				}

				bool isUselessPoint=false;
				float BrotherVal[4];	// [0] leftBrother | [1] rightBrother | [2] leftSubBrother | [3] rightSubBrother
				switch(dir)
				{
				case 1:
					BrotherVal[0]=m_InputData->_dataPtr[(row+1)*nCols+col];
					BrotherVal[1]=m_InputData->_dataPtr[row*nCols+col+1];
					BrotherVal[2]=m_InputData->_dataPtr[row*nCols+col-1];
					BrotherVal[3]=m_InputData->_dataPtr[(row-1)*nCols+col];
					if (BrotherVal[0]<crntValue && BrotherVal[1]<crntValue)
					{
						if (BrotherVal[2]<=crntValue || BrotherVal[3]<=crntValue)
						{
							isUselessPoint = true;
						}
					}
					break;
				case 3:
					BrotherVal[0]=m_InputData->_dataPtr[row*nCols+col-1];
					BrotherVal[1]=m_InputData->_dataPtr[(row+1)*nCols+col];
					BrotherVal[2]=m_InputData->_dataPtr[(row-1)*nCols+col];
					BrotherVal[3]=m_InputData->_dataPtr[row*nCols+col+1];
					if (BrotherVal[0]<crntValue && BrotherVal[1]<crntValue)
					{
						if (BrotherVal[2]<=crntValue || BrotherVal[3]<=crntValue)
						{
							isUselessPoint = true;
						}
					}
					break;
				case 7:
					BrotherVal[0]=m_InputData->_dataPtr[(row-1)*nCols+col];
					BrotherVal[1]=m_InputData->_dataPtr[row*nCols+col+1];
					BrotherVal[2]=m_InputData->_dataPtr[row*nCols+col-1];
					BrotherVal[3]=m_InputData->_dataPtr[(row+1)*nCols+col];
					if (BrotherVal[0]<crntValue && BrotherVal[1]<crntValue)
					{
						if (BrotherVal[2]<=crntValue || BrotherVal[3]<=crntValue)
						{
							isUselessPoint = true;
						}
					}
					break;
				case 9:
					BrotherVal[0]=m_InputData->_dataPtr[row*nCols+col-1];
					BrotherVal[1]=m_InputData->_dataPtr[(row-1)*nCols+col];
					BrotherVal[2]=m_InputData->_dataPtr[(row+1)*nCols+col];
					BrotherVal[3]=m_InputData->_dataPtr[row*nCols+col+1];
					if (BrotherVal[0]<crntValue && BrotherVal[1]<crntValue)
					{
						if (BrotherVal[2]<=crntValue || BrotherVal[3]<=crntValue)
						{
							isUselessPoint = true;
						}
					}
					break;
				case 2:
					BrotherVal[0]=m_InputData->_dataPtr[row*nCols+col-1];
					BrotherVal[1]=m_InputData->_dataPtr[row*nCols+col+1];
					if (BrotherVal[0]<crntValue && BrotherVal[1]<crntValue)
					{
						isUselessPoint = true;
					}
					break;
				case 4:
					BrotherVal[0]=m_InputData->_dataPtr[(row-1)*nCols+col];
					BrotherVal[1]=m_InputData->_dataPtr[(row+1)*nCols+col];
					if (BrotherVal[0]<crntValue && BrotherVal[1]<crntValue)
					{
						isUselessPoint = true;
					}
					break;
				case 6:
					BrotherVal[0]=m_InputData->_dataPtr[(row-1)*nCols+col];
					BrotherVal[1]=m_InputData->_dataPtr[(row+1)*nCols+col];
					if (BrotherVal[0]<crntValue && BrotherVal[1]<crntValue)
					{
						isUselessPoint = true;
					}
					break;
				case 8:
					BrotherVal[0]=m_InputData->_dataPtr[row*nCols+col-1];
					BrotherVal[1]=m_InputData->_dataPtr[row*nCols+col+1];
					if (BrotherVal[0]<crntValue && BrotherVal[1]<crntValue)
					{
						isUselessPoint = true;
					}
					break;
				}
				

				if(isUselessPoint == true)
				{	
					m_FlagData->SetValue(row,col,1);
					nOptiCells++;
					continue;
				}					
				else
				{	
					//ֱ�ӽ������ȼ�����
					m_FlagData->SetValue(row,col,1);
					m_PriQueue.push(newPt);					
				}
			}
		}	// end for for			
	}	//end while

	endTime=clock();	// ������ʱ
	//ʱ��ͳ��
	totalTime=(double)(endTime-stTime)/CLOCKS_PER_SEC;
	fillTime=(double)(endTime-endEdge)/CLOCKS_PER_SEC;

	if (m_FlagData != NULL)
	{
		delete m_FlagData;	//���ٱ�Ǿ���
		m_FlagData = NULL;
	}

	//������ 
	strAlgorithm="FillPattern";
	strTotalTime=m_DoubleToStr(totalTime);
	strFillTime=m_DoubleToStr(fillTime);
	strOptiPoints=m_IntToStr(nOptiCells);
	strValuePoints=m_IntToStr(nValueCells);
	logInfo= strAlgorithm+" "+m_GetFileName(inputFile)+" "+strTotalTime+" "+strFillTime+" "+strOptiPoints+" "+strValuePoints;
	cout<<logInfo<<endl;

	WriteLog(outputDir,logInfo);
	
	if (outputFile[0]!='\0')
	{
		rw.CreateFile(m_InputData,outputFile,(char*)"GTiff");
		rw.Write(m_InputData,outputFile);
	}
}



void Fill::GetMeta( char* inpurFile )
{
	CRasterReadWrite rw;
	m_InputData=rw.Read(inpurFile);

	if (m_InputData==NULL)
	{
		return;
	}
	int nRows;
	int nCols;
	nRows=m_InputData->NRow();
	nCols=m_InputData->NCol();
	cout<<"Rows:"<<nRows<<", Cols:"<<nCols<<endl;
}

Fill::~Fill( void )
{
	if (m_InputData!=NULL)
	{
		delete m_InputData;
	}
	if (m_FlagData!=NULL)
	{
		delete m_FlagData;
	}
}

void Fill::WriteLog(string fileDir,string logInfo )
{
	time_t timer;
	char bufferTime[20];
	time(&timer);
	strftime(bufferTime,20,"%Y-%m-%d %H:%M:%S",localtime(&timer));
	//FILE* fid=fopen("/opt/HaiBao/Info.log","a+");
	string LogFilePath=fileDir+"myInfo.Log";
	FILE* fid=fopen(LogFilePath.data(),"a+");//a+:û�ҵ�myInfo.log�ļ����򴴽���myInfo.log�����ִ���ļ�(Debug,ARM,Relese)��ͬһĿ¼
	if(fid==NULL)
	{
		printf("the myInfo.log is not exist!!!\n");
		return;
	}

	fseek(fid,0L,SEEK_END);	//��λ���ļ�ĩβ
	if (ftell(fid)==0)
	{
		fprintf(fid,"Date Time Algorithm DataName TotalTime FillTime OptiPoints TotalPoints\n");
	}	
	
	fprintf(fid,"%s ",bufferTime);
	fprintf(fid,"%s",logInfo.data());
	fprintf(fid,"\n");
	fclose(fid);
}

string Fill::m_DoubleToStr( double lf_num)
{
	string str;
	stringstream ss;
	ss<<lf_num;
	ss>>str;
	return str;
}

string Fill::m_IntToStr( int d_num )
{
	string str;
	stringstream ss;
	ss<<d_num;
	ss>>str;
	return str;
}

bool Fill::m_IsBdrPoint( int row, int col )
{
	const float _C_NODATA=m_InputData->NoDataValue();
	for (int r=row-1;r<=row+1;r++)
	{
		for (int c=col-1;c<=col+1;c++)
		{
			if (m_InputData->GetValue(r,c)==_C_NODATA)
			{
				return true;
			}
		}
	}	//end for for
	return false;
}

/*
*	1	2	3	
*	4	o	6	
*	7	8	9
*/
bool Fill::m_IsUselessPoint( int row, int col, int dir )
{
	float crntValue=m_InputData->GetValue(row, col);
	float BrotherVal[4];	// [0] leftBrother | [1] rightBrother | [2] leftSubBrother | [3] rightSubBrother
	switch(dir)
	{
	case 1:
		BrotherVal[0]=m_InputData->GetValue(row+1,col);
		BrotherVal[1]=m_InputData->GetValue(row,col+1);
		BrotherVal[2]=m_InputData->GetValue(row,col-1);
		BrotherVal[3]=m_InputData->GetValue(row-1,col);
		if (BrotherVal[0]<crntValue && BrotherVal[1]<crntValue)
		{
			if (BrotherVal[2]<=crntValue || BrotherVal[3]<=crntValue)
			{
				return true;
			}
		}
		break;
	case 3:
		BrotherVal[0]=m_InputData->GetValue(row,col-1);
		BrotherVal[1]=m_InputData->GetValue(row+1,col);
		BrotherVal[2]=m_InputData->GetValue(row-1,col);
		BrotherVal[3]=m_InputData->GetValue(row,col+1);
		if (BrotherVal[0]<crntValue && BrotherVal[1]<crntValue)
		{
			if (BrotherVal[2]<=crntValue || BrotherVal[3]<=crntValue)
			{
				return true;
			}
		}
		break;
	case 7:
		BrotherVal[0]=m_InputData->GetValue(row-1,col);
		BrotherVal[1]=m_InputData->GetValue(row,col+1);
		BrotherVal[2]=m_InputData->GetValue(row,col-1);
		BrotherVal[3]=m_InputData->GetValue(row+1,col);
		if (BrotherVal[0]<crntValue && BrotherVal[1]<crntValue)
		{
			if (BrotherVal[2]<=crntValue || BrotherVal[3]<=crntValue)
			{
				return true;
			}
		}
		break;
	case 9:
		BrotherVal[0]=m_InputData->GetValue(row,col-1);
		BrotherVal[1]=m_InputData->GetValue(row-1,col);
		BrotherVal[2]=m_InputData->GetValue(row+1,col);
		BrotherVal[3]=m_InputData->GetValue(row,col+1);
		if (BrotherVal[0]<crntValue && BrotherVal[1]<crntValue)
		{
			if (BrotherVal[2]<=crntValue || BrotherVal[3]<=crntValue)
			{
				return true;
			}
		}
		break;
	case 2:
		BrotherVal[0]=m_InputData->GetValue(row,col-1);
		BrotherVal[1]=m_InputData->GetValue(row,col+1);
		if (BrotherVal[0]<crntValue && BrotherVal[1]<crntValue)
		{
			return true;
		}
		break;
	case 4:
		BrotherVal[0]=m_InputData->GetValue(row-1,col);
		BrotherVal[1]=m_InputData->GetValue(row+1,col);
		if (BrotherVal[0]<crntValue && BrotherVal[1]<crntValue)
		{
			return true;
		}
		break;
	case 6:
		BrotherVal[0]=m_InputData->GetValue(row-1,col);
		BrotherVal[1]=m_InputData->GetValue(row+1,col);
		if (BrotherVal[0]<crntValue && BrotherVal[1]<crntValue)
		{
			return true;
		}
		break;
	case 8:
		BrotherVal[0]=m_InputData->GetValue(row,col-1);
		BrotherVal[1]=m_InputData->GetValue(row,col+1);
		if (BrotherVal[0]<crntValue && BrotherVal[1]<crntValue)
		{
			return true;
		}
		break;
	}
	return false;
}




//************************************
// Method: m_CanFlowPassby
// FullName: Fill::m_CanFlowPassby
// Discription: �����դ���ܷ��ƹ����ڵ�����
// Author: Ruqiao Jiang ( jiangruqiao@gmail.com )
// Date: 2014/04/24
// Access: private 
// Returns: <bool>
// Parameter: <int row>���жϵ�
// Parameter: <int col>���жϵ�
// Parameter: <int faRow>���ڵ㣨�ܷ��ƹ����ڵ㣩
// Parameter: <int faCol>���ڵ㣨�ܷ��ƹ����ڵ㣩
// Parameter: <int grandRow>�游�ڵ㣨���ڿ���������Χ��
// Parameter: <int grandCol>�游�ڵ㣨���ڿ���������Χ��
//************************************
bool Fill::m_CanFlowPassby( int row, int col, int faRow, int faCol, int grandRow, int grandCol )
{
	int minR=faRow;
	int minC=faCol;
	float minValue=m_InputData->GetValue(minR,minC);
	float faterValue=m_InputData->GetValue(faRow,faCol);	//���ڵ�ֵ

	for (int r=row-1;r<=row+1;r++)
	{	//�ܷ����դ����ֵܽڵ�ֱ������
		for (int c=col-1;c<=col+1;c++)
		{	
			if (m_InputData->IsNodataValue(r,c)) continue;			
			if (r==faRow && c==faCol) continue;
			if (m_FlagData->GetValue(r,c)==1)
			{	//�������դ������
				
				if (m_InputData->GetValue(r,c) <= faterValue)
				{
					return true;
				}
			}
			if (abs(r-grandRow)<=1 && abs(c-grandCol)<=1)
			{	//���ֵܽڵ�����
				if (m_InputData->GetValue(r,c) < faterValue)
				{
					return true;
				}
			}
		}
	}

	return false;	
// 	for (int r=row-1;r<=row+1;r++)
// 	{	//�ܷ��ƹ�����������
// 		for (int c=col-1;c<=col+1;c++)
// 		{
// 			if (m_InputData->IsNodataValue(r,c)) continue;
// 			if (r>=grandRow -2 && r<=grandRow +2 && c>=grandCol-2 && c<=grandCol+2)
// 			{
// 				float crntValue=m_InputData->GetValue(r,c);
// 				if (crntValue < minValue)
// 				{
// 					minR=r;
// 					minC=c;
// 					minValue=crntValue;
// 				}	
// 			}			
// 		}
// 	}	//end for for
// 
// 	if (minR==row && minC==col)
// 	{	
// 		return false;		
// 	}
// 	else if (minR == faRow && minC == faCol)
// 	{
// 		return false;
// 	}
// 	else 
// 	{
// 		return m_CanFlowPassby(minR, minC, faRow, faCol, grandRow, grandCol);
// 	}	
}

bool Fill::m_FindOutlet( int row, int col, int faRow, int faCol )
{
	int minR=row;
	int minC=col;
	float minValue=m_InputData->GetValue(minR,minC);

	for (int r=row-1;r<=row+1;r++)
	{
		for (int c=col-1;c<=col+1;c++)
		{
			if (m_FlagData->GetValue(r,c)==1)
			{
				float crntValue=m_InputData->GetValue(r,c);
				if (crntValue<minValue)
				{
					minR=r;
					minC=c;
					minValue=crntValue;
				}				
			}			
		}		
	}	//end for for

	if (minR==row && minC==col)
	{
		return false;
	}
	else
		return true;
}

string Fill::m_GetFileName( char* fullName )
{
	if (fullName[0]=='\0')
	{
		return string("");
	}	
	string str(fullName);
	int nPos=str.find_last_of('/');
	if (nPos=-1)
	{
		nPos=str.find_last_of('\\');
	}
	
	str.erase(0,nPos+1);
	return str;
}

string Fill::m_GetDirectory(char* fullName)
{
	if (fullName[0]=='\0')
	{		
		return string("");
	}	
	string str(fullName);
	int nPos=str.find_last_of('/');
	if (nPos==-1)
	{
		nPos=str.find_last_of('\\');
	}
	str=str.substr(0,nPos+1);
	return str;
}

bool Fill::FillCheck( char* correctFile)
{
	CRasterData* correctData;
	CRasterReadWrite rw;
	correctData = rw.Read(correctFile);
	
	int nRows=correctData->NRow();
	int nCols=correctData->NCol();
	float NODATA_C=correctData->NoDataValue();

	for (int r = 0; r < nRows; r++)
	{
		for (int c = 0;c < nCols; c++)
		{
			if (!m_InputData->IsNodataValue(r,c))
			{
				if (m_InputData->GetValue(r,c)-correctData->GetValue(r,c)>=0.0001)
				{
					cout<<"Result is Error!"<<endl;
					return false;
				}				
			}			
		}		
	}
	cout<<"Result is Correct!"<<endl;
	return true;
	
}

void Fill::Fill_MFF(char* inputFile, char* outputFile)
{
	CRasterReadWrite rw;

	m_InputData=rw.Read(inputFile);
	m_FlagData = new FlagData(m_InputData->NRow(),m_InputData->NCol());

	if (m_InputData==NULL)
	{
		cout<<"������������"<<endl;
		return;
	}

	const int nRows=m_InputData->NRow();
	const int nCols=m_InputData->NCol();
	const float NODATA_C=m_InputData->NoDataValue();
	clock_t stTime,endTime,endMarkTime;
	double totalTime,fillTime;	//��ʱ�䣬����ʱ��
	string strAlgorithm,strTotalTime, strFillTime, strOptiPoints, strValuePoints ;	//�����㷨����ʱ�䣬����ʱ�䣬�Ż�դ��������դ����
	int nOptiCells=0;	//�Ż������
	int nValueCells=0;	//��ֵդ����
	string outputDir=m_GetDirectory(outputFile);	//���Ŀ¼
	string logInfo;	//��־��Ϣ

	stTime=clock();

	//��ʼ����Ǿ��󣬲����߽����
	for (int row = 0; row < nRows; row++)
	{
		for (int col = 0; col < nCols; col++)
		{
			float midValue=m_InputData->GetValue(row,col);
			if (midValue==NODATA_C)	continue;
			nValueCells++;
			if (m_IsBdrPoint(row,col))
			{
				GPoint newPt;
				newPt.Row=row;
				newPt.Col=col;
				newPt.Value=midValue;

				m_PriQueue.push(newPt);
				m_FlagData->SetValue(row,col,1);
			}
			else
			{
				m_FlagData->SetValue(row,col,0);
			}			
		}
	}	//end for for

	endMarkTime=clock();

	while(true)
	{
		if (!m_PlainQueue.empty())
		{
			GPoint fatherPt=m_PlainQueue.front();
			m_PlainQueue.pop();
			
			for (int row=fatherPt.Row-1;row<=fatherPt.Row+1;row++)
			{
				for (int col=fatherPt.Col-1;col<=fatherPt.Col+1;col++)
				{
					float crntValue=m_InputData->GetValue(row,col);
					if (m_FlagData->GetValue(row,col)==1)	continue;
					if (crntValue==NODATA_C)	continue;

					GPoint newPt;
					newPt.Row=row;
					newPt.Col=col;
					newPt.Value=crntValue;

					if (crntValue<=fatherPt.Value)
					{	//���� ���������ȴ������

						newPt.Value=fatherPt.Value;
						m_InputData->SetValue(row,col,fatherPt.Value);
						m_FlagData->SetValue(row,col,1);
						m_PlainQueue.push(newPt);
						nOptiCells++;
					}
					else
					{	//ֱ�ӽ����ȼ�����
						m_FlagData->SetValue(row,col,1);
						m_PriQueue.push(newPt);
					}					
				}
			}	// end for for			
		}
		else if (!m_PriQueue.empty())
		{
			GPoint fatherPt=m_PriQueue.top();
			m_PriQueue.pop();
			int dir=0;
			for (int row=fatherPt.Row-1;row<=fatherPt.Row+1;row++)
			{
				for (int col=fatherPt.Col-1;col<=fatherPt.Col+1;col++)
				{
					dir++;
					float crntValue=m_InputData->GetValue(row,col);
					if (crntValue==NODATA_C)	continue;
					if (m_FlagData->GetValue(row,col)==1)	continue;

					GPoint newPt;
					newPt.Row=row;
					newPt.Col=col;
					newPt.Value=crntValue;

					if (crntValue<=fatherPt.Value)
					{	//���� ���������ȴ������

						newPt.Value=fatherPt.Value;
						m_InputData->SetValue(row,col,fatherPt.Value);
						m_FlagData->SetValue(row,col,1);
						m_PlainQueue.push(newPt);
						nOptiCells++;
						continue;
					}

					bool isUselessPoint=false;
					float BrotherVal[4];	// [0] leftBrother | [1] rightBrother | [2] leftSubBrother | [3] rightSubBrother
					switch(dir)
					{
					case 1:
						BrotherVal[0]=m_InputData->_dataPtr[(row+1)*nCols+col];
						BrotherVal[1]=m_InputData->_dataPtr[row*nCols+col+1];
						BrotherVal[2]=m_InputData->_dataPtr[row*nCols+col-1];
						BrotherVal[3]=m_InputData->_dataPtr[(row-1)*nCols+col];
						if (BrotherVal[0]<crntValue && BrotherVal[1]<crntValue)
						{
							if (BrotherVal[2]<=crntValue || BrotherVal[3]<=crntValue)
							{
								isUselessPoint = true;
							}
						}
						break;
					case 3:
						BrotherVal[0]=m_InputData->_dataPtr[row*nCols+col-1];
						BrotherVal[1]=m_InputData->_dataPtr[(row+1)*nCols+col];
						BrotherVal[2]=m_InputData->_dataPtr[(row-1)*nCols+col];
						BrotherVal[3]=m_InputData->_dataPtr[row*nCols+col+1];
						if (BrotherVal[0]<crntValue && BrotherVal[1]<crntValue)
						{
							if (BrotherVal[2]<=crntValue || BrotherVal[3]<=crntValue)
							{
								isUselessPoint = true;
							}
						}
						break;
					case 7:
						BrotherVal[0]=m_InputData->_dataPtr[(row-1)*nCols+col];
						BrotherVal[1]=m_InputData->_dataPtr[row*nCols+col+1];
						BrotherVal[2]=m_InputData->_dataPtr[row*nCols+col-1];
						BrotherVal[3]=m_InputData->_dataPtr[(row+1)*nCols+col];
						if (BrotherVal[0]<crntValue && BrotherVal[1]<crntValue)
						{
							if (BrotherVal[2]<=crntValue || BrotherVal[3]<=crntValue)
							{
								isUselessPoint = true;
							}
						}
						break;
					case 9:
						BrotherVal[0]=m_InputData->_dataPtr[row*nCols+col-1];
						BrotherVal[1]=m_InputData->_dataPtr[(row-1)*nCols+col];
						BrotherVal[2]=m_InputData->_dataPtr[(row+1)*nCols+col];
						BrotherVal[3]=m_InputData->_dataPtr[row*nCols+col+1];
						if (BrotherVal[0]<crntValue && BrotherVal[1]<crntValue)
						{
							if (BrotherVal[2]<=crntValue || BrotherVal[3]<=crntValue)
							{
								isUselessPoint = true;
							}
						}
						break;
					case 2:
						BrotherVal[0]=m_InputData->_dataPtr[row*nCols+col-1];
						BrotherVal[1]=m_InputData->_dataPtr[row*nCols+col+1];
						if (BrotherVal[0]<crntValue && BrotherVal[1]<crntValue)
						{
							isUselessPoint = true;
						}
						break;
					case 4:
						BrotherVal[0]=m_InputData->_dataPtr[(row-1)*nCols+col];
						BrotherVal[1]=m_InputData->_dataPtr[(row+1)*nCols+col];
						if (BrotherVal[0]<crntValue && BrotherVal[1]<crntValue)
						{
							isUselessPoint = true;
						}
						break;
					case 6:
						BrotherVal[0]=m_InputData->_dataPtr[(row-1)*nCols+col];
						BrotherVal[1]=m_InputData->_dataPtr[(row+1)*nCols+col];
						if (BrotherVal[0]<crntValue && BrotherVal[1]<crntValue)
						{
							isUselessPoint = true;
						}
						break;
					case 8:
						BrotherVal[0]=m_InputData->_dataPtr[row*nCols+col-1];
						BrotherVal[1]=m_InputData->_dataPtr[row*nCols+col+1];
						if (BrotherVal[0]<crntValue && BrotherVal[1]<crntValue)
						{
							isUselessPoint = true;
						}
						break;
					}

					if(isUselessPoint == true)
					{	
						m_FlagData->SetValue(row,col,1);
						nOptiCells++;
						continue;
					}					
					//ֱ�ӽ����ȼ�����
					m_FlagData->SetValue(row,col,1);
					m_PriQueue.push(newPt);

				}
			}	// end for for			
		}
		else
		{
			break;
		}
	}	//end while

	endTime=clock();	// ������ʱ
	//ʱ�����
	fillTime=(double)(endTime-endMarkTime)/CLOCKS_PER_SEC;
	totalTime=(double)(endTime-stTime)/CLOCKS_PER_SEC;

	if (m_FlagData != NULL)
	{
		delete m_FlagData;	//���ٱ�Ǿ���
		m_FlagData = NULL;
	}

	//������ 
	strAlgorithm="FillPatternPlain";
	strTotalTime=m_DoubleToStr(totalTime);
	strFillTime=m_DoubleToStr(fillTime);
	strOptiPoints=m_IntToStr(nOptiCells);
	strValuePoints=m_IntToStr(nValueCells);
	logInfo= strAlgorithm+" "+m_GetFileName(inputFile)+" "+strTotalTime+" "+strFillTime+" "+strOptiPoints+" "+strValuePoints;
	cout<<logInfo<<endl;
	WriteLog(outputDir,logInfo.data());

	if (outputFile[0]!='\0')
	{
		rw.CreateFile(m_InputData,outputFile,(char*)"GTiff");
		rw.Write(m_InputData,outputFile);
	}
}

void Fill::FillMV(char* inputFile, char* outputFile)
{

	//������ʼ����������ȡ����
	CRasterReadWrite rw;
	CRasterData* m_Flag=rw.Read(inputFile);
	m_InputData=rw.Read(inputFile);

	int NCOL=m_Flag->NCol();
	int NROW=m_Flag->NRow();


	for (int r=0; r<NROW; r++)
	{
		for (int c=0; c<NCOL; c++)
		{
			if (m_Flag->IsNodataValue(r,c))
			{
				m_Flag->SetValue(r,c,-9999);
			}

			if (m_InputData->IsNodataValue(r,c))
			{
				m_InputData->SetValue(r,c,-9999);
			}

		}		
	}


	m_InputData->SetNoDataValue(-9999);
	m_Flag->SetNoDataValue(-9999);


	if (m_InputData==NULL)
	{
		return;
	}


	float NODATA_VALUE = m_InputData->NoDataValue();

	clock_t stTime,endTime,endMarkTime;
	double totalTime,fillTime;	//��ʱ�䣬����ʱ��
	string strAlgorithm,strTotalTime, strFillTime, strOptiPoints, strValuePoints ;	//�����㷨����ʱ�䣬����ʱ�䣬�Ż�դ��������դ����
	int nOptiCells=0;	//�Ż������
	int nValueCells=0;	//��ֵդ����
	string outputDir=m_GetDirectory(outputFile);	//���Ŀ¼
	string logInfo;	//��־��Ϣ

	stTime=clock();



	short k=0;
	long icol=0,jrow=0;
	bool con=false, finished =true;

	float tempFloat=0, neighborFloat=0;

	const int d1[9] = { 0,1, 1, 0,-1,-1,-1,0,1};
	const int d2[9] = { 0,0,-1,-1,-1, 0, 1,1,1};

	int col=0,row=0;
	float MAX_VALUE = -NODATA_VALUE;
	for(row=0; row<NROW; row++)
	{
		for(col=0; col<NCOL; col++)
		{
			if(m_Flag->IsNodataValue(row,col)) 
			{
				m_InputData->SetValue(row,col,NODATA_VALUE); 
			}

			else if (!m_Flag->IsValidIndex(row, col-1) || !m_Flag->IsValidIndex(row,col+1) 
				||!m_Flag->IsValidIndex(row-1,col)|| !m_Flag->IsValidIndex(row+1,col))
			{
				m_InputData->SetValue(row, col,m_Flag->GetData(row,col,tempFloat)); 
			}
			else
			{ 
				con = false;
				for(k=1; k<=8 && !con; k++) 
				{
					icol = col+d1[k];
					jrow = row+d2[k];
					if(m_Flag->IsNodataValue(jrow,icol)) con=true;
				}
				if(con)
					m_InputData->SetValue(row,col,m_Flag->GetData(row,col,tempFloat));
				else if(!m_Flag->IsNodataValue(row,col))
					m_InputData->SetValue(row,col,MAX_VALUE); 
			}
		}
	} 

	endMarkTime=clock();

	finished = true;

	static	stack<int> s1, s2;

	for(row=0; row<NROW; row++)
	{
		for(col=0; col<NCOL; col++)
		{
			if(m_InputData->IsNodataValue(row,col)) continue;
			if(m_InputData->GetData(row,col,tempFloat) > m_Flag->GetValue(row,col))
			{
				neighborFloat = MAX_VALUE;
				for(k=1; k<=8; k++)
				{
					icol = col+d1[k];
					jrow = row+d2[k];
					if(m_InputData->IsNodataValue(jrow,icol)) continue;
					if(m_InputData->GetData(jrow,icol,tempFloat) < neighborFloat)
						neighborFloat = tempFloat; 
				}
				if(m_Flag->GetData(row, col,tempFloat) >= neighborFloat)
				{
					m_InputData->SetValue(row,col, tempFloat);
					finished = false; 				
				}
				else 
				{			
					s1.push(col);
					s1.push(row);
					if(m_InputData->GetValue(row,col) > neighborFloat)
					{
						m_InputData->SetValue(row,col,neighborFloat);
						finished = false;					
					}				
				} 
			}
		}
	}


	// �������ݵ����� 
	while(!finished)
	{ 		
		finished=true;
		while(!s1.empty())
		{ 
			row=s1.top();
			s1.pop();
			col=s1.top();
			s1.pop();

			neighborFloat = MAX_VALUE;
			for(k=1; k<=8; k++)
			{
				icol = col+d1[k];
				jrow = row+d2[k];
				if(m_InputData->IsNodataValue(jrow,icol)) continue;
				if(m_InputData->GetData(jrow,icol,tempFloat) < neighborFloat)
					m_InputData->GetData(jrow, icol,neighborFloat);
			}
			if(m_Flag->GetData(row, col,tempFloat) >= neighborFloat)
			{
				m_InputData->SetValue(row, col,tempFloat);
				finished = false;
			}
			else 
			{   
				s2.push(col);
				s2.push(row);
				if(m_InputData->GetData(row,col,tempFloat) > neighborFloat)
				{
					m_InputData->SetValue(row,col,neighborFloat);
					finished = false;
				}
			} 
		}

		finished=true;
		while(!s2.empty())
		{  
			row=s2.top();
			s2.pop();
			col=s2.top();
			s2.pop();

			neighborFloat = MAX_VALUE;
			for(k=1; k<=8; k++)
			{
				icol = col+d1[k];
				jrow = row+d2[k];
				if(m_InputData->IsNodataValue(jrow,icol)) continue;
				if(m_InputData->GetData(jrow,icol,tempFloat) < neighborFloat)
					m_InputData->GetData(jrow, icol,neighborFloat);
			}
			if(m_Flag->GetData(row, col,tempFloat) >= neighborFloat)
			{
				m_InputData->SetValue(row, col,tempFloat);
				finished = false;
			}
			else 
			{  
				s1.push(col);
				s1.push(row);
				if(m_InputData->GetData(row,col,tempFloat) > neighborFloat)
				{
					m_InputData->SetValue(row,col,neighborFloat);
					finished = false;
				}
			} 
		}

	} 


	endTime=clock();	// ������ʱ
	//ʱ�����
	fillTime=(double)(endTime-endMarkTime)/CLOCKS_PER_SEC;
	totalTime=(double)(endTime-stTime)/CLOCKS_PER_SEC;

	if (m_Flag != NULL)
	{
		delete m_Flag;	//���ٱ�Ǿ���
		m_Flag = NULL;
	}


	//������ 
	strAlgorithm="M&V";
	strTotalTime=m_DoubleToStr(totalTime);
	strFillTime=m_DoubleToStr(fillTime);
	strOptiPoints=m_IntToStr(nOptiCells);
	strValuePoints=m_IntToStr(nValueCells);
	logInfo= strAlgorithm+" "+m_GetFileName(inputFile)+" "+strTotalTime+" "+strFillTime+" "+strOptiPoints+" "+strValuePoints;
	cout<<logInfo<<endl;
	WriteLog(outputDir,logInfo.data());

	if (outputFile[0]!='\0')
	{
		rw.CreateFile(m_InputData,outputFile,(char*)"GTiff");
		rw.Write(m_InputData,outputFile);
	}

	return;
}



















