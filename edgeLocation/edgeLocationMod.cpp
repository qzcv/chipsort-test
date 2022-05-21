#include "edgeLocationMod.h"
#include "edgeLocationParam.h"
#include <QObject>
#include <ImageView.h>
#include <resultText.h>

edgeLocationMod::edgeLocationMod()
{
	iniData();
}

edgeLocationMod::edgeLocationMod(const QString& dirPath)
{
	iniData();
	load(dirPath, StructLevel);
}

edgeLocationMod::~edgeLocationMod()
{
	delete m_param;
}

int edgeLocationMod::command(int cmdID, void* dataIn, void* dataOut)
{
	return 0;
}

int edgeLocationMod::afterSetProperty(MetaProperty*)
{
	return 0;
}

void edgeLocationMod::save(const QString &dirPath)
{
	m_param->readWriteParam(false, dirPath, StructLevel);
	resetParamChanged();
}

void edgeLocationMod::load(const QString &dirPath, QvsParamLevel level)
{
	m_param->readWriteParam(true,dirPath,level);
	if (level == ProductLevel)
		iniDetector();
}

int edgeLocationMod::run(const QString &funName)
{
	return 0;
}

void edgeLocationMod::viewResult(ImageView *iv, const QString &funName, int)
{

}

void edgeLocationMod::textResult(ResultText *text, const QString &funName)
{

}

void edgeLocationMod::iniData()
{
	m_param = new edgeLocationParam;
	for (int i = 0;i < 4;i++)
		m_edgeLine[i] = new fitEdgeLine();

	createPins();
}

void edgeLocationMod::createPins()
{

}

void edgeLocationMod::iniDetector()
{
	int roiCount = 1;
	for (int i = 0;i < 4;i++) {
		// up right dw left
		m_edgeLine[i]->setPolarAndOrient((fitEdgeLine::Polarity)m_param->whiteBlackDir[i], m_dir[m_param->searchDir[i]][i]);
		m_edgeLine[i]->setThreshold(m_param->minGrayDiff[i]);
		m_edgeLine[i]->setSearchParam(m_param->searchLineGap[i], m_param->extendSearchLine[i]);
		m_edgeLine[i]->setFitFactor(m_param->clipfactor[i]);
		m_edgeLine[i]->setCrossGrayValid(m_param->CrossGrayValid[i], m_param->crossGray[i], m_param->crossStep[i]);
		m_edgeLine[i]->setEdgeMethod(0, 0);
		int drawRow1[20], drawRow2[20], drawCol1[20], drawCol2[20];  //4*5
		for (int j = 0;j < m_param->roiNum[i];j++) {
			drawRow1[j] = m_param->RoiRegionVector[j + roiCount].row1;
			drawRow2[j] = m_param->RoiRegionVector[j + roiCount].row2;
			drawCol1[j] = m_param->RoiRegionVector[j + roiCount].col1;
			drawCol2[j] = m_param->RoiRegionVector[j + roiCount].col2;
		}
		m_edgeLine[i]->setRoi(m_param->roiNum[i], drawRow1, drawCol1, drawRow2, drawCol2);
		roiCount += m_param->roiNum[i];
	}
}