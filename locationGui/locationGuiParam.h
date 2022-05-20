#pragma once
#include <QString>
#include <vector>
#define GRP_MAX_LEADNUM 10 //����ܽ������Ŀ
using std::vector;

class locationGuiParam
{
public:
	explicit locationGuiParam();
	~locationGuiParam();
	virtual void readWriteParam(bool r, const QString& dirPath, int level);

	typedef struct {
		float row1;
		float col1;
		float row2;
		float col2;
	}RoiRegion;

	int edgeSearchOrder; //0Ϊ��������_1Ϊ��������
	int IsEndPoint;      //�Ƿ���˵�0�ر�1����

	int upDwOrLfRt;      //�������վ�߻�������վ�ߣ�0��1

	int maxAngle;        //���Ƕ�
	int UpDetect;        //�����ϼ��
	int DwDetect;        //�����¼��
	int LfDetect;        //��������
	int RtDetect;        //�����Ҽ��

	int grpPinNum[2];       //��1��2�Ĺܽ���Ŀ

	int whiteBlackDir[4];   //��Ե�Ҷ�����,0�ڰ�_1�׺�    //�ϰ׺ڣ��ºڰ�,��׺ڣ��Һڰ�
	int searchDir[4];    //��Ѱ����,0����,1����  
	int extendSearchLine[4];//������Ѱ��
	int minGrayDiff[4];  //��С�ҶȲ���
	int searchLineGap[4];   //ɨ���߼��
	int lineOffset[4];      //����ƫ��
	int IsPaichuLine[4];    //�Ƿ������ų���
	int paichuLine[4];      //�ų���
	int IsCutLine[4];       //�Ƿ�Ͽ�����
	int offsetPlus[4];      //ƫ��ֵ�������������¶��������������Ҷ�
	int minArea[4];         //����������ı����г���������(վ��)����С��� 
	int standoffOffset[4];  //ƫ��������ı��ߵľ��룬������վ�߲��ֵĿ��
	int grayVal[4];         //����������ı����г���������ĻҶ���ֵ
	int edgesearchWidth[4]; //ɨ��˵��ʱ���measure_rec�Ŀ��
	int pinSearchWidth[4];  //�ܽ���������ʱ��ָ�ܽ��������õ��������Ŵ�С
	double clipfactor[4];   //��Ե����ϵ�ϵ����1.0,1.5,2.0,2.5,3.0
	int IsUseStand[4];      //�Ƿ�����վ��

	double TmpCenterRow;
	double TmpCenterCol;   //ģ�����ĵ�����
	double standOffCorr[2];    //վ����������/��,����
	double widthCorr[2];       //��Ȳ���
	double InterCorr[2];       //��ಹ��


	double pinlength;      //����Ĺܽ�ʵ�ʳ���

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

	//��Χ
	int errPinGapValid;           //�Ƿ����ùܽż��
	int errPinStandOffValid;      //�Ƿ�����վ�����ж�
	int errPinStandOffDiffValid;  //�Ƿ�����վ���Ȳ����ж�
	int errPinWidthValid;         //�Ƿ����ùܽſ��

	double errPinGapMin[2][GRP_MAX_LEADNUM];  //�ܽż����Сֵ��
	double errPinGapMax[2][GRP_MAX_LEADNUM];  //���ֵ

	double errPinStandOffMin[2][GRP_MAX_LEADNUM];  //վ������С1ֵ
	double errPinStandOffMax[2][GRP_MAX_LEADNUM];  //վ�������1ֵ

	double errPinStandOffDiffMax[2];

	double errPinWidthMin[2][GRP_MAX_LEADNUM];
	double errPinWidthMax[2][GRP_MAX_LEADNUM];
	int LTnum;
	int RBnum;
	int ChildTablenum;

	vector<RoiRegion> RoiRegionVector;  //[0]Ϊ������Χ,[1]-[4]Ϊ�������ҵ���1����

private:
	void iniData();
};

