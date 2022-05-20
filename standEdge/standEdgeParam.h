#pragma once
#include <QString>
#include <vector>

using std::vector;

class standEdgeParam
{
public:
	explicit standEdgeParam();
	~standEdgeParam();
	virtual void readWriteParam(bool r, const QString& dirPath, int level);

	typedef struct {
		float row1;
		float col1;
		float row2;
		float col2;
	}RoiRegion;
	int IsEndPoint;      //�Ƿ���˵�0�ر�1����

	int maxAngle;        //���Ƕ�

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
	int grayVal[4];         //����������ı����г���������ĻҶ���ֵ
	int edgeGray[4]; //ɨ��˵��ʱ���measure_rec�ĻҶ�
					 //int pinSearchWidth[4];  //�ܽ���������ʱ��ָ�ܽ��������õ��������Ŵ�С
	double clipfactor[4];   //��Ե����ϵ�ϵ����1.0,1.5,2.0,2.5,3.0

	int IsEdgeDir[4];       //�Ƿ����ñ߽��ķ�����ɸѡ�����ڸ����ų��ܽżⲿ�ı߽��
	int IsClearOutlier[4];  //�Ƿ�������Ⱥ��(�����Ĳ�������)

	double TmpCenterRow;
	double TmpCenterCol;   //ģ�����ĵ�����

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

	vector<RoiRegion> RoiRegionVector;  //[0]Ϊ������Χ,[1]-[4]Ϊ�������ҵ���1����

	int NowGrp;  //�������ң���ǰ������

private:
	void iniData();
};

