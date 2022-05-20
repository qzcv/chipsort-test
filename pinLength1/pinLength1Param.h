#pragma once
#include <QString>
#include <vector>
#define MAX_PINGRP_NUM 20  //�������ܽ���Ŀ
using std::vector;

class pinLength1Param
{
public:
	explicit pinLength1Param();
	~pinLength1Param();
	virtual void readWriteParam(bool r, const QString& dirPath, int level);

	typedef struct {
		double row1;
		double col1;
		double row2;
		double col2;
		//int threshold;//ÿ1�������ڵ���ֵ
	}segRegion;

	typedef struct {
		double row;
		double col;
		double angle;
		double len1;
		double len2;
	}Rec2Region;

	//��λ����
	int IsReferSection;//�Ƿ����òο�ģ�飬�߽���

	int PinAreaRefer;  //��ǰ��ģ�鴫��������ת���󣬶�λ�ܽ�����

	double offsetUp;    //ǰ��ģ�鴫�����ı��巶Χ��ƫ��
	double offsetDown;
	double offsetLeft;
	double offsetRight;

	double roiOffUp;    //ǰ��ģ�鴫�����ı��巶Χ��ƫ��
	double roiOffDown;
	double roiOffLeft;
	double roiOffRight;
	bool IsUseInputRegion;  //�Ƿ�ʹ�ô���������Ϊ��������

	int IsEdgeSopType;  //ʹ�ò�Ʒ�����������õ���ʶ�𷽷�
	int IsReferEdgeRev; //�Ƿ�ʹ�ñ߽�ο��Ե�
	int IsInterUseEdgeAngle; //��ȡ���sweep��ʹ�òο��߽�Ƕ�
	int IsOnlyReferEdge[2]; //�Ƿ���ο��߽�Ƕȶ���ǿ�ƶ���

							//�ܽŶ�λ
	int openAndClosing;            //���ղ����ĳߴ��С
								   //int pinLfUpCont,pinRtDwCont;   //��1����Ŀ,ui�ϵ�ֵ
	int pinCont[2];   //��1����Ŀ,ui�ϵ�ֵ
	int LRorUD;                //��λ������,0��ֱ1ˮƽ
	int IsWhitePin;                //��1����ð�ɫ�ܽţ�0�ر�1����
	int IsIgnoreRoot;              //��1����Ը�����0�ر�1����
	int IsThreshold;               //��1��������ֵ����
	int threValue;                 //��1ֵ�����ֵ
	int pinSize;               //��1�ż��
	int leadAreaMin;                  //ȥ1��minֵ
	int IsWidth;                   //�Ƿ�ѡȡ���
	int widthMin;
	int widthOffset;          //���ƫ��
	int leadContrast;         //�ܽ�ʶ��߽�ĻҶ�

	int IsSOPType; //0:qfn,1:sop


				   //�������
	double LeadLengthCorr[2];          //��1�ϳ�������
	double LeadWidthCorr[2];           //��1�Ͽ������
	double LeadInterCorr[2];           //��1�ϼ������

	double LeadLengthCorrNew[2][MAX_PINGRP_NUM]; //��1�ϳ�������
	double LeadWidthCorrNew[2][MAX_PINGRP_NUM];  //��1�Ͽ������
	double LeadInterCorrNew[2][MAX_PINGRP_NUM];  //��1�ϼ������
	int IsUpdateCorr;

	double LeadSpanCorr[MAX_PINGRP_NUM];         //��ಹ��

	double SizeRation[2];            //���ϡ����µĳߴ������
	int IsInputSize;                 //�Ƿ���������� 
									 //��Χ
	int errWidthValid[2];             //���ϣ���1�ÿ�ȣ�0�ر�1����
	double errWidthMin[2][MAX_PINGRP_NUM], errWidthMax[2][MAX_PINGRP_NUM];
	int errLengthValid[2];            //���ϣ���1�ó��ȣ�0�ر�1����
	double errLengthMin[2][MAX_PINGRP_NUM], errLengthMax[2][MAX_PINGRP_NUM];
	int errInterValid[2];             //���ϣ���1�ü�࣬0�ر�1����
	double errInterMin[2][MAX_PINGRP_NUM], errInterMax[2][MAX_PINGRP_NUM];
	int errMaxDiffValid[2];           //���ϣ���1����󳤶Ȳ��죬0�ر�1����
	double errMaxDiffMin[2];
	double errMaxDiff[2];
	int errSpanValid;                 //���
	double errSpanMin[MAX_PINGRP_NUM];
	double errSpanMax[MAX_PINGRP_NUM];

	int errSweepValid[2];

	int sweepOffset[2];

	int IsHNW;  //��ŵ΢��������ʱ��������Ŀ����ȡ��ʽ

				//cross
	int CrossGrayValid;     //���ô�Խֵ
	int crossGray;
	int crossStep;          //�߽���


	int errAllMaxDiffValid;            //ȫ1������������
	double errAllMaxDiffMin;
	double errAllMaxDiff;

	//ROI��Ϣ
	vector<segRegion> segRegionVector;   //0Ϊ��/��ROI��1Ϊ��/��ROI��ʣ���Ϊ���ҹܽŶ�λ��1��
	//int segRoiCounts[2];    //�ָ��������������roi��Ŀ����ͬ��UI�ϵĹܽ���Ŀ

	segRegion rotateRoi;  //������ת����ʹ�õ�roi
	Rec2Region SearchEdgeRoi;   //������ʱ���ų��ⲿ�����ROI
	int IsDelOffSearchRoi;      //�Ƿ��ų���Ϳ�ڣ�����������
	int backGray;               //�����Ҷ�    

	int IsUseDir;         //�Ƿ�ʹ�ýǶ�����������ת�ܽ�����ķ���
	int AngleType;        //0: 0/180   1:0/90/180/270
	int startAngle;       //��ʼ�Ƕȣ���ǰ�ü���ģ�鱣��һ��

	int IsShowAfterDet;   //�����ɺ��Ƿ���ʾ����
	int leadLossIndex;    //ȱ�ŵ����
	int IsGroupEnableSame;  //��ͬ�ܽ�������Ŀ�Ƿ�ͳһ

	bool IsNoDetectTop;     //�Ƿ���ż��

private:
	void iniData();
};

