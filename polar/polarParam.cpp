#include "polarParam.h"
#include <iniParam.h>
#include "qvglobal.h"

polarParam::polarParam()
{
	iniData();
}

polarParam::~polarParam()
{

}

void polarParam::readWriteParam(bool r, const QString& dirPath, int level)
{
	SETTING_INIT(dirPath + "/polar.ini");

	if (r&&level != ProductLevel)
		return;

	for (size_t i = 0;i < 2;++i)
	{
		RW_VALUE1(r, assistRectR1[i], i);
		RW_VALUE1(r, assistRectC1[i], i);
		RW_VALUE1(r, assistRectR2[i], i);
		RW_VALUE1(r, assistRectC2[i], i);
	}

	for (size_t i = 0;i < 4;++i)
	{
		RW_VALUE1(r, rectRow[i], i);
		RW_VALUE1(r, rectCol[i], i);
		RW_VALUE1(r, rectLe1[i], i);
		RW_VALUE1(r, rectLe2[i], i);
		RW_VALUE1(r, rectAng[i], i);

		RW_VALUE1(r, step[i], i);
		RW_VALUE1(r, rmDist[i], i);
		RW_VALUE1(r, gradient[i], i);
		RW_VALUE1(r, degRange[i], i);
		RW_VALUE1(r, iteratNum[i], i);

		RW_VALUE1(r, isFirst[i], i);
		RW_VALUE1(r, pol[i], i);
	}

	RW_VALUE(r, lineIndex);

	RW_VALUE(r, isLeft0);
	RW_VALUE(r, isDefaultSig);
	RW_VALUE(r, isMatrixRefer);
	RW_VALUE(r, isDispAfterDet);

	for (size_t i = 0;i < 2;++i)
	{
		RW_VALUE1(r, isRangeEnable[i], i);
		RW_VALUE1(r, minAng[i], i);
		RW_VALUE1(r, maxAng[i], i);
	}
}

void polarParam::iniData()
{
	for (size_t i = 0;i < 2;++i)
	{
		assistRectR1[i] = 100 + 10 * i;
		assistRectC1[i] = 100 + 10 * i;
		assistRectR2[i] = 200 + 10 * i;
		assistRectC2[i] = 200 + 10 * i;
	}

	for (size_t i = 0;i < 4;++i)
	{
		rectRow[i] = 100 + 10 * i;
		rectCol[i] = 100 + 10 * i;
		rectLe1[i] = 50;
		rectLe2[i] = 50;
		rectAng[i] = 0;

		step[i] = 1;
		rmDist[i] = 5;
		gradient[i] = 30;
		degRange[i] = 10;
		iteratNum[i] = 5;
		isFirst[i] = 1;
		pol[i] = 0;
	}

	lineIndex = 0;

	isLeft0 = 0;
	isMatrixRefer = 0;
	isDispAfterDet = 0;
	isDefaultSig = 0;

	for (size_t i = 0;i < 2;++i)
	{
		isRangeEnable[i] = 0;
		minAng[i] = 50;
		maxAng[i] = 75;
	}
}

