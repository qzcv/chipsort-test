#include "orientAreaModule.h"
#include "orientAreaParam.h"
#include <QObject>
#include <ImageView.h>
#include <resultText.h>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include "HalOpencv.h"

orientAreaModule::orientAreaModule()
{
	iniData();
}

orientAreaModule::orientAreaModule(const QString& dirPath)
{
	iniData();
	load(dirPath);
}

orientAreaModule::~orientAreaModule()
{
	delete m_param;
}

int orientAreaModule::command(int cmdID, void* dataIn, void* dataOut)
{
	return 0;
}

int orientAreaModule::afterSetProperty(MetaProperty*)
{
	return 0;
}

void orientAreaModule::save(const QString &dirPath)
{
	m_param->readWriteParam(false, dirPath, StructLevel);
	resetParamChanged();
}

void orientAreaModule::load(const QString &dirPath)
{
	m_param->readWriteParam(true, dirPath, StructLevel);
}

void orientAreaModule::load(const QString &dirPath, QvsParamLevel level)
{
	m_param->readWriteParam(true, dirPath, level);
	if (level == QvsParamLevel::ProductLevel)
	{
		//setDirPinVisible(m_param->usePinDir);
	}
	else if (level == MachineLevel)
	{
		//p_noneNGIdx.setData(m_param->noneNgIdx);
		//p_lfIdx.setData(m_param->pin1Idx);
		//p_rtIdx.setData(m_param->pin2Idx);
	}
}

int orientAreaModule::run(const QString &funName)
{
	m_detRes = noDet;
	m_sz[0] = m_sz[1] = 0;
	m_area[0] = m_area[1] = 0;
	int allok = 0;
	*p_reg = RlRegion();
 	p_pickIdx.setData(0);
	p_find.setData(0);
	p_noneNGValid.setData(0);

	if (p_img->empty())
	{
		m_detRes = emptyImg;
		return -1;
	}
	HTuple tmpHm2d;
	hom_mat2d_identity(&tmpHm2d);
	if (m_param->referSection)
	{
		if (p_homMat2d->size() != cv::Size(3, 2))
		{
			m_detRes = wrongMat;
			return -1;
		}
		else
			HalOpenCV::cv2HalHomMat2D(*p_homMat2d, &tmpHm2d);
	}


	cv::Mat grayImg;
	if (p_img->channels() == 3)
		cvtColor(**p_img, grayImg, CV_BGR2GRAY);
	else
		grayImg = *p_img;

	m_detRes = detSucceed;

	for (auto i = 0;i < 2;++i)
	{
		if (m_param->IsSingleTarget&&i == 1)
			continue;
		
		m_affineReg[i].genRect(m_param->RoiCol1[i], m_param->RoiRow1[i], m_param->RoiCol2[i], m_param->RoiRow2[i]);
		if (m_param->referSection)
		{
			cv::Mat hm2d;
			HalOpenCV::hal2cvHomMat2D(tmpHm2d, hm2d);
			m_affineReg[i].affineTrans(m_affineReg[i], hm2d, false);
		}
	}

	//get region: m_reg[2]
	if (m_param->IsSingleTarget)
	{
		selectShape(m_affineReg[0]);
		*p_reg = m_reg[0];
	}
	else
	{
		selectShape(m_affineReg[0], m_affineReg[1]);
		RlRegion uReg;
		uReg.unionReg(m_reg[0], m_reg[1]);
		p_reg->unionReg(m_reg[0], m_reg[1]);
	}	

	for (auto i = 0;i < 2;++i)
	{
		m_isOK[i] = true;
		if (m_param->IsSingleTarget&&i == 1)
			break;
		if (m_param->errRegionValid)
		{
			RlRegions regs;
			m_reg[i].connect(regs);
			m_sz[i] = regs.size();
			if (m_sz[i] < m_param->errRegionMin || m_sz[i] > m_param->errRegionMax)
			{
				m_isOK[i] = false;
				allok = -2;
			}
		}
		if (m_param->errAreaValid)
		{
			float cx, cy;
			m_reg[i].areaCenter(m_area[i], cx, cy);
			if (m_area[i] < m_param->errAreaMin || m_area[i] > m_param->errAreaMax)
			{
				m_isOK[i] = false;
				allok = -3;
			}
		}
	}

	if (m_param->IsSingleTarget)
		if (m_detRes == detSucceed&&m_isOK[0])
			allok = 0;
		else
			allok = -1;
	else
	{
		if (m_param->errAreaCompareValid)
		{
			if (m_param->errAreaCompareIndex == 0 && (m_area[0] <= m_area[1]) ||
				m_param->errAreaCompareIndex == 1 && (m_area[0] >= m_area[1]))
			{
				m_isOK[0] = m_isOK[1] = false;
				allok = -4;
			}
		}		

		if (m_param->usePinDir)
		{
			bool isLeft = m_area[0] > m_area[1] ? 1 : 0;
			if (m_param->IsReverse)
			{
				p_lfDir.setData(!isLeft);
				p_rtDir.setData(isLeft);
				p_pickIdx.setData(isLeft);
			}
			else
			{
				p_lfDir.setData(isLeft);
				p_rtDir.setData(!isLeft);
				p_pickIdx.setData(!isLeft);
			}				
		}
	}
	p_find.setData(allok == 0 ? 1 : 0);
	if (m_param->noneNgValid)
		p_noneNGValid.setData(allok);
	else
		p_noneNGValid.setData(0);
	return allok;
}

