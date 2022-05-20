#pragma once
#include <QString>
#include <vector>
#include "HalconCpp.h"

using std::vector;
using namespace Halcon;

class surface_pParam
{
public:
	explicit surface_pParam();
	~surface_pParam();
	virtual void readWriteParam(bool r, const QString& dirPath, int level);

	typedef struct {
		double row;
		double col;
		double length1;
		double length2;
		double angle;
	}deleteRegion;

	double cirRow, cirCol, cirR;   //圆形区域
								   //检测ROI参数的设置
	int roiMinArea;   //设置绘制区域时候的最小筛选面积
	int roiMaxArea;   //设置绘制区域时候的最小筛选面积

	int roiThreMin;      //设置绘制区域时候的分割阈值
	int roiThreMax;      //
						 //检测区域
	int referSection; //旋转矩阵参考模组
	int roiType;          //roi的类型，0矩形，1圆形，2传入点区域，3传入面区域，4绘制区域 
	int inputAreaZoom;           //传进来的面积区域的胀缩
	int zoom_up;      //缩放
	int zoom_lf;
	int zoom_dw;
	int zoom_rt;

	int roundVal; //圆角值


	int IsPixOrAct;  //是像素尺寸还是实际尺寸，0像素，1实际
	double pixSize;            //比例尺大小

	int IsReferPtoDel;         //是否引用参考点区域作为排除区域
	int delPointZoom;          //引用参考点区域的胀缩 

							   //排除区域
	int deleteArea_section[3];  //排除模组选择
	int deleteArea_zoom[3];

	int addDeleteArea;     //添加排除区1域
	int isShowDel;         //是否测后显示排除区域

						   //瑕疵定义
	double badArea_min;
	double badArea_max;
	int black_bad_detect;
	int white_bad_detect;
	double badLengthMin;
	double badLengthMax;
	double badWidthMax;
	double badWidthMin;
	int fixThreMin;
	int fixThreMax;
	int IsGauss;      //是否对处理的原图进行高斯滤波
	int GaussSize;    //高斯滤波核的大小

	int meanSize;   //采用平滑背景时候的
	int IsFillOut;  //采用平滑背景的时候外部是否填充

	int holeAreaMin;
	int holeAreaMax;
	int holeNumMin;
	int holeNumMax;

	int minRag;     //处理毛刺时开操作的值
	int minLoss;    //处理缺失时候的闭操作的值
	int minRagLossArea; //检测出来的毛刺缺失的最小值
	int IsFitRec;   //是否拟合完整矩形以检测缺损和毛边

	int closeVal;
	int openVal;

	//误差范围
	int single_badArea_valid;
	double single_badArea_min;
	double single_badArea_max;
	int total_badArea_valid;
	double total_badArea_min;
	double total_badArea_max;
	int area_width_valid;
	double area_width_min;
	double area_width_max;
	int area_length_valid;
	double area_length_min;
	double area_length_max;

	int area_count_valid;
	int area_count_min;
	int area_count_max;

	vector<deleteRegion> deleteRegionVector;   //0为搜索区域,后续为排除区域
	Hobject roiRegion;         //绘制的ROI区域             

	typedef enum { averageThre = 0, fixedThre, meanThre, detHole, ragAndLoss, Fourier }TestType;    //检测类型
	TestType testType;   //每个ROI的检测类型
	int tmptype;   //帮助存储枚举值的

	bool IsDispROI;

private:
	void iniData();
};

