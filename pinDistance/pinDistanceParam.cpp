#include "pinDistanceParam.h"
#include <iniParam.h>
#include "qvglobal.h"

pinDistanceParam::pinDistanceParam()
{
	iniData();
}

pinDistanceParam::~pinDistanceParam()
{

}

void pinDistanceParam::readWriteParam(bool r, const QString& dirPath, int level)
{
	SETTING_INIT(dirPath + "/pinDistance.ini");

	if (r&&level != ProductLevel)
		return;

	RW_VALUE(r, currentGrp);
	RW_VALUE(r, IsRefer);
	for (int i = 0;i < GRP_NUM;i++)
	{
		RW_VALUE1(r, edgeOffset[i], i);
		RW_VALUE1(r, grpEnable[i], i);
		RW_VALUE1(r, grpPinNum[i], i);
		RW_VALUE1(r, sizeRatio[i], i);
		RW_VALUE1(r, edgeThreshold[i], i);
		RW_VALUE1(r, pointsNum[i], i);
		RW_VALUE1(r, pointsInter[i], i);

		//zhangao
		RW_VALUE1(r, errDistanceMin[i], i);
		RW_VALUE1(r, errDistanceMax[i], i);
	}
	RW_VALUE(r, errDistanceValid);
	RoiRegion seg;
	seg.row1 = seg.col1 = 40;
	seg.row2 = seg.col2 = 70;
	for (int i = 0;i < GRP_NUM;i++)
	{
		if (r) {
			RoiVec[i].clear();
			for (int j = 0;j < grpPinNum[i];j++) {
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
			for (int j = 0;j < grpPinNum[i];j++) {
				RW_VALUE2(r, RoiVec[i][j].row1, i, j);
				RW_VALUE2(r, RoiVec[i][j].col1, i, j);
				RW_VALUE2(r, RoiVec[i][j].row2, i, j);
				RW_VALUE2(r, RoiVec[i][j].col2, i, j);
			}
		}
	}
}

void pinDistanceParam::iniData()
{
	currentGrp = 0;
	IsRefer = 0;
	errDistanceValid = 1;
	for (int i = 0;i < GRP_NUM;i++) {
		edgeOffset[i] = 0;
		grpEnable[i] = 1;
		grpPinNum[i] = 2;
		sizeRatio[i] = 1.0;
		edgeThreshold[i] = 50;
		pointsNum[i] = 3;
		pointsInter[i] = 3;
		//zhangao

		errDistanceMin[i] = 0;
		errDistanceMax[i] = 0;
	}
	RoiRegion seg;
	seg.row1 = seg.col1 = 40;
	seg.row2 = seg.col2 = 70;
	for (int i = 0;i < GRP_NUM;i++) {
		RoiVec[i].clear();
		for (int j = 0;j < grpPinNum[i];j++) {
			seg.row1 += 10;
			seg.row2 += 10;
			seg.col1 += 10;
			seg.col2 += 10;
			RoiVec[i].push_back(seg);
		}
	}
}

