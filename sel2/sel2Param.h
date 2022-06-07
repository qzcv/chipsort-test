#pragma once
#include <QString>

class sel2Param
{
public:
	explicit sel2Param();
	~sel2Param();
	virtual void readWriteParam(bool r, const QString& dirPath, int level);

	int outIdx;

private:
	void iniData();
};

