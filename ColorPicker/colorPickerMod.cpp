#include "colorPickerMod.h"
#include "colorPickerParam.h"
#include <QObject>
#include <ImageView.h>
#include <resultText.h>
#include "halOpenCV.h"
#include <opencv2/highgui.hpp>

ColorPickerMod::ColorPickerMod()
{
	iniData();
}

ColorPickerMod::ColorPickerMod(const QString& dirPath)
{
	iniData();
	load(dirPath, StructLevel);
}

ColorPickerMod::~ColorPickerMod()
{
	delete m_param;
	delete m_image;
	delete m_inAreaRegion;
	delete m_allReg;
	delete m_unionRoiReg;
	delete m_unionMinusReg;
	delete m_lightReg;
}

int ColorPickerMod::command(int cmdID, void* dataIn, void* dataOut)
{
	return 0;
}

int ColorPickerMod::afterSetProperty(MetaProperty*)
{
	return 0;
}

void ColorPickerMod::save(const QString &dirPath)
{
	m_param->readWriteParam(false, dirPath, StructLevel);
	resetParamChanged();
}

void ColorPickerMod::load(const QString &dirPath, QvsParamLevel level)
{
	m_param->readWriteParam(true,dirPath,level);
}

int ColorPickerMod::run(const QString &funName)
{
// 	for (int i = 0;i < MAX_TESTITEMS_NUM;i++) {
// 		m_testItemsStatus[i] = 1;
// 	}

//	bool allok = true;
// 	if (!m_image || m_image->Id() == H_EMPTY_REGION) {
// 		allok = false;
// 		setDetectOutData(ImageOk, false);
// 		setDetectOutData(AllOk, allok);
// 		m_testItemsStatus[0] = 0;
// 		return allok;
// 	}
// 	else {
// 		Hlong chans;
// 		count_channels(*m_image, &chans);
// 		if (chans != 3) {
// 			allok = false;
// 			setDetectOutData(ImageOk, false);
// 			setDetectOutData(AllOk, allok);
// 			m_testItemsStatus[0] = 0;
// 			return allok;
// 		}
// 		setDetectOutData(ImageOk, true);
// 	}
	m_allok = 0;
	m_detRes = noDet;
	if (p_im->empty()) {
		m_detRes = emptyImg;
		return -1;
	}
	else {
		HalOpenCV::cv2halImg(*p_im, m_image, false);

		Hlong chans;
		count_channels(*m_image, &chans);
		if (chans != 3) {
			m_detRes = wrongChannelNum;
			return -1;
		}
	}
	HTuple homMat2D;
	if (m_param->IsReferSection) {
		try {
			HalOpenCV::cv2HalHomMat2D(*p_homMat2d, &homMat2D);
			double tv = homMat2D[0].D();
		}
		catch (HException& ex) {
			hom_mat2d_identity(&homMat2D);
		}
		//setDetectOutData(TmpHomMat, homMat2D);
	}
	if (m_param->referType == 0 && m_param->roiCount == 0) {
		//setDetectOutData(AllOk, false);
		//m_testItemsStatus[0] = 0;
		m_detRes = noRoi;
		return -1;
	}
	Hobject unionRoi, roiregion;
	if (m_param->referType == 0) {
		for (int i = 0;i < m_param->roiCount;i++) {
			if (m_param->RoiVec[i].regionType == 0) {
				gen_rectangle2(&roiregion, m_param->RoiVec[i].RecRow, m_param->RoiVec[i].RecCol, m_param->RoiVec[i].RecAngle,
					m_param->RoiVec[i].RecLength1, m_param->RoiVec[i].RecLength2);
			}
			else {
				gen_circle(&roiregion, m_param->RoiVec[i].CirRow, m_param->RoiVec[i].CirCol, m_param->RoiVec[i].CirR);
			}

			union2(unionRoi, roiregion, &unionRoi);
		}
		if (m_param->AreaZoom > 0) {
			dilation_rectangle1(unionRoi, &unionRoi, m_param->AreaZoom, m_param->AreaZoom);
		}
		else if (m_param->AreaZoom < 0) {
			erosion_rectangle1(unionRoi, &unionRoi, abs(m_param->AreaZoom), abs(m_param->AreaZoom));
		}
		if (m_param->IsReferSection&&m_param->referType == 0) {
			affine_trans_region(unionRoi, &unionRoi, homMat2D, "false");
		}
	}
	else if (m_param->referType == 1) {  //传入面区域
		//HTuple inareap;
		//getDetectInData(InputAreaRegion, inareap);
		try {
			//copy_obj(*(Hobject*)inareap[0].L(), m_inAreaRegion, 1, 1);
			Hobject tmpReg;
			HalOpenCV::cv2halRegion(*p_inReg, &tmpReg);
			copy_obj(tmpReg, m_inAreaRegion, 1, 1);
			if (m_param->AreaZoom > 0) {
				dilation_rectangle1(*m_inAreaRegion, m_inAreaRegion, m_param->AreaZoom, m_param->AreaZoom);
			}
			else if (m_param->AreaZoom < 0) {
				erosion_rectangle1(*m_inAreaRegion, m_inAreaRegion, abs(m_param->AreaZoom), abs(m_param->AreaZoom));
			}
		}
		catch (HException& ex) {
			gen_rectangle1(m_inAreaRegion, 50, 50, 100, 100);
		}

		copy_obj(*m_inAreaRegion, &unionRoi, 1, 1);
	}
	else if (m_param->referType == 2) {  //传入点区域
		HTuple fianalrow, fianalcol;
		if (p_corners->size() < 4)
		{
			fianalcol[0] = 20;fianalrow[0] = 20;
			fianalcol[1] = 50;fianalrow[1] = 20;
			fianalcol[2] = 50;fianalrow[2] = 50;
			fianalcol[3] = 20;fianalrow[3] = 50;
			fianalcol[4] = 20;fianalrow[4] = 20;
		}
		else
		{
			for (auto i = 0;i < 4;++i)	//只取前面四个角点
			{
				fianalcol[i] = p_corners->at(i).x;
				fianalrow[i] = p_corners->at(i).y;
			}
			fianalcol[4] = fianalcol[0];
			fianalrow[4] = fianalrow[0];
		}
		//getDetectInData(ReferRow, fianalrow);
		//getDetectInData(ReferCol, fianalcol);
		//try {
		//	fianalcol[4] = fianalcol[0].D();
		//	fianalrow[4] = fianalrow[0].D();
		//}
		//catch (HException& ex) {
		//	fianalcol[4] = fianalrow[4] = fianalcol[0] = fianalrow[0] = 20;
		//	fianalcol[1] = 50;fianalrow[1] = 20;
		//	fianalcol[2] = 50;fianalrow[2] = 50;
		//	fianalcol[3] = 20;fianalrow[3] = 50;
		//}
		gen_region_polygon_filled(&unionRoi, fianalrow, fianalcol);
		if (m_param->AreaZoom > 0) {
			dilation_rectangle1(unionRoi, &unionRoi, m_param->AreaZoom, m_param->AreaZoom);
		}
		else if (m_param->AreaZoom < 0) {
			erosion_rectangle1(unionRoi, &unionRoi, abs(m_param->AreaZoom), abs(m_param->AreaZoom));
		}
	}
	//setDetectOutObject(UnionRoiRegion, unionRoi);
	*m_unionRoiReg = unionRoi;

	Hobject unionMin, Minregion;
	for (int i = 0;i < m_param->MinusCount;i++) {
		if (m_param->MinusVec[i].regionType == 0) {
			gen_rectangle2(&Minregion, m_param->MinusVec[i].RecRow, m_param->MinusVec[i].RecCol, m_param->MinusVec[i].RecAngle,
				m_param->MinusVec[i].RecLength1, m_param->MinusVec[i].RecLength2);
		}
		else {
			gen_circle(&Minregion, m_param->MinusVec[i].CirRow, m_param->MinusVec[i].CirCol, m_param->MinusVec[i].CirR);
		}
		union2(unionMin, Minregion, &unionMin);
	}
	if (m_param->IsReferSection&&m_param->MinusCount > 0) {
		affine_trans_region(unionMin, &unionMin, homMat2D, "false");
	}
	//setDetectOutObject(UnionMinusRegion, unionMin);
	*m_unionMinusReg = unionMin;

	Hlong wid, hei;
	Hobject ImgRec;
	Hobject dstImg;
	get_image_size(*m_image, &wid, &hei);
	gen_rectangle1(&ImgRec, 0, 0, hei - 1, wid - 1);

	if (m_param->MinusCount > 0) {
		difference(ImgRec, unionMin, &unionMin);
		reduce_domain(*m_image, unionMin, &unionMin);
		reduce_domain(unionMin, unionRoi, &dstImg);
	}
	else {
		reduce_domain(*m_image, unionRoi, &dstImg);
	}
	Hobject grayArea, lightarea;
	if (m_param->IsDeleteLightArea) {
		rgb1_to_gray(dstImg, &grayArea);
		threshold(grayArea, &lightarea, m_param->lightAreaThreshold, 255);
		connection(lightarea, &lightarea);
		mselect_shape(lightarea, &lightarea, "area", "and", m_param->lightArea, 99999);
		union1(lightarea, &lightarea);
		//setDetectOutObject(LightAreaDisp, lightarea);
		*m_lightReg = lightarea;
		Hobject diffRegion;
		difference(dstImg, lightarea, &diffRegion);
		reduce_domain(dstImg, diffRegion, &dstImg);
	}
	HTuple dstArea, dstRow, dstCol;
	area_center(dstImg, &dstArea, &dstRow, &dstCol);
//	setDetectOutData(RoiArea, dstArea);
	m_roiArea = dstArea[0].I();
	Hobject redImg, greenImg, blueImg;
	Hobject HueImg, saturImg, IntyImg;
	Hobject saturatedImg, hueSaturated, currentColor, selectColor;
	decompose3(dstImg, &redImg, &greenImg, &blueImg);
	trans_from_rgb(redImg, greenImg, blueImg, &HueImg, &saturImg, &IntyImg, "hsv");
	threshold(saturImg, &saturatedImg, m_param->SaturationMin, m_param->SaturationMax < m_param->SaturationMin ? m_param->SaturationMin : m_param->SaturationMax);
	reduce_domain(HueImg, saturatedImg, &hueSaturated);
	threshold(hueSaturated, &currentColor, m_param->HueMin, m_param->HueMax < m_param->HueMin ? m_param->HueMin : m_param->HueMax);
	if (m_param->IsClosing) {
		closing_rectangle1(currentColor, &currentColor, m_param->closeVal, m_param->closeVal);
	}
	if (m_param->IsOpening) {
		opening_rectangle1(currentColor, &currentColor, m_param->openVal, m_param->openVal);
	}
	connection(currentColor, &currentColor);
	mselect_shape(currentColor, &selectColor, "area", "and", m_param->areaMin, m_param->areaMax);
	if (m_param->IsHeight) {
		mselect_shape(selectColor, &selectColor, "height", "and", m_param->heightMin, m_param->heightMax);
	}
	if (m_param->IsWidth) {
		mselect_shape(selectColor, &selectColor, "width", "and", m_param->widthMin, m_param->widthMax);
	}
	if (m_param->IsFillUpHole) {
		fill_up(selectColor, &selectColor);
	}
	m_detRes = detSucceed;
	HTuple colorArea, colorRow, colorCol;
	area_center(selectColor, &colorArea, &colorRow, &colorCol);
	//double minArea, maxArea, totalarea;
	//minArea = 99999999;
	//maxArea = 0;
	//totalarea = 0;
	m_singleAreaMin = 9999999;
	m_singleAreaMax = 0;
	m_totalArea = 0;
	int areaCounts = colorArea.Num();
	union1(selectColor, &selectColor);
	//setDetectOutObject(AllRegionDisp, selectColor);
	*m_allReg = selectColor;

	if (areaCounts == 0) {
		//minArea = maxArea = 0;
		//totalarea = 0;
		m_singleAreaMax = m_singleAreaMin = 0;
		m_totalArea = 0;
	}
	else {
		for (int i = 0;i < areaCounts;i++) {
// 			if (colorArea[i].D() < minArea) {
// 				minArea = colorArea[i].D();
// 			}
// 			if (colorArea[i].D() > maxArea) {
// 				maxArea = colorArea[i].D();
// 			}
// 			totalarea += colorArea[i].D();
			if (colorArea[i].I() < m_singleAreaMin)
				m_singleAreaMin = colorArea[i].I();
			if (colorArea[i].I() > m_singleAreaMax)
				m_singleAreaMax = colorArea[i].I();
			m_totalArea += colorArea[i].I();
		}
	}
	//bool singleok = true;
	//bool totalok = true;
	m_singleAreaOK = true;
	m_totalAreaOK = true;
	if (m_param->IsErrSingleArea) {
		if (m_singleAreaMin < m_param->errSingleMin ||
			m_singleAreaMax > m_param->errSingleMax) {
			//singleok = false;
			//allok = false;
			//m_testItemsStatus[2] = 0;
			m_singleAreaOK = false;
			m_allok = -3;
		}
		//setDetectOutData(SingleMinArea, minArea);
		//setDetectOutData(SingleMaxArea, maxArea);
		//setDetectOutData(SingleBadOk, singleok);
	}
	if (m_param->IsErrAllArea) {
		if (!m_param->IsNotLess) {
			if (m_totalArea > m_param->errAllMax || 
				m_totalArea < m_param->errAllMin) {
				//totalok = false;
				//allok = false;
				//m_testItemsStatus[1] = 0;
				m_totalAreaOK = false;
				m_allok = -2;
			}
		}
		else {
			int minusarea = dstArea[0].I() - m_totalArea;
			//minusarea = dstArea[0].D() - totalarea;

			if (minusarea > m_param->errAllMax || 
				minusarea < m_param->errAllMin) {
				//totalok = false;
				//allok = false;
				//m_testItemsStatus[1] = 0;
				m_totalAreaOK = false;
				m_allok = -2;
			}
		}
		//setDetectOutData(TotalArea, totalarea);
		//setDetectOutData(TotalAreaOk, totalok);
	}
	//setDetectOutData(AllOk, allok);
	//return allok;
	return m_allok;
}

