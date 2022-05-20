#include "composeND_preParam.h"
#include <iniParam.h>
#include "qvglobal.h"

composeND_preParam::composeND_preParam()
{
	iniData();
}

composeND_preParam::~composeND_preParam()
{

}

void composeND_preParam::readWriteParam(bool r, const QString& dirPath, int level)
{
	SETTING_INIT(dirPath + "/composeND_pre.ini");

	if (r&&level != ProductLevel)
		return;

	for (auto i = 0;i < MAX_REGION;i++)
	{
		for (auto j = 0;j < ROI_MAX_NUM;j++)
		{
			RW_VALUE2(r, regRow0[i][j], i, j);
			RW_VALUE2(r, regCol0[i][j], i, j);
			RW_VALUE2(r, regRow1[i][j], i, j);
			RW_VALUE2(r, regCol1[i][j], i, j);
		}
		RW_VALUE1(r, imageIndex[i], i);
		RW_VALUE1(r, roiNum[i], i);
	}
	RW_VALUE(r, allRegNum);
	RW_VALUE(r, baseImgIdx);
}

void composeND_preParam::iniData()
{
	for (auto i = 0;i < MAX_REGION;i++)
	{
		for (auto j = 0;j < ROI_MAX_NUM;j++)
		{
			regRow0[i][j] = 50 + i * 10;
			regCol0[i][j] = 50 + i * 10;
			regRow1[i][j] = 100 + i * 10;
			regCol1[i][j] = 120 + i * 10;
		}
		imageIndex[i] = 1;
		roiNum[i] = 2;
	}
	allRegNum = MAX_REGION;
	baseImgIdx = 0;
}

