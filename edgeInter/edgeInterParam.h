#pragma once
#include <QString>
#include <vector>

using std::vector;
#define MAX_EDGE_LINE 3

class edgeInterParam
{
public:
	explicit edgeInterParam();
	~edgeInterParam();
	virtual void readWriteParam(bool r, const QString& dirPath, int level);

	typedef struct {
		double row;
		double col;
		double angle;
		double length1;
		double length2;
	}RoiRegion;
	int IsShowRegionAfterDet; //测后是否显示得出的区域
	int IsRefer;         //参考矩阵

	int gradient[MAX_EDGE_LINE];     //边界对比度
	int stepVal[MAX_EDGE_LINE];         //垂直边界方向搜索范围
	int pol[MAX_EDGE_LINE];          //边界方向
	int first[MAX_EDGE_LINE];        //边界是否是第一条(1)还是最大对比度那条(0)
	int degRange[MAX_EDGE_LINE];     //角度范围
	int rmDist[MAX_EDGE_LINE];       //多大范围外的点作为噪音排除
	int iterateNum[MAX_EDGE_LINE];   //拟合直线时迭代次数

	int lineOffset[MAX_EDGE_LINE];      //边线偏移

										//错误范围
	int IsErrInter;      //是否启用焦点距离
	double errInterMin;
	double errInterMax;

	RoiRegion roiReg[MAX_EDGE_LINE];
	int currentEdge;  //当前的边界
	double pixSize;   //图像比例尺
	double interOffset;  //交点距离补偿值 

						 //参考中心
	double referX, referY;    //参考中心的坐标值
	int IsErrX, IsErrY;       //中心坐标启用
	double errXMin, errXMax;  //中心X坐标范围
	double errYMin, errYMax;  //中心X坐标范围

private:
	void iniData();
};

