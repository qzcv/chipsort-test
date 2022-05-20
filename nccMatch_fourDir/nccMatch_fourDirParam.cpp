#include "nccMatch_fourDirParam.h"
#include <iniParam.h>
#include "qvglobal.h"

nccMatch_fourDirParam::nccMatch_fourDirParam()
{
	iniData();
}

nccMatch_fourDirParam::~nccMatch_fourDirParam()
{

}

void nccMatch_fourDirParam::readWriteParam(bool r, const QString& dirPath, int level)
{
	SETTING_INIT(dirPath + "/nccMatch_fourDir.ini");

	if (!r || (r&&level == ProductLevel))
	{
		RW_VALUE(r, IsRefer);
		RW_VALUE(r, IsDefaultSig);
		RW_VALUE(r, IsDefaultEdit);
		RW_VALUE(r, StartAngle);

		RW_VALUE(r, Is2Dor4D);
		RW_VALUE(r, currentPat);

		RW_VALUE(r, IsUsePinDir);
		RW_VALUE(r, IsMirrorDir);
		RW_VALUE(r, HorOrVerMir);

		RW_VALUE(r, matchAngleEnable);
		RW_VALUE(r, matchAngle);

		RW_VALUE(r, acceptEnable);
		RW_VALUE(r, acceptValue);
		RW_VALUE(r, relativeEnable);
		RW_VALUE(r, minRelative);
		RW_VALUE(r, maxRelative);
		RW_VALUE(r, similarity);
		RW_VALUE(r, seekingAngle);
		RW_VALUE(r, maxOverlap);
		RW_VALUE(r, findNum);
		RW_VALUE(r, IsAutoRotate);
		if (!r) {
			IsAutoRotate = 1;
		}

		RW_VALUE(r, RecRoiRow1);
		RW_VALUE(r, RecRoiRow2);
		RW_VALUE(r, RecRoiCol1);
		RW_VALUE(r, RecRoiCol2);

		for (int i = 0;i < 2;i++) {
			for (int j = 0;j < 4;j++) {
				RW_VALUE2(r, sigEdit[i][j], i, j);
			}
		}
		for (int i = 0;i < MAX_PAT_NUM;i++) {
			RW_VALUE1(r, roiRow1[i], i);
			RW_VALUE1(r, roiRow2[i], i);
			RW_VALUE1(r, roiCol1[i], i);
			RW_VALUE1(r, roiCol2[i], i);

			RW_VALUE1(r, patternH[i], i);
			RW_VALUE1(r, patternW[i], i);
			RW_VALUE1(r, patternX[i], i);
			RW_VALUE1(r, patternY[i], i);

			char imgPath[256];
			if (i > 1 && Is2Dor4D == 0) {
				continue;
			}
			else {
				sprintf(imgPath, "%s/patternFOURDIR%d.bmp", dirPath.toLocal8Bit().constData(), i);
				if (r) {
					set_check("~give_error");
					read_image(&patImage[i], imgPath);
					set_check("give_error");
				}
				else {
					set_check("~give_error");
					write_image(patImage[i], "bmp", 0, imgPath);
					set_check("give_error");
				}
			}
		}

		RW_VALUE(r, outRegionCount);
		RW_VALUE(r, outDeleteCount);
		RW_VALUE(r, outAreaMin);
		RW_VALUE(r, outAreaMax);
		RW_VALUE(r, outThreMin);
		RW_VALUE(r, outThreMax);
		RW_VALUE(r, IsUseOutArea);
		RW_VALUE(r, IsShowOutArea);
		RW_VALUE(r, IsFillUp);
		segRegion seg2;
		seg2.row = seg2.col = 130;
		seg2.length1 = seg2.length2 = 20;
		seg2.angle = 0;
		delRegion dseg2;
		dseg2.drow = dseg2.dcol = 130;
		dseg2.dlength1 = dseg2.dlength2 = 20;
		dseg2.dangle = 0;
		for (int i = 0;i < outRegionCount;i++) {
			if (r&&i > 0) {
				seg2.row += 20;
				seg2.col += 20;
				outRegionVec.push_back(seg2);
			}
			RW_VALUE1(r, outRegionVec[i].row, i);
			RW_VALUE1(r, outRegionVec[i].col, i);
			RW_VALUE1(r, outRegionVec[i].length1, i);
			RW_VALUE1(r, outRegionVec[i].length2, i);
			RW_VALUE1(r, outRegionVec[i].angle, i);
		}
		for (int i = 0;i < outDeleteCount;i++) {
			if (r) {
				dseg2.drow += 20;
				dseg2.dcol += 20;
				deleteRegionVec.push_back(dseg2);
			}
			RW_VALUE1(r, deleteRegionVec[i].drow, i + outRegionCount);
			RW_VALUE1(r, deleteRegionVec[i].dcol, i + outRegionCount);
			RW_VALUE1(r, deleteRegionVec[i].dlength1, i + outRegionCount);
			RW_VALUE1(r, deleteRegionVec[i].dlength2, i + outRegionCount);
			RW_VALUE1(r, deleteRegionVec[i].dangle, i + outRegionCount);
		}
		char imgPath[256];
		for (int i = 0;i < MAX_PAT_NUM;i++) {
			sprintf(imgPath, "%s/fourDir_outArea%d.bmp", dirPath.toLocal8Bit().constData(), i);
			if (r) {
				set_check("~give_error");
				read_image(&OutImg[i], imgPath);
				set_check("give_error");
			}
			else {
				set_check("~give_error");
				write_image(OutImg[i], "bmp", 0, imgPath);
				set_check("give_error");
			}
		}
	}
	if (!r || (r&&level == MachineLevel))
	{
		RW_VALUE(r, pin1OutIdx);
		RW_VALUE(r, pin2OutIdx);
	}
	
}

