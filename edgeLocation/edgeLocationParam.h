#pragma once
#include <QString>
#include <vector>

using std::vector;

class edgeLocationParam
{
public:
	explicit edgeLocationParam();
	~edgeLocationParam();
	virtual void readWriteParam(bool r, const QString& dirPath, int level);

	typedef struct {
		double row1;
		double col1;
		double row2;
		double col2;
	}RoiRegion;

	int IsInputPixel; //是否传入比例
	double pixSize;            //比例尺大小
	int IsCsp;

	int IsShowRegionAfterDet; //测后是否显示得出的区域，0-不显示任何内容，1-显示轮廓，2-显示所有内容
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

	int IsFixWidth[4];      //是否采用固定长度,即长度为与roi的相关交点
	int fixWidth;        //固定长度
	int IsSideNoDetect;    //是否部分边不检测
	int SideDetectType;    //0-上下，1-左右，2单边
	int onlyOneSideIndex;  //唯一不检测边

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

	int IsErrWidthDiff;
	double errWidthDiff;
	int IsErrHeightDiff;
	double errHeightDiff;

	int CrossGrayValid[4];     //启用穿越值
	int crossGray[4];
	int crossStep[4];          //边界厚度

	vector<RoiRegion> RoiRegionVector;  //[0]为搜索范围,[1]-[4]为上右下左的搜1索框

private:
	void iniData();
};

