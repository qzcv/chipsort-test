#pragma once
#include <QString>
#include "HalconCpp.h"
#include <vector>

using namespace Halcon;
using std::vector;

#define MAX_PAT_NUM 4

class nccMatch_fourDirParam
{
public:
	explicit nccMatch_fourDirParam();
	~nccMatch_fourDirParam();
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

	int IsRefer;   //�Ƿ���òο�����ע��˴���������ʱֻ֧��rectangle1
	int similarity;
	int seekingAngle;
	int minusArea[MAX_PAT_NUM];
	int currentPat;   //��ǰģ��ָ��
	int acceptEnable;
	int relativeEnable;
	int acceptValue;
	int minRelative, maxRelative;
	int matchAngleEnable;
	int matchAngle;  //һ�����ж�

	double roiCol1[MAX_PAT_NUM], roiCol2[MAX_PAT_NUM], roiRow1[MAX_PAT_NUM], roiRow2[MAX_PAT_NUM];
	double RecRoiRow1, RecRoiRow2, RecRoiCol1, RecRoiCol2;

	int patternX[MAX_PAT_NUM], patternY[MAX_PAT_NUM], patternW[MAX_PAT_NUM], patternH[MAX_PAT_NUM];

	double minusRow[MAX_PAT_NUM], minusCol[MAX_PAT_NUM], minusLength1[MAX_PAT_NUM], minusLength2[MAX_PAT_NUM];
	int minusA[MAX_PAT_NUM];

	double minusR[MAX_PAT_NUM], minusCX[MAX_PAT_NUM], minusCY[MAX_PAT_NUM];
	int findNum;
	double maxOverlap;
	Hobject patImage[MAX_PAT_NUM];  //���������������ʱ�����0,1�洢ͼƬ

	int IsUsePinDir;   //�Ƿ����÷���
	int pin1OutIdx;    //�ܽ�1�����
	int pin2OutIdx;
	int IsDefaultSig;  //�Ƿ�����Ĭ���ź�
	int Is2Dor4D;      //�������������������ĸ���������
	int IsMirrorDir;   //������ģʽ�Ƿ���þ���ģʽ
	int HorOrVerMir;   //���¾��������Ҿ���

	int IsDefaultEdit; //�Ƿ���Ĭ�ϱ༭˳��
	int sigEdit[2][4]; //�༭�Ķ������ź�ʹ��ģʽ  [2]�����������źŶ� [4]�ĸ�����

    //int multAngle;     //Ĭ�ϵ����ö�ģ��ĽǶȣ�0-90,1-180,2-270
	int IsAutoRotate;  //�Ƿ��Զ���תģ��
	int StartAngle;    //�Զ�ģ��ʱ��ʼ�ĽǶ� 0-0,1-90,2-180,3-270

	//out area
	Hobject OutImg[MAX_PAT_NUM];   //��������,���������������ʱ�����0,1�洢����
	int outRegionCount;
	int outDeleteCount;
	int outAreaMin;   //�����С
	int outAreaMax;
	int outThreMin;   //��ֵ
	int outThreMax;
	int IsUseOutArea;  //�Ƿ�ʹ�ô�������
	int IsShowOutArea; //�����ʾ��������
	int IsFillUp;      //�Ƿ���䴫��ģ��

	vector<segRegion> outRegionVec;  //����ROI
	vector<delRegion> deleteRegionVec;  //�ų�ROI

private:
	void iniData();
};

