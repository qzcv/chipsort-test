#include "pinLength1Param.h"
#include <iniParam.h>
#include "qvglobal.h"

pinLength1Param::pinLength1Param()
{
	iniData();
}

pinLength1Param::~pinLength1Param()
{

}

void pinLength1Param::readWriteParam(bool r, const QString& dirPath, int level)
{
	SETTING_INIT(dirPath + "/pinLength1.ini");

	if (r&&level != ProductLevel)
		return;

	RW_VALUE(r, IsNoDetectTop);
	RW_VALUE(r, IsGroupEnableSame);
	RW_VALUE(r, IsSOPType);
	RW_VALUE(r, IsEdgeSopType);
	RW_VALUE(r, IsHNW);
	RW_VALUE(r, IsDelOffSearchRoi);
	RW_VALUE(r, backGray);
	RW_VALUE(r, leadLossIndex);
	RW_VALUE(r, IsUseDir);
	RW_VALUE(r, AngleType);
	RW_VALUE(r, openAndClosing);
	RW_VALUE(r, IsReferSection);

	RW_VALUE(r, PinAreaRefer);
	RW_VALUE(r, offsetUp);
	RW_VALUE(r, offsetDown);
	RW_VALUE(r, offsetLeft);
	RW_VALUE(r, offsetRight);

	RW_VALUE(r, roiOffUp);
	RW_VALUE(r, roiOffDown);
	RW_VALUE(r, roiOffLeft);
	RW_VALUE(r, roiOffRight);
	RW_VALUE(r, IsUseInputRegion);

	RW_VALUE(r, IsInterUseEdgeAngle);
	RW_VALUE(r, IsReferEdgeRev);

	RW_VALUE1(r, pinCont[0], 0);
	RW_VALUE1(r, pinCont[1], 1);
	if (r) {
		for (int i = 0;i < 2;i++) {
			pinCont[i] = pinCont[i] > MAX_PINGRP_NUM ? MAX_PINGRP_NUM : pinCont[i];
		}
	}
	RW_VALUE(r, LRorUD);
	RW_VALUE(r, IsWhitePin);
	RW_VALUE(r, IsIgnoreRoot);
	RW_VALUE(r, IsThreshold);
	RW_VALUE(r, threValue);
	RW_VALUE(r, pinSize);
	RW_VALUE(r, leadAreaMin);
	RW_VALUE(r, leadContrast);
	RW_VALUE(r, widthMin);
	RW_VALUE(r, widthOffset);
	RW_VALUE(r, IsWidth);

	RW_VALUE1(r, SizeRation[0], 0);
	RW_VALUE1(r, SizeRation[1], 1);
	RW_VALUE(r, IsInputSize);
	RW_VALUE(r, errSpanValid);
	RW_VALUE(r, CrossGrayValid);
	RW_VALUE(r, crossStep);
	RW_VALUE(r, crossGray);
	RW_VALUE(r, IsUpdateCorr);
	for (int i = 0;i < 2;i++) {


		RW_VALUE1(r, IsOnlyReferEdge[i], i);
		RW_VALUE1(r, LeadLengthCorr[i], i);
		RW_VALUE1(r, LeadWidthCorr[i], i);
		RW_VALUE1(r, LeadInterCorr[i], i);
		RW_VALUE1(r, errWidthValid[i], i);
		RW_VALUE1(r, errLengthValid[i], i);
		RW_VALUE1(r, errInterValid[i], i);
		RW_VALUE1(r, errMaxDiffValid[i], i);
		RW_VALUE1(r, errMaxDiffMin[i], i);
		RW_VALUE1(r, errMaxDiff[i], i);

		RW_VALUE1(r, sweepOffset[i], i);

		for (int j = 0;j < pinCont[i];j++) {

			RW_VALUE1(r, errSpanMin[j], j);
			RW_VALUE1(r, errSpanMax[j], j);

			RW_VALUE1(r, LeadSpanCorr[j], j);

			RW_VALUE2(r, errWidthMin[i][j], i, j);
			RW_VALUE2(r, errWidthMax[i][j], i, j);
			RW_VALUE2(r, errLengthMin[i][j], i, j);
			RW_VALUE2(r, errLengthMax[i][j], i, j);
			RW_VALUE2(r, errInterMin[i][j], i, j);
			RW_VALUE2(r, errInterMax[i][j], i, j);

			RW_VALUE2(r, LeadLengthCorrNew[i][j], i, j);
			RW_VALUE2(r, LeadWidthCorrNew[i][j], i, j);
			RW_VALUE2(r, LeadInterCorrNew[i][j], i, j);
		}

	}

	if (r && !IsUpdateCorr) {
		for (int i = 0;i < 2;i++) {
			for (int j = 0;j < pinCont[i];j++) {
				LeadLengthCorrNew[i][j] = LeadLengthCorr[i];
				LeadWidthCorrNew[i][j] = LeadWidthCorr[i];
				LeadInterCorrNew[i][j] = LeadInterCorr[i];
			}
		}
		IsUpdateCorr = 1;
	}

	RW_VALUE(r, errAllMaxDiffValid);
	RW_VALUE(r, errAllMaxDiffMin);
	RW_VALUE(r, errAllMaxDiff);

	RW_VALUE1(r, segRegionVector[0].row1, 0);
	RW_VALUE1(r, segRegionVector[0].col1, 0);
	RW_VALUE1(r, segRegionVector[0].row2, 0);
	RW_VALUE1(r, segRegionVector[0].col2, 0);
	RW_VALUE1(r, segRegionVector[1].row1, 1);
	RW_VALUE1(r, segRegionVector[1].col1, 1);
	RW_VALUE1(r, segRegionVector[1].row2, 1);
	RW_VALUE1(r, segRegionVector[1].col2, 1);

	RW_VALUE(r, rotateRoi.row1);
	RW_VALUE(r, rotateRoi.row2);
	RW_VALUE(r, rotateRoi.col1);
	RW_VALUE(r, rotateRoi.col2);

	RW_VALUE(r, SearchEdgeRoi.row);
	RW_VALUE(r, SearchEdgeRoi.col);
	RW_VALUE(r, SearchEdgeRoi.angle);
	RW_VALUE(r, SearchEdgeRoi.len1);
	RW_VALUE(r, SearchEdgeRoi.len2);

	segRegion seg2;
	seg2.row1 = seg2.col1 = 90;
	seg2.row2 = seg2.col2 = 140;
	if (r) {
		vector<segRegion>::iterator itbegin = segRegionVector.begin() + 2;
		vector<segRegion>::iterator itEnd = segRegionVector.end();
		segRegionVector.erase(itbegin, itEnd);
	}
	for (int i = 0;i < pinCont[0] + pinCont[1];i++) {
		if (r) {			//read
			seg2.row1 += 10;
			seg2.col1 += 10;
			seg2.row2 += 10;
			seg2.col2 += 10;
			segRegionVector.push_back(seg2);
			RW_VALUE1(r, segRegionVector[i + 2].row1, i + 2);
			RW_VALUE1(r, segRegionVector[i + 2].col1, i + 2);
			RW_VALUE1(r, segRegionVector[i + 2].row2, i + 2);
			RW_VALUE1(r, segRegionVector[i + 2].col2, i + 2);
		}
		else {                            //write
			RW_VALUE1(r, segRegionVector[i + 2].row1, i + 2);
			RW_VALUE1(r, segRegionVector[i + 2].col1, i + 2);
			RW_VALUE1(r, segRegionVector[i + 2].row2, i + 2);
			RW_VALUE1(r, segRegionVector[i + 2].col2, i + 2);
		}
	}

	RW_VALUE(r, startAngle);
}

