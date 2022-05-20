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
	int IsReferSection;//参考模组
	int regionCount;   //区域数目
	int IsShowAfterDet; //是否检测后显示区域

	int threshold;     //measure threshold;
	int searchGap;     //扫描间距
	int noiseMin;      //噪点容忍值
	int edgeExtend;    //边界扩张

	int noiseMaxNum;   //最多允许的噪点数目
	int IsBlack;       //是否是黑色特征
	int IsBestEdge;    //是否是最佳边界

	vector<double> lengthMinVec;  //区域最小长度
	vector<double> lengthMaxVec;  //区域最大长度
	int IsValidPoints;     //是否采用有效点比例
	int validPointsPer;    //有效点比例
	vector<segRegion> RoiRegionVector;  //roi搜索区域

private:
	void iniData();
};

