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

	int IsInputPixel; //�Ƿ������
	double pixSize;            //�����ߴ�С

	int IsShowRegionAfterDet; //����Ƿ���ʾ�ó�������
	int maxAngle;        //���Ƕ�
	int IsRefer;         //�ο�����
	int whiteBlackDir[4];   //��Ե�Ҷ�����,0�ڰ�_1�׺�    //�ϰ׺ڣ��ºڰ�,��׺ڣ��Һڰ�

	int searchDir[4];       //�������� ������0��������1

	int roiNum[4];          //ÿ���������õ�������������Ŀ

	int extendSearchLine[4];//������Ѱ��
	int minGrayDiff[4];  //��С�ҶȲ���
	int searchLineGap[4];   //ɨ���߼��
	int lineOffset[4];      //����ƫ��
	double clipfactor[4];   //��Ե����ϵ�ϵ����1.0,1.5,2.0,2.5,3.0

	int IsFixWidth[4];      //�Ƿ���ù̶�����
	int fixWidth[4];        //�̶�����

	double TmpCenterRow;
	double TmpCenterCol;   //ģ�����ĵ�����

						   //����Χ
	int IsErrWidth;      //�Ƿ����ÿ��
	double errWidthMin;
	double errWidthMax;
	int IsErrHeight;
	double errHeightMin;
	double errHeightmax;
	int IsErrAngle;
	int errAngle;
	int IsErrCenterOffset;
	double errCenterOffset;

	vector<RoiRegion> RoiRegionVector;  //[0]���ж�ֵ������,[1]-[4]Ϊ�����������1����

										//��������Ƚ�
	int IsUseCompare;  //�Ƿ����ô�������Ƚ�

	int backGray;        //��Ҫ����䱳���ĻҶ�ֵ
	int inputRegionLarge;  //������������Ϊ�ų������ʱ��������ͣ�-��ʴ������
	int offThickMin;      //����������С��ȣ���Ϊ����
	int IsOffArea;        //�Ƿ�ʹ�ó����������ֵ
	int errOffAreaMin;    //�����������ֵ��Сֵ
	int IsShowSegDiff;    //�Ƿ���ʾ�ָ�����

private:
	void iniData();
};

