#pragma once
#include <QString>
#include <vector>
#define GRP_MAX_LEADNUM 15//单组管脚最大数目
using std::vector;

class standLocationParam
{
public:
	explicit standLocationParam();
	~standLocationParam();
	virtual void readWriteParam(bool r, const QString& dirPath, int level);

	typedef struct {
		float row1;
		float col1;
		float row2;
		float col2;
	}RoiRegion;

	int upDwOrLfRt;      //检测上下站高或者左右站高，0、1
	int UpDetect;        //启用上检测
	int DwDetect;        //启用下检测
	int LfDetect;        //启用左检测
	int RtDetect;        //启用右检测

	int grpPinNum[2];       //组1，2的管脚数目

	int extendSearchLine[4];//扩大搜寻线
	int minGrayDiff[4];  //最小灰度差异
						 //float grayPlus[4];   //灰度补偿值
						 //int grayMin[4];      //最小灰度值
						 //int grayMax[4];      //最大灰度值

	int minArea[4];         //根据求出来的边线切出来的区域(站高)的最小面积 
	int standoffOffset[4];  //偏离求出来的边线的距离，用以求站高部分的宽度
	int grayVal[4];         //根据求出来的边线切出来的区域的灰度阈值

	double standOffCorr[2][GRP_MAX_LEADNUM];    //站立度修正左/上,右下
	double widthCorr[2][GRP_MAX_LEADNUM];       //宽度补偿
	double InterCorr[2][GRP_MAX_LEADNUM];       //间距补偿

												//OFFSET
	double offStandOffCorr[2][GRP_MAX_LEADNUM];    //站立度修正左/上,右下   
	double offWidthCorr[2][GRP_MAX_LEADNUM];       //宽度补偿
	double offInterCorr[2][GRP_MAX_LEADNUM];       //间距补偿

	double selfStandOffCorr[2];        //自己加的站高补偿

									   //误差范围
	int errPinGapValid;           //是否启用管脚间距
	int errPinStandOffValid;      //是否启用站立度判断
	int errPinStandOffDiffValid;  //是否启用站立度差异判断
	int errPinWidthValid;         //是否启用管脚宽度
	int errCoplanarValid;         //是否采用共面性

	int IsLMSValid;  //是否使用LMS算法求取共面度平面
	double LMSFactor;

	double errPinGapMin[2][GRP_MAX_LEADNUM];  //管脚间距最小值；
	double errPinGapMax[2][GRP_MAX_LEADNUM];  //最大值

	double errPinStandOffMin[2][GRP_MAX_LEADNUM];  //站立度最小1值
	double errPinStandOffMax[2][GRP_MAX_LEADNUM];  //站立度最大1值

	double errPinStandOffDiffMin[2];
	double errPinStandOffDiffMax[2];
	double errCoplanarMin;
	double errCoplanar;

	double errPinWidthMin[2][GRP_MAX_LEADNUM];
	double errPinWidthMax[2][GRP_MAX_LEADNUM];

	double packageOff;  //塑封补偿

	vector<RoiRegion> RoiRegionVector;  //[0]为搜索范围,[1]-[4]为上下左右的搜1索框

	int NowGrp;  //上下左右，当前的组数

private:
	void iniData();
};

