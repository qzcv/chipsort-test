#pragma once
#include <QString>
#include <vector>

#define GRP_MAX_LEADNUM 15//����ܽ������Ŀ

using std::vector;

class standLocationNewParam
{
public:
	explicit standLocationNewParam();
	~standLocationNewParam();
	virtual void readWriteParam(bool r, const QString& dirPath, int level);

	typedef struct {
		float row1;
		float col1;
		float row2;
		float col2;
	}RoiRegion;

	int upDwOrLfRt;      //�������վ�߻�������վ�ߣ�0��1
	int UpDetect;        //�����ϼ��
	int DwDetect;        //�����¼��
	int LfDetect;        //��������
	int RtDetect;        //�����Ҽ��

	int grpPinNum[2];       //��1��2�Ĺܽ���Ŀ

	int extendSearchLine[4];//������Ѱ��
	int minGrayDiff[4];  //��С�ҶȲ���
						 //float grayPlus[4];   //�ҶȲ���ֵ
						 //int grayMin[4];      //��С�Ҷ�ֵ
						 //int grayMax[4];      //���Ҷ�ֵ

	int minArea[4];         //����������ı����г���������(վ��)����С��� 
	int standoffOffset[4];  //ƫ��������ı��ߵľ��룬������վ�߲��ֵĿ���
	int grayVal[4];         //����������ı����г���������ĻҶ���ֵ

	double standOffCorr[2][GRP_MAX_LEADNUM];    //վ����������/��,����

												//OFFSET
	double offStandOffCorr[2][GRP_MAX_LEADNUM];    //վ����������/��,����   
	double selfStandOffCorr[2];        //�Լ��ӵ�վ�߲���

									   //��Χ
	int errPinStandOffValid;      //�Ƿ�����վ�����ж�
	int errPinStandOffDiffValid;  //�Ƿ�����վ���Ȳ����ж�
	int errCoplanarValid;         //�Ƿ���ù�����

	int IsLMSValid;  //�Ƿ�ʹ��LMS�㷨��ȡ�����ƽ��
	double LMSFactor;

	double errPinStandOffMin[2][GRP_MAX_LEADNUM];  //վ������С1ֵ
	double errPinStandOffMax[2][GRP_MAX_LEADNUM];  //վ�������1ֵ

	double errPinStandOffDiffMin[2];
	double errPinStandOffDiffMax[2];
	double errCoplanarMin;
	double errCoplanar;

	double packageOff;  //�ܷⲹ��

	vector<RoiRegion> RoiRegionVector;  //[0]Ϊ������Χ,[1]-[4]Ϊ�������ҵ���1����

	int NowGrp;  //�������ң���ǰ������

private:
	void iniData();
};
