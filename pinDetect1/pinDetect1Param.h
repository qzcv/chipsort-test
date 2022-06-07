#pragma once
#include <QString>
#include <vector>

#define GRP_NUM 2   //�ܽ�������
#define GRP_MAXLEAD 10 //�����ܽ������ܽ���Ŀ

using std::vector;

class pinDetect1Param
{
public:
	explicit pinDetect1Param();
	~pinDetect1Param();
	virtual void readWriteParam(bool r, const QString& dirPath, int level);

	typedef struct {
		double row1;
		double col1;
		double row2;
		double col2;
	}RoiRegion;

	typedef struct {
		double row;
		double col;
		double angle;
		double len1;
		double len2;
	}Rec2Region;

	//int onlyUseEdge1;            //������������ʹ�õ�һ��������ĵ��ߺ͹ܽż�ı߽�
	int currentGrp;              //ui��ĵ�ǰ���
	int IsRefer[GRP_NUM];        //�ο�����
	int IsDetBase;      //���оƬ���ߣ�0Ϊ�������ߣ�1Ϊ�����ߣ�2Ϊ�������,�������ʹ��ͬһ
	int InputBaseIndex[GRP_NUM]; //������ߵ�λ�ã���������
	int baseLineOffset[GRP_NUM]; //����ƫ��
	int LRorUD;                //�ܽŷ���,0��ֱ1ˮƽ

							   //int grpEnable[GRP_NUM];      //�ܽ���ʹ��
	int IsBaseInvert[GRP_NUM];     //���߷��򣬼�ⷽ���෴

	int IsReferEdgeRev; //�Ƿ�ʹ�ñ߽�ο��Ե�

	int grpPinNum[GRP_NUM];      //����Ĺܽ���Ŀ

	int whiteBlackDir[GRP_NUM];   //��Ե�Ҷ�����,0�ڰ�_1�׺�    //�ϰ׺ڣ��ºڰ�,��׺ڣ��Һڰ�
	int searchDir[GRP_NUM];    //��Ѱ����,0����,1����2���£�3����  
	int extendSearchLine[GRP_NUM];//������Ѱ��
	int minGrayDiff[GRP_NUM];  //���վ�ߵ���С�ҶȲ���
	int WidthGrayDiff[GRP_NUM];   //����ȵĻҶȲ�
	int widSearchLength[GRP_NUM]; //�����ʱ��������򳤶�
	int searchLineGap[GRP_NUM];   //ɨ���߼��
	int widLineOffset[GRP_NUM];      //���ȵı���ƫ��
	int widOffsetRange[GRP_NUM];  //���ƫ�Ʒ�Χ
								  //int standoffOffset[GRP_NUM];  //ƫ��������ı��ߵľ��룬������վ�߲��ֵĿ��
	int deleteEdge[GRP_NUM];      //ȥ��ͷβ������ļ�����

	double clipfactor[GRP_NUM];   //��Ե����ϵ�ϵ����1.0,1.5,2.0,2.5,3.0
	int clipDis[GRP_NUM];
	int IsAmpBest[GRP_NUM];       //�Ƿ����������ж���ѹܽű߽�

	double sizeRatio[GRP_NUM];       //�ߴ��������
	int IsInputRatio[GRP_NUM];       //�Ƿ���ߴ��������

	int CrossGrayValid[GRP_NUM];     //���ô�Խֵ
	int crossGray[GRP_NUM];
	int crossStep[GRP_NUM];          //�߽���

									 //�����ж�
	int minArea[GRP_NUM];   //�ܽ���С�����ʵ�������ӵ��������
	int noiseGray[GRP_NUM]; //���Ҷ�ֵ��������ӵ����С�Ҷ�
	int multGrayTimes[GRP_NUM];  //���ĻҶȶ�μ��Ĵ���

								 //��Χ 
	int errPinGapValid[GRP_NUM];           //�Ƿ����ùܽż��
	double errPinGapMin[GRP_NUM][GRP_MAXLEAD];  //�ܽż����Сֵ��
	double errPinGapMax[GRP_NUM][GRP_MAXLEAD];  //���ֵ
	int errPinLengthValid[GRP_NUM];      //�Ƿ�����վ�����ж�
	double errPinLengthMin[GRP_NUM][GRP_MAXLEAD];  //վ������С1ֵ
	double errPinLengthMax[GRP_NUM][GRP_MAXLEAD];  //վ�������1ֵ
	int errPinStandOffDiffValid[GRP_NUM];  //�Ƿ�����վ���Ȳ����ж�
	double errPinStandOffDiffMin[GRP_NUM];
	double errPinStandOffDiffMax[GRP_NUM];
	int errPinWidthValid[GRP_NUM];         //�Ƿ����ùܽſ��
	double errPinWidthMin[GRP_NUM][GRP_MAXLEAD];
	double errPinWidthMax[GRP_NUM][GRP_MAXLEAD];

	int errPinSweepValid[GRP_NUM];
	double errPinSweepMin[GRP_NUM][GRP_MAXLEAD];
	double errPinSweepMax[GRP_NUM][GRP_MAXLEAD];

	int IsSameValid; //�Ƿ����е�������������valid����ͬһֵ

	int shoulderShift[GRP_NUM];  //�粿sweep��׼����ȡ��ƫ��refer�ߵľ���

	int widthMethod; //�����������,0�̶���1��С��2���3��С���,����ͬ����sweep

					 //����ֵ
	double lengthCorr[GRP_NUM];    //վ��������
	double widthCorr[GRP_NUM];       //��Ȳ���
	double InterCorr[GRP_NUM];       //��ಹ��

	double lengthCorrNew[GRP_NUM][GRP_MAXLEAD];    //վ��������
	double widthCorrNew[GRP_NUM][GRP_MAXLEAD];       //��Ȳ���
	double InterCorrNew[GRP_NUM][GRP_MAXLEAD];       //��ಹ��
	int isUpdateCorr;    //�Ƿ�ִ�жԲ���ֵ�ĸ���

						 //autoThre
	RoiRegion AutoThreReg[GRP_NUM]; //�Զ��ָ�����
	int ThreMinArea[GRP_NUM];       //�ָ���� 
	int ThreGrayVal[GRP_NUM];       //�ָ���ֵ
	int ThreLeadSize[GRP_NUM];      //�ܽ��Ƽ��ߴ�
	int IsAutoWhite[GRP_NUM];       //��ɫ�ܽ�

									//�ų�����
	int offsetUp;    //ǰ��ģ�鴫�����ı��巶Χ��ƫ��
	int offsetDown;
	int offsetLeft;
	int offsetRight;
	bool IsUseInputRegion;  //�Ƿ�ʹ�ô���������Ϊ��������

	vector<RoiRegion> RoiVec[GRP_NUM];  //������Ĺܽ������򣬵�һ���Ǽ��߽�Ŀ򣬺���Ϊ�ܽſ�
	Rec2Region SearchEdgeRoi;        //������ʱ���ų��ⲿ�����ROI
	int IsDelOffSearchRoi;           //�Ƿ��ų���Ϳ�ڣ�����������
	int IsShowBlack;

private:
	void iniData();
};

