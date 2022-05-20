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

	double cirRow, cirCol, cirR;   //Բ������
								   //���ROI����������
	int roiMinArea;   //���û�������ʱ�����Сɸѡ���
	int roiMaxArea;   //���û�������ʱ�����Сɸѡ���

	int roiThreMin;      //���û�������ʱ��ķָ���ֵ
	int roiThreMax;      //
						 //�������
	int referSection; //��ת����ο�ģ��
	int roiType;          //roi�����ͣ�0���Σ�1Բ�Σ�2���������3����������4�������� 
	int inputAreaZoom;           //��������������������
	int zoom_up;      //����
	int zoom_lf;
	int zoom_dw;
	int zoom_rt;

	int roundVal; //Բ��ֵ


	int IsPixOrAct;  //�����سߴ绹��ʵ�ʳߴ磬0���أ�1ʵ��
	double pixSize;            //�����ߴ�С

	int IsReferPtoDel;         //�Ƿ����òο���������Ϊ�ų�����
	int delPointZoom;          //���òο������������ 

							   //�ų�����
	int deleteArea_section[3];  //�ų�ģ��ѡ��
	int deleteArea_zoom[3];

	int addDeleteArea;     //����ų���1��
	int isShowDel;         //�Ƿ�����ʾ�ų�����

						   //覴ö���
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
	int IsGauss;      //�Ƿ�Դ����ԭͼ���и�˹�˲�
	int GaussSize;    //��˹�˲��˵Ĵ�С

	int meanSize;   //����ƽ������ʱ���
	int IsFillOut;  //����ƽ��������ʱ���ⲿ�Ƿ����

	int holeAreaMin;
	int holeAreaMax;
	int holeNumMin;
	int holeNumMax;

	int minRag;     //����ë��ʱ��������ֵ
	int minLoss;    //����ȱʧʱ��ıղ�����ֵ
	int minRagLossArea; //��������ë��ȱʧ����Сֵ
	int IsFitRec;   //�Ƿ�������������Լ��ȱ���ë��

	int closeVal;
	int openVal;

	//��Χ
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

	vector<deleteRegion> deleteRegionVector;   //0Ϊ��������,����Ϊ�ų�����
	Hobject roiRegion;         //���Ƶ�ROI����             

	typedef enum { averageThre = 0, fixedThre, meanThre, detHole, ragAndLoss, Fourier }TestType;    //�������
	TestType testType;   //ÿ��ROI�ļ������
	int tmptype;   //�����洢ö��ֵ��

	bool IsDispROI;

private:
	void iniData();
};

