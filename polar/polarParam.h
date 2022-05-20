#pragma once
#include <QString>

class polarParam
{
public:
	explicit polarParam();
	~polarParam();
	virtual void readWriteParam(bool r, const QString& dirPath, int level);

	double assistRectR1[2];
	double assistRectC1[2];
	double assistRectR2[2];
	double assistRectC2[2];

	double rectRow[4];
	double rectCol[4];
	double rectLe1[4];
	double rectLe2[4];
	double rectAng[4];

	int gradient[4];
	int step[4];
	int pol[4];
	int isFirst[4];
	int degRange[4];
	int iteratNum[4];
	int rmDist[4];

	int lineIndex;
	bool isLeft0;
	bool isMatrixRefer;
	bool isDispAfterDet;
	bool isDefaultSig;

	bool isRangeEnable[2];
	int minAng[2];
	int maxAng[2];

private:
	void iniData();
};

