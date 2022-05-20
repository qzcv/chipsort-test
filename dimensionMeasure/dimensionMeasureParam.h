#pragma once
#include <QString>
#include <vector>
using std::vector;

#define Max_Roi_Num 10

class dimensionMeasureParam
{
public:
	explicit dimensionMeasureParam();
	~dimensionMeasureParam();
	virtual void readWriteParam(bool r, const QString& dirPath, int level);

	typedef struct {
		double row;
		double col;
		double angle;
		double length1;
		double length2;
	}segRegion;
	int IsReferSection;//�ο�ģ��
	int regionCount;   //������Ŀ
	int IsShowAfterDet; //�Ƿ������ʾ����

	int threshold;     //measure threshold;
	int searchGap;     //ɨ����
	int noiseMin;      //�������ֵ
	int edgeExtend;    //�߽�����

	int noiseMaxNum;   //�������������Ŀ
	int IsBlack;       //�Ƿ��Ǻ�ɫ����
	int IsBestEdge;    //�Ƿ�����ѱ߽�

	vector<double> lengthMinVec;  //������С����
	vector<double> lengthMaxVec;  //������󳤶�
	int IsValidPoints;     //�Ƿ������Ч�����
	int validPointsPer;    //��Ч�����
	vector<segRegion> RoiRegionVector;  //roi��������

private:
	void iniData();
};

