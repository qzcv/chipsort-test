#include "surfaceMultParam.h"
#include <iniParam.h>
#include "qvglobal.h"

surfaceMultParam::surfaceMultParam()
{
	iniData();
}

surfaceMultParam::~surfaceMultParam()
{

}

void surfaceMultParam::readWriteParam(bool r, const QString& dirPath, int level)
{
	SETTING_INIT(dirPath + "/surfaceMult.ini");

	if (r&&level != ProductLevel)
		return;

	RW_VALUE(r, IsPixOrAct);
	RW_VALUE(r, pixSize);

	RW_VALUE(r, rotateRow1);
	RW_VALUE(r, rotateRow2);
	RW_VALUE(r, rotateCol1);
	RW_VALUE(r, rotateCol2);
	RW_VALUE(r, multAngle);

	RW_VALUE(r, DetectModel);
	RW_VALUE(r, IsChangeModel);
	RW_VALUE(r, deleteRefer);
	RW_VALUE(r, referSection);
	RW_VALUE(r, pointRegionRefer);
	RW_VALUE(r, areaZoom);
	RW_VALUE(r, areaRegionRefer);
	RW_VALUE(r, IsDirPattern);
	RW_VALUE(r, IsDirDelete);
	RW_VALUE(r, IsReferPtoDel);
	RW_VALUE(r, delPointZoom);

	RW_VALUE(r, isShowDel);
	RW_VALUE(r, IsSameParam);
	RW_VALUE(r, RecRoiNum);
	RW_VALUE(r, CirRoiNum);
	RW_VALUE(r, RecDeleteNum);
	RW_VALUE(r, CirDeleteNum);

	RW_VALUE(r, zoom_lf);
	RW_VALUE(r, zoom_up);
	RW_VALUE(r, zoom_rt);
	RW_VALUE(r, zoom_down);

	for (int i = 0;i < RecRoiNum + CirRoiNum;i++) {
		if (r) {
			RW_VALUE1(r, tmptype[i], i);
			testType[i] = (TestType)tmptype[i];
		}
		else {
			tmptype[i] = (int)testType[i];
			RW_VALUE1(r, tmptype[i], i);
		}
		RW_VALUE1(r, IsGauss[i], i);
		RW_VALUE1(r, GaussSize[i], i);
		RW_VALUE1(r, badLengthMin[i], i);
		RW_VALUE1(r, badLengthMax[i], i);
		RW_VALUE1(r, badWidthMin[i], i);
		RW_VALUE1(r, badWidthMax[i], i);
		RW_VALUE1(r, fixThreMin[i], i);
		RW_VALUE1(r, fixThreMax[i], i);
		RW_VALUE1(r, meanSize[i], i);
		RW_VALUE1(r, IsFillOut[i], i);

		RW_VALUE1(r, openVal[i], i);
		RW_VALUE1(r, closeVal[i], i);

		RW_VALUE1(r, holeAreaMax[i], i);
		RW_VALUE1(r, holeAreaMin[i], i);
		RW_VALUE1(r, holeNumMax[i], i);
		RW_VALUE1(r, holeNumMin[i], i);
		RW_VALUE1(r, minRag[i], i);
		RW_VALUE1(r, minLoss[i], i);
		RW_VALUE1(r, minRagLossArea[i], i);

		RW_VALUE1(r, badArea_min[i], i);
		RW_VALUE1(r, badArea_max[i], i);
		RW_VALUE1(r, black_bad_detect[i], i);
		RW_VALUE1(r, white_bad_detect[i], i);
		RW_VALUE1(r, single_badArea_valid[i], i);
		RW_VALUE1(r, single_badArea_min[i], i);
		RW_VALUE1(r, single_badArea_max[i], i);
		RW_VALUE1(r, total_badArea_valid[i], i);
		RW_VALUE1(r, total_badArea_min[i], i);
		RW_VALUE1(r, total_badArea_max[i], i);
		RW_VALUE1(r, area_width_valid[i], i);
		RW_VALUE1(r, area_width_min[i], i);
		RW_VALUE1(r, area_width_max[i], i);
		RW_VALUE1(r, area_length_valid[i], i);
		RW_VALUE1(r, area_length_min[i], i);
		RW_VALUE1(r, area_length_max[i], i);
		RW_VALUE1(r, area_count_valid[i], i);
		RW_VALUE1(r, area_count_min[i], i);
		RW_VALUE1(r, area_count_max[i], i);
	}
	
	RW_VALUE(r, currentRoi);
	for (int i = 0;i < 3;i++) {
		RW_VALUE1(r, deleteArea_section[i], i);
	}
	for (int i = 0;i < 3;i++) {
		RW_VALUE1(r, deleteArea_zoom[i], i);
	}
	for (int i = 0;i < RecDeleteNum;i++) {
		RW_VALUE1(r, DeleteRec[i].drow, i);
		RW_VALUE1(r, DeleteRec[i].dcol, i);
		RW_VALUE1(r, DeleteRec[i].dangle, i);
		RW_VALUE1(r, DeleteRec[i].dlength1, i);
		RW_VALUE1(r, DeleteRec[i].dlength2, i);
	}
	for (int i = 0;i < CirDeleteNum;i++) {
		RW_VALUE1(r, DeleteCir[i].drrow, i);
		RW_VALUE1(r, DeleteCir[i].drcol, i);
		RW_VALUE1(r, DeleteCir[i].dr, i);
	}
	for (int i = 0;i < RecRoiNum;i++) {
		RW_VALUE1(r, RecRoi[i].row, i);
		RW_VALUE1(r, RecRoi[i].col, i);
		RW_VALUE1(r, RecRoi[i].angle, i);
		RW_VALUE1(r, RecRoi[i].length1, i);
		RW_VALUE1(r, RecRoi[i].length2, i);
	}
	for (int i = 0;i < CirRoiNum;i++) {
		RW_VALUE1(r, CirRoi[i].rrow, i);
		RW_VALUE1(r, CirRoi[i].rcol, i);
		RW_VALUE1(r, CirRoi[i].r, i);
	}

	RW_VALUE(r, IsDispROI);
}

