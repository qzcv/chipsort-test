#include "pinWarpParam.h"
#include <iniParam.h>
#include "qvglobal.h"

pinWarpParam::pinWarpParam()
{
	iniData();
}

pinWarpParam::~pinWarpParam()
{

}

void pinWarpParam::readWriteParam(bool r, const QString& dirPath, int level)
{
	SETTING_INIT(dirPath + "/pinWarp.ini");
	if (r&&level != ProductLevel)
		return;

	RW_VALUE(r, currentGrp);
	for (int i = 0;i < GRP_NUM;i++)
	{
		RW_VALUE1(r, IsDetBase[i], i);

		RW_VALUE1(r, IsWhitePin[i], i);
		RW_VALUE1(r, grpEnable[i], i);
		RW_VALUE1(r, whiteBlackDir[i], i);
		RW_VALUE1(r, searchDir[i], i);
		RW_VALUE1(r, IsBaseSameWithLead[i], i);

		RW_VALUE1(r, extendSearchLine[i], i);

		RW_VALUE1(r, searchLineGap[i], i);

		RW_VALUE1(r, leadSearchLength[i], i);
		RW_VALUE1(r, leadGrayDiff[i], i);
		RW_VALUE1(r, baseGrayDiff[i], i);
		RW_VALUE1(r, clipfactor[i], i);
		RW_VALUE1(r, sizeRatio[i], i);
		RW_VALUE1(r, IsInputRatio[i], i);
		RW_VALUE1(r, deleteEdge[i], i);

		RW_VALUE1(r, noiseMax[i], i);
		RW_VALUE1(r, normalOffMin[i], i);
		RW_VALUE1(r, normalOffMax[i], i);
		//zhangao
		RW_VALUE1(r, errWidthValid[i], i);
		RW_VALUE1(r, errWidthMin[i], i);
		RW_VALUE1(r, errWidthMax[i], i);
		RW_VALUE1(r, errWarpValid[i], i);
		RW_VALUE1(r, errWarp[i], i);
	}

	for (int i = 0;i < GRP_NUM;i++)
	{
		for (int j = 0;j < 2;j++) {
			RW_VALUE2(r, RoiReg[i][j].row1, i, j);
			RW_VALUE2(r, RoiReg[i][j].col1, i, j);
			RW_VALUE2(r, RoiReg[i][j].row2, i, j);
			RW_VALUE2(r, RoiReg[i][j].col2, i, j);
		}
	}
}

void pinWarpParam::iniData()
{
	currentGrp = 0;
	for (int i = 0;i < GRP_NUM;i++) {
		IsDetBase[i] = 1;
		IsBaseSameWithLead[i] = 0;

		IsWhitePin[i] = 0;
		grpEnable[i] = 0;
		whiteBlackDir[i] = 0;
		searchDir[i] = i;
		extendSearchLine[i] = 3;
		leadGrayDiff[i] = 20;
		baseGrayDiff[i] = 20;
		searchLineGap[i] = 3;
		clipfactor[i] = 1.0;
		deleteEdge[i] = 0;
		sizeRatio[i] = 1.0;
		IsInputRatio[i] = 0;
		leadSearchLength[i] = 10;
		normalOffMin[i] = 10;
		normalOffMax[i] = 50;
		noiseMax[i] = 0;

		//zhangao
		errWidthValid[i] = 1;
		errWidthMin[i] = 0;
		errWidthMax[i] = 0;
		errWarpValid[i] = 0;
		errWarp[i] = 0;
	}
	RoiRegion seg;
	seg.row1 = seg.col1 = 40;
	seg.row2 = seg.col2 = 70;
	for (int i = 0;i < GRP_NUM;i++) {
		for (int j = 0;j < 2;j++) {
			RoiReg[i][j].row1 = seg.row1 + j * 10;
			RoiReg[i][j].col1 = seg.col1 + j * 10;
			RoiReg[i][j].row2 = seg.row2 + j * 10;
			RoiReg[i][j].col2 = seg.col2 + j * 10;
		}
	}
}

