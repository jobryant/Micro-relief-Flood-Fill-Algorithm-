#include<iostream>
#include<string.h>
#include "PriorityFill.h"
using namespace std;
#define MAXLN 4096


int mainxxx(int argc,char **argv)
{
	Fill fill;
	char inputFile[MAXLN];char correctFile[MAXLN];char outputFile[MAXLN];char fillType[MAXLN];char isCheck[MAXLN];
	if (argc != 6)
	{
		printf("Error: please input correct parameter orderly: \n");
		return 0;
	}
		
	strcpy(inputFile,argv[1]);
	strcpy(correctFile,argv[2]);
	strcpy(outputFile,argv[3]);
	strcpy(fillType,argv[4]);
	int _fillType=atoi(fillType);
	strcpy(isCheck,argv[5]);
	int _isCheck=atoi(isCheck);

	cout<<"StartFill:";


	switch(_fillType)
	{
	case 1:
		fill.Fill_WL(inputFile,outputFile);
		break;
	case 2:
		fill.Fill_FlatPitOptimal(inputFile,outputFile);
		break;
	case 3:
		fill.Fill_JudgeSentence(inputFile,outputFile);
		break;
	case 4:
		fill.Fill_MFF(inputFile,outputFile);
		break;
	default:
		cout<<"请选择正确的填洼模式"<<endl;
		break;
	}

	if (_isCheck==1)
	{
		fill.FillCheck(correctFile);
	}

	cout<<"Finished"<<endl;
	return 0;
}

