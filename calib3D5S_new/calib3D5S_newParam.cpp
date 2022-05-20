#include "calib3D5S_newParam.h"
#include <iniParam.h>
#include "qvglobal.h"
#include "cv.h"

calib3D5S_newParam::calib3D5S_newParam()
{
	iniData();
}

calib3D5S_newParam::~calib3D5S_newParam()
{

}

void calib3D5S_newParam::readWriteParam(bool r, const QString& dirPath, int level)
{
	SETTING_INIT(dirPath + "/calib3D5S_new.ini");

	if (r&&level != MachineLevel)
		return;

	for (int i = 0;i < 5;i++) {
		RW_VALUE1(r, imgDist[i], i);
		RW_VALUE1(r, pixelSize[i], i);
		RW_VALUE1(r, actualSize[i], i);
		RW_VALUE1(r, sizeRow1[i], i);
		RW_VALUE1(r, sizeCol1[i], i);
		RW_VALUE1(r, sizeRow2[i], i);
		RW_VALUE1(r, sizeCol2[i], i);
	}
	for (int i = 0;i < 4;i++) {
		RW_VALUE1(r, standH[i], i);
		RW_VALUE1(r, prjRatio[i], i);
		RW_VALUE1(r, actualDepth[i], i);
		RW_VALUE1(r, depthRow1[i], i);
		RW_VALUE1(r, depthCol1[i], i);
		RW_VALUE1(r, depthRow2[i], i);
		RW_VALUE1(r, depthCol2[i], i);
		RW_VALUE1(r, standSize[i], i);
		RW_VALUE1(r, standOffset[i], i);
	}
	for (int i = 0;i < 4;i++) {
		RW_VALUE1(r, angle[i], i);
		RW_VALUE1(r, lineRow1[i], i);
		RW_VALUE1(r, lineCol1[i], i);
		RW_VALUE1(r, lineRow2[i], i);
		RW_VALUE1(r, lineCol2[i], i);
		RW_VALUE1(r, standRow1[i], i);
		RW_VALUE1(r, standCol1[i], i);
		RW_VALUE1(r, standRow2[i], i);
		RW_VALUE1(r, standCol2[i], i);
	}

	RW_VALUE(r, horizontalHorVerticalH);
}

void calib3D5S_newParam::iniData()
{
	for (int i = 0;i < 5;i++) {
		imgDist[i] = 0;
		pixelSize[i] = 0.01;
		actualSize[i] = 2;
		sizeRow1[i] = 50;
		sizeCol1[i] = 50;
		sizeRow2[i] = 120;
		sizeCol2[i] = 100;
	}
	for (int i = 0;i < 4;i++) {
		if (i % 2 == 0) {
			standH[i] = 0.30;
		}
		else {
			standH[i] = 0.15;
		}
		actualDepth[i] = 2;
		prjRatio[i] = 0;
		depthRow1[i] = 50;
		depthCol1[i] = 50;
		depthRow2[i] = 120;
		depthCol2[i] = 100;
		standSize[i] = 0;
		standOffset[i] = 0;
	}
	for (int i = 0;i < 4;i++) {
		angle[i] = i < 2 ? CV_PI / 2 : 0;
		lineRow1[i] = 50;
		lineCol1[i] = 50;
		lineRow2[i] = 120;
		lineCol2[i] = 100;
		standRow1[i] = 50;
		standCol1[i] = 50;
		standRow2[i] = 120;
		standCol2[i] = 100;

	}
	horizontalHorVerticalH = 1;
}

