#include "nccMatch_outNParam.h"
#include <iniParam.h>
#include "qvglobal.h"

nccMatch_outNParam::nccMatch_outNParam()
{
	iniData();
}

nccMatch_outNParam::~nccMatch_outNParam()
{

}

void nccMatch_outNParam::readWriteParam(bool r, const QString& dirPath, int level)
{
	SETTING_INIT(dirPath + "/nccMatch_outN.ini");

	if (r&&level != ProductLevel)
		return;

	RW_VALUE(r, currentGrp);
	for (int i = 0;i < MAX_TEMP_NUM;i++) {
		RW_VALUE1(r, GrpEnable[i], i);
		RW_VALUE1(r, IsUseOutArea[i], i);

		if (!GrpEnable[i] && !IsUseOutArea[i]) {
			continue;
		}
		RW_VALUE1(r, outRegionCount[i], i);
		RW_VALUE1(r, outDeleteCount[i], i);
		RW_VALUE1(r, outAreaMin[i], i);
		RW_VALUE1(r, outAreaMax[i], i);
		RW_VALUE1(r, outThreMin[i], i);
		RW_VALUE1(r, outThreMax[i], i);
		RW_VALUE1(r, IsShowOutArea[i], i);
		RW_VALUE1(r, IsFillUp[i], i);
		RW_VALUE1(r, openVal[i], i);
		RW_VALUE1(r, closeVal[i], i);
		segRegion seg2;
		seg2.row = seg2.col = 130;
		seg2.length1 = seg2.length2 = 20;
		seg2.angle = 0;
		delRegion dseg2;
		dseg2.drow = dseg2.dcol = 130;
		dseg2.dlength1 = dseg2.dlength2 = 20;
		dseg2.dangle = 0;
		for (int j = 0;j < outRegionCount[i];j++) {
			if (r&&j > 0) {
				seg2.row += 20;
				seg2.col += 20;
				outRegionVec[i].push_back(seg2);
			}
			RW_VALUE2(r, outRegionVec[i][j].row, i, j);
			RW_VALUE2(r, outRegionVec[i][j].col, i, j);
			RW_VALUE2(r, outRegionVec[i][j].length1, i, j);
			RW_VALUE2(r, outRegionVec[i][j].length2, i, j);
			RW_VALUE2(r, outRegionVec[i][j].angle, i, j);
		}
		for (int j = 0;j < outDeleteCount[i];j++) {
			if (r) {
				dseg2.drow += 20;
				dseg2.dcol += 20;
				deleteRegionVec[i].push_back(dseg2);
			}
			RW_VALUE2(r, deleteRegionVec[i][j].drow, i, j + outRegionCount[i]);
			RW_VALUE2(r, deleteRegionVec[i][j].dcol, i, j + outRegionCount[i]);
			RW_VALUE2(r, deleteRegionVec[i][j].dlength1, i, j + outRegionCount[i]);
			RW_VALUE2(r, deleteRegionVec[i][j].dlength2, i, j + outRegionCount[i]);
			RW_VALUE2(r, deleteRegionVec[i][j].dangle, i, j + outRegionCount[i]);
		}
		char imgPath[2][256];
		sprintf(imgPath[0], "%s/pattern_outArea_%d.bmp", dirPath.toLocal8Bit().constData(), i);
		sprintf(imgPath[1], "%s/outPattern_outArea_%d.bmp", dirPath.toLocal8Bit().constData(), i);
		if (r) {
			set_check("~give_error");
			read_image(&patImage[i], imgPath[0]);
			read_image(&OutImg[i], imgPath[1]);
			set_check("give_error");
		}
		else {
			set_check("~give_error");
			write_image(patImage[i], "bmp", 0, imgPath[0]);
			write_image(OutImg[i], "bmp", 0, imgPath[1]);
			set_check("give_error");
		}
		if (!GrpEnable[i] && IsUseOutArea[i]) {
			continue;
		}
		RW_VALUE1(r, IsShowMinusAfterDet[i], i);
		RW_VALUE1(r, IsShowRoiAfterDet[i], i);
		RW_VALUE1(r, similarity[i], i);
		RW_VALUE1(r, seekingAngle[i], i);
		RW_VALUE1(r, acceptEnable[i], i);
		RW_VALUE1(r, acceptValue[i], i);
		RW_VALUE1(r, offsetEnable[i], i);
		RW_VALUE1(r, offsetValue[i], i);
		RW_VALUE1(r, relativeEnable[i], i);
		RW_VALUE1(r, minRelative[i], i);
		RW_VALUE1(r, maxRelative[i], i);
		RW_VALUE1(r, countEnable[i], i);
		RW_VALUE1(r, minCount[i], i);
		RW_VALUE1(r, maxCount[i], i);
		RW_VALUE1(r, roix[i], i);
		RW_VALUE1(r, roiy[i], i);
		RW_VALUE1(r, roih[i], i);
		RW_VALUE1(r, roiw[i], i);

		RW_VALUE1(r, patternH[i], i);
		RW_VALUE1(r, patternW[i], i);
		RW_VALUE1(r, patternX[i], i);
		RW_VALUE1(r, patternY[i], i);

		RW_VALUE1(r, findNum[i], i);
		RW_VALUE1(r, minusA[i], i);

		RW_VALUE1(r, minusRow[i], i);
		RW_VALUE1(r, minusCol[i], i);
		RW_VALUE1(r, minusLength1[i], i);
		RW_VALUE1(r, minusLength2[i], i);

		RW_VALUE1(r, minusCX[i], i);
		RW_VALUE1(r, minusCY[i], i);
		RW_VALUE1(r, minusR[i], i);

		RW_VALUE1(r, minusArea[i], i);
		RW_VALUE1(r, maxOverlap[i], i);

		RW_VALUE1(r, IsAxisAngle[i], i);
		RW_VALUE1(r, maxAxisAngle[i], i);
	}
}

