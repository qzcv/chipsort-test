#pragma once
#include <QString>

#define MAX_REGION 3	//最多多少个区块参与贴图
#define ROI_MAX_NUM 2   //单个区块最多的ROI数目

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
	int roiNum[MAX_REGION]; //各区块ROI数目
	int allRegNum;          //启用区块总数目

private:
	void iniData();
};

