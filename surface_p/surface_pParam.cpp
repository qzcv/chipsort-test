#include "surface_pParam.h"
#include <iniParam.h>
#include <HalconCpp.h>
#include "qvglobal.h"

surface_pParam::surface_pParam()
{
	iniData();
}

surface_pParam::~surface_pParam()
{

}

void surface_pParam::readWriteParam(bool r, const QString& dirPath, int level)
{
	SETTING_INIT(dirPath + "/surface_p.ini");

	if (r&&level != ProductLevel)
		return;

	if (r) {
		RW_VALUE(r, tmptype);
		testType = (TestType)tmptype;
	}
	else {
		tmptype = (int)testType;
		RW_VALUE(r, tmptype);
	}
	RW_VALUE(r, referSection);

	RW_VALUE(r, roiMinArea);
	RW_VALUE(r, roiThreMin);
	RW_VALUE(r, roiMaxArea);
	RW_VALUE(r, roiThreMax);

	RW_VALUE(r, roundVal);
	RW_VALUE(r, roiType);
	RW_VALUE(r, IsFitRec);
	RW_VALUE(r, inputAreaZoom);
	RW_VALUE(r, cirRow);
	RW_VALUE(r, cirCol);
	RW_VALUE(r, cirR);

	RW_VALUE(r, zoom_lf);
	RW_VALUE(r, zoom_up);
	RW_VALUE(r, zoom_rt);
	RW_VALUE(r, zoom_dw);

	RW_VALUE(r, isShowDel);

	RW_VALUE(r, IsReferPtoDel);
	RW_VALUE(r, delPointZoom);
	RW_VALUE(r, pixSize);
	RW_VALUE(r, IsPixOrAct);

	RW_VALUE(r, IsGauss);
	RW_VALUE(r, GaussSize);
	RW_VALUE(r, badLengthMin);
	RW_VALUE(r, badLengthMax);
	RW_VALUE(r, badWidthMin);
	RW_VALUE(r, badWidthMax);

	RW_VALUE(r, fixThreMin);
	RW_VALUE(r, fixThreMax);

	RW_VALUE(r, meanSize);
	RW_VALUE(r, IsFillOut);
	RW_VALUE(r, holeAreaMax);
	RW_VALUE(r, holeAreaMin);
	RW_VALUE(r, holeNumMax);
	RW_VALUE(r, holeNumMin);
	RW_VALUE(r, minRagLossArea);
	RW_VALUE(r, minRag);
	RW_VALUE(r, minLoss);

	RW_VALUE(r, openVal);
	RW_VALUE(r, closeVal);

	for (int i = 0;i < 3;i++) {
		RW_VALUE1(r, deleteArea_section[i], i);
	}
	for (int i = 0;i < 3;i++) {
		RW_VALUE1(r, deleteArea_zoom[i], i);
	}
	RW_VALUE(r, addDeleteArea);
	RW_VALUE(r, badArea_min);
	RW_VALUE(r, badArea_max);
	RW_VALUE(r, black_bad_detect);
	RW_VALUE(r, white_bad_detect);
	RW_VALUE(r, single_badArea_valid);
	RW_VALUE(r, single_badArea_min);
	RW_VALUE(r, single_badArea_max);
	RW_VALUE(r, total_badArea_valid);
	RW_VALUE(r, total_badArea_min);
	RW_VALUE(r, total_badArea_max);
	RW_VALUE(r, area_width_valid);
	RW_VALUE(r, area_width_min);
	RW_VALUE(r, area_width_max);
	RW_VALUE(r, area_length_valid);
	RW_VALUE(r, area_length_min);
	RW_VALUE(r, area_length_max);
	RW_VALUE(r, area_count_valid);
	RW_VALUE(r, area_count_min);
	RW_VALUE(r, area_count_max);
	deleteRegion seg2;
	//seg1.row1=50;
	//seg1.col1=50;
	//seg1.row2=200;
	//seg1.col2=200;
	seg2.row = seg2.col = 110;
	seg2.length1 = seg2.length2 = 20;
	seg2.angle = 0;
	for (int i = 0;i < addDeleteArea + 1;i++) {
		if (r) {			//read
			if (i > 0) {
				seg2.row += 10;
				seg2.col += 10;
				deleteRegionVector.push_back(seg2);
			}
			RW_VALUE1(r, deleteRegionVector[i].row, i);
			RW_VALUE1(r, deleteRegionVector[i].col, i);
			RW_VALUE1(r, deleteRegionVector[i].length1, i);
			RW_VALUE1(r, deleteRegionVector[i].length2, i);
			RW_VALUE1(r, deleteRegionVector[i].angle, i);
		}
		else {                            //write
			RW_VALUE1(r, deleteRegionVector[i].row, i);
			RW_VALUE1(r, deleteRegionVector[i].col, i);
			RW_VALUE1(r, deleteRegionVector[i].length1, i);
			RW_VALUE1(r, deleteRegionVector[i].length2, i);
			RW_VALUE1(r, deleteRegionVector[i].angle, i);
		}
	}

	for (int i = 0;i < addDeleteArea;i++) {
		if (r) {			//read
			deleteRegion seg;
			seg.row = seg.row = seg.length1 = seg.length2 = 10;
			seg.angle = 0;
			deleteRegionVector.push_back(seg);
			RW_VALUE1(r, deleteRegionVector[i].row, i);
			RW_VALUE1(r, deleteRegionVector[i].col, i);
			RW_VALUE1(r, deleteRegionVector[i].length1, i);
			RW_VALUE1(r, deleteRegionVector[i].length2, i);
			RW_VALUE1(r, deleteRegionVector[i].angle, i);
		}
		else {                            //write
			RW_VALUE1(r, deleteRegionVector[i].row, i);
			RW_VALUE1(r, deleteRegionVector[i].col, i);
			RW_VALUE1(r, deleteRegionVector[i].length1, i);
			RW_VALUE1(r, deleteRegionVector[i].length2, i);
			RW_VALUE1(r, deleteRegionVector[i].angle, i);
		}
	}
	char imgPath[256];
	sprintf(imgPath, "%s/surfaceROi.reg", dirPath.toLocal8Bit().constData());
	if (r) {
		set_check("~give_error");
		read_region(&roiRegion, imgPath);
		set_check("give_error");
	}
	else {
		set_check("~give_error");
		write_region(roiRegion, imgPath);
		set_check("give_error");
	}

	RW_VALUE(r, IsDispROI);
}

