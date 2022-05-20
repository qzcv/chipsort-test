#pragma once
#include <QString>
#include <vector>

#define TYPE_NUM 13
#define ROI_MAX_NUM 10

using namespace std;

class arithmetic_preParam
{
public:
	explicit arithmetic_preParam();
	~arithmetic_preParam();
	virtual void readWriteParam(bool r, const QString& dirPath, int level);
	typedef struct {
		double row1;
		double col1;
		double row2;
		double col2;
	}segRegion;

	bool enable;
	bool allImgValid;

	int roiNum;
	int idx1;
	int idx2;
	int operatorIdx;
	int outputIdx;

	double pA[TYPE_NUM];
	double pB[TYPE_NUM];
	double pC[TYPE_NUM];

	segRegion segRegArray[ROI_MAX_NUM];

private:
	void iniData();
};

