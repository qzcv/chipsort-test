#include "transform_preParam.h"
#include <iniParam.h>
#include "qvglobal.h"

transform_preParam::transform_preParam()
{
	iniData();
}

transform_preParam::~transform_preParam()
{

}

void transform_preParam::readWriteParam(bool r, const QString& dirPath, int level)
{
	SETTING_INIT(dirPath + "/transform_pre.ini");

	if (r&&level != ProductLevel)
		return;
	RW_VALUE(r, transformType);
}

void transform_preParam::iniData()
{
	transformType = 0;
}