void surface_pParam::iniData()
{
	roundVal = 5;
	IsFitRec = 0;
	IsFillOut = 0;
	testType = averageThre;
	tmptype = 0;
	roiMinArea = 10;
	roiThreMin = 0;
	roiMaxArea = 999999;
	roiThreMax = 255;
	IsPixOrAct = 0;
	referSection = 0;
	inputAreaZoom = 0;
	zoom_up = zoom_lf = 0;
	zoom_dw = zoom_rt = 0;
	cirRow = cirCol = 50;
	cirR = 50;
	roiType = 0;
	pixSize = 1.0;

	IsReferPtoDel = 0;
	delPointZoom = 0;

	deleteArea_section[0] = deleteArea_section[1] = deleteArea_section[2] = 0;
	deleteArea_zoom[0] = deleteArea_zoom[1] = deleteArea_zoom[2] = 0;
	addDeleteArea = 0;

	badArea_min = 0;
	badArea_max = 100000;
	black_bad_detect = 255;
	white_bad_detect = 255;
	isShowDel = 0;

	badLengthMin = 0;
	badLengthMax = 999;
	badWidthMax = 999;
	badWidthMin = 0;
	fixThreMin = 0;
	fixThreMax = 255;
	meanSize = 5;
	holeAreaMax = 999;
	holeAreaMin = 0;
	holeNumMin = 0;
	holeNumMax = 10;
	IsGauss = 0;
	GaussSize = 3;
	openVal = 0;
	closeVal = 0;

	single_badArea_valid = 0;
	single_badArea_min = 0;
	single_badArea_max = 0;
	total_badArea_valid = total_badArea_min = total_badArea_max = 0;
	area_width_valid = area_width_min = area_width_max = 0;
	area_length_valid = area_length_min = area_length_max = 0;
	area_count_valid = 0;
	area_count_min = 0;
	area_count_max = 5;
	deleteRegionVector.clear();
	deleteRegion seg1;
	seg1.row = 130;
	seg1.col = 130;
	seg1.angle = 0;
	seg1.length1 = 80;
	seg1.length2 = 80;
	deleteRegionVector.push_back(seg1);

	minRag = 0;
	minLoss = 0;
	minRagLossArea = 0;

	IsDispROI = 0;
}
