#include "composeND_preParam.h"
#include <iniParam.h>
#include "qvglobal.h"

composeND_preParam::composeND_preParam()
{
	iniData();
}

composeND_preParam::~composeND_preParam()
{

}

void composeND_preParam::readWriteParam(bool r, const QString& dirPath, int level)
{
	SETTING_INIT(dirPath + "/composeND_pre.ini");

	if (!r || (r&&level == StructLevel))
		RW_VALUE(r, imNum);

	if (!r || (r&&level == MachineLevel))
	{
		if (r)
		{
			roiNum = QVector<int>(imNum, 1);
			regRow0.clear();
			regCol0.clear();
			regRow1.clear();
			regCol1.clear();
		}

		for (auto i = 0; i < imNum; ++i)
		{
			RW_VALUE1(r, roiNum[i], i);

			if (r)
			{
				regRow0.append(QVector<double>(roiNum[i], 100));
				regCol0.append(QVector<double>(roiNum[i], 100));
				regRow1.append(QVector<double>(roiNum[i], 200));
				regCol1.append(QVector<double>(roiNum[i], 200));
			}
			for (auto j = 0; j < roiNum[i]; ++j)
			{
				RW_VALUE2(r, regRow0[i][j], i, j);
				RW_VALUE2(r, regCol0[i][j], i, j);
				RW_VALUE2(r, regRow1[i][j], i, j);
				RW_VALUE2(r, regCol1[i][j], i, j);
			}
		}
	}
}

void composeND_preParam::iniData()
{
	imNum = 1;
	roiNum = QVector<int>(imNum, 1);
	
	for (auto i = 0; i < imNum; i++)
	{
		regRow0.append(QVector<double>(roiNum[i], 100));
		regCol0.append(QVector<double>(roiNum[i], 100));
		regRow1.append(QVector<double>(roiNum[i], 200));
		regCol1.append(QVector<double>(roiNum[i], 200));
	}
}

