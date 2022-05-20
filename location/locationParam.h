#pragma once
#include <QString>
#include <vector>

#define GRP_MAX_LEADNUM 15 //����ܽ������Ŀ

using std::vector;

class locationParam
{
public:
	explicit locationParam();
	~locationParam();
	virtual void readWriteParam(bool r, const QString& dirPath, int level);

	typedef struct {
		float row1;
		float col1;
		float row2;
		float col2;
	}RoiRegion;
	int IsEndPoint;      //�Ƿ���˵�0�ر�1����
	int IsPadStand;     //�Ƿ�����pad���͵�վ�ߣ�����ʱֻ����վ�߶��㣬�Ҽ���վ��ʱ�Ĺ�ʽ���޸ġ���ȣ�������Ϣ����ʹ��

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
	int IsPaichuLine[4];    //�Ƿ������ų���,�߽�����䳬�����پͻᱻ�����ܽż��ѳ����ĵ�
	int paichuLine[4];      //�ų���
	double scalePaichu[4];      //��԰�ʿ1212�ȱ߽�ϳ��Ĳ�Ʒ��Ϊ�˷�ֹ��ƫ�ƽϴ���ų�����ɵ�Ӱ�죬����ֻ���ǲ��ֵı߽硣��ʼ0, 0-1.0
	int isLeadSize[4];  //�Ƿ���ùܽųߴ���п�����ȥ���ܽż������ұ߽�
	int leadSize[4];    //�ܽųߴ�

	int IsCutLine[4];       //�Ƿ�Ͽ�����
	int offsetPlus[4];      //ƫ��ֵ�������������¶��������������Ҷ�
	int minArea[4];         //����������ı����г���������(վ��)����С��� 
	int standoffOffset[4];  //ƫ��������ı��ߵľ��룬������վ�߲��ֵĿ��
	int grayVal[4];         //����������ı����г���������ĻҶ���ֵ
	int edgeGray[4]; //ɨ��˵��ʱ���measure_rec�ĻҶ�
					 //int pinSearchWidth[4];  //�ܽ���������ʱ��ָ�ܽ��������õ��������Ŵ�С
	double clipfactor[4];   //��Ե����ϵ�ϵ����1.0,1.5,2.0,2.5,3.0

	int IsEdgeDir[4];       //�Ƿ����ñ߽��ķ�����ɸѡ�����ڸ����ų��ܽżⲿ�ı߽��

	double TmpCenterRow;
	double TmpCenterCol;   //ģ�����ĵ�����
	double standOffCorr[2][GRP_MAX_LEADNUM];    //վ����������/��,����
	double widthCorr[2][GRP_MAX_LEADNUM];       //��Ȳ���
	double InterCorr[2][GRP_MAX_LEADNUM];       //��ಹ��

												//OFFSET
	double offStandOffCorr[2][GRP_MAX_LEADNUM];    //վ����������/��,����   
	double offWidthCorr[2][GRP_MAX_LEADNUM];       //��Ȳ���
	double offInterCorr[2][GRP_MAX_LEADNUM];       //��ಹ��

	double selfStandOffCorr[2];        //�Լ��ӵ�վ�߲���

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
	int errCoplanarValid;         //�Ƿ���ù�����

	double errPinGapMin[2][GRP_MAX_LEADNUM];  //�ܽż����Сֵ��
	double errPinGapMax[2][GRP_MAX_LEADNUM];  //���ֵ

	double errPinStandOffMin[2][GRP_MAX_LEADNUM];  //վ������С1ֵ
	double errPinStandOffMax[2][GRP_MAX_LEADNUM];  //վ�������1ֵ

	double errPinStandOffDiffMin[2];
	double errPinStandOffDiffMax[2];
	double errCoplanarMin;
	double errCoplanar;

	double errPinWidthMin[2][GRP_MAX_LEADNUM];
	double errPinWidthMax[2][GRP_MAX_LEADNUM];

	vector<RoiRegion> RoiRegionVector;  //[0]Ϊ������Χ,[1]-[4]Ϊ�������ҵ���1����

	int NowGrp;  //�������ң���ǰ������

private:
	void iniData();
};

