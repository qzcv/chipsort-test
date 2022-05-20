#pragma once
#include <QString>
#include <vector>

using std::vector;

class ColorPickerParam
{
public:
	explicit ColorPickerParam();
	~ColorPickerParam();
	virtual void readWriteParam(bool r, const QString& dirPath, int level);

	typedef struct {
		int regionType;  //0��Ϊ��ת���Σ�
		double RecRow;
		double RecCol;
		double RecAngle;
		double RecLength1;
		double RecLength2;
		double CirR;
		double CirRow;
		double CirCol;
	}segRegion;

	int IsShowAllRegion;  //�Ƿ���ѡ������ʱ������������

	int IsReferSection;//�ο�ģ��
	int SaturationMin; //���Ͷ�
	int SaturationMax; //���Ͷ�
	int HueMin;        //ɫ��
	int HueMax;        //ɫ��

	int IsOpening;     //�Ƿ񿪲���
	int openVal;       //������ֵ
	int IsClosing;     //�Ƿ�ղ���
	int closeVal;      //�ղ���ֵ


	int IsNotLess;     //0Ϊ������ɫ�����������࣬1Ϊ�������ڴ�����ɫ���������������
	int areaMin, areaMax;  //�����ֵ
	int IsShowRegionAfterDetect;  //������Ƿ���ʾ����
	int IsFillUpHole;     //�Ƿ����׶�

	int IsDeleteLightArea;  //�Ƿ�ȥ����������
	int lightAreaThreshold; //����������ֵ
	int lightArea; //��������С���

	int IsHeight;
	int heightMin, heightMax;
	int IsWidth;
	int widthMin, widthMax;

	int nowIsRecOrCir;       //�����Ǿ���0����Բ��1
	int roiCount;              //roi��Ŀ;
	int MinusCount;            //�ų�������Ŀ
	int currentRoiIndex;       //��ǰ����ָ��
	int currentMinusIndex;     //��ǰ�ų�����ָ��
	vector<segRegion> RoiVec;  //roi��������
	vector<segRegion> MinusVec;  //�ų�����

	int IsErrAllArea;
	int errAllMin, errAllMax;
	int IsErrSingleArea;
	int errSingleMin, errSingleMax;

	int referType;        //����ο����ͣ� 0����ѡ����ο���1��������ο���2��������ο�
	int AreaZoom;         //��������ֵ

private:
	void iniData();
};

