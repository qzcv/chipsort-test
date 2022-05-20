#include "spiltImage_preParam.h"
#include <iniParam.h>
#include "qvglobal.h"

spiltImage_preParam::spiltImage_preParam()
{
	iniData();
}

spiltImage_preParam::~spiltImage_preParam()
{

}

void spiltImage_preParam::readWriteParam(bool r, const QString& dirPath, int level)
{
	SETTING_INIT(dirPath + "/spiltImage_pre.ini");

	if (r&&level != ProductLevel)
		return;

	RW_VALUE(r, moduleValid);
	RW_VALUE(r, RoiValid);
	RW_VALUE(r, RoiCounts);
	if (r) {
		segRegionVector.clear();
		segRegion seg;
		seg.row1 = seg.col1 = 40;
		seg.row2 = seg.col2 = 90;
		for (int i = 0;i < RoiCounts;i++) {
			seg.row1 += 10;
			seg.row2 += 10;
			seg.col1 += 10;
			seg.col2 += 10;
			segRegionVector.push_back(seg);
		}
	}
	for (int i = 0;i < RoiCounts;i++) {
		RW_VALUE1(r, segRegionVector[i].row1, i);
		RW_VALUE1(r, segRegionVector[i].col1, i);
		RW_VALUE1(r, segRegionVector[i].row2, i);
		RW_VALUE1(r, segRegionVector[i].col2, i);
	}
}

void spiltImage_preParam::iniData()
{
	moduleValid = 0;
	RoiValid = 0;
	RoiCounts = 0;
	segRegionVector.clear();
	segRegion seg;
	seg.row1 = seg.col1 = 40;
	seg.row2 = seg.col2 = 90;
	for (int i = 0;i < RoiCounts;i++) {
		seg.row1 += 10;
		seg.row2 += 10;
		seg.col1 += 10;
		seg.col2 += 10;
		segRegionVector.push_back(seg);
	}
}

