#include "composeND_preModule.h"
#include "composeND_preParam.h"
#include <QObject>
#include <ImageView.h>
#include <resultText.h>

#define PIN_IM "im"

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
	changePinsNum(m_param->imNum);
}

void composeND_preModule::load(const QString &dirPath, QvsParamLevel level)
{
	m_param->readWriteParam(true, dirPath, level);

	if (level == StructLevel)
	{
		changePinsNum(m_param->imNum);
	}
}

int composeND_preModule::run(const QString &funName)
{
	*p_outIm = cv::Mat();
	m_res = noDet;
	m_emptyIdx = -1;

	bool isEmpty = true;
	for (auto i = 0; i < p_inIm.size(); ++i)
	{
		isEmpty = (*p_inIm[i])->empty();
		m_emptyIdx = i;
		if (isEmpty)
			break;
	}
	if (isEmpty)
	{
		m_res = emptyImg;
		return -1;
	}

	cv::Size sz = (*p_inIm[0])->size();
	for (auto i = 1; i < p_inIm.size(); ++i)
	{
		if (sz != (*p_inIm[i])->size())
		{
			m_res = wrongSize;
			return -1;
		}
	}

	*p_outIm = (*p_inIm[0])->clone();
	for (auto i = 0;i < m_param->imNum;++i)
	{
		for (auto k = 0;k < m_param->roiNum[i];++k)
		{
			int x = m_param->regCol0[i][k];
			int y = m_param->regRow0[i][k];
			int wid = m_param->regCol1[i][k] - m_param->regCol0[i][k];
			int hei = m_param->regRow1[i][k] - m_param->regRow0[i][k];
 			cv::Rect rec(x, y, wid, hei);
			if (p_outIm->size().height < y + hei || p_outIm->size().width < x + wid)
			{
				*p_outIm = cv::Mat(0, 0, CV_8UC1);
				return - 1;
			}
			cv::Mat segImg(*p_outIm, rec);
			(***p_inIm[i])(rec).copyTo(segImg);
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
	createEvents();

	m_res = noDet;
	m_emptyIdx = -1;
}

void composeND_preModule::createPins()
{
	addPin(&p_outIm, QString("im"));
}

void composeND_preModule::createEvents()
{
	addEvent(ENT_DISPIMG);
}

void composeND_preModule::changePinsNum(int imNum)
{
	while (p_inIm.size() > imNum)
	{
		removePin(PIN_IM + QString("%1").arg(p_inIm.size()), true);
		p_inIm.pop_back();
	}
	while (p_inIm.size() < imNum)
	{
		UnitInputPin<cv::Mat> *im = new UnitInputPin<cv::Mat>;
		addPin(im, PIN_IM + QString("%1").arg(p_inIm.size() + 1));
		p_inIm.push_back(im);
	}
}
