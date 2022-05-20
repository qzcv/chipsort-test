#pragma once
#include <QString>

class transform_preParam
{
public:
	explicit transform_preParam();
	~transform_preParam();
	virtual void readWriteParam(bool r, const QString& dirPath, int level);
	int transformType;

private:
	void iniData();
};