void nccMatch_outNParam::iniData()
{
	currentGrp = 0;
	for (int i = 0;i < MAX_TEMP_NUM;i++) {
		GrpEnable[i] = 0;
		IsShowRoiAfterDet[i] = 0;
		IsShowMinusAfterDet[i] = 0;
		similarity[i] = 45;
		seekingAngle[i] = 10;
		minusArea[i] = 0;
		acceptEnable[i] = 0;
		offsetEnable[i] = 0;
		relativeEnable[i] = 0;
		countEnable[i] = 0;
		acceptValue[i] = 50;
		offsetValue[i] = 15;
		minRelative[i] = maxRelative[i] = 0;
		minCount[i] = maxCount[i] = 1;
		roix[i] = roiy[i] = 0;
		roiw[i] = roih[i] = 100;

		patternX[i] = patternY[i] = 20;
		patternW[i] = patternH[i] = 50;

		minusA[i] = 0;
		minusCX[i] = minusCY[i] = 100;

		minusRow[i] = minusCol[i] = 50;
		minusLength1[i] = minusLength2[i] = 20;

		minusR[i] = 50;
		findNum[i] = 1;
		maxOverlap[i] = 0.001;
		outRegionCount[i] = 1;
		outDeleteCount[i] = 0;
		outAreaMin[i] = 0;
		outAreaMax[i] = 99999;
		outThreMin[i] = 0;
		outThreMax[i] = 255;
		IsUseOutArea[i] = 0;
		IsShowOutArea[i] = 0;
		IsFillUp[i] = 0;
		openVal[i] = 0;
		closeVal[i] = 0;

		IsAxisAngle[i] = 0;
		maxAxisAngle[i] = 45;

		outRegionVec[i].clear();
		deleteRegionVec[i].clear();
		segRegion seg;
		seg.row = 130;
		seg.col = 130;
		seg.length1 = 80;
		seg.angle = 0;
		seg.length2 = 80;
		outRegionVec[i].push_back(seg);
	}
	GrpEnable[0] = 1;
}

