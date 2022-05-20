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
	int IsShowRegionAfterDet; //����Ƿ���ʾ�ó�������
	int IsRefer;         //�ο�����

	int leadDir;    //Ŀ��ĳ��� ,����0������1������2������3
	int IsWhite;         //Ŀ���Ǻ�ɫ0���ǰ�ɫ1


	int gradient[MAX_EDGE_LINE];     //�߽�Աȶ�
	int stepVal[MAX_EDGE_LINE];         //��ֱ�߽緽��������Χ
	int pol[MAX_EDGE_LINE];          //�߽緽��
	int first[MAX_EDGE_LINE];        //�߽��Ƿ��ǵ�һ��(1)�������Աȶ�����(0)
	int degRange[MAX_EDGE_LINE];     //�Ƕȷ�Χ
	int rmDist[MAX_EDGE_LINE];       //���Χ��ĵ���Ϊ�����ų�
	int iterateNum[MAX_EDGE_LINE];   //���ֱ��ʱ��������

	int lineOffset[MAX_EDGE_LINE];      //����ƫ��
	int leadThre[MAX_EDGE_LINE];     //�ż�Աȶ�

									 //�ָ����
	int leadGray[2];    //��С�Ҷ�
	int leadAreaMin[2]; //��С���

						//����Χ
	int IsErrWarp;      //�Ƿ����ùܽ�����
	double errWarpMin;
	double errWarpMax;
	int IsErrWarpDiff;  //�Ƿ����������������ԣ�����
	double errWarpDiffMin;
	double errWarpDiffMax;

	RoiRegion roiReg[MAX_EDGE_LINE];
	int currentEdge;  //��ǰ�ı߽�
	double pixSize;   //ͼ�������
	int isInputPix;   //�Ƿ������

	double warpOffset;  //������벹��ֵ 

private:
	void iniData();
};

