#include "arithmetic_preModule.h"
#include "arithmetic_preParam.h"
#include <QObject>
#include <ImageView.h>
#include <resultText.h>
#include "RlRegion.h"

arithmetic_preModule::arithmetic_preModule()
{
	iniData();
}

arithmetic_preModule::arithmetic_preModule(const QString& dirPath)
{
	iniData();
	load(dirPath);
}

arithmetic_preModule::~arithmetic_preModule()
{
	delete m_param;
}

int arithmetic_preModule::command(int cmdID, void* dataIn, void* dataOut)
{
	return 0;
}

int arithmetic_preModule::afterSetProperty(MetaProperty*)
{
	return 0;
}

void arithmetic_preModule::save(const QString &dirPath)
{
	m_param->readWriteParam(false, dirPath, StructLevel);
	resetParamChanged();
}

void arithmetic_preModule::load(const QString &dirPath)
{
	m_param->readWriteParam(true,dirPath,StructLevel);
}

void arithmetic_preModule::load(const QString &dirPath, QvsParamLevel level)
{
	m_param->readWriteParam(true,dirPath,level);
}

int arithmetic_preModule::run(const QString &funName)
{
	*p_outIm = cv::Mat(0, 0, CV_8UC1);

	if (p_inIm1->empty() || p_inIm2->empty())
		return -1;
	if (p_inIm1->size() != p_inIm2->size())
		return -1;

	if (m_param->enable)
	{
		cv::Mat img1 = (m_param->idx1 == 0) ? p_inIm1->clone() : p_inIm2->clone();
		cv::Mat img2 = (m_param->idx2 == 0) ? p_inIm1->clone() : p_inIm2->clone();
		cv::Mat mask;

		if (!m_param->allImgValid)
		{
			RlRegion rect;
			for (auto i = 0;i < m_param->roiNum;++i)
			{
				RlRegion r;
				r.genRect(
					m_param->segRegArray[i].col1, m_param->segRegArray[i].row1,
					m_param->segRegArray[i].col2, m_param->segRegArray[i].row2);
				rect.unionReg(rect, r);
			}
			cv::Size sz = p_inIm1->size();
			mask = rect.toBinImage(255, 0, sz.height, sz.width);

			bitwise_and(img1, mask, img1);
			bitwise_and(img2, mask, img2);
		}

		Type idx = (Type)m_param->operatorIdx;
		if (idx == Type::ADD)
			cv::addWeighted(img1, m_param->pA[idx], img2, m_param->pB[idx], m_param->pC[idx], **p_outIm);
		else if (idx == Type::SUB)
			cv::addWeighted(img1, m_param->pA[idx], img2, -m_param->pB[idx], m_param->pC[idx], **p_outIm);
		else if (idx == Type::MULT)
			cv::multiply(img1, img2, **p_outIm, m_param->pA[idx]);
		else if (idx == Type::DIV)
			cv::divide(img1, img2, **p_outIm, m_param->pA[idx]);
		else if (idx == Type::MAX)
			cv::max(img1, img2, **p_outIm);
		else if (idx == Type::MIN)
			cv::min(img1, img2, **p_outIm);
		else if (idx == Type::AND)
			cv::bitwise_and(img1, img2, **p_outIm, mask);
		else if (idx == Type::OR)
			cv::bitwise_or(img1, img2, **p_outIm, mask);
		else if (idx == Type::NOT)
			cv::bitwise_not(img1, **p_outIm, mask);
		else if (idx == Type::NAND)
		{
			cv::bitwise_and(img1, img2, **p_outIm, mask);
			cv::bitwise_not(**p_outIm, **p_outIm, mask);
		}
		else if (idx == Type::NOR)
		{
			cv::bitwise_or(img1, img2, **p_outIm, mask);
			cv::bitwise_not(**p_outIm, **p_outIm, mask);
		}
		else if (idx == Type::XOR)
			cv::bitwise_xor(img1, img2, **p_outIm, mask);
		else if (idx == Type::XNOR)
		{
			cv::bitwise_xor(img1, img2, **p_outIm, mask);
			cv::bitwise_not(**p_outIm, **p_outIm, mask);
		}
	}
	else
	{
		if (m_param->outputIdx == 0)
			*p_outIm = p_inIm1->clone();		
		else if (m_param->outputIdx == 1)
			*p_outIm = p_inIm2->clone();			
	}

	cv::Mat m1 = *p_outIm;
	return 0;
}

void arithmetic_preModule::viewResult(ImageView *iv, const QString &funName, int)
{

}

void arithmetic_preModule::textResult(ResultText *text, const QString &funName)
{

}

void arithmetic_preModule::iniData()
{
	m_param = new arithmetic_preParam;
	createPins();
}

void arithmetic_preModule::createPins()
{
	addPin(&p_inIm1, "im1");
	addPin(&p_inIm2, "im2");

	addPin(&p_outIm, "im");
}

