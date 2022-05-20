#include "arithmetic_preParam.h"
#include <iniParam.h>
#include "qvglobal.h"

arithmetic_preParam::arithmetic_preParam()
{
	iniData();
}

arithmetic_preParam::~arithmetic_preParam()
{

}

void arithmetic_preParam::readWriteParam(bool r, const QString& dirPath, int level)
{
	SETTING_INIT(dirPath + "/arithmetic_pre.ini");

	if (r&&level != ProductLevel)
		return;

	RW_VALUE(r, enable);
	RW_VALUE(r, allImgValid);

	RW_VALUE(r, roiNum);
	RW_VALUE(r, idx1);
	RW_VALUE(r, idx2);
	RW_VALUE(r, operatorIdx);
	RW_VALUE(r, outputIdx);

	for (auto i = 0;i < TYPE_NUM;++i)
	{
		RW_VALUE1(r, pA[i], i);
		RW_VALUE1(r, pB[i], i);
		RW_VALUE1(r, pC[i], i);
	}

	for (auto i = 0;i < roiNum;++i)
	{
		RW_VALUE1(r, segRegArray[i].row1, i);
		RW_VALUE1(r, segRegArray[i].col1, i);
		RW_VALUE1(r, segRegArray[i].row2, i);
		RW_VALUE1(r, segRegArray[i].col2, i);
	}
}

void arithmetic_preParam::iniData()
{
	enable = 0;
	allImgValid = 0;

	roiNum = 1;
	idx1 = 0;
	idx2 = 0;
	operatorIdx = 0;
	outputIdx = 0;
	
	for (auto i = 0;i < TYPE_NUM;++i)
	{
		pA[i] = 1;
		pB[i] = 1;
		pC[i] = 0;
	}

	for (auto i = 0;i < ROI_MAX_NUM;++i)
	{
		segRegArray[i].col1 = 50 + 20 * i;
		segRegArray[i].row1 = 50 + 20 * i;
		segRegArray[i].col2 = 100 + 20 * i;
		segRegArray[i].row2 = 100 + 20 * i;
	}
}