void ColorPickerMod::viewResult(ImageView *iv, const QString &funName, int)
{
	if (m_detRes == noDet)
		return;
	else if (m_detRes == emptyImg)
		return;
	else if (m_detRes == wrongChannelNum)
		return;
	else if (m_detRes == noRoi)
		return;
	else if (m_detRes == detSucceed)
	{
		Hlong objnum;
		count_obj(*m_allReg, &objnum);
		if (m_allReg->Id() != H_EMPTY_REGION&&objnum > 0) {
			if (m_allok != 0)
				iv->setColor(Qt::red);
			else
				iv->setColor(Qt::green);
			Hobject rad;
			union1(*m_allReg, &rad);
			RlRegion rlReg;
			HalOpenCV::hal2cvRegion(rad, &rlReg);
			iv->dispRegion(&rlReg);
		}
		if (m_param->IsDeleteLightArea) {
			Hlong objnum1;
			count_obj(*m_lightReg, &objnum1);
			if (m_lightReg->Id() != H_EMPTY_REGION&&objnum1 > 0) {
				iv->setColor(Qt::black);
				Hobject lad;
				union1(*m_lightReg, &lad);
				RlRegion rlReg;
				HalOpenCV::hal2cvRegion(lad, &rlReg);
				iv->dispRegion(&rlReg);
			}
		}
		if (m_param->IsShowRegionAfterDetect) {
			if (m_allok != 0)
				iv->setColor(Qt::red);
			else
				iv->setColor(Qt::green);
			iv->setFilled(false);
			Hobject hobj1;
			RlRegion r1;
			union1(*m_unionRoiReg, &hobj1);
			HalOpenCV::hal2cvRegion(hobj1, &r1);
			iv->dispRegion(&r1);
			iv->setColor(Qt::blue);
			if (m_param->MinusCount > 0) {
				Hobject hobj2;
				RlRegion r2;
				union1(*m_unionMinusReg, &hobj2);
				HalOpenCV::hal2cvRegion(hobj2, &r2);
				iv->dispRegion(&r2);
			}
		}
	}	
// 	HTuple imgok;
// 	getDispOutData(ImageOk, imgok);
// 	if (!imgok[0].I()) {
// 		return;
// 	}
// 	if (m_param->referType == 0 && m_param->roiCount == 0) {
// 		return;
// 	}
// 	HTuple allok;
// 	getDispOutData(AllOk, allok);
// 	Hlong objnum;
// 	count_obj(*getDispObject(AllRegionDisp), &objnum);
// 	if (getDispObject(AllRegionDisp)->Id() != H_EMPTY_REGION&&objnum > 0) {
// 		if (allok[0].I() == 0) {
// 			m_view->setColor(255, 0, 0);
// 		}
// 		else {
// 			m_view->setColor(0, 255, 0);
// 		}
// 		Hobject rad;
// 		union1(*getDispObject(AllRegionDisp), &rad);
// 		m_view->dispRegion(rad);
// 	}
// 	if (m_param->IsDeleteLightArea) {
// 		Hlong objnum1;
// 		count_obj(*getDispObject(LightAreaDisp), &objnum1);
// 		if (getDispObject(LightAreaDisp)->Id() != H_EMPTY_REGION&&objnum1 > 0) {
// 			m_view->setColor(0, 0, 0);
// 			Hobject lad;
// 			union1(*getDispObject(LightAreaDisp), &lad);
// 			m_view->dispRegion(lad);
// 		}
// 	}
// 	if (m_param->IsShowRegionAfterDetect) {
// 		if (allok[0].I() == 0) {
// 			m_view->setColor(255, 0, 0);
// 		}
// 		else {
// 			m_view->setColor(0, 255, 0);
// 		}
// 		m_view->setFilled(false);
// 		m_view->dispRegion(*getDispObject(UnionRoiRegion));
// 		m_view->setColor(0, 0, 255);
// 		if (m_param->MinusCount > 0) {
// 			m_view->dispRegion(*getDispObject(UnionMinusRegion));
// 		}
// 	}
}

