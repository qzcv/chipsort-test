#pragma once
#include <QString>

class orientAreaParam
{
public:
	explicit orientAreaParam();
	~orientAreaParam();
	virtual void readWriteParam(bool r, const QString& dirPath, int level);

	int areaMin;     //��С���
	int areaMax;     //������
	int widthMin;    //��С���
	int widthMax;    //�����
	int heightMin;   //��С�߶�
	int heightMax;
	int grayMin;
	int grayMax;
	int Opening;        //��������ֵ
	int Closing;       //�ղ�����ֵ
	int errRegionMin;
	int errRegionMax;
	int errAreaMin;
	int errAreaMax;
 	int pin1Idx;
 	int pin2Idx;
	int errAreaCompareIndex;
	int noneNgIdx;//�������NG��OUTPUT���

	bool noneNgValid;
	bool IsSingleTarget;
	bool whiteGuy;
	bool IsFillUp;
	bool IsShowRegion;
	bool referSection;
	bool usePinDir;
	bool IsReverse;
	bool errAreaCompareValid;
	bool closeValid;  //�Ƿ�����close,0�ر�,1����
	bool openValid;   //�Ƿ�����open,0�ر�,1����
	bool errRegionValid;
	bool errAreaValid;      //�Ƿ�������

	//ROI��Ϣ
	double RoiRow1[2];     //��������
	double RoiCol1[2];
	double RoiRow2[2];
	double RoiCol2[2];

private:
	void iniData();
};

