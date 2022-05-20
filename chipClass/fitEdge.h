#pragma once
#include <cv.h>
#include "HalconCpp.h"
#include <vector>
#include <memory.h>
#include <set>
#include <iostream>
#include <random>
#include "inclu.h"

using std::vector;
using namespace Halcon;
using namespace std;

namespace danke{

class EXPORTS fitEdgeLine
{
public:

	typedef enum{ToWhite=0, ToBlack}Polarity;
	typedef enum{ToUp=0,ToDown,ToLeft,ToRight}Orient;
	typedef enum{rect1=0,rect2}roirectType;
	typedef enum{ResOK=0,PointsNg,SortNg,FitNg}ResType;

	fitEdgeLine();
	//int RansacLinear(stPoint* pstData,unsigned int nDataCnt,
	//	unsigned int nMinCnt,unsigned int nMaxIterCnt,
	//	unsigned int nConsesusCntThreshold, double dMaxErrorThreshold,
	//	double& dAA,double& dBB,double& dCC,double& dModelMeanError);
	void setRoi(int roiNum, const int* row1,const int* col1,const int* row2,const int* col2);
	void setRoi(int roiNum, const int* row, const int* col, const int* length1, const int* length2, const double* angle );

	void setRefer(HTuple hommat);
	void setEdgeReferAngle(bool referValid,double angle=0); //是否启用角度以及参考角度
	void setPolarAndOrient(Polarity pol,Orient orient);
	void setThreshold(int thre);
	void setSearchParam(int searchGap,int searchWidth);
	void setFitFactor(double factor);
	void getEdgePoints(HTuple& edgeRow,HTuple& edgeCol); //返回检测出来的边界坐标点
	void setEdgeMethod(bool ransacValid,bool sortValid); //
	void setContinuousRefer(bool valid ,int referPointsNumber);  //是否在求取连续性边界时，参考前后点的相对距离
	ResType calculateLine(const Hobject* img, CvPoint2D32f line[2], int& lineNum);  //计算边界起始点
	//bool calculateEdgePoints(const Hobject* img, int isWhite ,HTuple& row1,HTuple& col1,HTuple& row2,HTuple& col2,double percent); //计算边缘的宽度点
	~fitEdgeLine();

private:
	void calRangePoints(); //计算查找边界时候的ROI的起始坐标点
	bool measureEdgePoints(const Hobject* img,bool isPairEdge=0); //计算边界点,第二个参数决定是否采用两个边界
	bool fitEdgePoints();  //整理边界点，进行边界拟合
	bool sortEdgePointsGroup(); //对边界点集进行分组，分成多组边界的边界点集
	HTuple m_edgeRow1,m_edgeCol1; //收录的所有边界点
	HTuple m_edgeRow2,m_edgeCol2; //收录的所有边界点
	HTuple m_lineRow[2],m_lineCol[2]; //收录的所有边界线的端点
	CvPoint2D32f *m_p[2]; //查找边界时候的ROI的起始结束坐标点，0,1顺序为矢量方向（矢量方向为orient角度加PI/2.0之后的角度）

	int m_roiNum;
	bool m_ransacValid;   //是否执行ransac处理边界点
	bool m_sortValid;     //是否对边界点进行分组
	bool m_continousReferValid; //是否进行连续边界点参考
	int m_referPointsNumber;    //连续参考点数目
	HTuple m_referHommat; //旋转矩阵
	bool m_referAngleValid; //是否启用参考角度
	bool m_referAngle;      //底线参考
	//int *m_RoiWidth;     //roi宽度
	//int *m_RoiHeight;    //roi高度,用于检测算子的mealength1

	roirectType m_roirectType;

	CvRect *m_roiRect;  //将存储进来的roi全部以此方式存储
	double *m_roiAngle;


	int m_threshold;    //灰度差异
	double m_factor;    //拟合因素
    int m_searchWidth;  //扫描时的横向宽度
	int m_searchGap;      //扫描间距
	double m_angle;     //搜索区域的指向角度(包含旋转后的角度)
	Polarity m_polar;
	Orient m_orient;
	Hlong m_imgH,m_imgW;  //图片宽高
	const Hobject* m_image;   
	int m_skipGap;      //执行边界点分组策略的时候跳变的距离
	int m_selcetOutsider; //在选取最后的边界点分组时，选择方向矢量靠外的还是靠内的边界
};

}
