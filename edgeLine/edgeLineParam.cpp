#include "edgeLineParam.h"
#include <iniParam.h>
#include "qvglobal.h"

edgeLineParam::edgeLineParam()
{
	iniData();
}

edgeLineParam::~edgeLineParam()
{

}

void edgeLineParam::readWriteParam(bool r, const QString& dirPath, int level)
{
	SETTING_INIT(dirPath + "/edgeLine.ini");

	if (r&&level != ProductLevel)
		return;

	RW_VALUE(r, IsShowRegionAfterDet);
	RW_VALUE(r, IsRefer);
	for (int i = 0;i < 4;i++)
	{
		RW_VALUE1(r, gradient[i], i);
		RW_VALUE1(r, stepVal[i], i);
		RW_VALUE1(r, pol[i], i);
		RW_VALUE1(r, first[i], i);
		RW_VALUE1(r, degRange[i], i);
		RW_VALUE1(r, rmDist[i], i);
		RW_VALUE1(r, iterateNum[i], i);

		RW_VALUE1(r, lineOffset[i], i);
		RW_VALUE1(r, IsFixWidth[i], i);
		RW_VALUE1(r, fixWidth[i], i);
	}
	RW_VALUE(r, IsErrHeight);
	RW_VALUE(r, IsErrWidth);
	RW_VALUE(r, IsErrAngle);
	RW_VALUE(r, IsErrCenterOffset);
	RW_VALUE(r, errWidthMin);
	RW_VALUE(r, errHeightMin);
	RW_VALUE(r, errWidthMax);
	RW_VALUE(r, errHeightmax);

	RW_VALUE(r, errAngle);
	RW_VALUE(r, errCenterOffset);

	RW_VALUE(r, TmpCenterRow);
	RW_VALUE(r, TmpCenterCol);
	for (int i = 0;i < 4;i++) {
		RW_VALUE1(r, roiReg[i].row, i);
		RW_VALUE1(r, roiReg[i].col, i);
		RW_VALUE1(r, roiReg[i].angle, i);
		RW_VALUE1(r, roiReg[i].length1, i);
		RW_VALUE1(r, roiReg[i].length2, i);
	}
}

void edgeLineParam::iniData()
{
	IsShowRegionAfterDet = 1;
	IsRefer = 0;
	for (int i = 0;i < 4;i++) {
		gradient[i] = 20;
		stepVal[i] = 3;
		pol[i] = 0;
		first[i] = 0;
		degRange[i] = 10;
		rmDist[i] = 5;
		iterateNum[i] = 5;

		lineOffset[i] = 0;
		IsFixWidth[i] = 0;
		fixWidth[i] = 0;
	}
	IsErrHeight = IsErrWidth = IsErrAngle = IsErrCenterOffset = 0;
	errWidthMin = errHeightMin = 0;
	errWidthMax = errHeightmax = 5000;
	errAngle = 0;
	errCenterOffset = 0;
	TmpCenterRow = TmpCenterCol = 200;

	for (int i = 0;i < 4;i++) {
		roiReg[i].row = 50 + i * 20;
		roiReg[i].col = 50 + i * 20;
		roiReg[i].angle = 0;
		roiReg[i].length1 = 20;
		roiReg[i].length2 = 40;
	}
}

