#include "dimensionMeasureParam.h"
#include <iniParam.h>
#include "qvglobal.h"

dimensionMeasureParam::dimensionMeasureParam()
{
	iniData();
}

dimensionMeasureParam::~dimensionMeasureParam()
{

}

void dimensionMeasureParam::readWriteParam(bool r, const QString& dirPath, int level)
{
	SETTING_INIT(dirPath + "/dimensionMeasure.ini");

	if (r&&level != ProductLevel)
		return;

	RW_VALUE(r, IsReferSection);
	RW_VALUE(r, IsShowAfterDet);
	RW_VALUE(r, regionCount);
	RW_VALUE(r, threshold);
	RW_VALUE(r, searchGap);
	RW_VALUE(r, noiseMaxNum);
	RW_VALUE(r, noiseMin);
	RW_VALUE(r, IsBestEdge);
	RW_VALUE(r, IsBlack);
	RW_VALUE(r, IsValidPoints);
	RW_VALUE(r, validPointsPer);
	dimensionMeasureParam::segRegion seg;
	seg.row = seg.col = 50;
	seg.length1 = seg.length2 = 40;
	seg.angle = 0;
	double lengthmin = 0;
	double lengthmax = 100.0;
	for (int i = 0;i < regionCount;i++) {
		if (r&&i > 0) {
			seg.row += 20;
			seg.col += 20;
			RoiRegionVector.push_back(seg);
			lengthMinVec.push_back(lengthmin);
			lengthMaxVec.push_back(lengthmax);
		}
		RW_VALUE1(r, RoiRegionVector[i].row, i);
		RW_VALUE1(r, RoiRegionVector[i].col, i);
		RW_VALUE1(r, RoiRegionVector[i].length1, i);
		RW_VALUE1(r, RoiRegionVector[i].length2, i);
		RW_VALUE1(r, RoiRegionVector[i].angle, i);
		RW_VALUE1(r, lengthMinVec[i], i);
		RW_VALUE1(r, lengthMaxVec[i], i);
	}
}

void dimensionMeasureParam::iniData()
{
	IsReferSection = 0;
	regionCount = 1;
	IsShowAfterDet = 1;
	threshold = 30;
	searchGap = 10;
	noiseMaxNum = 0;
	noiseMin = 99;

	double lengthVal = 0;
	lengthMinVec.clear();
	lengthMinVec.push_back(lengthVal);
	lengthVal = 100.0;
	lengthMaxVec.clear();
	lengthMaxVec.push_back(lengthVal);
	dimensionMeasureParam::segRegion seg;
	seg.row = seg.col = 50;
	seg.length1 = seg.length2 = 40;
	seg.angle = 0;
	RoiRegionVector.clear();
	RoiRegionVector.push_back(seg);
	IsBlack = 1;
	IsBestEdge = 0;
	IsValidPoints = 0;
	validPointsPer = 50;
}

