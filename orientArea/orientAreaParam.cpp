#include "orientAreaParam.h"
#include <iniParam.h>
#include "qvglobal.h"


orientAreaParam::orientAreaParam()
{
	iniData();
}

orientAreaParam::~orientAreaParam()
{

}

void orientAreaParam::readWriteParam(bool r, const QString& dirPath, int level)
{
	SETTING_INIT(dirPath + "/orientArea.ini");

	if (!r || (r&&level == ProductLevel))
	{
		RW_VALUE(r, areaMin);
		RW_VALUE(r, areaMax);
		RW_VALUE(r, widthMin);
		RW_VALUE(r, widthMax);
		RW_VALUE(r, heightMin);
		RW_VALUE(r, heightMax);
		RW_VALUE(r, grayMin);
		RW_VALUE(r, grayMax);
		RW_VALUE(r, Opening);
		RW_VALUE(r, Closing);
		RW_VALUE(r, errRegionMin);
		RW_VALUE(r, errRegionMax);
		RW_VALUE(r, errAreaMin);
		RW_VALUE(r, errAreaMax);
		RW_VALUE(r, errAreaCompareIndex);

		RW_VALUE(r, noneNgValid);
		RW_VALUE(r, IsSingleTarget);
		RW_VALUE(r, whiteGuy);
		RW_VALUE(r, IsFillUp);
		RW_VALUE(r, IsShowRegion);
		RW_VALUE(r, referSection);
		RW_VALUE(r, usePinDir);
		RW_VALUE(r, IsReverse);
		RW_VALUE(r, openValid);
		RW_VALUE(r, closeValid);
		RW_VALUE(r, errRegionValid);
		RW_VALUE(r, errAreaValid);
		RW_VALUE(r, errAreaCompareValid);

		for (auto i = 0;i < 2;++i)
		{
			RW_VALUE1(r, RoiRow1[i], i);
			RW_VALUE1(r, RoiRow2[i], i);
			RW_VALUE1(r, RoiCol1[i], i);
			RW_VALUE1(r, RoiCol2[i], i);
		}
	}
	if (!r || (r&&level == MachineLevel))
	{
		RW_VALUE(r, pin1Idx);
		RW_VALUE(r, pin2Idx);
		RW_VALUE(r, noneNgIdx);
	}
}

void orientAreaParam::iniData()
{
	for (auto i = 0;i < 2;++i)
	{
		RoiRow1[i] = RoiCol1[i] = 120;
		RoiRow2[i] = RoiCol2[i] = 240;
	}
	areaMin = 0;
	areaMax = 200000;
	widthMin = 0;
	widthMax = 10000;
	heightMin = 0;
	heightMax = 10000;
	grayMin = 0;
	grayMax = 255;
	Opening = 0;
	Closing = 0;
	errRegionMin = 0;
	errRegionMax = 0;
	errAreaMin = 0;
	errAreaMax = 10000;
 	pin1Idx = 0;
 	pin2Idx = 0;
	errAreaCompareIndex = 0;
	noneNgIdx = 0;

	noneNgValid = false;
	IsSingleTarget = false;
	whiteGuy = false;
	IsFillUp = false;
	IsShowRegion = false;
	referSection = false;
	usePinDir = false;
	IsReverse = false;
	openValid = false;
	closeValid = false;
	errRegionValid = false;
	errAreaValid = false;
	errAreaCompareValid = false;
}

