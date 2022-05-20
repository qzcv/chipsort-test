#pragma once
#include <QString>
#include <vector>

using std::vector;

class standEdgeParam
{
public:
	explicit standEdgeParam();
	~standEdgeParam();
	virtual void readWriteParam(bool r, const QString& dirPath, int level);

	typedef struct {
		float row1;
		float col1;
		float row2;
		float col2;
	}RoiRegion;
	int IsEndPoint;      //是否检测端点0关闭1启用

	int maxAngle;        //最大角度

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
	int grayVal[4];         //根据求出来的边线切出来的区域的灰度阈值
	int edgeGray[4]; //扫描端点的时候的measure_rec的灰度
					 //int pinSearchWidth[4];  //管脚区域搜索时候分割管脚区域所用的区域扩张大小
	double clipfactor[4];   //边缘线拟合的系数，1.0,1.5,2.0,2.5,3.0

	int IsEdgeDir[4];       //是否启用边界点的方向性筛选，用于辅助排除管脚尖部的边界点
	int IsClearOutlier[4];  //是否消除离群点(单独的不连续点)

	double TmpCenterRow;
	double TmpCenterCol;   //模板中心的坐标

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

	vector<RoiRegion> RoiRegionVector;  //[0]为搜索范围,[1]-[4]为上下左右的搜1索框

	int NowGrp;  //上下左右，当前的组数

private:
	void iniData();
};

