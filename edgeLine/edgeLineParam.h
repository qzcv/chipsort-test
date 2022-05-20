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
	int IsShowRegionAfterDet; //����Ƿ���ʾ�ó�������
	int IsRefer;         //�ο�����

	int gradient[4];     //�߽�Աȶ�
	int stepVal[4];         //��ֱ�߽緽��������Χ
	int pol[4];          //�߽緽��
	int first[4];        //�߽��Ƿ��ǵ�һ��(1)�������Աȶ�����(0)
	int degRange[4];     //�Ƕȷ�Χ
	int rmDist[4];       //���Χ��ĵ���Ϊ�����ų�
	int iterateNum[4];   //���ֱ��ʱ��������

	int lineOffset[4];      //����ƫ��
	int IsFixWidth[4];      //�Ƿ���ù̶�����
	int fixWidth[4];        //�̶�����

	double TmpCenterRow;
	double TmpCenterCol;   //ģ�����ĵ�����

						   //����Χ
	int IsErrWidth;      //�Ƿ����ÿ��
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

