#pragma once
#include <QString>
#include <vector>

#define GRP_NUM 4   //管脚总组数

using std::vector;

class pinWarpParam
{
public:
	explicit pinWarpParam();
	~pinWarpParam();
	virtual void readWriteParam(bool r, const QString& dirPath, int level);

	typedef struct {
		float row1;
		float col1;
		float row2;
		float col2;
	}RoiRegion;

	int whiteBlackDir[GRP_NUM];   //底线边缘灰度类型,0黑白_1白黑
	int currentGrp;              //ui里的当前组别
	int IsDetBase[GRP_NUM];      //检测芯片底线，0为不检测底线，1为检测底线

	int grpEnable[GRP_NUM];      //管脚组使能
	int IsBaseInvert[GRP_NUM];      //底线反向，检测方向相反
	int IsBaseSameWithLead[GRP_NUM]; //管脚是否在底线搜寻方向

	int searchDir[GRP_NUM];    //搜寻方向,0左右,1右左，2上下，3下上  
	int extendSearchLine[GRP_NUM];//扩大搜寻线
	int leadGrayDiff[GRP_NUM];  //检测管脚的最小灰度差异
	int baseGrayDiff[GRP_NUM];   //检测底线的灰度差
	int leadSearchLength[GRP_NUM]; //检测管脚时候的搜索框长度
	int searchLineGap[GRP_NUM];   //扫描线间距
	int noiseMax[GRP_NUM];        //最多容忍的噪点数目
	int deleteEdge[GRP_NUM];      //去除头尾的区域的几个点
	int IsWhitePin[GRP_NUM];      //是否是白色管脚
	double clipfactor[GRP_NUM];   //边缘线拟合的系数，1.0,1.5,2.0,2.5,3.0
	double normalOffMin[GRP_NUM]; //正常管脚底线偏离范围
	double normalOffMax[GRP_NUM]; //正常管脚底线偏离范围

	double sizeRatio[GRP_NUM];       //尺寸比例因子
	int IsInputRatio[GRP_NUM];       //是否传入比例

	//误差范围
	int errWidthValid[GRP_NUM];           //是否启用管脚间距
	double errWidthMin[GRP_NUM];  //宽度最小值
	double errWidthMax[GRP_NUM];  //最大值
	int errWarpValid[GRP_NUM];      //是否启用翘曲判断
	double errWarp[GRP_NUM];  //翘曲容忍值

	RoiRegion RoiReg[GRP_NUM][2];  //所有组的管脚搜索框，第一个是检测边界的框，第二个为管脚框

private:
	void iniData();
};

