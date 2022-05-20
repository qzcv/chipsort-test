#pragma once
#include <QString>
#include <vector>

#define GRP_MAX_LEADNUM 15 //单组管脚最大数目

using std::vector;

class locationParam
{
public:
	explicit locationParam();
	~locationParam();
	virtual void readWriteParam(bool r, const QString& dirPath, int level);

	typedef struct {
		float row1;
		float col1;
		float row2;
		float col2;
	}RoiRegion;
	int IsEndPoint;      //是否检测端点0关闭1启用
	int IsPadStand;     //是否启用pad类型的站高，启用时只计算站高顶点，且计算站高时的公式需修改。宽度，间距等信息不可使用

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
	int IsPaichuLine[4];    //是否启用排除线,边界点跳变超过多少就会被当做管脚尖搜出来的点
	int paichuLine[4];      //排除线
	double scalePaichu[4];      //针对安士1212等边界较长的产品，为了防止线偏移较大对排除线造成的影响，可以只考虑部分的边界。起始0, 0-1.0
	int isLeadSize[4];  //是否采用管脚尺寸进行开操作去除管脚尖来查找边界
	int leadSize[4];    //管脚尺寸

	int IsCutLine[4];       //是否断开边线
	int offsetPlus[4];      //偏置值，左右区域上下动，上下区域左右动
	int minArea[4];         //根据求出来的边线切出来的区域(站高)的最小面积 
	int standoffOffset[4];  //偏离求出来的边线的距离，用以求站高部分的宽度
	int grayVal[4];         //根据求出来的边线切出来的区域的灰度阈值
	int edgeGray[4]; //扫描端点的时候的measure_rec的灰度
					 //int pinSearchWidth[4];  //管脚区域搜索时候分割管脚区域所用的区域扩张大小
	double clipfactor[4];   //边缘线拟合的系数，1.0,1.5,2.0,2.5,3.0

	int IsEdgeDir[4];       //是否启用边界点的方向性筛选，用于辅助排除管脚尖部的边界点

	double TmpCenterRow;
	double TmpCenterCol;   //模板中心的坐标
	double standOffCorr[2][GRP_MAX_LEADNUM];    //站立度修正左/上,右下
	double widthCorr[2][GRP_MAX_LEADNUM];       //宽度补偿
	double InterCorr[2][GRP_MAX_LEADNUM];       //间距补偿

												//OFFSET
	double offStandOffCorr[2][GRP_MAX_LEADNUM];    //站立度修正左/上,右下   
	double offWidthCorr[2][GRP_MAX_LEADNUM];       //宽度补偿
	double offInterCorr[2][GRP_MAX_LEADNUM];       //间距补偿

	double selfStandOffCorr[2];        //自己加的站高补偿

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
	int errCoplanarValid;         //是否采用共面性

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

	vector<RoiRegion> RoiRegionVector;  //[0]为搜索范围,[1]-[4]为上下左右的搜1索框

	int NowGrp;  //上下左右，当前的组数

private:
	void iniData();
};

