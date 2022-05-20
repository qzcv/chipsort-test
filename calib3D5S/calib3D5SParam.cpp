#include "calib3D5SParam.h"
#include <iniParam.h>
#include "qvglobal.h"
#include "HalconCpp.h"

using namespace Halcon;

calib3D5SParam::calib3D5SParam()
{
	iniData();
}

calib3D5SParam::~calib3D5SParam()
{

}

void calib3D5SParam::readWriteParam(bool r, const QString& dirPath, int level)
{
	SETTING_INIT(dirPath + "/calib3D5S.ini");

	if (!r || (r&&level == MachineLevel))
	{
		for (int i = 0;i < 5;i++) {
			RW_VALUE1(r, pixelSize[i], i);
		}
		for (int i = 0;i < 4;i++) {
			RW_VALUE1(r, prjRatio[i], i);
			RW_VALUE1(r, actualDepth[i], i);
		}
		for (int i = 0;i < 4;i++) {
			RW_VALUE1(r, angle[i], i);
		}
	}
	if (!r || (r&&level == ProductLevel))
	{
		for (int i = 0;i < 5;i++) {
			RW_VALUE1(r, actualSize[i], i);
			RW_VALUE1(r, sizeRow1[i], i);
			RW_VALUE1(r, sizeCol1[i], i);
			RW_VALUE1(r, sizeRow2[i], i);
			RW_VALUE1(r, sizeCol2[i], i);
		}
		for (int i = 0;i < 4;i++) {
			RW_VALUE1(r, depthRow1[i], i);
			RW_VALUE1(r, depthCol1[i], i);
			RW_VALUE1(r, depthRow2[i], i);
			RW_VALUE1(r, depthCol2[i], i);
		}
		for (int i = 0;i < 4;i++) {
			RW_VALUE1(r, lineRow1[i], i);
			RW_VALUE1(r, lineCol1[i], i);
			RW_VALUE1(r, lineRow2[i], i);
			RW_VALUE1(r, lineCol2[i], i);

			RW_VALUE1(r, prjOffset[i], i);
			RW_VALUE1(r, prjActualOffset[i], i);
		}
		RW_VALUE(r, horizontalHorVerticalH);
	}	
}

void calib3D5SParam::iniData()
{
	for (int i = 0;i < 5;i++) {
		pixelSize[i] = 0.01;
		actualSize[i] = 2;
		sizeRow1[i] = 50;
		sizeCol1[i] = 50;
		sizeRow2[i] = 120;
		sizeCol2[i] = 100;
	}
	for (int i = 0;i < 4;i++) {
		actualDepth[i] = 2;
		prjRatio[i] = 0;
		depthRow1[i] = 50;
		depthCol1[i] = 50;
		depthRow2[i] = 120;
		depthCol2[i] = 100;
	}
	for (int i = 0;i < 4;i++) {
		angle[i] = i < 2 ? M_PI / 2 : 0;
		lineRow1[i] = 50;
		lineCol1[i] = 50;
		lineRow2[i] = 120;
		lineCol2[i] = 100;

		prjOffset[i] = 100;
		prjActualOffset[i] = 100;
	}
	horizontalHorVerticalH = 1;
}

