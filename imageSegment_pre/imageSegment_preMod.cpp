#include "imageSegment_preMod.h"
#include "imageSegment_preParam.h"
#include <QObject>
#include <ImageView.h>
#include <resultText.h>
#include "halOpenCV.h"

imageSegment_preMod::imageSegment_preMod()
{
	iniData();
}

imageSegment_preMod::imageSegment_preMod(const QString& dirPath)
{
	iniData();
	load(dirPath, StructLevel);
}

imageSegment_preMod::~imageSegment_preMod()
{
	delete m_param;
	delete m_image;
}

int imageSegment_preMod::command(int cmdID, void* dataIn, void* dataOut)
{
	return 0;
}

int imageSegment_preMod::afterSetProperty(MetaProperty*)
{
	return 0;
}

void imageSegment_preMod::save(const QString &dirPath)
{
	m_param->readWriteParam(false, dirPath, StructLevel);

	resetParamChanged();
}

void imageSegment_preMod::load(const QString &dirPath, QvsParamLevel level)
{
	m_param->readWriteParam(true,dirPath,level);
}

int imageSegment_preMod::run(const QString &funName)
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

	Hobject* segImg = new Hobject;
	calReg(segImg);
	
	if (m_param->thresholdType == 0) {
		select_shape(*segImg, segImg, "area", "and", m_param->minArea[0], m_param->maxArea[0] >= m_param->minArea[0] ? m_param->maxArea[0] : m_param->minArea[0]);
	}
	Hlong num;
	count_obj(*segImg, &num);
	if (num == 0) {
		gen_empty_region(segImg);
	}
	else {
		union1(*segImg, segImg);
	}
	HTuple imgwid, imgheight;
	get_image_size(*m_image, &imgwid, &imgheight);
	region_to_bin(*segImg, segImg, 255, 0, imgwid[0].I(), imgheight[0].I());
	
	HalOpenCV::hal2cvImg(*segImg, *p_oIm, true);
	delete segImg;
	return 0;
}

void imageSegment_preMod::viewResult(ImageView *iv, const QString &funName, int)
{

}

void imageSegment_preMod::textResult(ResultText *text, const QString &funName)
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
	text->append(QObject::tr("Succeed!"));
	return;
}

void imageSegment_preMod::iniData()
{
	m_param = new imageSegment_preParam;
	m_image = new Hobject;
	createPins();
}

void imageSegment_preMod::createPins()
{
	addPin(&p_iIm, "iIm");

	addPin(&p_oIm, "oIm");
}

void imageSegment_preMod::calReg(Hobject * segImg)
{
	HalOpenCV::cv2halImg(*p_iIm, m_image, false);

	Hobject roiRec, unionRec, tmpImg;
	int roiNum;
	if (!m_param->allImgValid) {
		roiNum = m_param->RoiCounts;
	}
	else {
		roiNum = 1;
	}
	HTuple imgwid, imgheight;
	get_image_size(*m_image, &imgwid, &imgheight);
	for (int i = 0;i < roiNum;i++)
	{
		if (i == 0 && m_param->allImgValid) {
			gen_rectangle1(&roiRec, 0, 0, imgheight[0].I() - 1, imgwid[0].I() - 1);
		}
		else {
			gen_rectangle1(&roiRec,
				m_param->segRegionVector[i].row1, m_param->segRegionVector[i].col1,
				m_param->segRegionVector[i].row2, m_param->segRegionVector[i].col2);
		}
		reduce_domain(*m_image, roiRec, &tmpImg);
		if (m_param->thresholdType == 0) {
			threshold(tmpImg, &tmpImg, m_param->minGray[i], m_param->maxGray[i] >= m_param->minGray[i] ? m_param->maxGray[i] : m_param->minGray[i]);
			connection(tmpImg, &tmpImg);
			select_shape(tmpImg, &tmpImg, "area", "and", m_param->minArea[i], m_param->maxArea[i] >= m_param->minArea[i] ? m_param->maxArea[i] : m_param->minArea[i]);
		}
		else if (m_param->thresholdType == 1) {

			int mcenterx, mcentery;
			if (m_param->centerType == 0) {
				mcenterx = (m_param->segRegionVector[i].col1 + m_param->segRegionVector[i].col2) / 2;
				mcentery = (m_param->segRegionVector[i].row1 + m_param->segRegionVector[i].row2) / 2;
			}
			else if (m_param->centerType == 1) {
				mcenterx = m_param->centerX[i] > (imgwid[0].I() - 1) ? (imgwid[0].I() - 1) : m_param->centerX[i];
				mcentery = m_param->centerY[i] > (imgheight[0].I() - 1) ? (imgheight[0].I() - 1) : m_param->centerY[i];
			}
			if (m_param->IsMeanGrow) {
				regiongrowing_mean(tmpImg, &tmpImg, mcentery, mcenterx, m_param->thresholdTol, m_param->minGrowArea);
				connection(tmpImg, &tmpImg);
			}
			else {
				regiongrowing(tmpImg, &tmpImg, m_param->rowWid, m_param->colWid, m_param->thresholdTol, m_param->minGrowArea);
				connection(tmpImg, &tmpImg);
				if (m_param->IsNoCenter) {
					//
				}
				else {
					select_region_point(tmpImg, &tmpImg, mcentery, mcenterx);
					union1(tmpImg, &tmpImg);
				}
			}
		}
		union2(*segImg, tmpImg, segImg);
	}
	if (m_param->algorithmType == 1) {
		if (m_param->param1[1] > 0 && m_param->param2[1] > 0) {
			closing_rectangle1(*segImg, segImg, m_param->param1[1], m_param->param2[1]);
		}
	}
	else if (m_param->algorithmType == 2) {
		if (m_param->param1[2] > 0 && m_param->param2[2] > 0) {
			opening_rectangle1(*segImg, segImg, m_param->param1[2], m_param->param2[2]);
		}
	}
	else if (m_param->algorithmType == 3) {
		if (m_param->param1[3] > 0) {
			closing_circle(*segImg, segImg, m_param->param1[3]);
		}
	}
	else if (m_param->algorithmType == 4) {
		if (m_param->param1[4] > 0) {
			opening_circle(*segImg, segImg, m_param->param1[4]);
		}
	}
	connection(*segImg, segImg);
}

