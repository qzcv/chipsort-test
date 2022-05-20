#pragma once
#include <QString>
#include <vector>

using std::vector;

#define MAX_EDGE_LINE 3

class edgeWarpParam
{
public:
	explicit edgeWarpParam();
	~edgeWarpParam();
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

	int leadDir;    //目标的朝向 ,上下0，下上1，左右2，右左3
	int IsWhite;         //目标是黑色0还是白色1


	int gradient[MAX_EDGE_LINE];     //边界对比度
	int stepVal[MAX_EDGE_LINE];         //垂直边界方向搜索范围
	int pol[MAX_EDGE_LINE];          //边界方向
	int first[MAX_EDGE_LINE];        //边界是否是第一条(1)还是最大对比度那条(0)
	int degRange[MAX_EDGE_LINE];     //角度范围
	int rmDist[MAX_EDGE_LINE];       //多大范围外的点作为噪音排除
	int iterateNum[MAX_EDGE_LINE];   //拟合直线时迭代次数

	int lineOffset[MAX_EDGE_LINE];      //边线偏移
	int leadThre[MAX_EDGE_LINE];     //脚尖对比度

									 //分割参数
	int leadGray[2];    //最小灰度
	int leadAreaMin[2]; //最小面积

						//错误范围
	int IsErrWarp;      //是否启用管脚翘曲
	double errWarpMin;
	double errWarpMax;
	int IsErrWarpDiff;  //是否启用翘曲（共面性）差异
	double errWarpDiffMin;
	double errWarpDiffMax;

	RoiRegion roiReg[MAX_EDGE_LINE];
	int currentEdge;  //当前的边界
	double pixSize;   //图像比例尺
	int isInputPix;   //是否传入比例

	double warpOffset;  //交点距离补偿值 

private:
	void iniData();
};

