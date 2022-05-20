#pragma once
#include <QString>
#include <vector>

#define GRP_NUM 4   //�ܽ�������

using std::vector;

class pinWarpParam
{
public:
	explicit pinWarpParam();
	~pinWarpParam();
	virtual void readWriteParam(bool r, const QString& dirPath, int level);

	typedef struct {
		float row1;
		float col1;
		float row2;
		float col2;
	}RoiRegion;

	int whiteBlackDir[GRP_NUM];   //���߱�Ե�Ҷ�����,0�ڰ�_1�׺�
	int currentGrp;              //ui��ĵ�ǰ���
	int IsDetBase[GRP_NUM];      //���оƬ���ߣ�0Ϊ�������ߣ�1Ϊ������

	int grpEnable[GRP_NUM];      //�ܽ���ʹ��
	int IsBaseInvert[GRP_NUM];      //���߷��򣬼�ⷽ���෴
	int IsBaseSameWithLead[GRP_NUM]; //�ܽ��Ƿ��ڵ�����Ѱ����

	int searchDir[GRP_NUM];    //��Ѱ����,0����,1����2���£�3����  
	int extendSearchLine[GRP_NUM];//������Ѱ��
	int leadGrayDiff[GRP_NUM];  //���ܽŵ���С�ҶȲ���
	int baseGrayDiff[GRP_NUM];   //�����ߵĻҶȲ�
	int leadSearchLength[GRP_NUM]; //���ܽ�ʱ��������򳤶�
	int searchLineGap[GRP_NUM];   //ɨ���߼��
	int noiseMax[GRP_NUM];        //������̵������Ŀ
	int deleteEdge[GRP_NUM];      //ȥ��ͷβ������ļ�����
	int IsWhitePin[GRP_NUM];      //�Ƿ��ǰ�ɫ�ܽ�
	double clipfactor[GRP_NUM];   //��Ե����ϵ�ϵ����1.0,1.5,2.0,2.5,3.0
	double normalOffMin[GRP_NUM]; //�����ܽŵ���ƫ�뷶Χ
	double normalOffMax[GRP_NUM]; //�����ܽŵ���ƫ�뷶Χ

	double sizeRatio[GRP_NUM];       //�ߴ��������
	int IsInputRatio[GRP_NUM];       //�Ƿ������

	//��Χ
	int errWidthValid[GRP_NUM];           //�Ƿ����ùܽż��
	double errWidthMin[GRP_NUM];  //�����Сֵ
	double errWidthMax[GRP_NUM];  //���ֵ
	int errWarpValid[GRP_NUM];      //�Ƿ����������ж�
	double errWarp[GRP_NUM];  //��������ֵ

	RoiRegion RoiReg[GRP_NUM][2];  //������Ĺܽ������򣬵�һ���Ǽ��߽�Ŀ򣬵ڶ���Ϊ�ܽſ�

private:
	void iniData();
};