void ColorPickerMod::textResult(ResultText *text, const QString &funName)
{
	if (m_detRes == noDet)
		return;
	else if (m_detRes == emptyImg)
	{
		text->setTextColor(Qt::red);
		text->append(QObject::tr("Empty image!"));
		return;
	}
	else if (m_detRes == wrongChannelNum)
	{
		text->setTextColor(Qt::red);
		text->append(QObject::tr("Please enter a color image!"));
		return;
	}
	else if (m_detRes == noRoi) {
		text->setTextColor(Qt::red);
		text->append(QObject::tr("No ROI area!"));
		return;
	}
	else if (m_detRes == detSucceed)
	{
		if (m_param->IsErrSingleArea)
		{
			double minpercent = (m_singleAreaMin / (double)m_roiArea)*100.0;
			double maxpercent = (m_singleAreaMax / (double)m_roiArea)*100.0;
			if (m_singleAreaOK == 0)
				text->setTextColor(Qt::red);
			else
				text->setTextColor(Qt::black);
			text->append(QObject::tr("single area: (%1[%5%],%2[%6%])out of(%3,%4)")
				.arg(m_singleAreaMin).arg(m_singleAreaMax).arg(m_param->errSingleMin).arg(m_param->errSingleMax)
				.arg(minpercent, 0, 'f', 3).arg(maxpercent, 0, 'f', 3));
		}
		if (m_param->IsErrAllArea) {
			if (m_totalAreaOK == 0)
				text->setTextColor(Qt::red);
			else
				text->setTextColor(Qt::black);
			if (m_param->IsNotLess) {
				text->append(QObject::tr("total area: allRoiArea-totalArea=%1-%2=%3[%6%], (%4,%5)")
					.arg(m_roiArea).arg(m_totalArea).arg(m_roiArea - m_totalArea)
					.arg(m_param->errAllMin).arg(m_param->errAllMax)
					.arg((m_roiArea - m_totalArea) / (double)m_roiArea*100.0, 0, 'f', 3));
			}
			else {
				text->append(QObject::tr("total area: %1[%4%], (%2,%3)")
					.arg(m_totalArea).arg(m_param->errAllMin).arg(m_param->errAllMax)
					.arg(m_totalArea / (double)m_roiArea*100.0, 0, 'f', 3));
			}
		}
	}
// 	HTuple imgok;
// 	getDispOutData(ImageOk, imgok);
// 	if (!imgok[0].I()) {
// 		return;
// 	}
// 	if (m_param->referType == 0 && m_param->roiCount == 0) {
// 		return;
// 	}
// 	HTuple allok;
// 	getDispOutData(AllOk, allok);
// 	HTuple roiarea;
// 	getDispOutData(RoiArea, roiarea);
// 	if (m_param->IsErrSingleArea) {
// 		HTuple areamax, areamin, areasingleok;
// 		getDispOutData(SingleBadOk, areasingleok);
// 		getDispOutData(SingleMinArea, areamin);
// 		getDispOutData(SingleMaxArea, areamax);
// 		double minpercent, maxpercent;
// 		minpercent = (areamin[0].D() / roiarea[0].D())*100.0;
// 		maxpercent = (areamax[0].D() / roiarea[0].D())*100.0;
// 		if (areasingleok[0].I() == 0) {
// 			m_text->setTextColor(QColor(255, 0, 0));
// 			m_text->append(QString::fromLocal8Bit("single area: (%1[%5%],%2[%6%])out of(%3,%4)").arg(QString::number(areamin[0].D(), 'g', 6))
// 				.arg(QString::number(areamax[0].D(), 'g', 6)).arg(QString::number(m_param->errSingleMin)).arg(QString::number(m_param->errSingleMax))
// 				.arg(QString::number(minpercent, 'g', 3)).arg(QString::number(maxpercent, 'g', 3)));
// 		}
// 		else {
// 			m_text->setTextColor(QColor(0, 0, 0));
// 			m_text->append(QString::fromLocal8Bit("single area: (%1[%5%],%2[%6%]) in (%3,%4)").arg(QString::number(areamin[0].D(), 'g', 6))
// 				.arg(QString::number(areamax[0].D(), 'g', 6)).arg(QString::number(m_param->errSingleMin)).arg(QString::number(m_param->errSingleMax))
// 				.arg(QString::number(minpercent, 'g', 3)).arg(QString::number(maxpercent, 'g', 3)));
// 		}
// 	}
// 	if (m_param->IsErrAllArea) {
// 		HTuple totalarea, areaallok;
// 		getDispOutData(TotalAreaOk, areaallok);
// 		getDispOutData(TotalArea, totalarea);
// 		double tot = totalarea[0].D();
// 		if (areaallok[0].I() == 0) {
// 			m_text->setTextColor(QColor(255, 0, 0));
// 		}
// 		else {
// 			m_text->setTextColor(QColor(0, 0, 0));
// 		}
// 		if (m_param->IsNotLess) {
// 			m_text->append(QString::fromLocal8Bit("total area: allRoiArea-totalArea=%1-%2=%3[%6%], (%4,%5)").arg(QString::number(roiarea[0].D(), 'g', 6))
// 				.arg(QString::number(totalarea[0].D(), 'g', 6)).arg(QString::number(roiarea[0].D() - totalarea[0].D(), 'g', 6))
// 				.arg(QString::number(m_param->errAllMin)).arg(QString::number(m_param->errAllMax))
// 				.arg(QString::number((roiarea[0].D() - totalarea[0].D()) / roiarea[0].D()*100.0, 'g', 3)));
// 		}
// 		else {
// 			m_text->append(QString::fromLocal8Bit("total area: %1[%4%], (%2,%3)").arg(QString::number(totalarea[0].D(), 'g', 6))
// 				.arg(QString::number(m_param->errAllMin)).arg(QString::number(m_param->errAllMax))
// 				.arg(QString::number((totalarea[0].D() / roiarea[0].D())*100.0, 'g', 3)));
// 		}
// 	}
}

void ColorPickerMod::iniData()
{
	m_param = new ColorPickerParam;
	m_image = new Hobject;
	m_inAreaRegion = new Hobject;
	m_allReg = new Hobject;
	m_unionRoiReg = new Hobject;
	m_unionMinusReg = new Hobject;
	m_lightReg = new Hobject;
	createPins();

	m_detRes = noDet;
	m_allok = 0;

	m_roiArea = 0;

	m_singleAreaMin = 0;
	m_singleAreaMax = 999999;
	m_singleAreaOK = true;

	m_totalArea = 0;
	m_totalAreaOK = true;
}

void ColorPickerMod::createPins()
{
	addPin(&p_im, "im");
	addPin(&p_homMat2d, "hm2d");
	addPin(&p_corners, "corners");
	addPin(&p_inReg, "reg");
}

void ColorPickerMod::mselect_shape(const Hobject & Regions, Hobject * SelectedRegions, const char * Features, const char * Operation, double Min, double Max)
{
	select_shape(Regions, SelectedRegions, Features, Operation, Min, Max >= Min ? Max : Min);
}

