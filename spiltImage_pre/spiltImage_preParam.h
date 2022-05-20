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

	int moduleValid; //是否启用模块
	int RoiValid;    //是否启用ROI
	int RoiCounts;   //Roi数量

	vector<segRegion> segRegionVector;

private:
	void iniData();
};

