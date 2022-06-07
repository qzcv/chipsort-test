#include "surface_pMod.h"
#include "surface_pParam.h"
#include <QObject>
#include <ImageView.h>
#include <resultText.h>
#include "HalconCpp.h"
#include "halOpenCV.h"

surface_pMod::surface_pMod()
{
	iniData();
}

surface_pMod::surface_pMod(const QString& dirPath)
{
	iniData();
	load(dirPath, StructLevel);
}

surface_pMod::~surface_pMod()
{
	delete m_param;
	delete m_image;

	delete m_delRegion[0];
	delete m_delRegion[1];
	delete m_delRegion[2];
	delete m_inAreaRegion;
	delete m_showUnionDelReg;
	delete m_showUnionRoiReg;
}

int surface_pMod::command(int cmdID, void* dataIn, void* dataOut)
{
	return 0;
}

int surface_pMod::afterSetProperty(MetaProperty*)
{
	return 0;
}

void surface_pMod::save(const QString &dirPath)
{
	m_param->readWriteParam(false, dirPath, StructLevel);
	resetParamChanged();
}

void surface_pMod::load(const QString &dirPath, QvsParamLevel level)
{
	m_param->readWriteParam(true,dirPath,level);
}

int surface_pMod::run(const QString &funName)
{
// 	for (int i = 0;i < MAX_TESTITEMS_NUM;i++) {
// 		m_testItemsStatus[i] = 1;
// 	}
//	bool allok = true;
// 	if (!m_image || m_image->Id() == H_EMPTY_REGION) {
// 		allok = false;
// 		setDetectOutData(AllOk, allok);
// 		setDetectOutData(ImageFull, false);
// 		m_testItemsStatus[0] = 0;
// 		return false;
// 	}
// 	else {
// 		setDetectOutData(ImageFull, true);
// 	}
	//int allok = 0;
	m_detRes = noDet;
	m_allok = 0;
	if (p_im->empty())
	{
		m_detRes = emptyImg;
		return -1;
	}		
	if (m_param->referSection&&p_homMat2D->size() != cv::Size(3, 2))
	{
		m_detRes = wrongMat;
		return -1;
	}		

	Hobject dstImg;
	calTargetReg(&dstImg);
	
	//Hlong objcount;
	m_detRes = detSucceed;
	count_obj(dstImg, /*&objcount*/&m_objCnt);
	if (/*objcount*/m_objCnt == 0) {
		//setDetectOutData(AllOk, true);
		//setDetectOutData(AreaCount, objcount);
		return 0;
	}
	//setDetectOutData(AreaCount, objcount);
	//setDetectOutObject(AllRegionDisp, dstImg);
	Hobject tmpu,tmpOutReg;
	union1(dstImg, &tmpu);
	copy_obj(tmpu, &tmpOutReg, 1, 1);
	HalOpenCV::hal2cvRegion(tmpOutReg, &(*p_outReg));
	//copy_obj(tmpu, m_areaRegion, 1, 1);
	//setDetectOutData(DISPCONTOURPOINTER, (Hlong)m_areaRegion);

	if (m_param->area_count_valid) {
		//bool countOk = true;
		m_objCntOK = true;
		if (/*objcount*/m_objCnt<m_param->area_count_min || /*objcount*/m_objCnt>m_param->area_count_max) {
			//countOk = false;
			m_objCntOK = false;
			m_allok = -6;
			//allok = false;
			//m_testItemsStatus[5] = 0;
		}
		//setDetectOutData(AreaCountOk, countOk);
	}
	if (m_param->single_badArea_valid) {
		Hobject singleMax;
		select_shape_std(dstImg, &singleMax, "max_area", 95);
		HTuple Area, row, col;
		area_center(singleMax, &Area, &row, &col);
		//bool singleOk = true;
		m_singleBadOK = true;
		m_singleBad = Area[0].I()*(m_param->pixSize*m_param->pixSize);
		m_singleBadMax = singleMax;
		if (m_singleBad < m_param->single_badArea_min ||
			m_singleBad > m_param->single_badArea_max) {
			//singleOk = false;
			m_singleBadOK = false;
			m_allok = -2;
			//allok = false;
			//m_testItemsStatus[1] = 0;
		}
// 		setDetectOutData(SingleBadOk, singleOk);
// 		setDetectOutData(SingleBad, Area[0].I()*(m_param->pixSize*m_param->pixSize));
// 		setDetectOutObject(MaxObject, singleMax);
	}
	HTuple regionRow, regionCol, regionArea, row1, col1, row2, col2;
	area_center(dstImg, &regionArea, &regionRow, &regionCol);
	smallest_rectangle1(dstImg, &row1, &col1, &row2, &col2);
	long totalArea = 0;
	m_lenVec.resize(m_objCnt);
	m_widVec.resize(m_objCnt);
	m_lenOKVec.resize(m_objCnt);
	m_widOKVec.resize(m_objCnt);
	for (int i = 0;i < /*objcount*/m_objCnt;i++) {
		if (m_param->area_width_valid) {
			//bool widthok1 = true;
			double wid = (col2[i].I() - col1[i].I())*m_param->pixSize;
			m_widVec[i] = wid;
			m_widOKVec[i] = true;
			if (wid<m_param->area_width_min || 
				wid>m_param->area_width_max) {
				//widthok1 = false;
				m_widOKVec[i] = false;
				m_allok = -5;
				//allok = false;
				//m_testItemsStatus[4] = 0;
			}
// 			setDetectOutData(Width, (col2[i].I() - col1[i].I())*m_param->pixSize, i);
// 			setDetectOutData(WidthOk, widthok1, i);
		}
		if (m_param->area_length_valid) {
			//bool lengthok1 = true;
			double len = (row2[i].I() - row1[i].I())*m_param->pixSize;
			m_lenVec[i] = len;
			m_lenOKVec[i] = true;
			if (len<m_param->area_length_min || 
				len>m_param->area_length_max) {
				//lengthok1 = false;
				m_lenOKVec[i] = false;
				m_allok = -4;
				//allok = false;
				//m_testItemsStatus[3] = 0;
			}
// 			setDetectOutData(LengthOk, lengthok1, i);
// 			setDetectOutData(Length, (row2[i].I() - row1[i].I())*m_param->pixSize, i);
		}
		totalArea += regionArea[i].I();
	}
	if (m_param->total_badArea_valid) {
		//bool totalok1 = true;
		m_totalBadOK = true;
		m_totalBad = totalArea*(m_param->pixSize*m_param->pixSize);
		if (m_totalBad < m_param->total_badArea_min || 
			m_totalBad > m_param->total_badArea_max) {
			//totalok1 = false;
			m_totalBadOK = false;
			m_allok = -3;
			//allok = false;
			//m_testItemsStatus[2] = 0;
		}
// 		setDetectOutData(TotalBadOk, totalok1);
// 		setDetectOutData(TotalBad, totalArea*(m_param->pixSize*m_param->pixSize));
	}
	//setDetectOutData(AllOk, allok);
	return m_allok;

	//return 0;
}

