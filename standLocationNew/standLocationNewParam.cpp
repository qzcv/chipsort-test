#include "standLocationNewParam.h"
#include <iniParam.h>
#include "qvglobal.h"

standLocationNewParam::standLocationNewParam()
{
	iniData();
}

standLocationNewParam::~standLocationNewParam()
{

}

void standLocationNewParam::readWriteParam(bool r, const QString& dirPath, int level)
{
	SETTING_INIT(dirPath + "/standLocationNew.ini");

	if (r&&level != ProductLevel)
		return;

	RW_VALUE(r, IsLMSValid);
	RW_VALUE(r, LMSFactor);
	RW_VALUE(r, packageOff);
	RW_VALUE(r, NowGrp);
	if (r&&NowGrp >= 4) {
		NowGrp = 3;
	}
	RW_VALUE(r, upDwOrLfRt);
	RW_VALUE(r, UpDetect);
	RW_VALUE(r, DwDetect);
	RW_VALUE(r, LfDetect);
	RW_VALUE(r, RtDetect);
	for (int i = 0; i < 2; i++) {
		RW_VALUE1(r, grpPinNum[i], i);
	}
	for (int i = 0; i < 4; i++)
	{
		RW_VALUE1(r, extendSearchLine[i], i);
		RW_VALUE1(r, minGrayDiff[i], i);
		RW_VALUE1(r, minArea[i], i);
		RW_VALUE1(r, standoffOffset[i], i);
		RW_VALUE1(r, grayVal[i], i);
	}
	RW_VALUE(r, errCoplanarMin);
	RW_VALUE(r, errCoplanar);
	RW_VALUE(r, errCoplanarValid);

	//zhangao
	RW_VALUE(r, errPinStandOffValid);
	RW_VALUE(r, errPinStandOffDiffValid);

	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < grpPinNum[i]; j++) {
			RW_VALUE2(r, errPinStandOffMin[i][j], i, j);
			RW_VALUE2(r, errPinStandOffMax[i][j], i, j);

			RW_VALUE2(r, standOffCorr[i][j], i, j);
			RW_VALUE2(r, offStandOffCorr[i][j], i, j);
		}
		RW_VALUE1(r, errPinStandOffDiffMin[i], i);
		RW_VALUE1(r, errPinStandOffDiffMax[i], i);
		RW_VALUE1(r, selfStandOffCorr[i], i);
	}
	//RoiRegion seg;
	//seg.row1=seg.col1=40;
	//seg.row2=seg.col2=90;
	for (int i = 0; i < 5; i++)
	{
		if (r) {
			RW_VALUE1(r, RoiRegionVector[i].row1, i);
			RW_VALUE1(r, RoiRegionVector[i].col1, i);
			RW_VALUE1(r, RoiRegionVector[i].row2, i);
			RW_VALUE1(r, RoiRegionVector[i].col2, i);
		}
		else {
			RW_VALUE1(r, RoiRegionVector[i].row1, i);
			RW_VALUE1(r, RoiRegionVector[i].col1, i);
			RW_VALUE1(r, RoiRegionVector[i].row2, i);
			RW_VALUE1(r, RoiRegionVector[i].col2, i);
		}
	}
}

void standLocationNewParam::iniData()
{
	IsLMSValid = 0;
	LMSFactor = 2.5;
	packageOff = 0.11;
	NowGrp = 0;
	upDwOrLfRt = 1;
	UpDetect = DwDetect = LfDetect = RtDetect = 1;

	for (int i = 0; i < 4; i++)
	{
		extendSearchLine[i] = 3;
		minGrayDiff[i] = 30;
		minArea[i] = 50;
		standoffOffset[i] = 0;
		grayVal[i] = 100;
	}
	grpPinNum[0] = grpPinNum[1] = 3;

	//zhangao
	errPinStandOffValid = 0;
	errPinStandOffDiffValid = 0;

	errCoplanarValid = 0;
	errCoplanarMin = 0;
	errCoplanar = 0;
	for (int i = 0; i < 2; i++) {
		selfStandOffCorr[i] = 0;
		for (int j = 0; j < GRP_MAX_LEADNUM; j++) {
			errPinStandOffMin[i][j] = 0;
			errPinStandOffMax[i][j] = 10;

			standOffCorr[i][j] = 0;    //站立度修正
			offStandOffCorr[i][j] = 0;    //站立度修正
		}
		errPinStandOffDiffMin[i] = 0;
		errPinStandOffDiffMax[i] = 0;
	}
	RoiRegionVector.clear();
	RoiRegion seg;
	seg.row1 = seg.col1 = 40;
	seg.row2 = seg.col2 = 90;
	for (int i = 0; i < 5; i++) {
		seg.row1 += 10;
		seg.row2 += 10;
		seg.col1 += 10;
		seg.col2 += 10;
		RoiRegionVector.push_back(seg);
	}
}