void orientAreaModule::viewResult(ImageView *iv, const QString &funName, int)
{
	if (m_detRes == noDet || m_detRes == emptyImg)
		return;
	else if (m_detRes == wrongMat)
		return;
	else if (m_detRes == detFailed)
	{
		if (m_param->IsShowRegion)
		{
			iv->setColor(Qt::red);
			iv->setFilled(false);
			iv->dispRegion(&m_affineReg[0], QObject::tr("left"));
			if (m_param->IsSingleTarget)
				iv->dispRegion(&m_affineReg[1], QObject::tr("right"));
		}
	}
	else if (m_detRes == detSucceed)
	{
		iv->setFilled(false);
		if (m_param->IsShowRegion)
		{
			if (m_isOK[0])
				iv->setColor(Qt::green);
			else
				iv->setColor(Qt::red);
			iv->dispRegion(&m_reg[0]);
			iv->dispRect1(m_param->RoiRow1[0], m_param->RoiCol1[0], m_param->RoiRow2[0], m_param->RoiCol2[0], QObject::tr("left"));

			if (!m_param->IsSingleTarget)
			{
				if (m_isOK[1])
					iv->setColor(Qt::green);
				else
					iv->setColor(Qt::red);
				iv->dispRegion(&m_reg[1]);
				iv->dispRect1(m_param->RoiRow1[1], m_param->RoiCol1[1], m_param->RoiRow2[1], m_param->RoiCol2[1], QObject::tr("right"));
			}
		}
		if (m_param->usePinDir && !m_param->IsSingleTarget)
		{
			iv->setColor(255, 0, 255);
			if (/**p_pickIdx*/*p_rtDir == 0)
				iv->dispText(400, 500, QObject::tr("leftDir"));
			else
				iv->dispText(400, 500, QObject::tr("rightDir"));
		}
	}
}

void orientAreaModule::textResult(ResultText *text, const QString &funName)
{
	if (m_detRes == noDet)
		return;
	if (m_detRes == emptyImg)
	{
		text->setTextColor(Qt::red);
		text->append(QObject::tr("Empty image!"));
		return;
	}
	else if (m_detRes == wrongMat)
	{
		text->setTextColor(Qt::red);
		text->append(QObject::tr("Need to input a 2*3 matrix!"));
		return;
	}
	else if (m_detRes == detFailed)
	{
		text->setTextColor(Qt::red);
		text->append(QObject::tr("Detect failed!"));
		return;
	}
	else if (m_detRes == detSucceed)
	{
		for (auto i = 0;i < 2;++i)
		{
			if (m_param->IsSingleTarget&&i == 1)
				break;

			if (m_param->errRegionValid)
			{
				if (m_sz[i]<m_param->errRegionMin || m_sz[i]>m_param->errRegionMax)
					text->setTextColor(Qt::red);
				else
					text->setTextColor(Qt::blue);
				text->append(QObject::tr("number of %1 region:%2 (%3,%4)")
					.arg(i ? QObject::tr("right") : QObject::tr("left")).arg(m_sz[i])
					.arg(m_param->errRegionMin).arg(m_param->errRegionMax));
			}
			if (m_param->errAreaValid)
			{
				if (m_area[i]<m_param->errAreaMin || m_area[i]>m_param->errAreaMax)
					text->setTextColor(Qt::red);
				else
					text->setTextColor(Qt::blue);
				text->append(QObject::tr("area of %1 region:%2 (%3,%4)")
					.arg(i ? QObject::tr("right") : QObject::tr("left")).arg(m_area[i])
					.arg(m_param->errAreaMin).arg(m_param->errAreaMax));
			}
			if (m_param->errAreaCompareValid&&i == 1)
			{
				if (m_param->errAreaCompareIndex == 0 && (m_area[0] <= m_area[1]) ||
					m_param->errAreaCompareIndex == 1 && (m_area[0] >= m_area[1]))
				{
					text->setTextColor(Qt::red);
					text->append(QObject::tr("comparing of area wrong!"));
				}
				else
				{
					text->setTextColor(Qt::blue);
					text->append(QObject::tr("comparing of area right!"));
				}
			}
		}
	}
}

