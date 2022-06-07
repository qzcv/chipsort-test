#include "pinDetect1Param.h"
#include <iniParam.h>
#include "qvglobal.h"

pinDetect1Param::pinDetect1Param()
{
	iniData();
}

pinDetect1Param::~pinDetect1Param()
{

}

void pinDetect1Param::readWriteParam(bool r, const QString& dirPath, int level)
{
	SETTING_INIT(dirPath + "/pinDetect1.ini");

	if (r&&level != ProductLevel)
		return;

	RW_VALUE(r, IsDetBase);
	RW_VALUE(r, IsReferEdgeRev);
	RW_VALUE(r, widthMethod);
	RW_VALUE(r, LRorUD);
	RW_VALUE(r, IsShowBlack);
	RW_VALUE(r, IsSameValid);
	RW_VALUE(r, currentGrp);
	//RW_VALUE(r,onlyUseEdge1);
	RW_VALUE(r, IsDelOffSearchRoi);

	RW_VALUE(r, SearchEdgeRoi.row);
	RW_VALUE(r, SearchEdgeRoi.col);
	RW_VALUE(r, SearchEdgeRoi.angle);
	RW_VALUE(r, SearchEdgeRoi.len1);
	RW_VALUE(r, SearchEdgeRoi.len2);

	RW_VALUE(r, isUpdateCorr);

	RW_VALUE(r, IsUseInputRegion);
	RW_VALUE(r, offsetDown);
	RW_VALUE(r, offsetLeft);
	RW_VALUE(r, offsetRight);
	RW_VALUE(r, offsetUp);


	for (int i = 0;i < GRP_NUM;i++)
	{
		RW_VALUE1(r, shoulderShift[i], i);
		RW_VALUE1(r, ThreMinArea[i], i);
		RW_VALUE1(r, ThreGrayVal[i], i);
		RW_VALUE1(r, ThreLeadSize[i], i);
		RW_VALUE1(r, IsAutoWhite[i], i);
		RW_VALUE1(r, AutoThreReg[i].row1, i);
		RW_VALUE1(r, AutoThreReg[i].col1, i);
		RW_VALUE1(r, AutoThreReg[i].row2, i);
		RW_VALUE1(r, AutoThreReg[i].col2, i);

		//RW_VALUE1(r,IsDetBase[i],i);
		RW_VALUE1(r, IsRefer[i], i);
		RW_VALUE1(r, InputBaseIndex[i], i);
		RW_VALUE1(r, baseLineOffset[i], i);
		RW_VALUE1(r, IsBaseInvert[i], i);

		RW_VALUE1(r, CrossGrayValid[i], i);
		RW_VALUE1(r, crossStep[i], i);
		RW_VALUE1(r, crossGray[i], i);

		RW_VALUE1(r, IsAmpBest[i], i);
		//RW_VALUE1(r,grpEnable[i],i);
		RW_VALUE1(r, grpPinNum[i], i);
		RW_VALUE1(r, whiteBlackDir[i], i);
		RW_VALUE1(r, searchDir[i], i);
		RW_VALUE1(r, extendSearchLine[i], i);
		RW_VALUE1(r, minGrayDiff[i], i);
		RW_VALUE1(r, searchLineGap[i], i);

		//
		RW_VALUE1(r, minArea[i], i);
		RW_VALUE1(r, noiseGray[i], i);
		RW_VALUE1(r, multGrayTimes[i], i);

		RW_VALUE1(r, widSearchLength[i], i);
		RW_VALUE1(r, WidthGrayDiff[i], i);
		RW_VALUE1(r, widLineOffset[i], i);
		if (r) {
			widLineOffset[i] = abs(widLineOffset[i]);
		}
		RW_VALUE1(r, widOffsetRange[i], i);
		//RW_VALUE1(r,standoffOffset[i],i);
		RW_VALUE1(r, clipfactor[i], i);

		RW_VALUE1(r, lengthCorr[i], i);
		RW_VALUE1(r, widthCorr[i], i);
		RW_VALUE1(r, InterCorr[i], i);

		for (int j = 0;j < grpPinNum[i];j++) {
			RW_VALUE2(r, lengthCorrNew[i][j], i, j);
			RW_VALUE2(r, widthCorrNew[i][j], i, j);
			RW_VALUE2(r, InterCorrNew[i][j], i, j);
		}
		RW_VALUE1(r, sizeRatio[i], i);
		RW_VALUE1(r, IsInputRatio[i], i);
		RW_VALUE1(r, deleteEdge[i], i);
		//zhangao
		RW_VALUE1(r, errPinGapValid[i], i);
		RW_VALUE1(r, errPinLengthValid[i], i);
		RW_VALUE1(r, errPinStandOffDiffValid[i], i);
		RW_VALUE1(r, errPinWidthValid[i], i);
		RW_VALUE1(r, errPinSweepValid[i], i);
		RW_VALUE1(r, errPinStandOffDiffMin[i], i);
		RW_VALUE1(r, errPinStandOffDiffMax[i], i);
		for (int j = 0;j < grpPinNum[i];j++) {
			RW_VALUE2(r, errPinGapMin[i][j], i, j);
			RW_VALUE2(r, errPinGapMax[i][j], i, j);
			RW_VALUE2(r, errPinLengthMin[i][j], i, j);
			RW_VALUE2(r, errPinLengthMax[i][j], i, j);
			RW_VALUE2(r, errPinWidthMin[i][j], i, j);
			RW_VALUE2(r, errPinWidthMax[i][j], i, j);
			RW_VALUE2(r, errPinSweepMin[i][j], i, j);
			RW_VALUE2(r, errPinSweepMax[i][j], i, j);
		}
	}

	if (r && !isUpdateCorr) {
		for (int i = 0;i < GRP_NUM;i++) {
			for (int j = 0;j < grpPinNum[i];j++) {
				lengthCorrNew[i][j] = lengthCorr[i];
				widthCorrNew[i][j] = widthCorr[i];
				InterCorrNew[i][j] = InterCorr[i];
			}
		}
		isUpdateCorr = 1;
	}

	RoiRegion seg;
	seg.row1 = seg.col1 = 40;
	seg.row2 = seg.col2 = 70;
	for (int i = 0;i < GRP_NUM;i++)
	{
		if (r) {
			RoiVec[i].clear();
			for (int j = 0;j < 1 + grpPinNum[i];j++) {
				seg.row1 += 10;
				seg.row2 += 10;
				seg.col1 += 10;
				seg.col2 += 10;
				RoiVec[i].push_back(seg);
				RW_VALUE2(r, RoiVec[i][j].row1, i, j);
				RW_VALUE2(r, RoiVec[i][j].col1, i, j);
				RW_VALUE2(r, RoiVec[i][j].row2, i, j);
				RW_VALUE2(r, RoiVec[i][j].col2, i, j);
			}
		}
		else {
			for (int j = 0;j < 1 + grpPinNum[i];j++) {
				RW_VALUE2(r, RoiVec[i][j].row1, i, j);
				RW_VALUE2(r, RoiVec[i][j].col1, i, j);
				RW_VALUE2(r, RoiVec[i][j].row2, i, j);
				RW_VALUE2(r, RoiVec[i][j].col2, i, j);
			}
		}
	}
}