void pinLength1Param::iniData()
{
	IsNoDetectTop = 0;
	IsGroupEnableSame = 1;
	IsSOPType = 1;
	IsEdgeSopType = 1;
	IsHNW = 0;
	IsDelOffSearchRoi = 0;
	backGray = 40;
	leadLossIndex = 1;
	IsShowAfterDet = 1;
	IsUseDir = 0;
	AngleType = 0;
	openAndClosing = 0;
	IsReferSection = 0;
	IsOnlyReferEdge[0] = IsOnlyReferEdge[1] = 0;
	PinAreaRefer = 0;
	offsetUp = 0;
	offsetDown = 0;
	offsetLeft = 0;
	offsetRight = 0;

	roiOffUp = 0;
	roiOffDown = 0;
	roiOffLeft = 0;
	roiOffRight = 0;
	IsUseInputRegion = false;

	IsReferEdgeRev = 0;
	IsInterUseEdgeAngle = 0;

	pinCont[0] = pinCont[1] = 4;
	LRorUD = 0;
	IsWhitePin = 0;
	IsIgnoreRoot = 0;
	IsThreshold = 1;
	threValue = 100;
	pinSize = 30;
	leadAreaMin = 1;
	widthMin = 1;
	IsWidth = 0;

	widthOffset = 5;
	leadContrast = 20;

	LeadLengthCorr[0] = LeadLengthCorr[1] = 0;
	LeadWidthCorr[0] = LeadWidthCorr[1] = 0;
	LeadInterCorr[0] = LeadInterCorr[1] = 0;

	SizeRation[0] = SizeRation[1] = 1.0;
	IsInputSize = 0;
	errSpanValid = 0;

	for (int i = 0;i < 2;i++) {
		for (int j = 0;j < MAX_PINGRP_NUM;j++) {
			errWidthMin[i][j] = 0;
			errWidthMax[i][j] = 1;
			errLengthMin[i][j] = 0;
			errLengthMax[i][j] = 1;
			errInterMin[i][j] = 0;
			errInterMax[i][j] = 1;
			if (i == 0) {
				errSpanMin[j] = 0;
				errSpanMax[j] = 100.0;
				LeadSpanCorr[j] = 0;
			}
			LeadLengthCorrNew[i][j] = 0;    //修正
			LeadWidthCorrNew[i][j] = 0;
			LeadInterCorrNew[i][j] = 0;
		}
		errWidthValid[i] = 0;
		errLengthValid[i] = 0;
		errInterValid[i] = 0;
		errMaxDiffValid[i] = 0;
		errMaxDiffMin[i] = 0;
		errMaxDiff[i] = 0;
		errSweepValid[i] = 0;

		sweepOffset[i] = 2;

	}

	crossStep = 2;
	CrossGrayValid = 0;
	crossGray = 150;
	errAllMaxDiffValid = 0;            //全1部长度最大差异
	errAllMaxDiffMin = 0;
	errAllMaxDiff = 0;

	segRegionVector.clear();
	segRegion seg0, seg1, seg2;
	seg0.row1 = seg1.row1 = 20;
	seg0.row2 = seg1.row2 = 100;
	seg0.col1 = 20;
	seg0.col2 = 50;
	seg1.col1 = 70;
	seg1.col2 = 100;
	segRegionVector.push_back(seg0);
	segRegionVector.push_back(seg1);
	seg2.row1 = seg2.col1 = 100;
	seg2.row2 = seg2.col2 = 150;
	for (int i = 0;i < pinCont[0] + pinCont[1];i++) {
		seg2.row1 += 10;
		seg2.col1 += 10;
		seg2.row2 += 10;
		seg2.col2 += 10;
		segRegionVector.push_back(seg2);
	}
	rotateRoi.row1 = 150;
	rotateRoi.col1 = 150;
	rotateRoi.row2 = 250;
	rotateRoi.col2 = 250;

	startAngle = 0;

	SearchEdgeRoi.row = 250;
	SearchEdgeRoi.col = 250;
	SearchEdgeRoi.angle = 0;
	SearchEdgeRoi.len1 = 120;
	SearchEdgeRoi.len2 = 100;
}