void surface_pMod::viewResult(ImageView *iv, const QString &funName, int)
{
	if (m_detRes == noDet)
		return;
	else if (m_detRes == emptyImg)
		return;
	else if (m_detRes == wrongMat)
		return;
	else if (m_detRes == detFailed)
		return;
	else if (m_detRes == detSucceed)
	{
		iv->setFilled(false);
		if (m_allok != 0) {
			iv->setColor(255, 0, 0);
		}
		else {
			iv->setColor(0, 255, 0);
		}
		if (m_allok != 0 || m_param->isShowDel) {
			RlRegion roiReg;
			HalOpenCV::hal2cvRegion(*m_showUnionRoiReg, &roiReg);
			iv->dispRegion(&roiReg);
		}
		iv->setColor(255, 255, 0);
		iv->setFilled(false);
		if (m_allok != 0 || m_param->isShowDel) {
			RlRegion delReg;
			HalOpenCV::hal2cvRegion(*m_showUnionDelReg, &delReg);
			iv->dispRegion(&delReg);
		}

		if (m_objCnt == 0) {
			return;
		}
		else {
			Hobject detectRegion;
			HalOpenCV::cv2halRegion(*p_outReg, &detectRegion);
			if (m_allok != 0)
			{
				if ((m_param->area_count_valid && !m_objCntOK) ||
					(m_param->total_badArea_valid && !m_totalBadOK))
				{
					iv->setColor(Qt::red);
					RlRegion reg;
					HalOpenCV::hal2cvRegion(detectRegion, &reg);
					iv->dispRegion(&reg);
					return;
				}
				connection(detectRegion, &detectRegion);

				HTuple num;
				count_obj(detectRegion, &num);

				for (int j = 1;j <= num[0].I();++j)
				{
					Hobject partRegion;
					select_obj(detectRegion, &partRegion, j);

					bool isRed = false;

					//判断是否启用单个面积作为检测条件
					if (m_param->single_badArea_valid)
					{
						HTuple area, row, col;
						area_center(partRegion, &area, &row, &col);

						//判断单个面积是否在判定范围之内，如果是，则通过
						if (area[0].I()*pow(m_param->pixSize, 2) < m_param->single_badArea_min ||
							area[0].I()*pow(m_param->pixSize, 2) > m_param->single_badArea_max)
						{
							isRed = true;
						}
					}

					//判断是否启用单个区域的宽，高作为检测条件
					HTuple row1, col1, row2, col2;
					smallest_rectangle1(partRegion, &row1, &col1, &row2, &col2);
					if (m_param->area_width_valid)
					{
						if (abs(col2[0].I() - col1[0].I())*m_param->pixSize<m_param->area_width_min ||
							abs(col2[0].I() - col1[0].I())*m_param->pixSize>m_param->area_width_max)
						{
							isRed = true;
						}
					}
					if (m_param->area_length_valid)
					{
						if ((row2[0].I() - row1[0].I())*m_param->pixSize<m_param->area_length_min ||
							(row2[0].I() - row1[0].I())*m_param->pixSize>m_param->area_length_max)
						{
							isRed = true;
						}
					}
					isRed ? iv->setColor(255, 0, 0) : iv->setColor(0, 255, 0);
					RlRegion partReg;
					HalOpenCV::hal2cvRegion(partRegion, &partReg);
					iv->dispRegion(&partReg);
				}
			}
			else
			{
				iv->setColor(0, 255, 0);
				RlRegion dispReg;
				HalOpenCV::hal2cvRegion(detectRegion, &dispReg);
				iv->dispRegion(&dispReg);
			}
		}
	}	
}