void pinDetect1Param::iniData()
{
	IsReferEdgeRev = 0;
	widthMethod = 0;
	LRorUD = 1;
	IsShowBlack = 0;
	IsSameValid = 1;
	currentGrp = 0;
	IsDelOffSearchRoi = 0;
	isUpdateCorr = 0;

	IsUseInputRegion = 0;
	offsetUp = 0;
	offsetRight = 0;
	offsetLeft = 0;
	offsetDown = 0;

	IsDetBase = 2;

	for (int i = 0;i < GRP_NUM;i++) {
		shoulderShift[i] = 10;
		ThreMinArea[i] = 10;
		ThreGrayVal[i] = 50;
		ThreLeadSize[i] = 30;
		IsAutoWhite[i] = 1;
		IsRefer[i] = 1;
		lengthCorr[i] = 0;    //ÐÞÕý
		widthCorr[i] = 0;
		InterCorr[i] = 0;

		IsBaseInvert[i] = 0;
		InputBaseIndex[i] = i;
		baseLineOffset[i] = 0;
		IsAmpBest[i] = 1;
		grpPinNum[i] = 2;
		whiteBlackDir[i] = 0;
		searchDir[i] = i;
		extendSearchLine[i] = 3;
		minGrayDiff[i] = 20;
		searchLineGap[i] = 3;
		widLineOffset[i] = 1;
		widOffsetRange[i] = 10;
		//standoffOffset[i]=0;
		clipfactor[i] = 2.0;
		deleteEdge[i] = 0;
		sizeRatio[i] = 1.0;
		IsInputRatio[i] = 0;
		WidthGrayDiff[i] = 20;
		widSearchLength[i] = 10;

		crossStep[i] = 2;
		CrossGrayValid[i] = 0;
		crossGray[i] = 150;
		//
		minArea[i] = 500;
		noiseGray[i] = 80;
		multGrayTimes[i] = 2;
		//zhangao
		errPinGapValid[i] = 0;
		errPinLengthValid[i] = 0;
		errPinStandOffDiffValid[i] = 0;
		errPinWidthValid[i] = 1;
		errPinSweepValid[i] = 0;
		for (int j = 0;j < GRP_MAXLEAD;j++) {
			errPinLengthMin[i][j] = 0;
			errPinLengthMax[i][j] = 10;
			errPinWidthMin[i][j] = 0;
			errPinWidthMax[i][j] = 10;
			errPinGapMin[i][j] = 0;
			errPinGapMax[i][j] = 10;
			errPinSweepMin[i][j] = 0;
			errPinSweepMax[i][j] = 10.0;
			lengthCorrNew[i][j] = 0;    //ÐÞÕý
			widthCorrNew[i][j] = 0;
			InterCorrNew[i][j] = 0;
		}
		errPinStandOffDiffMax[i] = 0;
		errPinStandOffDiffMin[i] = 0;
	}
	RoiRegion seg;
	seg.row1 = seg.col1 = 40;
	seg.row2 = seg.col2 = 70;
	for (int i = 0;i < GRP_NUM;i++) {
		AutoThreReg[i].row1 = 100;
		AutoThreReg[i].col1 = 100;
		AutoThreReg[i].row2 = 300;
		AutoThreReg[i].col2 = 300;
		RoiVec[i].clear();
		for (int j = 0;j < 1 + grpPinNum[i];j++) {
			seg.row1 += 10;
			seg.row2 += 10;
			seg.col1 += 10;
			seg.col2 += 10;
			RoiVec[i].push_back(seg);
		}
	}
	SearchEdgeRoi.row = 250;
	SearchEdgeRoi.col = 250;
	SearchEdgeRoi.angle = 0;
	SearchEdgeRoi.len1 = 120;
	SearchEdgeRoi.len2 = 100;
}

