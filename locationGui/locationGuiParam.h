#pragma once
#include <QString>
#include <vector>
#define GRP_MAX_LEADNUM 10 //单组管脚最大数目
using std::vector;

class locationGuiParam
{
public:
	explicit locationGuiParam();
	~locationGuiParam();
	virtual void readWriteParam(bool r, const QString& dirPath, int level);

	typedef struct {
		float row1;
		float col1;
		float row2;
		float col2;
	}RoiRegion;

	int edgeSearchOrder; //0为上下左右_1为左右上下
	int IsEndPoint;      //是否检测端点0关闭1启用

	int upDwOrLfRt;      //检测上下站高或者左右站高，0、1

	int maxAngle;        //最大角度
	int UpDetect;        //启用上检测
	int DwDetect;        //启用下检测
	int LfDetect;        //启用左检测
	int RtDetect;        //启用右检测

	int grpPinNum[2];       //组1，2的管脚数目

	int whiteBlackDir[4];   //边缘灰度类型,0黑白_1白黑    //上白黑，下黑白,左白黑，右黑白
	int searchDir[4];    //搜寻方向,0左右,1上下  
	int extendSearchLine[4];//扩大搜寻线
	int minGrayDiff[4];  //最小灰度差异
	int searchLineGap[4];   //扫描线间距
	int lineOffset[4];      //边线偏移
	int IsPaichuLine[4];    //是否启用排除线
	int paichuLine[4];      //排除线
	int IsCutLine[4];       //是否断开边线
	int offsetPlus[4];      //偏置值，左右区域上下动，上下区域左右动
	int minArea[4];         //根据求出来的边线切出来的区域(站高)的最小面积 
	int standoffOffset[4];  //偏离求出来的边线的距离，用以求站高部分的宽度
	int grayVal[4];         //根据求出来的边线切出来的区域的灰度阈值
	int edgesearchWidth[4]; //扫描端点的时候的measure_rec的宽度
	int pinSearchWidth[4];  //管脚区域搜索时候分割管脚区域所用的区域扩张大小
	double clipfactor[4];   //边缘线拟合的系数，1.0,1.5,2.0,2.5,3.0
	int IsUseStand[4];      //是否启用站高

	double TmpCenterRow;
	double TmpCenterCol;   //模板中心的坐标
	double standOffCorr[2];    //站立度修正左/上,右下
	double widthCorr[2];       //宽度补偿
	double InterCorr[2];       //间距补偿


	double pinlength;      //输入的管脚实际长度

	//错误范围
	int IsErrWidth;      //是否启用宽度
	int errWidthMin;
	int errWidthMax;
	int IsErrHeight;
	int errHeightMin;
	int errHeightmax;
	int IsErrAngle;
	int errAngle;
	int IsErrCenterOffset;
	int errCenterOffset;

	//误差范围
	int errPinGapValid;           //是否启用管脚间距
	int errPinStandOffValid;      //是否启用站立度判断
	int errPinStandOffDiffValid;  //是否启用站立度差异判断
	int errPinWidthValid;         //是否启用管脚宽度

	double errPinGapMin[2][GRP_MAX_LEADNUM];  //管脚间距最小值；
	double errPinGapMax[2][GRP_MAX_LEADNUM];  //最大值

	double errPinStandOffMin[2][GRP_MAX_LEADNUM];  //站立度最小1值
	double errPinStandOffMax[2][GRP_MAX_LEADNUM];  //站立度最大1值

	double errPinStandOffDiffMax[2];

	double errPinWidthMin[2][GRP_MAX_LEADNUM];
	double errPinWidthMax[2][GRP_MAX_LEADNUM];
	int LTnum;
	int RBnum;
	int ChildTablenum;

	vector<RoiRegion> RoiRegionVector;  //[0]为搜索范围,[1]-[4]为上下左右的搜1索框

private:
	void iniData();
};