void surface_pMod::textResult(ResultText *text, const QString &funName)
{
	if (m_detRes == noDet)
		return;
	else if (m_detRes == emptyImg)
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
		if (m_objCnt == 0) {
			text->setTextColor(Qt::black);
			text->append(QObject::tr("no object found"));
			return;
		}

		if (m_param->area_count_valid) {
			if (m_objCntOK == 0) {
				text->setTextColor(Qt::red);
			}
			else {
				text->setTextColor(Qt::black);
			}
			text->append(QObject::tr("area count: %1(%2,%3)").arg(m_objCnt)
				.arg(m_param->area_count_min).arg(m_param->area_count_max));
		}
		if (m_param->single_badArea_valid) {
			if (m_singleBadOK == 0) {
				text->setTextColor(Qt::red);
			}
			else {
				text->setTextColor(Qt::black);
			}
			if (m_param->IsPixOrAct == 0) {
				double ratio = m_param->pixSize*m_param->pixSize;
				text->append(QObject::tr("max single area: %1(%2,%3)").arg(int(m_singleBad / ratio))
					.arg(int(m_param->single_badArea_min / ratio)).arg(int(m_param->single_badArea_max / ratio)));
			}
			else {
				text->append(QObject::tr("max single area: %1(%2,%3)").arg(m_singleBad, 0, 'f', 6)
					.arg(m_param->single_badArea_min, 0, 'f', 6).arg(m_param->single_badArea_max, 0, 'f', 6));
			}
		}
		for (int i = 0;i < m_objCnt;i++) {
			if (m_param->area_length_valid) {
				if (m_lenOKVec[i] == 0) {
					text->setTextColor(Qt::red);
				}
				else {
					text->setTextColor(Qt::black);
				}
				if (m_param->IsPixOrAct == 0) {
					double ratio = m_param->pixSize;
					text->append(QObject::tr(" length%1: %2(%3,%4)").arg(i + 1).arg(int(m_lenVec[i] / ratio))
						.arg(int(m_param->area_length_min / ratio)).arg(int(m_param->area_length_max / ratio)));
				}
				else {
					text->append(QObject::tr(" length%1: %2(%3,%4)").arg(i + 1).arg(m_lenVec[i], 0, 'f', 6)
						.arg(m_param->area_length_min, 0, 'f', 6).arg(m_param->area_length_max, 0, 'f', 6));
				}
			}
			if (m_param->area_width_valid) {
				if (m_widOKVec[i] == 0) {
					text->setTextColor(Qt::red);
				}
				else {
					text->setTextColor(Qt::black);
				}
				if (m_param->IsPixOrAct == 0) {
					double ratio = m_param->pixSize;
					text->append(QObject::tr(" width%1: %2(%3,%4)").arg(i + 1).arg(int(m_widVec[i] / ratio))
						.arg(int(m_param->area_width_min / ratio)).arg(int(m_param->area_width_max / ratio)));
				}
				else {
					text->append(QObject::tr(" width%1: %2(%3,%4)").arg(i + 1).arg(m_widVec[i], 0, 'f', 6)
						.arg(m_param->area_width_min, 0, 'f', 6).arg(m_param->area_width_max, 0, 'f', 6));
				}
			}
		}
		if (m_param->total_badArea_valid) {
			if (m_totalBadOK == 0) {
				text->setTextColor(Qt::red);
			}
			else {
				text->setTextColor(Qt::black);
			}
			if (m_param->IsPixOrAct == 0) {
				double ratio = m_param->pixSize*m_param->pixSize;
				text->append(QObject::tr(" total bad area: %1(%2,%3)").arg(int(m_totalBad / ratio))
					.arg(int(m_param->total_badArea_min / ratio)).arg(int(m_param->total_badArea_max / ratio)));
			}
			else {
				text->append(QObject::tr(" total bad area: %1(%2,%3)").arg(m_totalBad, 0, 'f', 6)
					.arg(m_param->total_badArea_min, 0, 'f', 6).arg(m_param->total_badArea_max, 0, 'f', 6));
			}
		}
	}
}

