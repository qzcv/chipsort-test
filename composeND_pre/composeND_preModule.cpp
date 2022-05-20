#include "composeND_preModule.h"
#include "composeND_preParam.h"
#include <QObject>
#include <ImageView.h>
#include <resultText.h>

composeND_preModule::composeND_preModule()
{
	iniData();
}

composeND_preModule::composeND_preModule(const QString& dirPath)
{
	iniData();
	load(dirPath);
}

composeND_preModule::~composeND_preModule()
{
	delete m_param;
}

int composeND_preModule::command(int cmdID, void* dataIn, void* dataOut)
{
	return 0;
}

int composeND_preModule::afterSetProperty(MetaProperty*)
{
	return 0;
}

void composeND_preModule::save(const QString &dirPath)
{
	m_param->readWriteParam(false, dirPath, StructLevel);
	resetParamChanged();
}

void composeND_preModule::load(const QString &dirPath)
{
	m_param->readWriteParam(true,dirPath,StructLevel);
}

void composeND_preModule::load(const QString &dirPath, QvsParamLevel level)
{
	m_param->readWriteParam(true,dirPath,level);
}

int composeND_preModule::run(const QString &funName)
{
	*p_outIm = cv::Mat();
	m_res = noDet;
	m_emptyIdx = -1;

	bool isEmpty = p_inIm[m_param->baseImgIdx]->empty();
	m_emptyIdx = m_param->baseImgIdx;
	if (!isEmpty)
	{
		for (auto i = 0;i < m_param->allRegNum;++i)
		{
			isEmpty = p_inIm[m_param->imageIndex[i]]->empty();
			m_emptyIdx = m_param->imageIndex[i];
			if (isEmpty)
				break;
		}
	}
	if (isEmpty)
	{
		m_res = emptyImg;
		return -1;
	}

	cv::Size sz = p_inIm[m_param->baseImgIdx]->size();
	for (auto i = 0;i < m_param->allRegNum;++i)
	{
		if (sz != p_inIm[m_param->imageIndex[i]]->size())
		{
			m_res = wrongSize;
			return -1;
		}
	}

	*p_outIm = p_inIm[m_param->baseImgIdx]->clone();
	for (auto i = 0;i < m_param->allRegNum;++i)
	{
		for (auto k = 0;k < m_param->roiNum[i];++k)
		{
			int x = m_param->regCol0[i][k];
			int y = m_param->regRow0[i][k];
			int wid = m_param->regCol1[i][k] - m_param->regCol0[i][k];
			int hei = m_param->regRow1[i][k] - m_param->regRow0[i][k];
 			cv::Rect rec(x, y, wid, hei);
			cv::Mat segImg(*p_outIm, rec);
			(**p_inIm[m_param->imageIndex[i]])(rec).copyTo(segImg);
		}
	}
	m_res = composeSucceed;

	return 0;
}

void composeND_preModule::viewResult(ImageView *iv, const QString &funName, int)
{

}

void composeND_preModule::textResult(ResultText *text, const QString &funName)
{
	if (m_res == noDet)
		return;
	else if (m_res == emptyImg)
	{
		text->setTextColor(Qt::red);
		text->append(QObject::tr("img%1 is an empty image!").arg(m_emptyIdx + 1));
	}
	else if (m_res == wrongSize)
	{
		text->setTextColor(Qt::red);
		text->append(QObject::tr("All images need the same size!"));
	}
	else if (m_res == composeSucceed)
	{
		text->setTextColor(Qt::blue);
		text->append(QObject::tr("compose succeed!"));
	}
}

void composeND_preModule::iniData()
{
	m_param = new composeND_preParam;
	createPins();

	m_res = noDet;
	m_emptyIdx = -1;
}

void composeND_preModule::createPins()
{
	for (auto i = 0;i < MAX_REGION + 1;++i)
	{
		addPin(&p_inIm[i], QString("im%1").arg(i + 1));
	}
	addPin(&p_outIm, QString("im"));
}
