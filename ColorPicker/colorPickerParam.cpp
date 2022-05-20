#include "colorPickerParam.h"
#include <iniParam.h>
#include "qvglobal.h"

ColorPickerParam::ColorPickerParam()
{
	iniData();
}

ColorPickerParam::~ColorPickerParam()
{

}

void ColorPickerParam::readWriteParam(bool r, const QString& dirPath, int level)
{
	SETTING_INIT(dirPath + "/ColorPicker.ini");

	if (r&&level != ProductLevel)
		return;

	RW_VALUE(r, IsClosing);
	RW_VALUE(r, closeVal);
	RW_VALUE(r, IsOpening);
	RW_VALUE(r, openVal);
	RW_VALUE(r, referType);
	RW_VALUE(r, AreaZoom);
	RW_VALUE(r, IsShowRegionAfterDetect);
	RW_VALUE(r, IsShowAllRegion);
	RW_VALUE(r, IsReferSection);
	RW_VALUE(r, IsDeleteLightArea);
	RW_VALUE(r, IsFillUpHole);
	RW_VALUE(r, IsWidth);
	RW_VALUE(r, IsHeight);
	RW_VALUE(r, heightMin);
	RW_VALUE(r, heightMax);
	RW_VALUE(r, widthMin);
	RW_VALUE(r, widthMax);
	RW_VALUE(r, lightAreaThreshold);
	RW_VALUE(r, lightArea);
	RW_VALUE(r, SaturationMin);
	RW_VALUE(r, SaturationMax);
	RW_VALUE(r, HueMin);
	RW_VALUE(r, HueMax);
	RW_VALUE(r, IsNotLess);
	RW_VALUE(r, nowIsRecOrCir);
	RW_VALUE(r, roiCount);
	RW_VALUE(r, MinusCount);
	RW_VALUE(r, currentRoiIndex);
	RW_VALUE(r, currentMinusIndex);
	RW_VALUE(r, areaMin);
	RW_VALUE(r, areaMax);

	RW_VALUE(r, IsErrAllArea);
	RW_VALUE(r, errAllMin);
	RW_VALUE(r, errAllMax);
	RW_VALUE(r, IsErrSingleArea);
	RW_VALUE(r, errSingleMin);
	RW_VALUE(r, errSingleMax);

	ColorPickerParam::segRegion seg;
	seg.regionType = 0;
	seg.RecRow = seg.RecCol = 100;
	seg.RecLength1 = seg.RecLength2 = 40;
	seg.RecAngle = 0;
	seg.CirCol = seg.CirRow = 100;
	seg.CirR = 40;
	for (int i = 0;i < roiCount;i++) {
		if (r&&i > 0) {
			seg.RecRow += 20;
			seg.RecCol += 20;
			seg.CirRow += 20;
			seg.CirCol += 20;
			RoiVec.push_back(seg);
		}
		RW_VALUE1(r, RoiVec[i].regionType, i);
		RW_VALUE1(r, RoiVec[i].RecRow, i);
		RW_VALUE1(r, RoiVec[i].RecCol, i);
		RW_VALUE1(r, RoiVec[i].RecLength1, i);
		RW_VALUE1(r, RoiVec[i].RecLength2, i);
		RW_VALUE1(r, RoiVec[i].RecAngle, i);
		RW_VALUE1(r, RoiVec[i].CirRow, i);
		RW_VALUE1(r, RoiVec[i].CirCol, i);
		RW_VALUE1(r, RoiVec[i].CirR, i);
	}
	if (MinusCount > 0) {
		seg.RecRow = seg.RecCol = 200;
		seg.CirCol = seg.CirRow = 200;
		for (int i = 0;i < MinusCount;i++) {
			if (r) {
				seg.RecRow += 20;
				seg.RecCol += 20;
				seg.CirRow += 20;
				seg.CirCol += 20;
				MinusVec.push_back(seg);
			}
			RW_VALUE1(r, MinusVec[i].regionType, i);
			RW_VALUE1(r, MinusVec[i].RecRow, i);
			RW_VALUE1(r, MinusVec[i].RecCol, i);
			RW_VALUE1(r, MinusVec[i].RecLength1, i);
			RW_VALUE1(r, MinusVec[i].RecLength2, i);
			RW_VALUE1(r, MinusVec[i].RecAngle, i);
			RW_VALUE1(r, MinusVec[i].CirRow, i);
			RW_VALUE1(r, MinusVec[i].CirCol, i);
			RW_VALUE1(r, MinusVec[i].CirR, i);
		}
	}
}

void ColorPickerParam::iniData()
{
	referType = 0;
	AreaZoom = 0;
	IsShowRegionAfterDetect = 0;
	IsShowAllRegion = 0;
	IsReferSection = 0;
	IsDeleteLightArea = 0;
	IsFillUpHole = 1;
	lightAreaThreshold = 255;
	lightArea = 0;
	SaturationMin = 0;
	SaturationMax = 255;
	HueMin = 0;
	HueMax = 255;
	IsOpening = false;
	openVal = 1;
	IsClosing = 0;
	closeVal = 1;

	IsWidth = 0;
	widthMin = 0;
	widthMax = 9999;
	IsHeight = 0;
	heightMin = 0;
	heightMax = 9999;

	IsNotLess = 0;
	areaMin = 0;
	areaMax = 9999;
	roiCount = 1;
	MinusCount = 0;
	currentRoiIndex = 0;
	currentMinusIndex = 0;
	IsErrAllArea = 0;
	errAllMin = 0;
	errAllMax = 9999;
	IsErrSingleArea = 0;
	errSingleMin = 0;
	errSingleMax = 9999;
	nowIsRecOrCir = 0;
	ColorPickerParam::segRegion seg;
	seg.regionType = 0;
	seg.RecRow = seg.RecCol = 100;
	seg.RecLength1 = seg.RecLength2 = 40;
	seg.RecAngle = 0;
	seg.CirCol = seg.CirRow = 100;
	seg.CirR = 40;
	RoiVec.clear();
	RoiVec.push_back(seg);
	MinusVec.clear();
}

