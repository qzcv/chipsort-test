#pragma once
#include <QString>

#define MAX_REGION 3	//�����ٸ����������ͼ
#define ROI_MAX_NUM 2   //������������ROI��Ŀ

class composeND_preParam
{
public:
	explicit composeND_preParam();
	~composeND_preParam();
	virtual void readWriteParam(bool r, const QString& dirPath, int level);

	int baseImgIdx;
	int imageIndex[MAX_REGION];
	double regRow0[MAX_REGION][ROI_MAX_NUM];
	double regCol0[MAX_REGION][ROI_MAX_NUM];
	double regRow1[MAX_REGION][ROI_MAX_NUM];
	double regCol1[MAX_REGION][ROI_MAX_NUM];
	int roiNum[MAX_REGION]; //������ROI��Ŀ
	int allRegNum;          //������������Ŀ

private:
	void iniData();
};

