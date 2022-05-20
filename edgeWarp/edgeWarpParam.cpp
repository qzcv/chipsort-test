#include "edgeWarpParam.h"
#include <iniParam.h>
#include "qvglobal.h"

edgeWarpParam::edgeWarpParam()
{
	iniData();
}

edgeWarpParam::~edgeWarpParam()
{

}

void edgeWarpParam::readWriteParam(bool r, const QString& dirPath, int level)
{
	SETTING_INIT(dirPath + "/edgeWarp.ini");

	if (r&&level != ProductLevel)
		return;

	RW_VALUE(r, leadDir);
	RW_VALUE(r, IsWhite);
	RW_VALUE(r, isInputPix);
	RW_VALUE(r, warpOffset);
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
		RW_VALUE1(r, leadThre[i], i);
	}

	RW_VALUE(r, IsErrWarp);
	RW_VALUE(r, errWarpMin);
	RW_VALUE(r, errWarpMax);
	RW_VALUE(r, IsErrWarpDiff);
	RW_VALUE(r, errWarpDiffMin);
	RW_VALUE(r, errWarpDiffMax);
	for (int i = 0;i < MAX_EDGE_LINE;i++) {
		RW_VALUE1(r, roiReg[i].row, i);
		RW_VALUE1(r, roiReg[i].col, i);
		RW_VALUE1(r, roiReg[i].angle, i);
		RW_VALUE1(r, roiReg[i].length1, i);
		RW_VALUE1(r, roiReg[i].length2, i);
		if (r) {
			if (roiReg[i].row < 0 || roiReg[i].col < 0 || roiReg[i].length1 < 0 || roiReg[i].length2 < 0) {
				roiReg[i].row = 50 + i * 20;
				roiReg[i].col = 50 + i * 20;
				roiReg[i].angle = 0;
				roiReg[i].length1 = 20;
				roiReg[i].length2 = 40;
			}
		}
	}
}

void edgeWarpParam::iniData()
{
	leadDir = 0;
	isInputPix = 0;
	IsWhite = 0;
	warpOffset = 0;
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
		leadThre[i] = 20;
	}
	IsErrWarp = 0;
	errWarpMin = errWarpMax = 0;
	IsErrWarpDiff = 0;
	errWarpDiffMin = 0;
	errWarpDiffMax = 10.0;
	for (int i = 0;i < MAX_EDGE_LINE;i++) {
		roiReg[i].row = 50 + i * 20;
		roiReg[i].col = 50 + i * 20;
		roiReg[i].angle = 0;
		roiReg[i].length1 = 20;
		roiReg[i].length2 = 40;
	}
}

