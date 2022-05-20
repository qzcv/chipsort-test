#pragma once
#include <QString>
#include <vector>

#define GRP_NUM 4

using std::vector;

class pinDistanceParam
{
public:
	explicit pinDistanceParam();
	~pinDistanceParam();
	virtual void readWriteParam(bool r, const QString& dirPath, int level);

	typedef struct {
		float row1;
		float col1;
		float row2;
		float col2;
	}RoiRegion;

	int currentGrp;              //ui��ĵ�ǰ���
	int IsRefer;        //�ο�����

	int edgeThreshold[GRP_NUM]; //�߽���Աȶ�
	int pointsNum[GRP_NUM];  //�߽���������Ŀ 
	int pointsInter[GRP_NUM];  //�߽��������� 
	int edgeOffset[GRP_NUM]; //�������ƫ��ֵ

	int grpEnable[GRP_NUM];      //�ܽ���ʹ��
	int grpPinNum[GRP_NUM];      //����Ĺܽ���Ŀ
	double sizeRatio[GRP_NUM];       //�ߴ��������

									 //��Χ
	int errDistanceValid;           //�Ƿ����ùܽż��
	double errDistanceMin[GRP_NUM];  //�ܽż����Сֵ��
	double errDistanceMax[GRP_NUM];  //���ֵ

	vector<RoiRegion> RoiVec[GRP_NUM];  //������Ĺܽ������򣬵�һ���Ǽ��߽�Ŀ򣬺���Ϊ�ܽſ�

private:
	void iniData();
};