void surfaceMultParam::iniData()
{
	multAngle = 1;
	rotateRow1 = 100;
	rotateCol1 = 100;
	rotateRow2 = 300;
	rotateCol2 = 300;

	DetectModel = 0;
	IsChangeModel = 0;
	deleteRefer = 0;
	referSection = 0;
	pointRegionRefer = 0;
	areaRegionRefer = 0;
	areaZoom = 0;

	IsSameParam = 1;
	IsDirPattern = 0;
	IsDirDelete = 1;
	IsReferPtoDel = 0;
	delPointZoom = 0;

	IsPixOrAct = 0;
	pixSize = 1.0;

	deleteArea_section[0] = deleteArea_section[1] = deleteArea_section[2] = 0;
	deleteArea_zoom[0] = deleteArea_zoom[1] = deleteArea_zoom[2] = 0;
	isShowDel = 0;
	zoom_up = zoom_lf = zoom_down = zoom_rt = 0;
	for (int i = 0;i < MAX_Rec_Num + MAX_Cir_Num;i++) {
		testType[i] = averageThre;
		tmptype[i] = 0;

		badArea_min[i] = 1;
		badArea_max[i] = 9999999;
		black_bad_detect[i] = 255;
		white_bad_detect[i] = 255;

		badLengthMin[i] = 1;
		badLengthMax[i] = 999;
		badWidthMax[i] = 999;
		badWidthMin[i] = 1;
		fixThreMin[i] = 0;
		fixThreMax[i] = 255;
		meanSize[i] = 5;
		IsFillOut[i] = 0;
		holeAreaMax[i] = 999;
		holeAreaMin[i] = 0;
		holeNumMin[i] = 0;
		holeNumMax[i] = 10;
		IsGauss[i] = 0;
		GaussSize[i] = 3;
		minRag[i] = 0;
		minLoss[i] = 0;
		minRagLossArea[i] = 0;
		openVal[i] = 0;
		closeVal[i] = 0;

		single_badArea_valid[i] = 0;
		single_badArea_min[i] = 0;
		single_badArea_max[i] = 0;
		total_badArea_valid[i] = total_badArea_min[i] = total_badArea_max[i] = 0;
		area_width_valid[i] = area_width_min[i] = area_width_max[i] = 0;
		area_length_valid[i] = area_length_min[i] = area_length_max[i] = 0;
		area_count_valid[i] = 0;
		area_count_min[i] = 0;
		area_count_max[i] = 5;
	}
	RecRoiNum = 1;
	CirRoiNum = 0;
	RecDeleteNum = 0;
	CirDeleteNum = 0;
	currentRoi = 0;
	for (int i = 0;i < MAX_Delete_Rec_Num;i++) {
		DeleteRec[i].drow = 100;
		DeleteRec[i].dcol = 100;
		DeleteRec[i].dangle = 0;
		DeleteRec[i].dlength1 = 30;
		DeleteRec[i].dlength2 = 30;
	}
	for (int i = 0;i < MAX_Delete_Cir_Num;i++) {
		DeleteCir[i].drrow = 150;
		DeleteCir[i].drcol = 150;
		DeleteCir[i].dr = 40;
	}
	for (int i = 0;i < MAX_Rec_Num;i++) {
		RecRoi[i].row = 100;
		RecRoi[i].col = 100;
		RecRoi[i].angle = 0;
		RecRoi[i].length1 = 35;
		RecRoi[i].length2 = 35;
	}
	for (int i = 0;i < MAX_Cir_Num;i++) {
		CirRoi[i].rrow = 150;
		CirRoi[i].rcol = 150;
		CirRoi[i].r = 50;
	}

	IsDispROI = 0;
}

