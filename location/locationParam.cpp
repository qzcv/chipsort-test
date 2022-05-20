#include "locationParam.h"
#include <iniParam.h>
#include "qvglobal.h"

locationParam::locationParam()
{
	iniData();
}

locationParam::~locationParam()
{

}

void locationParam::readWriteParam(bool r, const QString& dirPath, int level)
{
	SETTING_INIT(dirPath + "/location.ini");

	if (r&&level != ProductLevel)
		return;

	RW_VALUE(r, IsPadStand);
	RW_VALUE(r, NowGrp);
	if (r&&NowGrp >= 4) {
		NowGrp = 3;
	}
	RW_VALUE(r, upDwOrLfRt);

	RW_VALUE(r, maxAngle);
	RW_VALUE(r, UpDetect);
	RW_VALUE(r, DwDetect);
	RW_VALUE(r, LfDetect);
	RW_VALUE(r, RtDetect);
	for (int i = 0;i < 2;i++) {
		RW_VALUE1(r, grpPinNum[i], i);
	}

	RW_VALUE(r, pinlength);
	for (int i = 0;i < 4;i++)
	{
		RW_VALUE1(r, extendSearchLine[i], i);
		RW_VALUE1(r, minGrayDiff[i], i);
		RW_VALUE1(r, searchLineGap[i], i);
		RW_VALUE1(r, lineOffset[i], i);
		RW_VALUE1(r, IsPaichuLine[i], i);
		RW_VALUE1(r, paichuLine[i], i);
		RW_VALUE1(r, scalePaichu[i], i);
		RW_VALUE1(r, IsCutLine[i], i);
		RW_VALUE1(r, offsetPlus[i], i);
		RW_VALUE1(r, minArea[i], i);
		RW_VALUE1(r, standoffOffset[i], i);
		RW_VALUE1(r, grayVal[i], i);
		RW_VALUE1(r, edgeGray[i], i);
		RW_VALUE1(r, clipfactor[i], i);

		RW_VALUE1(r, IsEdgeDir[i], i);
		RW_VALUE1(r, isLeadSize[i], i);
		RW_VALUE1(r, leadSize[i], i);
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
	RW_VALUE(r, errCoplanarMin);
	RW_VALUE(r, errCoplanar);
	RW_VALUE(r, errCoplanarValid);

	//zhangao
	RW_VALUE(r, errPinGapValid);
	RW_VALUE(r, errPinStandOffValid);
	RW_VALUE(r, errPinStandOffDiffValid);
	RW_VALUE(r, errPinWidthValid);
	for (int i = 0;i < 2;i++) {
		for (int j = 0;j < grpPinNum[i];j++) {
			RW_VALUE2(r, errPinGapMin[i][j], i, j);
			RW_VALUE2(r, errPinGapMax[i][j], i, j);
			RW_VALUE2(r, errPinStandOffMin[i][j], i, j);
			RW_VALUE2(r, errPinStandOffMax[i][j], i, j);
			RW_VALUE2(r, errPinWidthMin[i][j], i, j);
			RW_VALUE2(r, errPinWidthMax[i][j], i, j);

			RW_VALUE2(r, standOffCorr[i][j], i, j);
			RW_VALUE2(r, widthCorr[i][j], i, j);
			RW_VALUE2(r, InterCorr[i][j], i, j);
			RW_VALUE2(r, offStandOffCorr[i][j], i, j);
			RW_VALUE2(r, offWidthCorr[i][j], i, j);
			RW_VALUE2(r, offInterCorr[i][j], i, j);
		}
		RW_VALUE1(r, errPinStandOffDiffMin[i], i);
		RW_VALUE1(r, errPinStandOffDiffMax[i], i);
		RW_VALUE1(r, selfStandOffCorr[i], i);
	}
	RW_VALUE(r, TmpCenterRow);
	RW_VALUE(r, TmpCenterCol);

	for (int i = 0;i < 5;i++)
	{
		if (r) {
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
}

void locationParam::iniData()
{
	NowGrp = 0;
	IsPadStand = 0;
	IsEndPoint = 1;      //是否检测端点0关闭1启用
	upDwOrLfRt = 1;
	maxAngle = 10;
	UpDetect = DwDetect = LfDetect = RtDetect = 1;


	pinlength = 0.65;
	for (int i = 0;i < 4;i++)
	{
		extendSearchLine[i] = 3;
		minGrayDiff[i] = 30;
		searchLineGap[i] = 3;
		lineOffset[i] = 5;
		IsPaichuLine[i] = 0;
		paichuLine[i] = 10;
		scalePaichu[i] = 0;
		IsCutLine[i] = 0;
		offsetPlus[i] = 0;
		minArea[i] = 50;
		standoffOffset[i] = 0;
		grayVal[i] = 100;
		edgeGray[i] = 50;
		clipfactor[i] = 1.0;
		IsEdgeDir[i] = 1;
		isLeadSize[i] = 0;
		leadSize[i] = 0;
	}
	whiteBlackDir[0] = 1;
	searchDir[0] = 1;
	whiteBlackDir[1] = 0;
	searchDir[1] = 1;
	whiteBlackDir[2] = 1;
	searchDir[2] = 0;
	whiteBlackDir[3] = 0;
	searchDir[3] = 0;

	IsErrHeight = IsErrWidth = IsErrAngle = IsErrCenterOffset = 0;
	errWidthMin = errHeightMin = 0;
	grpPinNum[0] = grpPinNum[1] = 3;
	errWidthMax = errHeightmax = 5000;
	errAngle = 0;
	errCenterOffset = 0;

	//zhangao
	errPinGapValid = 0;
	errPinStandOffValid = 0;
	errPinStandOffDiffValid = 0;
	errPinWidthValid = 0;

	errCoplanarValid = 0;
	errCoplanarMin = 0;
	errCoplanar = 0;
	for (int i = 0;i < 2;i++) {
		selfStandOffCorr[i] = 0;
		for (int j = 0;j < GRP_MAX_LEADNUM;j++) {
			errPinStandOffMin[i][j] = 0;
			errPinStandOffMax[i][j] = 10;
			errPinWidthMin[i][j] = 0;
			errPinWidthMax[i][j] = 10;
			errPinGapMin[i][j] = 0;
			errPinGapMax[i][j] = 10;

			standOffCorr[i][j] = 0;    //站立度修正

			widthCorr[i][j] = 0;
			InterCorr[i][j] = 0;

			offStandOffCorr[i][j] = 0;    //站立度修正
			offWidthCorr[i][j] = 0;
			offInterCorr[i][j] = 0;
		}
		errPinStandOffDiffMin[i] = 0;
		errPinStandOffDiffMax[i] = 0;
	}

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
}

