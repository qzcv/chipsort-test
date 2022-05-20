#pragma once
#include <QString>

class smooth_preParam
{
public:
	explicit smooth_preParam();
	~smooth_preParam();
	virtual void readWriteParam(bool r, const QString& dirPath, int level);

	int smoothType;
	int smoothParam1;
	int smoothParam2;
	int smoothParam3;
	double smoothParam4;

private:
	void iniData();
};

