#include "sel2Param.h"
#include <iniParam.h>


sel2Param::sel2Param()
{
	iniData();
}

sel2Param::~sel2Param()
{

}

void sel2Param::readWriteParam(bool r, const QString& dirPath, int level)
{
	SETTING_INIT(dirPath + "/sel2.ini");

	RW_VALUE(r, outIdx);
}

void sel2Param::iniData()
{
	outIdx = 0;
}

