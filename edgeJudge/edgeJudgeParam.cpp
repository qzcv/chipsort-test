#include "edgeJudgeParam.h"
#include <iniParam.h>
#include "qvglobal.h"

edgeJudgeParam::edgeJudgeParam()
{
	iniData();
}

edgeJudgeParam::~edgeJudgeParam()
{

}

void edgeJudgeParam::readWriteParam(bool r, const QString& dirPath, int level)
{
	SETTING_INIT(dirPath + "/edgeJudge.ini");

	if (r&&level != ProductLevel)
		return;

	RW_VALUE(r, IsShowRegionAfterDet);
	RW_VALUE(r, maxAngle);
	RW_VALUE(r, IsRefer);
	RW_VALUE(r, IsInputPixel);
	RW_VALUE(r, pixSize);
	for (int i = 0;i < 4;i++)
	{
		RW_VALUE1(r, whiteBlackDir[i], i);
		RW_VALUE1(r, searchDir[i], i);
		RW_VALUE1(r, extendSearchLine[i], i);
		RW_VALUE1(r, minGrayDiff[i], i);
		RW_VALUE1(r, roiNum[i], i);

		RW_VALUE1(r, searchLineGap[i], i);
		RW_VALUE1(r, lineOffset[i], i);
		RW_VALUE1(r, clipfactor[i], i);
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
	RoiRegion seg;
	seg.row1 = seg.col1 = 40;
	seg.row2 = seg.col2 = 90;
	int allnum = 1;
	for (int i = 0;i < 4;i++) {
		allnum += roiNum[i];
	}
	if (r) {
		RoiRegionVector.clear();
		RoiRegionVector.push_back(seg);
	}
	for (int i = 0;i < allnum;i++)
	{
		if (r) {
			RoiRegionVector.push_back(seg);
			RW_VALUE1(r, RoiRegionVector[i].row1, i);
			RW_VALUE1(r, RoiRegionVector[i].col1, i);
			RW_VALUE1(r, RoiRegionVector[i].row2, i);
			RW_VALUE1(r, RoiRegionVector[i].col2, i);
		}
		else {
			RW_VALUE1(r, RoiRegionVector[i].row1, i);
			RW_VALUE1(r, RoiRegionVector[i].col1, i);
			RW_VALUE1(r, RoiRegionVector[i].row2, i);
			RW_VALUE1(r, RoiRegionVector[i].col2, i);
		}
	}

	//RW_VALUE(r,IsUseCompare);
	RW_VALUE(r, backGray);

	RW_VALUE(r, inputRegionLarge);
	RW_VALUE(r, offThickMin);

	RW_VALUE(r, IsOffArea);
	RW_VALUE(r, errOffAreaMin);
	RW_VALUE(r, IsShowSegDiff);
}

void edgeJudgeParam::iniData()
{
	IsShowRegionAfterDet = 1;
	maxAngle = 10;
	IsRefer = 0;
	IsInputPixel = 0;
	pixSize = 1.0;
	for (int i = 0;i < 4;i++)
	{
		whiteBlackDir[i] = 0;
		searchDir[i] = 0;
		extendSearchLine[i] = 3;
		minGrayDiff[i] = 30;
		searchLineGap[i] = 3;
		lineOffset[i] = 0;
		clipfactor[i] = 1.0;
		IsFixWidth[i] = 0;
		fixWidth[i] = 0;
		roiNum[i] = 1;
	}
	IsErrHeight = IsErrWidth = IsErrAngle = IsErrCenterOffset = 0;
	errWidthMin = errHeightMin = 0;
	errWidthMax = errHeightmax = 5000;
	errAngle = 0;
	errCenterOffset = 0;

	TmpCenterRow = TmpCenterCol = 200;
	RoiRegionVector.clear();
	RoiRegion seg;
	seg.row1 = seg.col1 = 40;
	seg.row2 = seg.col2 = 90;
	for (int i = 0;i < 5;i++) {
		seg.row1 += 10;
		seg.row2 += 10;
		seg.col1 += 10;
		seg.col2 += 10;
		RoiRegionVector.push_back(seg);
	}
	IsUseCompare = 1;

	backGray = 20;
	inputRegionLarge = 0;
	offThickMin = 0;
	IsOffArea = 0;
	errOffAreaMin = 1;
	IsShowSegDiff = 0;
}