void orientAreaModule::iniData()
{
	m_param = new orientAreaParam;
	createPins();
	createEvents();
	m_reg[0] = RlRegion();
	m_reg[1] = RlRegion();
	m_affineReg[0] = RlRegion();
	m_affineReg[1] = RlRegion();
	m_isOK[0] = true;
	m_isOK[1] = true;
}

void orientAreaModule::createPins()
{
	//input
	addPin(&p_img, "im");
	addPin(&p_homMat2d, "hm2d");

	//output
	addPin(&p_reg, "reg");
	//addPin(&p_lfIdx, "lfIdx");
	addPin(&p_lfDir, "lf");
	//addPin(&p_rtIdx, "rtIdx");
	addPin(&p_rtDir, "rt");
	addPin(&p_pickIdx, "pkIdx");
	addPin(&p_find, "find");
	//addPin(&p_noneNGIdx, "noneIdx");
	addPin(&p_noneNGValid, "noneVld");

	//p_lfIdx.setVisible(false);
	//p_rtIdx.setVisible(false);
	p_lfDir.setVisible(false);
	p_rtDir.setVisible(false);
	p_pickIdx.setVisible(false);
	p_find.setVisible(false);
	//p_noneNGIdx.setVisible(false);
	p_noneNGValid.setVisible(false);
}

void orientAreaModule::createEvents()
{

}

void orientAreaModule::setDirPinVisible(bool isVisible)
{
	p_lfDir.setVisible(isVisible);
	p_rtDir.setVisible(isVisible);
}

void orientAreaModule::selectShape(const RlRegion & lReg, const RlRegion & rReg)
{
	cv::Mat img;
	if (m_param->whiteGuy == 0)
		cv::bitwise_not(**p_img, img);
	else
		img = *p_img;

	RlRegion partReg;
	for (auto i = 0;i < 2;++i)
	{
		if (m_param->IsSingleTarget&&i == 2)
			break;
		partReg = i == 0 ? lReg : rReg;

		m_reg[i] = RlRegion();	//clear

		RlRegion thresReg;
		int gMin = m_param->grayMin;
		int gMax = m_param->grayMax < m_param->grayMin ? m_param->grayMin : m_param->grayMax;
		thresReg.threshold(img, gMin, gMax);
		partReg.interset(partReg, thresReg);

		if (m_param->openValid&&m_param->Opening > 0)
		{
			int opening = m_param->Opening;
			if (m_param->Opening % 2 == 0)	//开操作算子长宽需为奇数
				opening = m_param->Opening - 1;
			partReg.openingRect(partReg, opening, opening);
		}
		if (m_param->closeValid&&m_param->Closing > 0)
		{
			int closing = m_param->Closing;
			if (m_param->Closing % 2 == 0)	//闭操作算子长宽需为奇数
				closing = m_param->Closing - 1;
			partReg.closingRect(partReg, closing, closing);
		}

		RlRegions partRegs;
		partReg.connect(partRegs);

		RlRegion tempReg;
		int area, smaRectRow0, smaRectCol0, smaRectRow1, smaRectCol1;
		float cx, cy;
		for (auto i = 0;i < partRegs.size();++i)
		{
			partRegs[i].areaCenter(area, cx, cy);
			if (area<m_param->areaMin || area>m_param->areaMax)
				continue;

			partRegs[i].smallestRect(smaRectCol0, smaRectRow0, smaRectCol1, smaRectRow1);
			int h = smaRectRow1 - smaRectRow0;
			int w = smaRectCol1 - smaRectCol0;
			if (h<m_param->heightMin || h>m_param->heightMax)
				continue;
			if (w<m_param->widthMin || w>m_param->widthMax)
				continue;

			if (m_param->IsFillUp)
				partRegs[i].fillUp(partRegs[i]);

			tempReg.unionReg(tempReg, partRegs[i]);
		}
		m_reg[i].unionReg(m_reg[i], tempReg);
	}
}

