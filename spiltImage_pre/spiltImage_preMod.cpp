#include "spiltImage_preMod.h"
#include "spiltImage_preParam.h"
#include <QObject>
#include <ImageView.h>
#include <resultText.h>

spiltImage_preMod::spiltImage_preMod()
{
	iniData();
}

spiltImage_preMod::spiltImage_preMod(const QString& dirPath)
{
	iniData();
	load(dirPath, StructLevel);
}

spiltImage_preMod::~spiltImage_preMod()
{
	delete m_param;
}

int spiltImage_preMod::command(int cmdID, void* dataIn, void* dataOut)
{
	return 0;
}

int spiltImage_preMod::afterSetProperty(MetaProperty*)
{
	return 0;
}

void spiltImage_preMod::save(const QString &dirPath)
{
	m_param->readWriteParam(false, dirPath, StructLevel);
	resetParamChanged();
}

void spiltImage_preMod::load(const QString &dirPath, QvsParamLevel level)
{
	m_param->readWriteParam(true,dirPath,level);
}

int spiltImage_preMod::run(const QString &funName)
{
	if (p_iIm->empty())
	{
		*p_oIm = cv::Mat(0, 0, CV_8UC1);
		return -1;
	}		
	if (!m_param->moduleValid)
	{
		*p_oIm = *p_iIm;
		return 0;
	}

	RlRegion roiRec, unionRec;
	for (int i = 0;i < m_param->RoiCounts;i++) {
		roiRec.genRect(
			m_param->segRegionVector[i].row1, m_param->segRegionVector[i].col1,
			m_param->segRegionVector[i].row2, m_param->segRegionVector[i].col2);
		unionRec.unionReg(roiRec, unionRec);
	}
	cv::Size sz = p_iIm->size();
	if (!m_param->RoiValid)
	{
		RlRegion ImgRec;
		ImgRec.genRect(0, 0, sz.width - 1, sz.height - 1);
		unionRec.difference(ImgRec, unionRec);
	}
	cv::Mat i = unionRec.toBinImage(1, 0, sz.height, sz.width);
	*p_oIm = p_iIm->mul(i);
	return 0;
}

void spiltImage_preMod::viewResult(ImageView *iv, const QString &funName, int)
{

}

void spiltImage_preMod::textResult(ResultText *text, const QString &funName)
{
	if (p_iIm->empty())
	{
		text->setTextColor(Qt::red);
		text->append(QObject::tr("Empty image!"));
		return;
	}
	if (!m_param->moduleValid)
	{
		text->setTextColor(Qt::green);
		text->append(QObject::tr("Module invalid!"));
		return;
	}
	text->setTextColor(Qt::green);
	text->append(QObject::tr("Spilt succeed!"));
	return;
}

void spiltImage_preMod::iniData()
{
	m_param = new spiltImage_preParam;
	createPins();
}

void spiltImage_preMod::createPins()
{
	addPin(&p_iIm, "iIm");

	addPin(&p_oIm, "oIm");
}

