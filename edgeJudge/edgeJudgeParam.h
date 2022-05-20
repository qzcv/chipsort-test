#pragma once
#include <QString>
#include <vector>
using std::vector;

class edgeJudgeParam
{
public:
	explicit edgeJudgeParam();
	~edgeJudgeParam();
	virtual void readWriteParam(bool r, const QString& dirPath, int level);

	typedef struct {
		double row1;
		double col1;
		double row2;
		double col2;
	}RoiRegion;

	int IsInputPixel; //是否传入比例
	double pixSize;            //比例尺大小

	int IsShowRegionAfterDet; //测后是否显示得出的区域
	int maxAngle;        //最大角度
	int IsRefer;         //参考矩阵
	int whiteBlackDir[4];   //边缘灰度类型,0黑白_1白黑    //上白黑，下黑白,左白黑，右黑白

	int searchDir[4];       //搜索方向 内至外0，外至内1

	int roiNum[4];          //每个区域所用到的搜索区域数目

	int extendSearchLine[4];//扩大搜寻线
	int minGrayDiff[4];  //最小灰度差异
	int searchLineGap[4];   //扫描线间距
	int lineOffset[4];      //边线偏移
	double clipfactor[4];   //边缘线拟合的系数，1.0,1.5,2.0,2.5,3.0

	int IsFixWidth[4];      //是否采用固定长度
	int fixWidth[4];        //固定长度

	double TmpCenterRow;
	double TmpCenterCol;   //模板中心的坐标

						   //错误范围
	int IsErrWidth;      //是否启用宽度
	double errWidthMin;
	double errWidthMax;
	int IsErrHeight;
	double errHeightMin;
	double errHeightmax;
	int IsErrAngle;
	int errAngle;
	int IsErrCenterOffset;
	double errCenterOffset;

	vector<RoiRegion> RoiRegionVector;  //[0]进行二值的区域,[1]-[4]为上右下左的搜1索框

										//传入区域比较
	int IsUseCompare;  //是否启用传入区域比较

	int backGray;        //将要被填充背景的灰度值
	int inputRegionLarge;  //传入区域在作为排除区域的时候进行膨胀（-腐蚀）处理，
	int offThickMin;      //超出区域最小厚度，可为负数
	int IsOffArea;        //是否使用超出厚度像素值
	int errOffAreaMin;    //超出厚度像素值最小值
	int IsShowSegDiff;    //是否显示分割区域

private:
	void iniData();
};