void nccMatch_fourDirParam::iniData()
{
	for (int i = 0;i < MAX_PAT_NUM;i++) {
		minusArea[i] = 0;
		roiRow1[i] = roiCol1[i] = 50 + 20 * i;
		roiRow2[i] = roiCol2[i] = 100 + 20 * i;
		patternX[i] = patternY[i] = 20;
		patternW[i] = patternH[i] = 50;

		minusA[i] = 0;
		minusCX[i] = minusCY[i] = 100;

		minusRow[i] = minusCol[i] = 50;
		minusLength1[i] = minusLength2[i] = 20;
		minusR[i] = 50;
	}

	RecRoiRow1 = 250;
	RecRoiCol1 = 250;
	RecRoiRow2 = 450;
	RecRoiCol2 = 450;

	StartAngle = 0;
	IsAutoRotate = 1;
	IsRefer = 0;
	maxOverlap = 0.001;
	findNum = 1;
	similarity = 30;
	seekingAngle = 10;
	acceptEnable = 0;
	relativeEnable = 0;
	acceptValue = 50;
	minRelative = maxRelative = 0;

	IsUsePinDir = 0;
	pin1OutIdx = 1;
	pin2OutIdx = 2;
	IsDefaultSig = 0;
	IsMirrorDir = 0;
	HorOrVerMir = 0;
	matchAngleEnable = 0;
	matchAngle = 0;

	currentPat = 0;
	Is2Dor4D = 0;

	IsDefaultEdit = 1;
	sigEdit[0][0] = 0;sigEdit[1][0] = 0;
	sigEdit[0][1] = 0;sigEdit[1][1] = 1;
	sigEdit[0][2] = 1;sigEdit[1][2] = 0;
	sigEdit[0][3] = 1;sigEdit[1][3] = 1;

	outRegionCount = 1;
	outDeleteCount = 0;
	outAreaMin = 0;
	outAreaMax = 99999;
	outThreMin = 0;
	outThreMax = 255;
	IsUseOutArea = 0;
	IsShowOutArea = 0;
	IsFillUp = 0;

	outRegionVec.clear();
	deleteRegionVec.clear();
	segRegion seg;
	seg.row = 130;
	seg.col = 130;
	seg.length1 = 80;
	seg.angle = 0;
	seg.length2 = 80;
	outRegionVec.push_back(seg);
}

