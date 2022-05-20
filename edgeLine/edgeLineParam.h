#pragma once
#include <QString>

class edgeLineParam
{
public:
	explicit edgeLineParam();
	~edgeLineParam();
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

	int gradient[4];     //边界对比度
	int stepVal[4];         //垂直边界方向搜索范围
	int pol[4];          //边界方向
	int first[4];        //边界是否是第一条(1)还是最大对比度那条(0)
	int degRange[4];     //角度范围
	int rmDist[4];       //多大范围外的点作为噪音排除
	int iterateNum[4];   //拟合直线时迭代次数

	int lineOffset[4];      //边线偏移
	int IsFixWidth[4];      //是否采用固定长度
	int fixWidth[4];        //固定长度

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

	RoiRegion roiReg[4];

private:
	void iniData();
};