void surface_pMod::iniData()
{
	m_param = new surface_pParam;
	createPins();
	m_image = new Hobject;
	m_detRes = noDet;
	m_allok = 0;

	m_objCnt = 1;
	m_objCnt = true;

	m_singleBad = 0;
	m_singleBadOK = true;

	m_lenVec.clear();
	m_lenOKVec.clear();
	m_widVec.clear();
	m_widOKVec.clear();

	m_delRegion[0] = new Hobject;
	m_delRegion[1] = new Hobject;
	m_delRegion[2] = new Hobject;
	m_inAreaRegion = new Hobject;
	m_showUnionDelReg = new Hobject;
	m_showUnionRoiReg = new Hobject;
}

void surface_pMod::createPins()
{
	addPin(&p_im, "im");
	addPin(&p_homMat2D, "hm2d");
	addPin(&p_pixelSize, "pSz", "pSz", "pixelSize");
	addPin(&p_corners, "corners");
	addPin(&p_inReg, "inReg");

	p_pixelSize.setVisible(false);
	p_corners.setVisible(false);
	p_inReg.setVisible(false);
	for (auto i = 0;i < 3;++i)
	{
		addPin(&p_delReg[i], QString("del%1").arg(i), QString("del%1").arg(i), QString::fromLocal8Bit("排除区域%1").arg(i));
		p_delReg[i].setVisible(false);
	}

	addPin(&p_outReg, "outReg");
	p_outReg.setVisible(false);
}

