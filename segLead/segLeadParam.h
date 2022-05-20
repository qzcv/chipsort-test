#pragma once
#include <QString>
#include <vector>

#define Max_LeadNum 20
using std::vector;

class segLeadParam
{
public:
	explicit segLeadParam();
	~segLeadParam();
	virtual void readWriteParam(bool r, const QString& dirPath, int level);

	typedef struct {
		double row1;
		double col1;
		double row2;
		double col2;
	}roiRegion;

	//�������
	int referSection; //��ת����ο�ģ��
	int IsReferEdge;  //�Ƿ񴫽��߽�ο�

	int IsInputPixel; //�Ƿ������
	double pixSize;            //�����ߴ�С
	int showAfterDet;  //����Ƿ���ʾ����

					   //Ŀ�궨��
	int leadNum;     //�ܽŵ���Ŀ
	int areaMin;
	int areaMax;
	int openWidth;
	int openHeight;
	int closeWidth;
	int closeHeight;
	int meanWidth;   //����ƽ������ʱ���
	int meanHeight;
	int grayWhite;
	int grayBlack;
	int IsGauss;      //�Ƿ�Դ����ԭͼ���и�˹�˲�
	int GaussSize;    //��˹�˲��˵Ĵ�С
	int WOrB;         //Ŀ���ǰ�ɫ�Ļ��Ǻ�ɫ�ģ�0��ɫ��1��ɫ
	int HorOrVer;     //�ܽų����Ǻ���������

	int IsAutoWhite;  //�Զ��ָ�ʱ��ɫ��0��ɫ��1��ɫ

					  //regiongrowing
	int centerType;  //���ӵ���roi���Ļ������õ����ӵ� 0roi��1�������ӵ�,2��ʹ�����ӵ�
	int centerX, centerY;  //���ӵ�����
	int rowWid;
	int colWid;
	int thresholdTol;
	int minGrowArea;

	//�̶���ֵ
	int threMin;
	int threMax;

	//auto leadThreshold
	int autoLeadSize;  //�Զ��ָ�ܽųߴ�
	int autoLeadThre;  //�Զ��ָ�ܽ���ֵ

	int testType;   //�������,0ƽ��������1����������2�̶���ֵ

	//��Χ
	int errWidthValid;             //���ϣ���1�ÿ�ȣ�0�ر�1����
	double errWidthMin[Max_LeadNum], errWidthMax[Max_LeadNum];
	int errLengthValid;            //���ϣ���1�ó��ȣ�0�ر�1����
	double errLengthMin[Max_LeadNum], errLengthMax[Max_LeadNum];
	int errInterValid;             //���ϣ���1�ü�࣬0�ر�1����
	double errInterMin[Max_LeadNum], errInterMax[Max_LeadNum];
	int errEdgeDisValid;             //���ϣ���1�ü�࣬0�ر�1����
	double errEdgeDisMin[Max_LeadNum], errEdgeDisMax[Max_LeadNum];

	vector<roiRegion> RoiVec;   //0Ϊ��������,����Ϊ�����ܽ�����         

private:
	void iniData();
};

