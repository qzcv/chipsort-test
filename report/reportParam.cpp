#include "reportParam.h"
#include <iniParam.h>
#include "qvglobal.h"

GRRreportParam::GRRreportParam()
{
	iniData();
}

GRRreportParam::~GRRreportParam()
{

}

void GRRreportParam::readWriteParam(bool r, const QString& dirPath, int level)
{
	SETTING_INIT(dirPath + "/GRRreport.ini");

	if (!r || (r&&level == StructLevel))
		RW_VALUE(r, modNum);

	if (!r || (r&&level == ProductLevel))
	{
		RW_VALUE(r, GrrLotName);

		RW_VALUE(r, InputUnits);
		RW_VALUE(r, IsOutAll);
		RW_VALUE(r, IsGrrByTol);

		if (r)
		{
			itemNum = QVector<int>(modNum, 0);
			topText = QVector<QString>(modNum, "");
		}
		for (auto i = 0; i < modNum; i++)
		{
			RW_VALUE1(r, itemNum[i], i);
			RW_VALUE1(r, topText[i], i);
		}
		
		if (r)
		{
			enable.clear();
			subText.clear();
			for (auto i = 0; i < modNum; ++i)
			{
				enable.append(QVector<bool>(itemNum[i], true));
				subText.append(QVector<QString>(itemNum[i], ""));
				USL.append(QVector<double>(itemNum[i], 1.0));
				LSL.append(QVector<double>(itemNum[i], 0.0));
			}
		}
		for (auto i = 0; i < modNum; ++i)
			for (auto j = 0; j < itemNum[i]; ++j)
			{
				RW_VALUE2(r, enable[i][j], i, j);
				RW_VALUE2(r, subText[i][j], i, j);
				RW_VALUE2(r, USL[i][j], i, j);
				RW_VALUE2(r, LSL[i][j], i, j);
			}
	}

	if (r)
		isLoadFinished = level == ProductLevel;
}

void GRRreportParam::iniData()
{
	GrrLotName = "GRR/SPC TEST";

	modNum = 0;
	InputUnits = 10;
	IsOutAll = true;
	IsGrrByTol = false;

	itemNum.clear();
	topText.clear();
	enable.clear();
	subText.clear();
	USL.clear();
	LSL.clear();
}

