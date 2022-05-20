#include "standEdgeParam.h"
#include <iniParam.h>
#include "qvglobal.h"

standEdgeParam::standEdgeParam()
{
	iniData();
}

standEdgeParam::~standEdgeParam()
{

}

void standEdgeParam::readWriteParam(bool r, const QString& dirPath, int level)
{
	SETTING_INIT(dirPath + "/standEdge.ini");

	if (r&&level != ProductLevel)
		return;

	RW_VALUE(r, NowGrp);
	if (r&&NowGrp >= 4) {
		NowGrp = 3;
	}
	RW_VALUE(r, IsEndPoint);
	if (IsEndPoint == 0)
		IsEndPoint = 1;

	for (int i = 0;i < 4;i++)
	{
		//RW_VALUE1(r,whiteBlackDir[i],i);
		//RW_VALUE1(r,searchDir[i],i);
		RW_VALUE1(r, extendSearchLine[i], i);
		RW_VALUE1(r, minGrayDiff[i], i);
		//RW_VALUE1(r,grayPlus[i],i);
		//RW_VALUE1(r,grayMin[i],i);
		//RW_VALUE1(r,grayMax[i],i);
		RW_VALUE1(r, searchLineGap[i], i);
		RW_VALUE1(r, lineOffset[i], i);
		RW_VALUE1(r, IsPaichuLine[i], i);
		RW_VALUE1(r, paichuLine[i], i);
		RW_VALUE1(r, scalePaichu[i], i);
		RW_VALUE1(r, IsCutLine[i], i);
		RW_VALUE1(r, offsetPlus[i], i);
		RW_VALUE1(r, grayVal[i], i);
		RW_VALUE1(r, edgeGray[i], i);
		RW_VALUE1(r, clipfactor[i], i);
		RW_VALUE1(r, IsEdgeDir[i], i);
		RW_VALUE1(r, isLeadSize[i], i);
		RW_VALUE1(r, leadSize[i], i);
		RW_VALUE1(r, IsClearOutlier[i], i);
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
	//RoiRegion seg;
	//seg.row1=seg.col1=40;
	//seg.row2=seg.col2=90;
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

void standEdgeParam::iniData()
{
	NowGrp = 0;

	IsEndPoint = 1;      //是否检测端点0关闭1启用
	maxAngle = 10;

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
		grayVal[i] = 100;
		edgeGray[i] = 50;
		clipfactor[i] = 1.0;
		IsEdgeDir[i] = 1;
		isLeadSize[i] = 0;
		leadSize[i] = 0;
		IsClearOutlier[i] = 1;
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
}

