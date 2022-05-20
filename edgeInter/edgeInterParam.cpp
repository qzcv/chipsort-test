#include "edgeInterParam.h"
#include <iniParam.h>
#include "qvglobal.h"

edgeInterParam::edgeInterParam()
{
	iniData();
}

edgeInterParam::~edgeInterParam()
{

}

void edgeInterParam::readWriteParam(bool r, const QString& dirPath, int level)
{
	SETTING_INIT(dirPath + "/edgeInter.ini");

	if (r&&level != ProductLevel)
		return;

	RW_VALUE(r, IsErrX);
	RW_VALUE(r, IsErrY);
	RW_VALUE(r, errXMin);
	RW_VALUE(r, errXMax);
	RW_VALUE(r, errYMin);
	RW_VALUE(r, errYMax);
	RW_VALUE(r, referX);
	RW_VALUE(r, referY);

	RW_VALUE(r, interOffset);
	RW_VALUE(r, pixSize);
	RW_VALUE(r, currentEdge);
	RW_VALUE(r, IsShowRegionAfterDet);
	RW_VALUE(r, IsRefer);
	for (int i = 0;i < MAX_EDGE_LINE;i++)
	{
		RW_VALUE1(r, gradient[i], i);
		RW_VALUE1(r, stepVal[i], i);
		RW_VALUE1(r, pol[i], i);
		RW_VALUE1(r, first[i], i);
		RW_VALUE1(r, degRange[i], i);
		RW_VALUE1(r, rmDist[i], i);
		RW_VALUE1(r, iterateNum[i], i);

		RW_VALUE1(r, lineOffset[i], i);
	}

	RW_VALUE(r, IsErrInter);
	RW_VALUE(r, errInterMin);
	RW_VALUE(r, errInterMax);
	for (int i = 0;i < MAX_EDGE_LINE;i++) {
		RW_VALUE1(r, roiReg[i].row, i);
		RW_VALUE1(r, roiReg[i].col, i);
		RW_VALUE1(r, roiReg[i].angle, i);
		RW_VALUE1(r, roiReg[i].length1, i);
		RW_VALUE1(r, roiReg[i].length2, i);
	}
}

void edgeInterParam::iniData()
{
	IsErrX = IsErrY = 0;
	errXMin = errYMin = 0;
	errYMax = errXMax = 999.0;
	referX = 100.0;
	referY = 100.0;
	interOffset = 0;
	pixSize = 1.0;
	currentEdge = 0;
	IsShowRegionAfterDet = 1;
	IsRefer = 0;
	for (int i = 0;i < MAX_EDGE_LINE;i++) {
		gradient[i] = 20;
		stepVal[i] = 3;
		pol[i] = 0;
		first[i] = 0;
		degRange[i] = 10;
		rmDist[i] = 5;
		iterateNum[i] = 5;

		lineOffset[i] = 0;
	}
	IsErrInter = 0;
	errInterMin = errInterMax = 0;

	for (int i = 0;i < MAX_EDGE_LINE;i++) {
		roiReg[i].row = 50 + i * 20;
		roiReg[i].col = 50 + i * 20;
		roiReg[i].angle = 0;
		roiReg[i].length1 = 20;
		roiReg[i].length2 = 40;
	}
}

