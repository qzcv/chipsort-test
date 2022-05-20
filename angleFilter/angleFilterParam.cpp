#include "angleFilterParam.h"
#include <iniParam.h>
#include "qvglobal.h"

angleFilterParam::angleFilterParam()
{
	iniData();
}

angleFilterParam::~angleFilterParam()
{

}

void angleFilterParam::readWriteParam(bool r, const QString& dirPath, int level)
{
	SETTING_INIT(dirPath + "/angleFilter.ini");

	if (r&&level != ProductLevel)
		return;
	for (size_t i = 0;i < 3;++i)
	{
		RW_VALUE1(r, crow[i], i);
		RW_VALUE1(r, ccol[i], i);
		RW_VALUE1(r, len1[i], i);
		RW_VALUE1(r, len2[i], i);
		RW_VALUE1(r, ang[i], i);
	}
	for (size_t i = 0;i < 5;++i)
	{
		RW_VALUE1(r, step[i], i);
		RW_VALUE1(r, rmDist[i], i);
		RW_VALUE1(r, gradient[i], i);
		RW_VALUE1(r, degRange[i], i);
		RW_VALUE1(r, iteratNum[i], i);
		RW_VALUE1(r, isFirst[i], i);
		RW_VALUE1(r, pol[i], i);
	}

	for (int i = 0;i < 2;++i)
	{
		RW_VALUE1(r, maxAng[i], i);
		RW_VALUE1(r, minAng[i], i);
	}

	RW_VALUE(r, isMatrixRefer);
	RW_VALUE(r, isDispAfterDet);
}

void angleFilterParam::iniData()
{
	for (size_t i = 0;i < 3;++i)
	{
		crow[i] = 100 + 10 * i;
		ccol[i] = 100 + 10 * i;
		len1[i] = 50;
		len2[i] = 50;
		ang[i] = 0;
	}
	for (size_t i = 0;i < 5;++i)
	{
		step[i] = 1;
		rmDist[i] = 5;
		gradient[i] = 30;
		degRange[i] = 10;
		iteratNum[i] = 5;
		isFirst[i] = 1;
		pol[i] = 0;
	}

	for (int i = 0;i < 2;++i)
	{
		maxAng[i] = 360;
		minAng[i] = 0;
	}

	isMatrixRefer = 0;
	isDispAfterDet = 0;
}