void surface_pMod::calTargetReg(Hobject * obj, Hobject *showTmp)
{
	HalOpenCV::cv2halImg(*p_im, m_image, false);

	Hobject dstImg;
	double roirow, roicol, roilength1, roilength2, roiangle, roir;    //采用基准模式时的旋转角度
	if (m_param->roiType == 0) {
		roirow = m_param->deleteRegionVector[0].row;
		roicol = m_param->deleteRegionVector[0].col;
		roilength1 = m_param->deleteRegionVector[0].length1;
		roilength2 = m_param->deleteRegionVector[0].length2;
		roiangle = m_param->deleteRegionVector[0].angle;
	}
	else if (m_param->roiType == 1) {
		roirow = m_param->cirRow;
		roicol = m_param->cirCol;
		roir = m_param->cirR;
	}
	if (m_param->referSection) {
		try {
			//getDetectInData(HomMat2D, m_homeMat2D);
			HalOpenCV::cv2HalHomMat2D(*p_homMat2D, &m_homeMat2D);

			double tv = m_homeMat2D[0].D();
		}
		catch (HException& ex) {
			hom_mat2d_identity(&m_homeMat2D);
		}
		//setDetectOutData(TmpHomMat, m_homeMat2D);
	}
	HTuple wid, hei;
	Hobject ImgRec;
	Hobject roiRec;
	Hobject unionRec;
	get_image_size(*m_image, &wid, &hei);
	gen_rectangle1(&ImgRec, 0, 0, hei - 1, wid - 1);
	bool isstart = false;
	//HTuple deleteregion1;
	HTuple fianalrow, fianalcol;
	HTuple inareap;
	if (m_param->roiType == 2 || m_param->IsReferPtoDel) {
		//getDetectInData(ReferRow, fianalrow);
		//getDetectInData(ReferCol, fianalcol);
		for (auto i = 0;i < p_corners->size();++i)
		{
			fianalrow[i] = p_corners->at(i).y;
			fianalcol[i] = p_corners->at(i).x;
		}
		try {
			if (abs(m_param->zoom_up) > 0) {
				fianalrow[0] = double(fianalrow[0].D() - m_param->zoom_up);
				fianalrow[1] = double(fianalrow[1].D() - m_param->zoom_up);
			}
			if (abs(m_param->zoom_dw) > 0) {
				fianalrow[2] = double(fianalrow[2].D() + m_param->zoom_dw);
				fianalrow[3] = double(fianalrow[3].D() + m_param->zoom_dw);
			}
			if (abs(m_param->zoom_lf) > 0) {
				fianalcol[0] = double(fianalcol[0].D() - m_param->zoom_lf);
				fianalcol[3] = double(fianalcol[3].D() - m_param->zoom_lf);
			}
			if (abs(m_param->zoom_rt) > 0) {
				fianalcol[1] = double(fianalcol[1].D() + m_param->zoom_rt);
				fianalcol[2] = double(fianalcol[2].D() + m_param->zoom_rt);
			}
			fianalcol[4] = fianalcol[0].D();
			fianalrow[4] = fianalrow[0].D();
		}
		catch (HException& ex) {
			fianalcol[4] = fianalrow[4] = fianalcol[0] = fianalrow[0] = 20;
			fianalcol[1] = 50;fianalrow[1] = 20;
			fianalcol[2] = 50;fianalrow[2] = 50;
			fianalcol[3] = 20;
			fianalrow[3] = 50;
		}
	}

	for (int i = 0;i < 3;i++) {
		if (m_param->deleteArea_section[i]) {
			// 			if (i == 0)
			// 				getDetectInData(DeleteRegion1, deleteregion1);
			// 			else if (i == 1)
			// 				getDetectInData(DeleteRegion2, deleteregion1);
			// 			else if (i == 2)
			// 				getDetectInData(DeleteRegion3, deleteregion1);
			try {
				//copy_obj(*(Hobject*)deleteregion1[0].L(), m_delRegion[i], 1, 1);
				Hobject delReg;
				HalOpenCV::cv2halRegion(*p_delReg[i], &delReg);
				copy_obj(delReg, m_delRegion[i], 1, 1);
				if (m_param->deleteArea_zoom[0] > 0) {
					dilation_rectangle1(*m_delRegion[i], m_delRegion[i], m_param->deleteArea_zoom[i], m_param->deleteArea_zoom[i]);
				}
				else if (m_param->deleteArea_zoom[0] < 0) {
					erosion_rectangle1(*m_delRegion[i], m_delRegion[i], abs(m_param->deleteArea_zoom[i]), abs(m_param->deleteArea_zoom[i]));
				}
			}
			catch (HException& ex) {
				gen_rectangle1(m_delRegion[i], 50 + 10 * i, 50 + 10 * i, 150 + 10 * i, 150 + 10 * i);
			}
		}
	}
	for (int i = 0;i < 3;i++) {
		if (m_param->deleteArea_section[i] != 0) {
			if (!isstart) {
				Hlong cont;
				count_obj(*m_delRegion[i], &cont);
				copy_obj(*m_delRegion[i], &unionRec, 1, cont);
			}
			else {
				Hlong cont;
				count_obj(*m_delRegion[i], &cont);
				copy_obj(*m_delRegion[i], &roiRec, 1, cont);
				union2(roiRec, unionRec, &unionRec);
			}
			isstart = true;
		}
	}
	for (int i = 0;i < m_param->addDeleteArea;i++) {
		gen_rectangle2(&roiRec, m_param->deleteRegionVector[1 + i].row, m_param->deleteRegionVector[1 + i].col, m_param->deleteRegionVector[1 + i].angle
			, m_param->deleteRegionVector[1 + i].length1, m_param->deleteRegionVector[1 + i].length2);
		if (m_param->referSection == 1) {
			affine_trans_region(roiRec, &roiRec, m_homeMat2D, "false");
		}
		union2(roiRec, unionRec, &unionRec);
	}
	Hobject tmpReg;                //引用传入点区域为排除区域进行排除
	if (m_param->IsReferPtoDel) {   //点区域
		gen_region_polygon_filled(&tmpReg, fianalrow, fianalcol);
		if (m_param->delPointZoom > 0) {
			dilation_rectangle1(tmpReg, &tmpReg, m_param->delPointZoom, m_param->delPointZoom);
		}
		else if (m_param->delPointZoom < 0) {
			erosion_rectangle1(tmpReg, &tmpReg, abs(m_param->delPointZoom), abs(m_param->delPointZoom));
		}
		union2(tmpReg, unionRec, &unionRec);
	}
	//setDetectOutObject(TmpUnionDelRegion, unionRec);   //将计算到的排除区域整合传递给Viewresult显示
	*m_showUnionDelReg = unionRec;	//将计算到的排除区域整合传递给Viewresult显示
	difference(ImgRec, unionRec, &unionRec);

	Hlong channum;
	Hobject grayimg;
	count_channels(*m_image, &channum);
	if (channum == 3) {
		rgb1_to_gray(*m_image, &grayimg);
	}
	else {
		copy_image(*m_image, &grayimg);
	}

	reduce_domain(grayimg, unionRec, &dstImg);
	Hobject RoiRec, delEdgeRoi, meanImg;

	if (m_param->roiType == 0) {         //矩形区域
		if (m_param->roundVal > 0) {
			Hobject smallrecxld;
			gen_rectangle2_contour_xld(&smallrecxld, roirow, roicol, roiangle, roilength1, roilength2);
			HTuple smallrow, smallcol;
			get_contour_xld(smallrecxld, &smallrow, &smallcol);
			Hobject tmpxld;
			HTuple roundR;
			for (int t = 0;t < 5;t++) {
				roundR[t] = m_param->roundVal;
			}
			gen_contour_polygon_rounded_xld(&tmpxld, smallrow, smallcol, roundR, 1);
			gen_region_contour_xld(tmpxld, &RoiRec, "filled");
		}
		else {
			gen_rectangle2(&RoiRec, roirow, roicol, roiangle, roilength1, roilength2);
		}
		if (m_param->referSection == 1) {
			affine_trans_region(RoiRec, &RoiRec, m_homeMat2D, "false");
		}
	}
	else if (m_param->roiType == 1) {    //圆区域
		gen_circle(&RoiRec, roirow, roicol, roir);
		if (m_param->referSection == 1) {
			affine_trans_region(RoiRec, &RoiRec, m_homeMat2D, "false");
		}
	}
	else if (m_param->roiType == 2) {   //点区域

		if (m_param->roundVal > 0) {
			Hobject tmpxld;
			HTuple roundR;
			for (int t = 0;t < 5;t++) {
				roundR[t] = m_param->roundVal;
			}
			gen_contour_polygon_rounded_xld(&tmpxld, fianalrow, fianalcol, roundR, 1);
			gen_region_contour_xld(tmpxld, &RoiRec, "filled");
		}
		else {
			gen_region_polygon_filled(&RoiRec, fianalrow, fianalcol);
		}
	}
	else if (m_param->roiType == 3) { //面区域
									  //getDetectInData(InputAreaRegion, inareap);
		try {
			//copy_obj(*(Hobject*)inareap[0].L(), m_inAreaRegion, 1, 1);
			Hobject tmpInReg;
			HalOpenCV::cv2halRegion(*p_inReg, &tmpInReg);
			copy_obj(tmpInReg, m_inAreaRegion, 1, 1);
			//copy_obj(*p_inReg, m_inAreaRegion, 1, 1);
			if (m_param->inputAreaZoom > 0) {
				dilation_rectangle1(*m_inAreaRegion, m_inAreaRegion, m_param->inputAreaZoom, m_param->inputAreaZoom);
			}
			else if (m_param->inputAreaZoom < 0) {
				erosion_rectangle1(*m_inAreaRegion, m_inAreaRegion, abs(m_param->inputAreaZoom), abs(m_param->inputAreaZoom));
			}
		}
		catch (HException& ex) {
			gen_empty_region(m_inAreaRegion);
		}
		copy_obj(*m_inAreaRegion, &RoiRec, 1, 1);
		//setDetectOutObject(TmpAreaRegion, *m_inAreaRegion);
	}
	else if (m_param->roiType == 4) {
		copy_obj(m_param->roiRegion, &RoiRec, 1, 1);
		if (m_param->inputAreaZoom > 0) {
			dilation_rectangle1(RoiRec, &RoiRec, m_param->inputAreaZoom, m_param->inputAreaZoom);
		}
		else if (m_param->inputAreaZoom < 0) {
			erosion_rectangle1(RoiRec, &RoiRec, abs(m_param->inputAreaZoom), abs(m_param->inputAreaZoom));
		}
		if (m_param->referSection) {
			affine_trans_region(RoiRec, &RoiRec, m_homeMat2D, "false");
		}
	}
	//setDetectOutObject(TmpUnionRoiRegion, RoiRec);
	*m_showUnionRoiReg = RoiRec;
	reduce_domain(dstImg, RoiRec, &dstImg);
	if (m_param->IsGauss) {
		int vaaa = (m_param->GaussSize % 2) == 0 ? 1 : 0;
		gauss_image(dstImg, &dstImg, vaaa + m_param->GaussSize);
	}
	Hobject dstdomain;
	get_domain(dstImg, &dstdomain);
	if (m_param->testType == surface_pParam::fixedThre) {
		threshold(dstImg, &dstImg, m_param->fixThreMin, m_param->fixThreMax < m_param->fixThreMin ? m_param->fixThreMin : m_param->fixThreMax);
		if (m_param->closeVal > 0) {
			closing_rectangle1(dstImg, &dstImg, m_param->closeVal, m_param->closeVal);
		}
		if (m_param->openVal > 0) {
			opening_rectangle1(dstImg, &dstImg, m_param->openVal, m_param->openVal);
		}
		connection(dstImg, &dstImg);
		//opening_rectangle1(dstImg,&dstImg,2,2);
		mselect_shape(dstImg, &dstImg, "area", "and", int((m_param->badArea_min / (m_param->pixSize*m_param->pixSize) + 0.5) > 999999 ? 999999 : (m_param->badArea_min / (m_param->pixSize*m_param->pixSize) + 0.5)),
			int((m_param->badArea_max / (m_param->pixSize*m_param->pixSize) + 0.5) > 999999 ? 999999 : (m_param->badArea_max / (m_param->pixSize*m_param->pixSize) + 0.5)));
		mselect_shape(dstImg, &dstImg, "height", "and", int(m_param->badLengthMin / m_param->pixSize + 0.5), int(m_param->badLengthMax / m_param->pixSize + 0.5));
		mselect_shape(dstImg, &dstImg, "width", "and", int(m_param->badWidthMin / m_param->pixSize + 0.5), int(m_param->badWidthMax / m_param->pixSize + 0.5));
		//select_shape(dstImg, &dstImg, "circularity", "and", 0.8, 1.0);
	}
	else if (m_param->testType == surface_pParam::detHole) {
		threshold(dstImg, &dstImg, m_param->fixThreMin, m_param->fixThreMax < m_param->fixThreMin ? m_param->fixThreMin : m_param->fixThreMax);
		dilation_circle(dstImg, &dstImg, 1);
		if (m_param->closeVal > 0) {
			closing_rectangle1(dstImg, &dstImg, m_param->closeVal, m_param->closeVal);
		}
		if (m_param->openVal > 0) {
			opening_rectangle1(dstImg, &dstImg, m_param->openVal, m_param->openVal);
		}
		connection(dstImg, &dstImg);
		mselect_shape(dstImg, &dstImg, "area", "and", int((m_param->badArea_min / (m_param->pixSize*m_param->pixSize) + 0.5) > 999999 ? 999999 : (m_param->badArea_min / (m_param->pixSize*m_param->pixSize) + 0.5)),
			int((m_param->badArea_max / (m_param->pixSize*m_param->pixSize) + 0.5) > 999999 ? 999999 : (m_param->badArea_max / (m_param->pixSize*m_param->pixSize) + 0.5)));
		mselect_shape(dstImg, &dstImg, "height", "and", int(m_param->badLengthMin / m_param->pixSize + 0.5), int(m_param->badLengthMax / m_param->pixSize + 0.5));
		mselect_shape(dstImg, &dstImg, "width", "and", int(m_param->badWidthMin / m_param->pixSize + 0.5), int(m_param->badWidthMax / m_param->pixSize + 0.5));
		mselect_shape(dstImg, &dstImg, "holes_num", "and", m_param->holeNumMin, m_param->holeNumMax);
		mselect_shape(dstImg, &dstImg, "area_holes", "and", m_param->holeAreaMin, m_param->holeAreaMax);
		//shape_trans(dstImg,&dstImg,"convex");
	}
	//else if(m_param->testType==SurfacePixelParam::Fourier){
	//	Hobject gauss1,gauss2;
	//	gen_gauss_filter(&gauss1,m_param->sigma1,m_param->sigma2,);
	//}
	else if (m_param->testType == surface_pParam::ragAndLoss) {
		threshold(dstImg, &dstImg, m_param->fixThreMin, m_param->fixThreMax < m_param->fixThreMin ? m_param->fixThreMin : m_param->fixThreMax);
		connection(dstImg, &dstImg);
		mselect_shape(dstImg, &dstImg, "area", "and", m_param->badArea_min, m_param->badArea_max);
		mselect_shape(dstImg, &dstImg, "height", "and", m_param->badLengthMin, m_param->badLengthMax);
		mselect_shape(dstImg, &dstImg, "width", "and", m_param->badWidthMin, m_param->badWidthMax);
		if (m_param->IsFitRec) {
			if (m_param->closeVal > 0) {
				closing_rectangle1(dstImg, &dstImg, m_param->closeVal, m_param->closeVal);
			}
			if (m_param->openVal > 0) {
				opening_rectangle1(dstImg, &dstImg, m_param->openVal, m_param->openVal);
			}

		}
		union1(dstImg, &dstImg);
		if (showTmp)
			copy_obj(dstImg, showTmp, 1, 1);
		Hlong objc;
		count_obj(dstImg, &objc);
		if (objc != 0) {
			Hobject tmpimg1, tmpimg2;
			Hobject mrecImg;HTuple trow, tcol, tangle, tlen1, tlen2;
			if (m_param->IsFitRec) {
				smallest_rectangle2(dstImg, &trow, &tcol, &tangle, &tlen1, &tlen2);
				gen_rectangle2(&mrecImg, trow[0].D(), tcol[0].D(), tangle[0].D(), tlen1[0].D(), tlen2[0].D());
				copy_obj(dstImg, &tmpimg1, 1, 1);
				copy_obj(dstImg, &tmpimg2, 1, 1);
				if (m_param->minLoss > 0) {
					difference(mrecImg, tmpimg1, &tmpimg1);  //缺失
					opening_rectangle1(tmpimg1, &tmpimg1, m_param->minLoss, m_param->minLoss);
					connection(tmpimg1, &tmpimg1);
					mselect_shape(tmpimg1, &tmpimg1, "area", "and", m_param->minRagLossArea, 10000000);
					//select_shape(tmpimg1,&tmpimg1,"inner_radius","and",m_param->minLoss,10000000);
				}
				else {
					gen_empty_region(&tmpimg1);
				}
				if (m_param->minRag > 0) {
					difference(tmpimg2, mrecImg, &tmpimg2);  //毛刺
					opening_rectangle1(tmpimg2, &tmpimg2, m_param->minRag, m_param->minRag);
					connection(tmpimg2, &tmpimg2);
					mselect_shape(tmpimg2, &tmpimg2, "area", "and", m_param->minRagLossArea, 10000000);
					//select_shape(tmpimg2,&tmpimg2,"inner_radius","and",m_param->minRag,10000000);
				}
				else {
					gen_empty_region(&tmpimg2);
				}
				union2(tmpimg2, tmpimg1, &dstImg);
			}
			else {
				if (m_param->minRag > 0) {
					copy_obj(dstImg, &tmpimg1, 1, 1);
					opening_circle(tmpimg1, &tmpimg1, m_param->minRag);
					connection(tmpimg1, &tmpimg1);
					union1(tmpimg1, &tmpimg1);
					difference(dstImg, tmpimg1, &tmpimg1);
					connection(tmpimg1, &tmpimg1);
					mselect_shape(tmpimg1, &tmpimg1, "area", "and", m_param->minRagLossArea, 10000000);
				}
				else {
					gen_empty_region(&tmpimg1);
				}
				if (m_param->minLoss > 0) {
					copy_obj(dstImg, &tmpimg2, 1, 1);
					closing_circle(tmpimg2, &tmpimg2, m_param->minLoss);
					connection(tmpimg2, &tmpimg2);
					union1(tmpimg2, &tmpimg2);
					difference(tmpimg2, dstImg, &tmpimg2);
					connection(tmpimg2, &tmpimg2);
					mselect_shape(tmpimg2, &tmpimg2, "area", "and", m_param->minRagLossArea, 10000000);
				}
				else {
					gen_empty_region(&tmpimg2);
				}
				union2(tmpimg2, tmpimg1, &dstImg);
			}
		}
	}
	else {

		Hobject diff;
		//reduce_domain(unionRec,RoiRec,&diff);
		difference(ImgRec, dstdomain, &diff);

		HTuple absHisto, relativeHisto;

		gray_histo(dstdomain, dstImg, &absHisto, &relativeHisto);
		HTuple func, smoothFunc;
		create_funct_1d_array(absHisto, &func);
		smooth_funct_1d_gauss(func, 2, &smoothFunc);
		HTuple XVal, YVal;
		funct_1d_to_pairs(smoothFunc, &XVal, &YVal);
		HTuple Indices;
		tuple_sort_index(YVal, &Indices);
		HTuple Sel;
		tuple_select(Indices, 255, &Sel);
		int dddd = Sel[0].I();
		int ssd = Indices.Num();
		if (m_param->testType == surface_pParam::meanThre)                   //平滑背景
		{
			Hobject diffimg;
			if (m_param->IsFillOut) {
				paint_region(diff, grayimg, &diffimg, Sel[0].I(), "fill");
				reduce_domain(diffimg, dstdomain, &diffimg);
			}
			else {
				copy_image(dstImg, &diffimg);
			}
			mean_image(diffimg, &meanImg, m_param->meanSize, m_param->meanSize);
		}
		else if (m_param->testType == surface_pParam::averageThre)            //平均背景
		{
			gen_image_proto(grayimg, &meanImg, Sel[0].D());
			reduce_domain(meanImg, dstdomain, &meanImg);
		}

		Hobject dstDark, dstLight;
		dyn_threshold(dstImg, meanImg, &dstDark, m_param->black_bad_detect, "dark");
		dyn_threshold(dstImg, meanImg, &dstLight, m_param->white_bad_detect, "light");
		if (m_param->closeVal > 0) {
			closing_rectangle1(dstDark, &dstDark, m_param->closeVal, m_param->closeVal);
			closing_rectangle1(dstLight, &dstLight, m_param->closeVal, m_param->closeVal);
		}
		if (m_param->openVal > 0) {
			opening_rectangle1(dstDark, &dstDark, m_param->openVal, m_param->openVal);
			opening_rectangle1(dstLight, &dstLight, m_param->openVal, m_param->openVal);
		}
		connection(dstDark, &dstDark);
		connection(dstLight, &dstLight);
		mselect_shape(dstDark, &dstDark, "area", "and", int((m_param->badArea_min / (m_param->pixSize*m_param->pixSize) + 0.5) > 999999 ? 999999 : (m_param->badArea_min / (m_param->pixSize*m_param->pixSize) + 0.5)),
			int((m_param->badArea_max / (m_param->pixSize*m_param->pixSize) + 0.5) > 999999 ? 999999 : (m_param->badArea_max / (m_param->pixSize*m_param->pixSize) + 0.5)));
		mselect_shape(dstLight, &dstLight, "area", "and", int((m_param->badArea_min / (m_param->pixSize*m_param->pixSize) + 0.5) > 999999 ? 999999 : (m_param->badArea_min / (m_param->pixSize*m_param->pixSize) + 0.5)),
			int((m_param->badArea_max / (m_param->pixSize*m_param->pixSize) + 0.5) > 999999 ? 999999 : (m_param->badArea_max / (m_param->pixSize*m_param->pixSize) + 0.5)));
		mselect_shape(dstDark, &dstDark, "height", "and", int(m_param->badLengthMin / m_param->pixSize + 0.5), int(m_param->badLengthMax / m_param->pixSize + 0.5));
		mselect_shape(dstLight, &dstLight, "height", "and", int(m_param->badLengthMin / m_param->pixSize + 0.5), int(m_param->badLengthMax / m_param->pixSize + 0.5));
		mselect_shape(dstDark, &dstDark, "width", "and", int(m_param->badWidthMin / m_param->pixSize + 0.5), int(m_param->badWidthMax / m_param->pixSize + 0.5));
		mselect_shape(dstLight, &dstLight, "width", "and", int(m_param->badWidthMin / m_param->pixSize + 0.5), int(m_param->badWidthMax / m_param->pixSize + 0.5));
		union2(dstDark, dstLight, &dstImg);
		connection(dstImg, &dstImg);
	}
	*obj = dstImg;
}

void surface_pMod::mselect_shape(const Hobject & Regions, Hobject * SelectedRegions, const char * Features, const char * Operation, double Min, double Max)
{
	select_shape(Regions, SelectedRegions, Features, Operation, Min, Max >= Min ? Max : Min);
}

