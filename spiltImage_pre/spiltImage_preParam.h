#pragma once
#include <QString>
#include <vector>

using namespace std;

class spiltImage_preParam
{
public:
	explicit spiltImage_preParam();
	~spiltImage_preParam();
	virtual void readWriteParam(bool r, const QString& dirPath, int level);

	typedef struct {
		double row1;
		double col1;
		double row2;
		double col2;
	}segRegion;

	int moduleValid; //�Ƿ�����ģ��
	int RoiValid;    //�Ƿ�����ROI
	int RoiCounts;   //Roi����

	vector<segRegion> segRegionVector;

private:
	void iniData();
};

