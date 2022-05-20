#pragma once
#include <QString>
#include <vector>

#define GRP_NUM 4

using std::vector;

class pinDistanceParam
{
public:
	explicit pinDistanceParam();
	~pinDistanceParam();
	virtual void readWriteParam(bool r, const QString& dirPath, int level);

	typedef struct {
		float row1;
		float col1;
		float row2;
		float col2;
	}RoiRegion;

	int currentGrp;              //ui里的当前组别
	int IsRefer;        //参考矩阵

	int edgeThreshold[GRP_NUM]; //边界检测对比度
	int pointsNum[GRP_NUM];  //边界点数检测数目 
	int pointsInter[GRP_NUM];  //边界点数检测间距 
	int edgeOffset[GRP_NUM]; //传入底线偏移值

	int grpEnable[GRP_NUM];      //管脚组使能
	int grpPinNum[GRP_NUM];      //各组的管脚数目
	double sizeRatio[GRP_NUM];       //尺寸比例因子

									 //误差范围
	int errDistanceValid;           //是否启用管脚间距
	double errDistanceMin[GRP_NUM];  //管脚间距最小值；
	double errDistanceMax[GRP_NUM];  //最大值

	vector<RoiRegion> RoiVec[GRP_NUM];  //所有组的管脚搜索框，第一个是检测边界的框，后续为管脚框

private:
	void iniData();
};

