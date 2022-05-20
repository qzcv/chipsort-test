#pragma once
#include <QString>

class angleFilterParam
{
public:
	explicit angleFilterParam();
	~angleFilterParam();
	virtual void readWriteParam(bool r, const QString& dirPath, int level);

	double crow[3], ccol[3], ang[3], len1[3], len2[3];

	int gradient[5];
	int step[5];
	int isFirst[5];
	int degRange[5];
	int iteratNum[5];
	int rmDist[5];
	int pol[5];

	int minAng[2];
	int maxAng[2];

	bool isMatrixRefer;
	bool isDispAfterDet;

private:
	void iniData();
};

