#pragma once
#include <QString>
#include <vector>
#define MAX_ROI 3  //���������Ŀ
using std::vector;

class imageSegment_preParam
{
public:
	explicit imageSegment_preParam();
	~imageSegment_preParam();
	virtual void readWriteParam(bool r, const QString& dirPath, int level);

	typedef struct {
		double row1;
		double col1;
		double row2;
		double col2;
	}segRegion;
	int moduleValid;
	int thresholdType;  //��ֵ����ʽ�� 0��ֵ�ָ1��������

	int allImgValid;
	int RoiCounts;
	//threshold
	int minGray[MAX_ROI];
	int maxGray[MAX_ROI];
	int minArea[MAX_ROI];
	int maxArea[MAX_ROI];

	//regiongrowing
	int IsNoCenter;  //0ʹ�����ĵ㣬1��ʹ�����ĵ�
	int IsMeanGrow;  //0����regiongrow��1����regiongrow_mean
	int centerType;  //���ӵ���roi���Ļ������õ����ӵ� 0roi��1�������ӵ�,2��ʹ�����ӵ�
	int centerX[MAX_ROI], centerY[MAX_ROI];  //���ӵ�����
	int rowWid;
	int colWid;
	int thresholdTol;
	int minGrowArea;

	int algorithmType;
	int param1[5];
	int param2[5];
	int param3[5];
	vector<segRegion> segRegionVector;   //

private:
	void iniData();
};

