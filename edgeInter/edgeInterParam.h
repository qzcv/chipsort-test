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
	int IsShowRegionAfterDet; //����Ƿ���ʾ�ó�������
	int IsRefer;         //�ο�����

	int gradient[MAX_EDGE_LINE];     //�߽�Աȶ�
	int stepVal[MAX_EDGE_LINE];         //��ֱ�߽緽��������Χ
	int pol[MAX_EDGE_LINE];          //�߽緽��
	int first[MAX_EDGE_LINE];        //�߽��Ƿ��ǵ�һ��(1)�������Աȶ�����(0)
	int degRange[MAX_EDGE_LINE];     //�Ƕȷ�Χ
	int rmDist[MAX_EDGE_LINE];       //���Χ��ĵ���Ϊ�����ų�
	int iterateNum[MAX_EDGE_LINE];   //���ֱ��ʱ��������

	int lineOffset[MAX_EDGE_LINE];      //����ƫ��

										//����Χ
	int IsErrInter;      //�Ƿ����ý������
	double errInterMin;
	double errInterMax;

	RoiRegion roiReg[MAX_EDGE_LINE];
	int currentEdge;  //��ǰ�ı߽�
	double pixSize;   //ͼ�������
	double interOffset;  //������벹��ֵ 

						 //�ο�����
	double referX, referY;    //�ο����ĵ�����ֵ
	int IsErrX, IsErrY;       //������������
	double errXMin, errXMax;  //����X���귶Χ
	double errYMin, errYMax;  //����X���귶Χ

private:
	void iniData();
};

