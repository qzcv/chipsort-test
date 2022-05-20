#include "imageSegment_preParam.h"
#include <iniParam.h>
#include "qvglobal.h"

imageSegment_preParam::imageSegment_preParam()
{
	iniData();
}

imageSegment_preParam::~imageSegment_preParam()
{

}

void imageSegment_preParam::readWriteParam(bool r, const QString& dirPath, int level)
{
	SETTING_INIT(dirPath + "/imageSegment_pre.ini");

	if (r&&level != ProductLevel)
		return;

	RW_VALUE(r, moduleValid);
	RW_VALUE(r, thresholdType);
	RW_VALUE(r, allImgValid);
	RW_VALUE(r, RoiCounts);
	if (r) {
		segRegionVector.clear();
		segRegion seg;
		seg.row1 = seg.col1 = 40;
		seg.row2 = seg.col2 = 90;
		for (int i = 0;i < RoiCounts;i++) {
			seg.row1 += 10;
			seg.row2 += 10;
			seg.col1 += 10;
			seg.col2 += 10;
			segRegionVector.push_back(seg);
		}
	}
	RW_VALUE(r, IsMeanGrow);
	RW_VALUE(r, IsNoCenter);
	RW_VALUE(r, centerType);
	for (int i = 0;i < MAX_ROI;i++) {
		RW_VALUE1(r, centerX[i], i);
		RW_VALUE1(r, centerY[i], i);

		RW_VALUE1(r, minGray[i], i);
		RW_VALUE1(r, maxGray[i], i);
		RW_VALUE1(r, minArea[i], i);
		RW_VALUE1(r, maxArea[i], i);
	}

	RW_VALUE(r, rowWid);
	RW_VALUE(r, colWid);
	RW_VALUE(r, thresholdTol);
	RW_VALUE(r, minGrowArea);

	RW_VALUE(r, algorithmType);
	for (int i = 0;i < 5;i++) {
		RW_VALUE1(r, param1[i], i);
		RW_VALUE1(r, param2[i], i);
		RW_VALUE1(r, param3[i], i);
	}
	for (int i = 0;i < RoiCounts;i++)
	{
		RW_VALUE1(r, segRegionVector[i].row1, i);
		RW_VALUE1(r, segRegionVector[i].col1, i);
		RW_VALUE1(r, segRegionVector[i].row2, i);
		RW_VALUE1(r, segRegionVector[i].col2, i);
	}
}

void imageSegment_preParam::iniData()
{
	moduleValid = 0;
	thresholdType = 0;
	allImgValid = 0;
	RoiCounts = 1;
	for (int i = 0;i < MAX_ROI;i++) {
		minGray[i] = 0;
		maxGray[i] = 255;
		minArea[i] = 0;
		maxArea[i] = 10000;
	}

	IsMeanGrow = 0;
	IsNoCenter = 0;
	centerType = 0;
	for (int i = 0;i < MAX_ROI;i++) {
		centerX[i] = 100;
		centerY[i] = 100;
	}
	rowWid = 1;
	colWid = 1;
	thresholdTol = 5;
	minGrowArea = 100;

	algorithmType = 0;
	for (int i = 0;i < 5;i++)
	{
		param1[i] = 0;
		param2[i] = 0;
		param3[i] = 0;
	}
	segRegionVector.clear();
	segRegion seg;
	seg.row1 = seg.col1 = 40;
	seg.row2 = seg.col2 = 90;
	for (int i = 0;i < RoiCounts;i++) {
		seg.row1 += 10;
		seg.row2 += 10;
		seg.col1 += 10;
		seg.col2 += 10;
		segRegionVector.push_back(seg);
	}
}

