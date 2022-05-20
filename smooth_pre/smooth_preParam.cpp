#include "smooth_preParam.h"
#include <iniParam.h>
#include "qvglobal.h"

smooth_preParam::smooth_preParam()
{
	iniData();
}

smooth_preParam::~smooth_preParam()
{

}

void smooth_preParam::readWriteParam(bool r, const QString& dirPath, int level)
{
	SETTING_INIT(dirPath + "/smooth_pre.ini");

	if (r&&level != ProductLevel)
		return;

	RW_VALUE(r, smoothType);
	RW_VALUE(r, smoothParam1);
	RW_VALUE(r, smoothParam2);
	RW_VALUE(r, smoothParam3);
	RW_VALUE(r, smoothParam4);
}

void smooth_preParam::iniData()
{
	smoothType = 0;
	smoothParam1 = 3;
	smoothParam2 = 3;
	smoothParam3 = 0;
	smoothParam4 = 0;
}

