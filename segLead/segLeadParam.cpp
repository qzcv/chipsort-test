#include "segLeadParam.h"
#include <iniParam.h>
#include "qvglobal.h"

segLeadParam::segLeadParam()
{
	iniData();
}

segLeadParam::~segLeadParam()
{

}

void segLeadParam::readWriteParam(bool r, const QString& dirPath, int level)
{
	SETTING_INIT(dirPath + "/segLead.ini");

	if (r&&level != ProductLevel)
		return;

	RW_VALUE(r, referSection);
	RW_VALUE(r, IsReferEdge);
	RW_VALUE(r, IsInputPixel);
	RW_VALUE(r, pixSize);
	RW_VALUE(r, showAfterDet);
	RW_VALUE(r, leadNum);
	RW_VALUE(r, areaMin);
	RW_VALUE(r, areaMax);
	RW_VALUE(r, openWidth);
	RW_VALUE(r, openHeight);
	RW_VALUE(r, closeWidth);
	RW_VALUE(r, closeHeight);
	RW_VALUE(r, meanWidth);
	RW_VALUE(r, meanHeight);
	RW_VALUE(r, grayWhite);
	RW_VALUE(r, grayBlack);
	RW_VALUE(r, IsGauss);
	RW_VALUE(r, GaussSize);
	RW_VALUE(r, WOrB);
	RW_VALUE(r, HorOrVer);
	RW_VALUE(r, IsAutoWhite);
	//regionGrowing
	RW_VALUE(r, centerType);
	RW_VALUE(r, centerX);
	RW_VALUE(r, centerY);
	RW_VALUE(r, rowWid);
	RW_VALUE(r, colWid);
	RW_VALUE(r, thresholdTol);
	RW_VALUE(r, minGrowArea);
	RW_VALUE(r, threMin);
	RW_VALUE(r, threMax);

	RW_VALUE(r, testType);

	RW_VALUE(r, errWidthValid);
	RW_VALUE(r, errLengthValid);
	RW_VALUE(r, errInterValid);
	RW_VALUE(r, errEdgeDisValid);
	for (int i = 0;i < leadNum;i++) {
		RW_VALUE1(r, errWidthMin[i], i);
		RW_VALUE1(r, errLengthMin[i], i);
		RW_VALUE1(r, errInterMin[i], i);
		RW_VALUE1(r, errEdgeDisMin[i], i);
		RW_VALUE1(r, errWidthMax[i], i);
		RW_VALUE1(r, errLengthMax[i], i);
		RW_VALUE1(r, errInterMax[i], i);
		RW_VALUE1(r, errEdgeDisMax[i], i);
	}
	roiRegion seg2;
	seg2.row1 = seg2.col1 = 110;
	seg2.row2 = seg2.col2 = 150;
	if (r) {
		RoiVec.clear();
	}
	for (int i = 0;i < leadNum + 1;i++) {
		if (r) {			//read
			RoiVec.push_back(seg2);
		}
		RW_VALUE1(r, RoiVec[i].row1, i);
		RW_VALUE1(r, RoiVec[i].col1, i);
		RW_VALUE1(r, RoiVec[i].row2, i);
		RW_VALUE1(r, RoiVec[i].col2, i);
	}
	RW_VALUE(r, autoLeadSize);
	RW_VALUE(r, autoLeadThre);
}

void segLeadParam::iniData()
{
	IsReferEdge = 0;
	IsInputPixel = 0;
	referSection = 0;
	pixSize = 1.0;
	showAfterDet = 1;
	areaMin = 10;
	areaMax = 100000;
	meanWidth = 5;
	meanHeight = 5;
	IsGauss = 0;
	GaussSize = 3;
	leadNum = 3;
	openWidth = 1;
	openHeight = 3;
	closeWidth = 3;
	closeHeight = 5;
	grayWhite = 10;
	grayBlack = 255;
	WOrB = 1;
	HorOrVer = 0;
	IsAutoWhite = 1;

	//regiongrowing
	centerType = 0;
	centerX = 100;
	centerY = 100;
	rowWid = 1;
	colWid = 1;
	thresholdTol = 5;
	minGrowArea = 100;

	threMin = 0;
	threMax = 255;

	testType = 0;

	errWidthValid = 1;
	errLengthValid = 0;
	errInterValid = 0;
	errEdgeDisValid = 0;

	for (int i = 0;i < Max_LeadNum;i++) {
		errWidthMin[i] = 0;
		errWidthMax[i] = 10;
		errLengthMin[i] = 0;
		errLengthMax[i] = 10;
		errInterMin[i] = 0;
		errInterMax[i] = 10;
		errEdgeDisMin[i] = 0;
		errEdgeDisMax[i] = 10;
	}
	RoiVec.clear();
	roiRegion seg1;
	seg1.row1 = 130;
	seg1.col1 = 130;
	seg1.row2 = 180;
	seg1.col2 = 180;
	RoiVec.push_back(seg1);
	for (int i = 0;i < leadNum;i++) {
		seg1.row1 = seg1.col1 += 20;
		seg1.row2 = seg1.col2 += 20;
		RoiVec.push_back(seg1);
	}
	autoLeadThre = 100;
	autoLeadSize = 30;
}

