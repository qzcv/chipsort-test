#pragma once
#include <QString>
#include "HalconCpp.h"
#include <vector>

#define MAX_TEMP_NUM 4 //���ɽ���ģ����Ŀ
using namespace Halcon;
using std::vector;

class nccMatch_outNParam
{
public:
	explicit nccMatch_outNParam();
	~nccMatch_outNParam();
	virtual void readWriteParam(bool r, const QString& dirPath, int level);

	typedef struct {
		double row;
		double col;
		double length1;
		double length2;
		double angle;
	}segRegion;
	typedef struct {
		double drow;
		double dcol;
		double dlength1;
		double dlength2;
		double dangle;
	}delRegion;
	int currentGrp;  //��ǰģ��ָ��
	int GrpEnable[MAX_TEMP_NUM];
	int similarity[MAX_TEMP_NUM];
	int seekingAngle[MAX_TEMP_NUM];
	int minusArea[MAX_TEMP_NUM];
	int acceptEnable[MAX_TEMP_NUM];
	int offsetEnable[MAX_TEMP_NUM];
	int relativeEnable[MAX_TEMP_NUM];
	int countEnable[MAX_TEMP_NUM];
	int acceptValue[MAX_TEMP_NUM];
	int offsetValue[MAX_TEMP_NUM];
	int minRelative[MAX_TEMP_NUM], maxRelative[MAX_TEMP_NUM];
	int minCount[MAX_TEMP_NUM], maxCount[MAX_TEMP_NUM];

	int roix[MAX_TEMP_NUM], roiy[MAX_TEMP_NUM], roiw[MAX_TEMP_NUM], roih[MAX_TEMP_NUM];

	int patternX[MAX_TEMP_NUM], patternY[MAX_TEMP_NUM], patternW[MAX_TEMP_NUM], patternH[MAX_TEMP_NUM];

	double minusRow[MAX_TEMP_NUM], minusCol[MAX_TEMP_NUM], minusLength1[MAX_TEMP_NUM], minusLength2[MAX_TEMP_NUM];
	double minusA[MAX_TEMP_NUM];

	double minusR[MAX_TEMP_NUM], minusCX[MAX_TEMP_NUM], minusCY[MAX_TEMP_NUM];
	int findNum[MAX_TEMP_NUM];
	double maxOverlap[MAX_TEMP_NUM];
	Hobject patImage[MAX_TEMP_NUM];
	Hobject OutImg[MAX_TEMP_NUM];   //��������
	int outRegionCount[MAX_TEMP_NUM];
	int outDeleteCount[MAX_TEMP_NUM];
	int outAreaMin[MAX_TEMP_NUM];   //�����С
	int outAreaMax[MAX_TEMP_NUM];
	int outThreMin[MAX_TEMP_NUM];   //��ֵ
	int outThreMax[MAX_TEMP_NUM];
	int IsUseOutArea[MAX_TEMP_NUM];  //�Ƿ�ʹ�ô�������
	int IsShowOutArea[MAX_TEMP_NUM]; //�����ʾ��������
	int IsFillUp[MAX_TEMP_NUM];      //�Ƿ���䴫��ģ��

	int openVal[MAX_TEMP_NUM];    //����������п���
	int closeVal[MAX_TEMP_NUM];

	int IsAxisAngle[MAX_TEMP_NUM];   //������������ת�Ƕ�
	int maxAxisAngle[MAX_TEMP_NUM];     //���ƫת��

	int IsShowRoiAfterDet[MAX_TEMP_NUM];
	int IsShowMinusAfterDet[MAX_TEMP_NUM];  //�������Ƿ���ʾ�ų���roi����

	vector<segRegion> outRegionVec[MAX_TEMP_NUM];  //����ROI
	vector<delRegion> deleteRegionVec[MAX_TEMP_NUM];  //�ų�ROI

private:
	void iniData();
};

