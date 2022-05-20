#include "mark1Param.h"
#include <iniParam.h>
#include "qvglobal.h"

mark1Param::mark1Param()
{
	iniData();
}

mark1Param::~mark1Param()
{

}

void mark1Param::readWriteParam(bool r, const QString& dirPath, int level)
{
	SETTING_INIT(dirPath + "/mark1.ini");

	if (!r || (r&&level == ProductLevel))
	{
		QString tmp;
		if (r)//read
		{
			RW_VALUE(r, tmp);
			strcpy(AutoSaveTmpImgDir, tmp.toUtf8().constData());
		}
		else
		{
			tmp = AutoSaveTmpImgDir;
			RW_VALUE(r, tmp);
		}
		//RW_VALUE(r, AutoSaveTmpImgDir);
		RW_VALUE(r, OcrValid);
		RW_VALUE(r, errCrack);
		RW_VALUE(r, errCrackValid);
		RW_VALUE(r, errExcess);
		RW_VALUE(r, errExcessValid);
		RW_VALUE(r, IsInputRatio);
		RW_VALUE(r, pixelSize);

		RW_VALUE(r, IsReferTempAngle);
		RW_VALUE(r, HorOrVer);
		RW_VALUE(r, charGap);

		RW_VALUE(r, IsDefaultSig);
		RW_VALUE(r, IsInverse);
		RW_VALUE(r, Is2Dor4D);

		RW_VALUE(r, IsInputPatIndex);
		RW_VALUE(r, IsUsePinDirOut);
		RW_VALUE(r, isShowAngle);

		RW_VALUE(r, angleType);
		RW_VALUE(r, IsDirPattern);
		RW_VALUE(r, IsReferSection);
		RW_VALUE(r, IsMult);
		RW_VALUE(r, IsFontOrLayer);
		RW_VALUE(r, IsFillUp);
		RW_VALUE(r, NowPatternNum);
		RW_VALUE(r, autoRegionLarge);

		RW_VALUE(r, charThreMin);
		RW_VALUE(r, charThreMax);

		RW_VALUE(r, autoSegment);
		RW_VALUE(r, whiteChar);
		RW_VALUE(r, threshold);
		RW_VALUE(r, minArea);
		RW_VALUE(r, minHoles);
		RW_VALUE(r, CharDefectTOL);

		RW_VALUE(r, charOffsetValid);
		RW_VALUE(r, charOffset);
		RW_VALUE(r, searchAngleValid);
		RW_VALUE(r, searchAngle);
		RW_VALUE(r, acceptSimilar);
		RW_VALUE(r, errAngleValid);
		RW_VALUE(r, errAngle);
		RW_VALUE(r, errOffSetValid);
		RW_VALUE(r, errOffSet);

		RW_VALUE(r, errOffXMax);
		RW_VALUE(r, errOffXMin);
		RW_VALUE(r, errOffYMax);
		RW_VALUE(r, errOffYMin);
		RW_VALUE(r, errOffXValid);
		RW_VALUE(r, errOffYValid);

		RW_VALUE(r, errAreaPercent);
		RW_VALUE(r, errAreaValid);

		RW_VALUE(r, areaType);

		RW_VALUE(r, PatternNum);

		for (int i = 0;i < PatternNum;i++) {
			RW_VALUE1(r, charCounts[i], i);
			RW_VALUE1(r, RegionRoi[i].row1, i);
			RW_VALUE1(r, RegionRoi[i].col1, i);
			RW_VALUE1(r, RegionRoi[i].row2, i);
			RW_VALUE1(r, RegionRoi[i].col2, i);
		}

		for (int i = 0;i < PatternNum;i++) {
			RW_VALUE1(r, MultPatternEnable[i], i);
			if (!r && !MultPatternEnable[i]) {
				char imgPath[128];
				sprintf(imgPath, "%s/SegImg%d.bmp", dirPath.toUtf8().constData(), i);

				set_check("~give_error");
				delete_file(imgPath);
				set_check("give_error");
				gen_empty_obj(&SegImg[i]);
			}
		}

		segRegion seg2;
		seg2.row1 = seg2.col1 = 100;
		seg2.row2 = seg2.col2 = 150;
		for (int j = 0;j < PatternNum;j++) {
			for (int i = 0;i < charCounts[j];i++) {
				if (r) {			//read
					seg2.row1 += 10;
					seg2.row2 += 10;
					seg2.col1 += 10;
					seg2.col2 += 10;
					segRegionVector[j].push_back(seg2);
				}
				RW_VALUE2(r, segRegionVector[j][i].row1, j, i);
				RW_VALUE2(r, segRegionVector[j][i].col1, j, i);
				RW_VALUE2(r, segRegionVector[j][i].row2, j, i);
				RW_VALUE2(r, segRegionVector[j][i].col2, j, i);
			}
		}
		char imgPath[256];
		for (int i = 0;i < PatternNum;i++) {
			sprintf(imgPath, "%s/SegImg%d.bmp", dirPath.toUtf8().constData(), i);
			if (r) {
				set_check("~give_error");
				read_image(&SegImg[i], imgPath);
				set_check("give_error");
			}
			else {
				set_check("~give_error");
				write_image(SegImg[i], "bmp", 0, imgPath);
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

void mark1Param::iniData()
{
	sprintf(AutoSaveTmpImgDir, "D:/VisionTemplateImage");
	OcrValid = 0;
	errCrack = 5;
	errCrackValid = 0;
	errExcess = 5;
	errExcessValid = 0;
	IsInputRatio = 0;
	pixelSize = 1.0;
	IsReferTempAngle = 1;
	HorOrVer = 0;
	charGap = 50;
	IsReferSection = 2;
	IsFillUp = 0;
	charThreMax = 255;
	charThreMin = 0;
	autoSegment = 0;
	threshold = 70;
	whiteChar = 0;
	minArea = 20;
	minHoles = 0;
	//multTemp=0;
	//rbTmpDegree=0;
	CharDefectTOL = 1;
	areaType = 0;

	charOffsetValid = 1;
	charOffset = 10;
	searchAngleValid = 0;
	searchAngle = 0;

	acceptSimilar = 60;
	errAngleValid = 0;
	errAngle = 0;
	errOffSetValid = 0;
	errOffSet = 10;

	errOffXValid = 0;
	errOffYValid = 0;
	errOffXMin = 0;
	errOffXMax = 100;
	errOffYMin = 0;
	errOffYMax = 100;

	errAreaPercent = 20;
	errAreaValid = 0;

	for (int i = 0;i < MAX_MARK_NUM;i++) {
		charCounts[i] = 0;
		segRegionVector[i].clear();
		RegionRoi[i].row1 = RegionRoi[i].col1 = 20;
		RegionRoi[i].row2 = RegionRoi[i].col2 = 80;
	}

	autoRegionLarge = 0;
	for (int i = 0;i < MAX_MARK_NUM;i++)
	{
		MultPatternEnable[i] = 0;
		//patternImg[i].clear();
	}

	IsMult = 0;
	NowPatternNum = 0;
	PatternNum = 4;
	IsFontOrLayer = 0;
	IsDirPattern = 0;
	angleType = 0;

	IsUsePinDirOut = 0;
	pin1OutIdx = 1;
	pin2OutIdx = 2;
	IsDefaultSig = 0;
	IsInverse = 0;
	Is2Dor4D = 0;
	isShowAngle = 1;
	IsInputPatIndex = 0;
}

