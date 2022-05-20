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
	void setEdgeReferAngle(bool referValid,double angle=0); //�Ƿ����ýǶ��Լ��ο��Ƕ�
	void setPolarAndOrient(Polarity pol,Orient orient);
	void setThreshold(int thre);
	void setSearchParam(int searchGap,int searchWidth);
	void setFitFactor(double factor);
	void getEdgePoints(HTuple& edgeRow,HTuple& edgeCol); //���ؼ������ı߽������
	void setEdgeMethod(bool ransacValid,bool sortValid); //
	void setContinuousRefer(bool valid ,int referPointsNumber);  //�Ƿ�����ȡ�����Ա߽�ʱ���ο�ǰ������Ծ���
	ResType calculateLine(const Hobject* img, CvPoint2D32f line[2], int& lineNum);  //����߽���ʼ��
	//bool calculateEdgePoints(const Hobject* img, int isWhite ,HTuple& row1,HTuple& col1,HTuple& row2,HTuple& col2,double percent); //�����Ե�Ŀ�ȵ�
	~fitEdgeLine();

private:
	void calRangePoints(); //������ұ߽�ʱ���ROI����ʼ�����
	bool measureEdgePoints(const Hobject* img,bool isPairEdge=0); //����߽��,�ڶ������������Ƿ���������߽�
	bool fitEdgePoints();  //����߽�㣬���б߽����
	bool sortEdgePointsGroup(); //�Ա߽�㼯���з��飬�ֳɶ���߽�ı߽�㼯
	HTuple m_edgeRow1,m_edgeCol1; //��¼�����б߽��
	HTuple m_edgeRow2,m_edgeCol2; //��¼�����б߽��
	HTuple m_lineRow[2],m_lineCol[2]; //��¼�����б߽��ߵĶ˵�
	CvPoint2D32f *m_p[2]; //���ұ߽�ʱ���ROI����ʼ��������㣬0,1˳��Ϊʸ������ʸ������Ϊorient�Ƕȼ�PI/2.0֮��ĽǶȣ�

	int m_roiNum;
	bool m_ransacValid;   //�Ƿ�ִ��ransac����߽��
	bool m_sortValid;     //�Ƿ�Ա߽����з���
	bool m_continousReferValid; //�Ƿ���������߽��ο�
	int m_referPointsNumber;    //�����ο�����Ŀ
	HTuple m_referHommat; //��ת����
	bool m_referAngleValid; //�Ƿ����òο��Ƕ�
	bool m_referAngle;      //���߲ο�
	//int *m_RoiWidth;     //roi���
	//int *m_RoiHeight;    //roi�߶�,���ڼ�����ӵ�mealength1

	roirectType m_roirectType;

	CvRect *m_roiRect;  //���洢������roiȫ���Դ˷�ʽ�洢
	double *m_roiAngle;


	int m_threshold;    //�ҶȲ���
	double m_factor;    //�������
    int m_searchWidth;  //ɨ��ʱ�ĺ�����
	int m_searchGap;      //ɨ����
	double m_angle;     //���������ָ��Ƕ�(������ת��ĽǶ�)
	Polarity m_polar;
	Orient m_orient;
	Hlong m_imgH,m_imgW;  //ͼƬ���
	const Hobject* m_image;   
	int m_skipGap;      //ִ�б߽�������Ե�ʱ������ľ���
	int m_selcetOutsider; //��ѡȡ���ı߽�����ʱ��ѡ����ʸ������Ļ��ǿ��ڵı߽�
};

}
