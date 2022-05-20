#pragma once
#include <QString>
#include <vector>
#include <HalconCpp.h>

using namespace Halcon;
using std::vector;
#define MAX_MARK_NUM 4
#define MAX_CHAR_NUM 50

class mark1Param
{
public:
	explicit mark1Param();
	~mark1Param();
	virtual void readWriteParam(bool r, const QString& dirPath, int level);

	typedef struct {
		double row1;
		double col1;
		double row2;
		double col2;
	}segRegion;

	//��λ����
	int IsReferSection;//�ο�ģ�� 0��ʹ�òο���1ʹ�ô���ο���2ʹ�������ַ�����ο�

	//ѧϰģ��
	int whiteChar;           //���ð�ɫ�����ж�,0Ϊ�ر�,1����
	int autoSegment;         //�����Զ��ָ�,0Ϊ�ر�,1����
	int threshold;           //��ֵ�ָ��ֵ
	int minArea;             //�Զ��ָ��е���С���
	int minHoles;            //�γɿ׶�����С���
	int charCounts[MAX_MARK_NUM];         //�ַ���Ŀ
	int charOffsetValid;    //����ʱ�������charRegion��ƫ�Ʒ�Χ��������Ӧ��ģ��
	int charOffset;         //�ַ�ƫ��
	int searchAngleValid;   //�Ƿ�������Ѱ�Ƕ�
	int searchAngle;        //��Ѱ�Ƕ�
	int autoRegionLarge;    //�Զ�ģ��ģʽ����߽�

	int charThreMin;        //����ȥ���ַ��ָ������Ķ�ֵ
	int charThreMax;
	int IsFillUp;           //�Ƿ����
	int areaType;           //������ƫ��ʱ���ԭ��0-�������ȱ�٣�1-�������ಿ�֣�2-�����ȱ�ٲ���

	int IsReferTempAngle;   //�Ƿ����ж�ƫ�ƽǶȵ�ʱ�������ַ�ģ��ĽǶȼ�ȥ

	//ocr
	int OcrValid; //�Ƿ�ͨ��OCRģʽִ��ģ������
	//�Զ��洢ģ��ͼƬ
	char AutoSaveTmpImgDir[256];


	//��Χ
	int acceptSimilar;
	int errAngleValid;
	double errAngle;
	int errOffSetValid; //λ��
	int errOffSet; //λ�Ʊ仯

	int errOffXValid;  //
	double errOffXMin;
	double errOffXMax;

	int errOffYValid;  //
	double errOffYMin;
	double errOffYMax;

	int errAreaValid;     //���ƫ���ж�
	int errAreaPercent;   //���ƫ��ٷֱ�

	int errExcessValid;  //�ַ������ж�
	double errExcess;    //�������С�ߴ�

	int errCrackValid;   //�ַ���ȱ�ж�
	double errCrack;     //��ȱ����С�ߴ�

	int IsInputRatio;    //�Ƿ������
	double pixelSize;

	//ROI��Ϣ
	int PatternNum;          //��ģ�����Ŀ��4-10
	segRegion RegionRoi[MAX_MARK_NUM];             //��������Ҳ��ȡ����ģ��ʱ�������
	vector<segRegion> segRegionVector[MAX_MARK_NUM];  //��ģ����ַ�����ģ��roi ������ģ������������ͼƬ�е���ʵλ�á�ȡģ���ʱ����Ҫ�����λ��
	Hobject SegImg[MAX_MARK_NUM];   //��ģ��ĸ�ģ�����������ͼƬ

	int CharDefectTOL;                   //�ַ�ȱ�����̶�tolerance�����бղ����Ĵ�С��

	int MultPatternEnable[MAX_MARK_NUM];        //�Ƿ����ø�ģ�� 

	int IsMult;                      //�Ƿ����ö�ģ��

	int IsFontOrLayer;               //�����廹�Ƕ�ͼ�㣬�����弴��ģ��ʱ�ַ���Ŀһ��ÿ�ַ��ֲ飬��ͼ�㼴���ж�������һ��ͼ��
	int NowPatternNum;               //��ǰģ����
	int IsDirPattern;                //�Ƿ���ö༫��ģ�� ��ͬһģ������ж���������ת������ǰ�洫������ָ��ѡ����ת�Ƕ�
	int angleType;                   //�Ƕ����� 0:0/180 �� 1�� 0/90/180/270                

	int IsUsePinDirOut;   //�Ƿ����÷���
	int pin1OutIdx;    //�ܽ�1�����
	int pin2OutIdx;
	int IsDefaultSig;  //�Ƿ�����Ĭ���ź�
	int IsInverse;     //�Ƿ�����
	int Is2Dor4D;      //�������������������ĸ��������� 
	int isShowAngle;   //�Ƿ��ڽ��������ʾ�Ƕ���Ϣ

	int IsInputPatIndex; //�Ƿ�ʹ�ô���ģ�����

	int HorOrVer;            //��������ַ����������ַ�
	int charGap;             //�ַ���࣬�����ڷָ�����ʱ��ʹ��

private:
	void iniData();
};

