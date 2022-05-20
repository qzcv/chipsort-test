#pragma once
#include <QString>
#include <vector>

#define Max_LeadNum 20
using std::vector;

class segLeadParam
{
public:
	explicit segLeadParam();
	~segLeadParam();
	virtual void readWriteParam(bool r, const QString& dirPath, int level);

	typedef struct {
		double row1;
		double col1;
		double row2;
		double col2;
	}roiRegion;

	//检测区域
	int referSection; //旋转矩阵参考模组
	int IsReferEdge;  //是否传进边界参考

	int IsInputPixel; //是否传入比例
	double pixSize;            //比例尺大小
	int showAfterDet;  //测后是否显示区域

					   //目标定义
	int leadNum;     //管脚的数目
	int areaMin;
	int areaMax;
	int openWidth;
	int openHeight;
	int closeWidth;
	int closeHeight;
	int meanWidth;   //采用平滑背景时候的
	int meanHeight;
	int grayWhite;
	int grayBlack;
	int IsGauss;      //是否对处理的原图进行高斯滤波
	int GaussSize;    //高斯滤波核的大小
	int WOrB;         //目标是白色的还是黑色的，0白色，1黑色
	int HorOrVer;     //管脚朝向是横向还是纵向

	int IsAutoWhite;  //自动分割时颜色，0黑色，1白色

					  //regiongrowing
	int centerType;  //种子点是roi中心还是设置的种子点 0roi，1设置种子点,2不使用种子点
	int centerX, centerY;  //种子点坐标
	int rowWid;
	int colWid;
	int thresholdTol;
	int minGrowArea;

	//固定阈值
	int threMin;
	int threMax;

	//auto leadThreshold
	int autoLeadSize;  //自动分割管脚尺寸
	int autoLeadThre;  //自动分割管脚阈值

	int testType;   //检测类型,0平滑背景，1区域生长，2固定阈值

	//误差范围
	int errWidthValid;             //左上，启1用宽度，0关闭1启用
	double errWidthMin[Max_LeadNum], errWidthMax[Max_LeadNum];
	int errLengthValid;            //左上，启1用长度，0关闭1启用
	double errLengthMin[Max_LeadNum], errLengthMax[Max_LeadNum];
	int errInterValid;             //左上，启1用间距，0关闭1启用
	double errInterMin[Max_LeadNum], errInterMax[Max_LeadNum];
	int errEdgeDisValid;             //左上，启1用间距，0关闭1启用
	double errEdgeDisMin[Max_LeadNum], errEdgeDisMax[Max_LeadNum];

	vector<roiRegion> RoiVec;   //0为搜索区域,后续为各个管脚区域         

private:
	void iniData();
};

