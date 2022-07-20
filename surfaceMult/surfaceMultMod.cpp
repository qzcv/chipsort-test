#include "surfaceMultMod.h"
#include <QObject>
#include <ImageView.h>
#include <resultText.h>
#include "halOpenCV.h"

surfaceMultMod::surfaceMultMod()
{
	iniData();
}

surfaceMultMod::surfaceMultMod(const QString& dirPath)
{
	iniData();
	load(dirPath, StructLevel);
}

surfaceMultMod::~surfaceMultMod()
{
	delete m_param;
	delete m_image;

	delete m_delRegion[0];
	delete m_delRegion[1];
	delete m_delRegion[2];
	delete m_inAreaRegion;
	delete m_showUnionDelReg;
	delete m_showUnionRoiReg;
	for (auto i = 0;i < MAX_Rec_Num + MAX_Cir_Num;++i)
		delete m_detectRegion[i];
}

int surfaceMultMod::command(int cmdID, void* dataIn, void* dataOut)
{
	return 0;
}

int surfaceMultMod::afterSetProperty(MetaProperty*)
{
	return 0;
}

void surfaceMultMod::save(const QString &dirPath)
{
	m_param->readWriteParam(false, dirPath, StructLevel);
	resetParamChanged();
}

void surfaceMultMod::load(const QString &dirPath, QvsParamLevel level)
{
	m_param->readWriteParam(true,dirPath,level);
}

int surfaceMultMod::run(const QString &funName)
{
	return calculate();
}

void surfaceMultMod::viewResult(ImageView *iv, const QString &funName, int)
{
// 	HTuple imagefull;
// 	getDispOutData(ImageFull, imagefull);
// 	if (!imagefull[0].I()) {
// 		return;
// 	}
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
		int m_model = m_param->DetectModel;  //0缺陷,1目标
		if (m_param->IsChangeModel) {
			m_model = *p_changeMode;
		}
		//HTuple allok;
		//getDispOutData(AllOk, allok);
		iv->setFilled(false);
		if (/*allok[0].I()*/m_allok != 0) {
			iv->setColor(Qt::red);
		}
		else {
			iv->setColor(Qt::green);
		}
		HTuple homeMat2D;
		if (m_param->referSection)
		{
			//getDispOutData(TmpHomMat, homeMat2D);
			homeMat2D = m_homeMat2D;
		}

		if (m_param->pointRegionRefer) {
			HTuple fianalrow, fianalcol;
			fianalrow = m_TmpReferRow;
			fianalcol = m_TmpReferCol;
			//getDispOutData(TmpReferRow, fianalrow);
			//getDispOutData(TmpReferCol, fianalcol);

			Hobject RoiTmp;
			gen_region_polygon_filled(&RoiTmp, fianalrow, fianalcol);
			if (m_param->areaZoom > 0) {
				dilation_rectangle1(RoiTmp, &RoiTmp, m_param->areaZoom, m_param->areaZoom);
			}
			else if (m_param->areaZoom < 0) {
				erosion_rectangle1(RoiTmp, &RoiTmp, abs(m_param->areaZoom), abs(m_param->areaZoom));
			}
			iv->setFilled(false);
			if (/*allok[0].I()*/m_allok != 0) {
				RlRegion reg;
				HalOpenCV::hal2cvRegion(RoiTmp, &reg);
				iv->dispRegion(&reg, "搜寻ROI");
			}
		}
		else if (m_param->areaRegionRefer) {
			if (/*allok[0].I()*/m_allok != 0) {
				iv->dispRegion(&(*p_inReg));
			}
		}
		iv->setColor(255, 255, 0);
		iv->setFilled(false);
		if (/*allok[0].I()*/m_allok != 0 && m_param->isShowDel) {
			RlRegion rlReg;
			HalOpenCV::hal2cvRegion(*m_showUnionDelReg, &rlReg);
			iv->dispRegion(&rlReg);
			//iv->dispRegion(*getDispObject(TmpUnionDelRegion));
		}
		int stIndex = 0;
		int endIndex;
		int dirIndex;
		if (m_param->areaRegionRefer || m_param->pointRegionRefer) {
			endIndex = 1;
		}
		else if (m_param->IsDirPattern) {
// 			HTuple dir;
// 			getDispOutData(TmpInputIndex, dir);
// 			try {
// 				dirIndex = dir[0].I();
// 			}
// 			catch (HException& ex) {
// 				dirIndex = 0;
// 			}
			dirIndex = *p_dirIdx;
			if (dirIndex >= m_param->RecRoiNum + m_param->CirRoiNum) {
				if (m_param->CirRoiNum > 0) {
					stIndex = m_param->CirRoiNum - 1;
					endIndex = m_param->CirRoiNum;
				}
				else if (m_param->RecRoiNum > 0) {
					stIndex = m_param->RecRoiNum - 1;
					endIndex = m_param->RecRoiNum;
				}
			}
			else {
				stIndex = dirIndex;
				endIndex = dirIndex + 1;
			}
		}
		else {
			endIndex = m_param->RecRoiNum + m_param->CirRoiNum;
		}
		int pIndex;
		for (int i = stIndex;i < endIndex;i++) {
			pIndex = m_param->IsSameParam == 1 ? 0 : i;
			//HTuple roiok;
			//getDispOutData(RoiOk, i, roiok);
			if (m_param->areaRegionRefer || m_param->pointRegionRefer) {

			}
			else {
				if (/*roiok[0].I()*/m_RoiOK[i] == 0) {
					Hobject roirec;
					if (i < m_param->RecRoiNum) {
						gen_rectangle2(&roirec, m_param->RecRoi[i].row, m_param->RecRoi[i].col, m_param->RecRoi[i].angle,
							m_param->RecRoi[i].length1, m_param->RecRoi[i].length2);
						if (m_param->referSection == 1) {
							affine_trans_region(roirec, &roirec, homeMat2D, "false");
						}
					}
					else {
						gen_circle(&roirec, m_param->CirRoi[i - m_param->RecRoiNum].rrow, m_param->CirRoi[i - m_param->RecRoiNum].rcol,
							m_param->CirRoi[i - m_param->RecRoiNum].r);
						if (m_param->referSection == 1) {
							affine_trans_region(roirec, &roirec, homeMat2D, "false");
						}
					}
					iv->setFilled(false);
					iv->setColor(255, 0, 0);
					RlRegion roiRecReg;
					HalOpenCV::hal2cvRegion(roirec, &roiRecReg);
					iv->dispRegion(&roiRecReg);

				}
			}

			//旋转矩阵参考情况下，画出ROI区域
// 			if (ui.cb_useRefer->isChecked() && ui.cb_dispAfterDet->isChecked())
// 			{
// 				int viewGray = 200;
// 				m_view->setColor(viewGray, viewGray, viewGray);
// 				if (m_param->RecRoiNum > 0)
// 				{
// 					for (size_t i = 0;i < m_param->RecRoiNum;++i)
// 					{
// 						QString title = QString("%1").arg(i);
// 						m_view->dispRect2(
// 							m_param->RecRoi[i].row,
// 							m_param->RecRoi[i].col,
// 							m_param->RecRoi[i].angle,
// 							m_param->RecRoi[i].length1,
// 							m_param->RecRoi[i].length2,
// 							title.toUtf8().constData());
// 					}
// 				}
// 				if (m_param->CirRoiNum > 0)
// 				{
// 					for (size_t i = 0;i < m_param->CirRoiNum;++i)
// 					{
// 						QString title = QString("%1").arg(i);
// 						m_view->dispCircle(
// 							m_param->CirRoi[i].rrow,
// 							m_param->CirRoi[i].rcol,
// 							m_param->CirRoi[i].r,
// 							title.toUtf8().constData());
// 					}
// 				}
// 			}

			//HTuple regioncount;
			//getDispOutData(AreaCount, i, regioncount);
			if (/*regioncount[0].I()*/m_objCnt[i] == 0) {
				continue;
			}
			else {
				Hobject detectRegion = *m_detectRegion[i]/**getDispObject(RegionDisp, i)*/;

				if (/*roiok[0].I()*/m_RoiOK[i] == 0)
				{
					if ((m_param->area_count_valid && !m_objCntOK[i]) ||
						(m_param->total_badArea_valid && !m_totalBadOK[i]))
					{
						iv->setColor(Qt::red);
						RlRegion reg;
						HalOpenCV::hal2cvRegion(detectRegion, &reg);
						iv->dispRegion(&reg);
						continue;
					}

					connection(detectRegion, &detectRegion);

					HTuple num;
					count_obj(detectRegion, &num);
					pIndex = m_param->IsSameParam == 1 ? 0 : i;
					for (int j = 1;j <= num[0].I();++j)
					{
						Hobject partRegion;
						select_obj(detectRegion, &partRegion, j);

						bool isRed = false;

						//判断是否启用单个面积作为检测条件
						if (m_param->single_badArea_valid[pIndex] > 0)
						{
							HTuple area, row, col;
							area_center(partRegion, &area, &row, &col);

							//判断单个面积是否在判定范围之内，如果是，则通过
							if (area[0].I()*pow(m_param->pixSize, 2) < m_param->single_badArea_min[pIndex] || 
								area[0].I()*pow(m_param->pixSize, 2) > m_param->single_badArea_max[pIndex])
							{
								isRed = true;
							}
						}

						//判断是否启用单个区域的宽，高作为检测条件
						HTuple row1, col1, row2, col2;
						smallest_rectangle1(partRegion, &row1, &col1, &row2, &col2);
						if (m_param->area_width_valid[pIndex])
						{
							if (abs(col2[0].I() - col1[0].I())*m_param->pixSize<m_param->area_width_min[pIndex] || 
								abs(col2[0].I() - col1[0].I())*m_param->pixSize>m_param->area_width_max[pIndex])
							{
								isRed = true;
							}
						}
						if (m_param->area_length_valid[pIndex])
						{
							if (abs(row2[0].I() - row1[0].I())*m_param->pixSize<m_param->area_length_min[pIndex] || 
								abs(row2[0].I() - row1[0].I())*m_param->pixSize>m_param->area_length_max[pIndex])
							{
								isRed = true;
							}
						}
						isRed ? iv->setColor(Qt::red) : iv->setColor(Qt::green);
						RlRegion rlReg;
						HalOpenCV::hal2cvRegion(partRegion, &rlReg);
						iv->dispRegion(&rlReg);
					}
				}
				else {
					iv->setColor(Qt::green);
					RlRegion rlReg;
					HalOpenCV::hal2cvRegion(detectRegion, &rlReg);
					iv->dispRegion(&rlReg);
				}

				// 			if (m_param->IsChangeModel&&m_model==1){
				// 				continue;
				// 			}
				// 			if (m_param->single_badArea_valid[pIndex]){
				// 				HTuple singlebadok;
				// 				getDispOutData(SingleBadOk,i,singlebadok);
				// 				if (singlebadok[0].I()==0){
				// //					m_view->setColor(170,0,255);
				// 					m_view->setColor(255,0,0);
				// 				}
				// 				else{
				// 					m_view->setColor(0,255,0);
				// 				}
				// 				m_view->setFilled(false);
				// 				m_view->dispRegion(*getDispObject(MaxObject,i));
				// 			}
			}
		}
	}
}

void surfaceMultMod::textResult(ResultText *text, const QString &funName)
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
		int stIndex = 0;
		int endIndex;
		int dirIndex;
		if (m_param->areaRegionRefer || m_param->pointRegionRefer) {
			endIndex = 1;
		}
		else if (m_param->IsDirPattern) {
			dirIndex = *p_dirIdx;
			if (dirIndex >= m_param->RecRoiNum + m_param->CirRoiNum) {
				if (m_param->CirRoiNum > 0) {
					stIndex = m_param->CirRoiNum - 1;
					endIndex = m_param->CirRoiNum;
				}
				else if (m_param->RecRoiNum > 0) {
					stIndex = m_param->RecRoiNum - 1;
					endIndex = m_param->RecRoiNum;
				}
			}
			else {
				stIndex = dirIndex;
				endIndex = dirIndex + 1;
			}
		}
		else {
			endIndex = m_param->RecRoiNum + m_param->CirRoiNum;
		}

		int pIndex;
		int m_model = m_param->DetectModel;  //0缺陷,1目标
		if (m_param->IsChangeModel) {
			m_model = *p_changeMode;
		}
		for (int i = stIndex;i < endIndex;i++) {
			text->setTextColor(Qt::black);
			text->append(QObject::tr("roi%1:").arg(i + 1));
			pIndex = m_param->IsSameParam == 1 ? 0 : i;
			//HTuple areacount;
			//getDispOutData(AreaCount, i, areacount);
			if (/*areacount[0].I()*/m_objCnt[i] == 0) {
				if (m_model == 0) {
					text->setTextColor(Qt::black);
				}
				else {
					text->setTextColor(Qt::red);
				}
				text->append(QObject::tr("no object found"));
				continue;
			}
			else if (/*areacount[0].I()*/m_objCnt[i] != 0 && m_param->IsChangeModel) {
				if (m_model == 1) {
					text->setTextColor(Qt::black);
					text->append(QObject::tr(" target found"));
					continue;
				}
			}
			if (m_param->area_count_valid[pIndex]) {
				//HTuple countok;
				//getDispOutData(AreaCountOk, i, countok);
				if (/*countok[0].I()*/m_objCntOK[i] == 0) {
					text->setTextColor(Qt::red);
				}
				else {
					text->setTextColor(Qt::black);
				}
				text->append(QObject::tr("area count: %1(%2,%3)").arg(/*areacount[0].I()*/m_objCnt[i])
					.arg(m_param->area_count_min[pIndex]).arg(m_param->area_count_max[pIndex]));
			}
			if (m_param->single_badArea_valid[pIndex]) {
				//HTuple areamax, areammaxok;
				//getDispOutData(SingleBadOk, i, areammaxok);
				//getDispOutData(SingleBad, i, areamax);
				if (/*areammaxok[0].I()*/m_singleBadOK[i] == 0) {
					text->setTextColor(Qt::red);
				}
				else {
					text->setTextColor(Qt::black);
				}
				if (m_param->IsPixOrAct == 0) {
					double ratio = m_param->pixSize*m_param->pixSize;
					text->append(QObject::tr("max single area: %1 pix^2(%2,%3)").arg(int(m_singleBad[i] / ratio))
						.arg(int(m_param->single_badArea_min[pIndex] / ratio))
						.arg(int(m_param->single_badArea_max[pIndex] / ratio)));
				}
				else {
					text->append(QObject::tr("max single area: %1 mm^2(%2,%3)").arg(m_singleBad[i], 0, 'f', 6)
						.arg(m_param->single_badArea_min[pIndex], 0, 'f', 6)
						.arg(m_param->single_badArea_max[pIndex], 0, 'f', 6));
				}
			}
			for (int j = 0;j < /*areacount[0].I()*/m_objCnt[i];j++) {
				if (m_param->area_length_valid[pIndex]) {
					//HTuple lengthOk1, length1;
					//getDispOutData(LengthOk, i, j, lengthOk1);
					//getDispOutData(Length, i, j, length1);
					if (/*lengthOk1[0].I()*/m_lenOKVec[i][j] == 0) {
						text->setTextColor(Qt::red);
					}
					else {
						text->setTextColor(Qt::black);
					}
					if (m_param->IsPixOrAct == 0) {
						double ratio = m_param->pixSize;
						text->append(QObject::tr(" length%1: %2 pix(%3,%4)").arg(i + 1).arg(int(m_lenVec[i][j] / ratio))
							.arg(int(m_param->area_length_min[pIndex] / ratio))
							.arg(int(m_param->area_length_max[pIndex] / ratio)));
					}
					else {
						text->append(QObject::tr(" length%1: %2 mm(%3,%4)").arg(i + 1).arg(m_lenVec[i][j], 0, 'f', 6)
							.arg(m_param->area_length_min[pIndex], 0, 'f', 6)
							.arg(m_param->area_length_max[pIndex], 0, 'f', 6));
					}
				}
				if (m_param->area_width_valid[pIndex]) {
					//HTuple widthOk1, width1;
					//getDispOutData(WidthOk, i, j, widthOk1);
					//getDispOutData(Width, i, j, width1);
					if (m_widOKVec[i][j] == 0) {
						text->setTextColor(Qt::red);
					}
					else {
						text->setTextColor(Qt::black);
					}
					if (m_param->IsPixOrAct == 0) {
						double ratio = m_param->pixSize;
						text->append(QObject::tr(" width%1: %2 pix(%3,%4)").arg(i + 1).arg(int(m_widVec[i][j] / ratio))
							.arg(int(m_param->area_width_min[pIndex] / ratio))
							.arg(int(m_param->area_width_max[pIndex] / ratio)));
					}
					else {
						text->append(QObject::tr(" width%1: %2 mm(%3,%4)").arg(i + 1).arg(m_widVec[i][j], 0, 'f', 6)
							.arg(m_param->area_width_min[pIndex], 0, 'f', 6)
							.arg(m_param->area_width_max[pIndex], 0, 'f', 6));
					}
				}
			}
			if (m_param->total_badArea_valid[pIndex]) {
				//HTuple totalOk1, total1;
				//getDispOutData(TotalBadOk, i, totalOk1);
				//getDispOutData(TotalBad, i, total1);
				if (/*totalOk1[0].I()*/m_totalBadOK[i] == 0) {
					text->setTextColor(QColor(255, 0, 0));
				}
				else {
					text->setTextColor(QColor(0, 0, 0));
				}
				if (m_param->IsPixOrAct == 0) {
					double ratio = m_param->pixSize*m_param->pixSize;
					text->append(QObject::tr(" total bad area: %1 pix^2(%2,%3)").arg(int(m_totalBad[i] / ratio))
						.arg(int(m_param->total_badArea_min[pIndex] / ratio))
						.arg(int(m_param->total_badArea_max[pIndex] / ratio)));
				}
				else {
					text->append(QObject::tr(" total bad area: %1 mm^2(%2,%3)").arg(m_totalBad[i], 0, 'f', 6)
						.arg(m_param->total_badArea_min[pIndex], 0, 'f', 6)
						.arg(m_param->total_badArea_max[pIndex], 0, 'f', 6));
				}
			}
		}
	}
}

void surfaceMultMod::iniData()
{
	m_param = new surfaceMultParam;
	createPins();
	m_image = new Hobject;
	m_detRes = noDet;
	m_allok = 0;

	for (auto i = 0;i < MAX_Rec_Num + MAX_Cir_Num;++i)
	{
		m_objCnt[i] = 0;
		m_objCntOK[i] = true;

		m_singleBad[i] = 0;
		m_singleBadOK[i] = true;

		m_lenVec[i].clear();
		m_lenOKVec[i].clear();
		m_widVec[i].clear();
		m_widOKVec[i].clear();

		m_totalBad[i] = 0;
		m_totalBadOK[i] = true;

		m_RoiOK[i] = true;

		m_detectRegion[i] = new Hobject;
	}	

	for (auto i = 0;i < 3;++i)
		m_delRegion[i] = new Hobject;
	m_inAreaRegion = new Hobject;
	m_showUnionDelReg = new Hobject;
	m_showUnionRoiReg = new Hobject;
}

void surfaceMultMod::createPins()
{
	addPin(&p_im, "im");
	addPin(&p_homMat2D, "hm2d");
	addPin(&p_pixelSize, "pSz", "pSz", "pixelSize");
	addPin(&p_corners, "corners");
	addPin(&p_inReg, "inReg");
	for (auto i = 0;i < 3;++i)
		addPin(&p_delReg[i], QString("del%1").arg(i), QString("del%1").arg(i), QString::fromLocal8Bit("排除区域%1").arg(i));
	addPin(&p_changeMode, "cm", "cm", "changeMode");
	addPin(&p_delHomMat2D, "dhm2d", "dhm2d", "delHomMat2d");
	addPin(&p_dirIdx, "dirIdx");

	addPin(&p_outReg, "outReg");

	p_pixelSize.setVisible(false);
	p_corners.setVisible(false);
	p_inReg.setVisible(false);
	for (auto i = 0;i < 3;++i)
		p_delReg[i].setVisible(false);
	p_changeMode.setVisible(false);
	p_delHomMat2D.setVisible(false);
	p_dirIdx.setVisible(false);
	p_outReg.setVisible(false);
}

int surfaceMultMod::calculate(Hobject * disp)
{
	// 	for (int i = 0;i < MAX_TESTITEMS_NUM;i++) {
	// 		m_testItemsStatus[i] = 1;
	// 	}
	// 	bool allok = true;
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
	m_detRes = noDet;
	m_allok = 0;
	if (p_im->empty())
	{
		m_detRes = emptyImg;
		return -1;
	}
	if (m_param->referSection&&(p_homMat2D->size() != cv::Size(3, 2)))
	{
		m_detRes = wrongMat;
		return -1;
	}
	m_detRes = detSucceed;

	HalOpenCV::cv2halImg(*p_im, m_image, false);

	Hobject dstImg;
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
	int m_model = m_param->DetectModel;  //0缺陷,1目标
	if (m_param->IsChangeModel) {
		m_model = *p_changeMode;
		// 		try {
		// 			HTuple mmodel;
		// 			getDetectInData(ChangeModel, mmodel);
		// 			//m_model=1;
		// 			m_model = mmodel[0].I();
		// 		}
		// 		catch (HException& ex) {
		// 			m_model = 0;
		// 		}
		// 		setDetectOutData(TmpChangeModel, m_model);
	}
	HTuple wid, hei;
	Hobject ImgRec;
	Hobject roiRec;  //delete
	Hobject unionRec;
	get_image_size(*m_image, &wid, &hei);
	gen_rectangle1(&ImgRec, 0, 0, hei - 1, wid - 1);
	bool isstart = false;
	HTuple deleteregion[3];    //排除1,2，3等传入区域
	for (int i = 0;i < 3;i++) {
		if (m_param->deleteArea_section[i]) {
			//getDetectInData(m_delRegionBuf[i], deleteregion[i]);
			try {
				//copy_obj(*(Hobject*)deleteregion[i][0].L(), m_delRegion[i], 1, 1);
				Hobject tmpReg;
				HalOpenCV::cv2halRegion(*p_delReg[i], &tmpReg);
				copy_obj(tmpReg, m_delRegion[i], 1, 1);
				if (m_param->deleteArea_zoom[i] > 0) {
					dilation_rectangle1(*m_delRegion[i], m_delRegion[i], m_param->deleteArea_zoom[i], m_param->deleteArea_zoom[i]);
				}
				else if (m_param->deleteArea_zoom[i] < 0) {
					erosion_rectangle1(*m_delRegion[i], m_delRegion[i], abs(m_param->deleteArea_zoom[i]), abs(m_param->deleteArea_zoom[i]));
				}
			}
			catch (HException& ex) {
				gen_empty_region(m_delRegion[i]);
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
	//HTuple dirIndex;
	int dirIdx;
	HTuple delHommat;
	if (m_param->deleteRefer) {
		//getDetectInData(DeleteHomMat, delHommat);
		HalOpenCV::cv2HalHomMat2D(*p_delHomMat2D, &delHommat);
	}
	if (m_param->IsDirPattern)
	{
		//		getDetectInData(InputIndex, dirIndex);
		//		setDetectOutData(TmpInputIndex, dirIndex);
		// 		try {
		// 			dirIdx = dirIndex[0].I();
		// 		}
		// 		catch (HException& ex) {
		// 			dirIdx = 0;
		// 		}
		dirIdx = *p_dirIdx;
	}
	if (m_param->IsDirPattern&&m_param->IsDirDelete == 0) {  //采用极性模式 
		if (m_param->RecDeleteNum + m_param->CirDeleteNum > 0) {
			int idx;
			if (dirIdx >= m_param->RecDeleteNum + m_param->CirDeleteNum) {
				if (m_param->CirDeleteNum > 0) {
					idx = m_param->CirDeleteNum - 1;
					gen_circle(&roiRec, m_param->DeleteCir[idx].drrow, m_param->DeleteCir[idx].drcol
						, m_param->DeleteCir[idx].dr);
				}
				else if (m_param->RecDeleteNum > 0) {
					idx = m_param->RecDeleteNum - 1;
					gen_rectangle2(&roiRec, m_param->DeleteRec[idx].drow, m_param->DeleteRec[idx].dcol, m_param->DeleteRec[idx].dangle
						, m_param->DeleteRec[idx].dlength1, m_param->DeleteRec[idx].dlength2);
				}
			}
			else {
				if (dirIdx <= m_param->RecDeleteNum - 1) {
					idx = dirIdx;
					gen_rectangle2(&roiRec, m_param->DeleteRec[idx].drow, m_param->DeleteRec[idx].dcol, m_param->DeleteRec[idx].dangle
						, m_param->DeleteRec[idx].dlength1, m_param->DeleteRec[idx].dlength2);
				}
				else {
					idx = dirIdx - m_param->RecDeleteNum;
					gen_circle(&roiRec, m_param->DeleteCir[idx].drrow, m_param->DeleteCir[idx].drcol
						, m_param->DeleteCir[idx].dr);
				}
			}
			if (m_param->referSection == 1) {
				affine_trans_region(roiRec, &roiRec, m_homeMat2D, "false");
			}
			union2(roiRec, unionRec, &unionRec);
		}
	}
	else if (!m_param->IsDirPattern || (m_param->IsDirPattern&&m_param->IsDirDelete == 1)) {    //采用非极性模式或者采用极性模式但全部排除
		for (int i = 0;i < m_param->RecDeleteNum;i++) {
			gen_rectangle2(&roiRec, m_param->DeleteRec[i].drow, m_param->DeleteRec[i].dcol, m_param->DeleteRec[i].dangle
				, m_param->DeleteRec[i].dlength1, m_param->DeleteRec[i].dlength2);
			if (m_param->referSection == 1) {
				affine_trans_region(roiRec, &roiRec, m_homeMat2D, "false");
			}
			else if (m_param->deleteRefer) {
				affine_trans_region(roiRec, &roiRec, delHommat, "false");
			}
			union2(roiRec, unionRec, &unionRec);
		}
		for (int i = 0;i < m_param->CirDeleteNum;i++) {
			Hobject roicir;
			gen_circle(&roicir, m_param->DeleteCir[i].drrow, m_param->DeleteCir[i].drcol, m_param->DeleteCir[i].dr);
			if (m_param->referSection == 1) {
				affine_trans_region(roicir, &roicir, m_homeMat2D, "false");
			}
			else if (m_param->deleteRefer) {
				affine_trans_region(roicir, &roicir, delHommat, "false");
			}
			union2(roicir, unionRec, &unionRec);
		}
	}
	Hobject tmpReg;                //引用传入点区域为排除区域进行排除
	HTuple fianalrow, fianalcol;
	if (m_param->pointRegionRefer || m_param->IsReferPtoDel) {   ////左上角顺时针顺序，传进来的区域4个点
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
			if (abs(m_param->zoom_down) > 0) {
				fianalrow[2] = double(fianalrow[2].D() + m_param->zoom_down);
				fianalrow[3] = double(fianalrow[3].D() + m_param->zoom_down);
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
		m_TmpReferRow = fianalrow;
		m_TmpReferCol = fianalcol;
		//setDetectOutData(TmpReferRow, fianalrow);
		//setDetectOutData(TmpReferCol, fianalcol);
	}
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
	*m_showUnionDelReg = unionRec;						 //将计算到的排除区域整合传递给Viewresult显示
														 //setDetectOutObject(TmpUnionDelRegion, unionRec);   //将计算到的排除区域整合传递给Viewresult显示
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
	reduce_domain(grayimg, unionRec, &dstImg);      //得到被减掉排除区域的图像
	Hobject delDomain;
	get_domain(dstImg, &delDomain);
	Hobject RoiRec, meanImg;
	if (m_param->pointRegionRefer) {   //点区域
		gen_region_polygon_filled(&RoiRec, fianalrow, fianalcol);
		if (m_param->areaZoom > 0) {
			dilation_rectangle1(RoiRec, &RoiRec, m_param->areaZoom, m_param->areaZoom);
		}
		else if (m_param->areaZoom < 0) {
			erosion_rectangle1(RoiRec, &RoiRec, abs(m_param->areaZoom), abs(m_param->areaZoom));
		}
	}
	else if (m_param->areaRegionRefer) { //面区域
										 //getDetectInData(InputAreaRegion, inareap);
		try {
			//copy_obj(*(Hobject*)inareap[0].L(), m_inAreaRegion, 1, 1);
			Hobject tmpReg;
			HalOpenCV::cv2halRegion(*p_inReg, &tmpReg);
			copy_obj(tmpReg, m_inAreaRegion, 1, 1);
			if (m_param->areaZoom > 0) {
				dilation_rectangle1(*m_inAreaRegion, m_inAreaRegion, m_param->areaZoom, m_param->areaZoom);
			}
			else if (m_param->areaZoom < 0) {
				erosion_rectangle1(*m_inAreaRegion, m_inAreaRegion, abs(m_param->areaZoom), abs(m_param->areaZoom));
			}
			copy_obj(*m_inAreaRegion, &RoiRec, 1, 1);
		}
		catch (HException& ex) {
			gen_rectangle1(m_inAreaRegion, 20, 20, 100, 100);
			copy_obj(*m_inAreaRegion, &RoiRec, 1, 1);
		}
		//setDetectOutObject(TmpAreaRegion, *m_inAreaRegion);
	}
	int endIndex;
	int stIndex = 0;
	if (m_param->areaRegionRefer || m_param->pointRegionRefer) {
		endIndex = 1;
	}
	else if (m_param->IsDirPattern) {
		if (/*dirIndex*/dirIdx >= m_param->RecRoiNum + m_param->CirRoiNum) {
			if (m_param->CirRoiNum > 0) {
				stIndex = m_param->CirRoiNum - 1;
				endIndex = m_param->CirRoiNum;
			}
			else if (m_param->RecRoiNum > 0) {
				stIndex = m_param->RecRoiNum - 1;
				endIndex = m_param->RecRoiNum;
			}
		}
		else {
			stIndex = dirIdx;
			endIndex = dirIdx + 1;
		}

	}
	else {
		endIndex = m_param->RecRoiNum + m_param->CirRoiNum;
	}
	//bool roiok[MAX_Rec_Num + MAX_Cir_Num];
	int pIndex;
	Hobject outUdispRegion; //传出得到的分割区域
	for (int i = stIndex;i < endIndex;i++) {
		//roiok[i] = true;
		m_RoiOK[i] = true;
		pIndex = m_param->IsSameParam == 1 ? 0 : i;
		Hobject roiImg;
		if (m_param->areaRegionRefer || m_param->pointRegionRefer) {
			//
		}
		else {
			if (i < m_param->RecRoiNum) {
				gen_rectangle2(&RoiRec, m_param->RecRoi[i].row, m_param->RecRoi[i].col, m_param->RecRoi[i].angle,
					m_param->RecRoi[i].length1, m_param->RecRoi[i].length2);
				if (m_param->referSection == 1) {
					affine_trans_region(RoiRec, &RoiRec, m_homeMat2D, "false");
				}
			}
			else {
				gen_circle(&RoiRec, m_param->CirRoi[i - m_param->RecRoiNum].rrow, m_param->CirRoi[i - m_param->RecRoiNum].rcol,
					m_param->CirRoi[i - m_param->RecRoiNum].r);
				if (m_param->referSection == 1) {
					affine_trans_region(RoiRec, &RoiRec, m_homeMat2D, "false");
				}
			}
		}
		//setDetectOutObject(TmpAreaRegion,RoiRec);
		reduce_domain(dstImg, RoiRec, &roiImg);
		if (m_param->IsGauss[pIndex]) {
			int vaaa = (m_param->GaussSize[pIndex] % 2) == 0 ? 1 : 0;
			gauss_image(roiImg, &roiImg, vaaa + m_param->GaussSize[pIndex]);
		}
		Hobject dstdomain;
		get_domain(roiImg, &dstdomain);
		if (m_param->testType[pIndex] == surfaceMultParam::fixedThre) {
			threshold(roiImg, &roiImg, m_param->fixThreMin[pIndex], m_param->fixThreMax[pIndex] < m_param->fixThreMin[pIndex] ? m_param->fixThreMin[pIndex] : m_param->fixThreMax[pIndex]);
			if (m_param->closeVal[pIndex] > 0) {
				closing_rectangle1(roiImg, &roiImg, m_param->closeVal[pIndex], m_param->closeVal[pIndex]);
			}
			if (m_param->openVal[pIndex] > 0) {
				opening_rectangle1(roiImg, &roiImg, m_param->openVal[pIndex], m_param->openVal[pIndex]);
			}
			connection(roiImg, &roiImg);
			mselect_shape(roiImg, &roiImg, "area", "and", int((m_param->badArea_min[pIndex] / (m_param->pixSize*m_param->pixSize) + 0.5) > 999999 ? 999999 : (m_param->badArea_min[pIndex] / (m_param->pixSize*m_param->pixSize) + 0.5)),
				int((m_param->badArea_max[pIndex] / (m_param->pixSize*m_param->pixSize) + 0.5) > 999999 ? 999999 : (m_param->badArea_max[pIndex] / (m_param->pixSize*m_param->pixSize) + 0.5)));
			mselect_shape(roiImg, &roiImg, "height", "and", int(m_param->badLengthMin[pIndex] / m_param->pixSize + 0.5), int(m_param->badLengthMax[pIndex] / m_param->pixSize + 0.5));
			mselect_shape(roiImg, &roiImg, "width", "and", int(m_param->badWidthMin[pIndex] / m_param->pixSize + 0.5), int(m_param->badWidthMax[pIndex] / m_param->pixSize + 0.5));

			//select_shape(roiImg, &roiImg, "circularity", "and", 0.8, 1.0);
		}
		else if (m_param->testType[pIndex] == surfaceMultParam::detHole) {
			threshold(roiImg, &roiImg, m_param->fixThreMin[pIndex], m_param->fixThreMax[pIndex] < m_param->fixThreMin[pIndex] ? m_param->fixThreMin[pIndex] : m_param->fixThreMax[pIndex]);
			dilation_circle(roiImg, &roiImg, 1);
			if (m_param->closeVal[pIndex] > 0) {
				closing_rectangle1(roiImg, &roiImg, m_param->closeVal[pIndex], m_param->closeVal[pIndex]);
			}
			if (m_param->openVal[pIndex] > 0) {
				opening_rectangle1(roiImg, &roiImg, m_param->openVal[pIndex], m_param->openVal[pIndex]);
			}
			connection(roiImg, &roiImg);
			mselect_shape(roiImg, &roiImg, "area", "and", int((m_param->badArea_min[pIndex] / (m_param->pixSize*m_param->pixSize) + 0.5) > 999999 ? 999999 : (m_param->badArea_min[pIndex] / (m_param->pixSize*m_param->pixSize) + 0.5)),
				int((m_param->badArea_max[pIndex] / (m_param->pixSize*m_param->pixSize) + 0.5) > 999999 ? 999999 : (m_param->badArea_max[pIndex] / (m_param->pixSize*m_param->pixSize) + 0.5)));
			mselect_shape(roiImg, &roiImg, "height", "and", int(m_param->badLengthMin[pIndex] / m_param->pixSize + 0.5), int(m_param->badLengthMax[pIndex] / m_param->pixSize + 0.5));
			mselect_shape(roiImg, &roiImg, "width", "and", int(m_param->badWidthMin[pIndex] / m_param->pixSize + 0.5), int(m_param->badWidthMax[pIndex] / m_param->pixSize + 0.5));
			mselect_shape(roiImg, &roiImg, "holes_num", "and", m_param->holeNumMin[pIndex], m_param->holeNumMax[pIndex]);
			mselect_shape(roiImg, &roiImg, "area_holes", "and", m_param->holeAreaMin[pIndex], m_param->holeAreaMax[pIndex]);
			//shape_trans(roiImg,&roiImg,"convex");
		}
		else if (m_param->testType[pIndex] == surfaceMultParam::ragAndLoss) {
			threshold(roiImg, &roiImg, m_param->fixThreMin[pIndex], m_param->fixThreMax[pIndex] < m_param->fixThreMin[pIndex] ? m_param->fixThreMin[pIndex] : m_param->fixThreMax[pIndex]);
			connection(roiImg, &roiImg);
			mselect_shape(roiImg, &roiImg, "area", "and", int((m_param->badArea_min[pIndex] / (m_param->pixSize*m_param->pixSize) + 0.5) > 999999 ? 999999 : (m_param->badArea_min[pIndex] / (m_param->pixSize*m_param->pixSize) + 0.5)),
				int((m_param->badArea_max[pIndex] / (m_param->pixSize*m_param->pixSize) + 0.5) > 999999 ? 999999 : (m_param->badArea_max[pIndex] / (m_param->pixSize*m_param->pixSize) + 0.5)));
			mselect_shape(roiImg, &roiImg, "height", "and", int(m_param->badLengthMin[pIndex] / m_param->pixSize + 0.5), int(m_param->badLengthMax[pIndex] / m_param->pixSize + 0.5));
			mselect_shape(roiImg, &roiImg, "width", "and", int(m_param->badWidthMin[pIndex] / m_param->pixSize + 0.5), int(m_param->badWidthMax[pIndex] / m_param->pixSize + 0.5));
			union1(roiImg, &roiImg);
			Hobject tmpimg1, tmpimg2;
			if (m_param->minRag[pIndex] > 0) {
				copy_obj(dstImg, &tmpimg1, 1, 1);
				opening_circle(tmpimg1, &tmpimg1, m_param->minRag[pIndex]);
				connection(tmpimg1, &tmpimg1);
				union1(tmpimg1, &tmpimg1);
				difference(dstImg, tmpimg1, &tmpimg1);
				connection(tmpimg1, &tmpimg1);
				mselect_shape(tmpimg1, &tmpimg1, "area", "and", m_param->minRagLossArea[pIndex], 10000000);
			}
			else {
				gen_empty_region(&tmpimg1);
			}
			if (m_param->minLoss[pIndex] > 0) {
				copy_obj(dstImg, &tmpimg2, 1, 1);
				closing_circle(tmpimg2, &tmpimg2, m_param->minLoss[pIndex]);
				connection(tmpimg2, &tmpimg2);
				union1(tmpimg2, &tmpimg2);
				difference(tmpimg2, dstImg, &tmpimg2);
				connection(tmpimg2, &tmpimg2);
				mselect_shape(tmpimg2, &tmpimg2, "area", "and", m_param->minRagLossArea[pIndex], 10000000);
			}
			else {
				gen_empty_region(&tmpimg2);
			}
			union2(tmpimg2, tmpimg1, &roiImg);
		}
		else {
			if (m_param->testType[pIndex] == surfaceMultParam::meanThre) {
				Hobject diff;
				reduce_domain(unionRec, RoiRec, &diff);
				difference(ImgRec, diff, &diff);
				Hobject gaussimg;
				if (m_param->IsFillOut[pIndex]) {
					Hobject averImg;
					reduce_domain(grayimg, unionRec, &averImg);
					reduce_domain(averImg, RoiRec, &averImg);
					HTuple absHisto, relativeHisto;
					gray_histo(dstdomain, averImg, &absHisto, &relativeHisto);
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
					paint_region(diff, grayimg, &gaussimg, Sel[0].I(), "fill");
				}
				else {
					copy_image(grayimg, &gaussimg);
				}
				if (m_param->IsGauss[pIndex]) {
					int vaaa = (m_param->GaussSize[pIndex] % 2) == 0 ? 1 : 0;
					gauss_image(gaussimg, &gaussimg, vaaa + m_param->GaussSize[pIndex]);
				}
				reduce_domain(gaussimg, unionRec, &gaussimg);
				reduce_domain(gaussimg, RoiRec, &roiImg);
				mean_image(roiImg, &meanImg, m_param->meanSize[pIndex], m_param->meanSize[pIndex]);
				//gauss_image(roiImg,&meanImg,m_param->meanSize);
			}
			else if (m_param->testType[pIndex] == surfaceMultParam::averageThre) {
				HTuple absHisto, relativeHisto;
				Hobject averImg;
				reduce_domain(grayimg, unionRec, &averImg);
				reduce_domain(averImg, RoiRec, &averImg);
				gray_histo(dstdomain, averImg, &absHisto, &relativeHisto);
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
				gen_image_proto(grayimg, &meanImg, Sel[0].D());
				reduce_domain(meanImg, dstdomain, &meanImg);
			}
			Hobject dstDark, dstLight;
			dyn_threshold(roiImg, meanImg, &dstDark, m_param->black_bad_detect[pIndex], "dark");
			dyn_threshold(roiImg, meanImg, &dstLight, m_param->white_bad_detect[pIndex], "light");

			if (m_param->closeVal[pIndex] > 0) {
				closing_rectangle1(dstDark, &dstDark, m_param->closeVal[pIndex], m_param->closeVal[pIndex]);
				closing_rectangle1(dstLight, &dstLight, m_param->closeVal[pIndex], m_param->closeVal[pIndex]);
				//closing_circle(dstDark,&dstDark,m_param->closeVal[pIndex]);
				//closing_circle(dstLight,&dstLight,m_param->closeVal[pIndex]);
			}
			if (m_param->openVal[pIndex] > 0) {
				opening_rectangle1(dstDark, &dstDark, m_param->openVal[pIndex], m_param->openVal[pIndex]);
				opening_rectangle1(dstLight, &dstLight, m_param->openVal[pIndex], m_param->openVal[pIndex]);
				//opening_circle(dstDark,&dstDark,m_param->openVal[pIndex]);
				//opening_circle(dstLight,&dstLight,m_param->openVal[pIndex]);
			}
			connection(dstDark, &dstDark);
			connection(dstLight, &dstLight);
			mselect_shape(dstDark, &dstDark, "area", "and", m_param->badArea_min[pIndex], m_param->badArea_max[pIndex]);

			mselect_shape(dstLight, &dstLight, "area", "and", m_param->badArea_min[pIndex], m_param->badArea_max[pIndex]);
			mselect_shape(dstDark, &dstDark, "height", "and", m_param->badLengthMin[pIndex], m_param->badLengthMax[pIndex]);
			mselect_shape(dstLight, &dstLight, "height", "and", m_param->badLengthMin[pIndex], m_param->badLengthMax[pIndex]);
			mselect_shape(dstDark, &dstDark, "width", "and", m_param->badWidthMin[pIndex], m_param->badWidthMax[pIndex]);
			mselect_shape(dstLight, &dstLight, "width", "and", m_param->badWidthMin[pIndex], m_param->badWidthMax[pIndex]);

			mselect_shape(dstDark, &dstDark, "area", "and", int((m_param->badArea_min[pIndex] / (m_param->pixSize*m_param->pixSize) + 0.5) > 999999 ? 999999 : (m_param->badArea_min[pIndex] / (m_param->pixSize*m_param->pixSize) + 0.5)),
				int((m_param->badArea_max[pIndex] / (m_param->pixSize*m_param->pixSize) + 0.5) > 999999 ? 999999 : (m_param->badArea_max[pIndex] / (m_param->pixSize*m_param->pixSize) + 0.5)));
			mselect_shape(dstLight, &dstLight, "area", "and", int((m_param->badArea_min[pIndex] / (m_param->pixSize*m_param->pixSize) + 0.5) > 999999 ? 999999 : (m_param->badArea_min[pIndex] / (m_param->pixSize*m_param->pixSize) + 0.5)),
				int((m_param->badArea_max[pIndex] / (m_param->pixSize*m_param->pixSize) + 0.5) > 999999 ? 999999 : (m_param->badArea_max[pIndex] / (m_param->pixSize*m_param->pixSize) + 0.5)));
			mselect_shape(dstDark, &dstDark, "height", "and", int(m_param->badLengthMin[pIndex] / m_param->pixSize + 0.5), int(m_param->badLengthMax[pIndex] / m_param->pixSize + 0.5));
			mselect_shape(dstLight, &dstLight, "height", "and", int(m_param->badLengthMin[pIndex] / m_param->pixSize + 0.5), int(m_param->badLengthMax[pIndex] / m_param->pixSize + 0.5));
			mselect_shape(dstDark, &dstDark, "width", "and", int(m_param->badWidthMin[pIndex] / m_param->pixSize + 0.5), int(m_param->badWidthMax[pIndex] / m_param->pixSize + 0.5));
			mselect_shape(dstLight, &dstLight, "width", "and", int(m_param->badWidthMin[pIndex] / m_param->pixSize + 0.5), int(m_param->badWidthMax[pIndex] / m_param->pixSize + 0.5));
			union2(dstDark, dstLight, &roiImg);
			connection(roiImg, &roiImg);
		}

		if (disp)	//用于在testParam()函数里面显示检测得到的区域
		{
			union1(roiImg, &roiImg);
			union1(roiImg, disp);
			continue;
		}

		//Hlong objcount;
		count_obj(roiImg, /*&objcount*/&m_objCnt[i]);
		if (/*objcount*/m_objCnt[i] == 0) {
			if (m_model == 0) {
				//setDetectOutData(RoiOk, true, i);
				m_RoiOK[i] = true;
			}
			else {
				m_allok = -1;
				m_RoiOK[i] = false;
				//setDetectOutData(RoiOk, false, i);
				//m_testItemsStatus[0] = 0;
				//allok = false;
			}
			//setDetectOutData(AreaCount, objcount, i);
			continue;
		}
		else if (/*objcount*/m_objCnt[i] != 0 && m_param->IsChangeModel) {

			if (m_model == 1) {
				//setDetectOutData(RoiOk, true, i);
				//setDetectOutData(AreaCount, objcount, i);
				//setDetectOutObject(RegionDisp, roiImg, i);
				m_RoiOK[i] = true;
				union2(outUdispRegion, roiImg, &outUdispRegion);
				*m_detectRegion[i] = outUdispRegion;
				continue;
			}
			else {
				//setDetectOutData(RoiOk, false, i);
				//m_testItemsStatus[0] = 0;
				//allok = false;
				m_RoiOK[i] = false;
				m_allok = -1;
			}
		}
		union2(outUdispRegion, roiImg, &outUdispRegion);
		*m_detectRegion[i] = outUdispRegion;
		//setDetectOutData(AreaCount, objcount, i);
		//setDetectOutObject(RegionDisp, roiImg, i);

		if (m_param->area_count_valid[pIndex]) {
			//bool countOk = true;
			m_objCntOK[i] = true;
			if (/*objcount*/m_objCnt[i]<m_param->area_count_min[pIndex] ||
				/*objcount*/m_objCnt[i]>m_param->area_count_max[pIndex]) {
				//countOk = false;
				//roiok[i] = false;
				//m_testItemsStatus[5] = 0;
				//allok = false;
				m_objCntOK[i] = false;
				m_RoiOK[i] = false;
				m_allok = -6;
			}
			//setDetectOutData(AreaCountOk, countOk, i);
		}
		if (m_param->single_badArea_valid[pIndex]) {
			Hobject singleMax;
			select_shape_std(roiImg, &singleMax, "max_area", 95);
			HTuple Area, row, col;
			area_center(singleMax, &Area, &row, &col);
			//bool singleOk = true;
			m_singleBadOK[i] = true;
			m_singleBad[i] = Area[0].I()*(m_param->pixSize*m_param->pixSize);
			m_singleBadMax[i] = singleMax;
			if (m_singleBad[i] < m_param->single_badArea_min[pIndex] ||
				m_singleBad[i] > m_param->single_badArea_max[pIndex]) {
				//singleOk = false;
				//roiok[i] = false;
				//m_testItemsStatus[1] = 0;
				//allok = false;
				m_singleBadOK[i] = false;
				m_RoiOK[i] = false;
				m_allok = -2;
			}
			//setDetectOutData(SingleBadOk, singleOk, i);
			//setDetectOutData(SingleBad, Area[0].I()*(m_param->pixSize*m_param->pixSize), i);
			//setDetectOutObject(MaxObject, singleMax, i);
		}
		m_widVec[i].resize(m_objCnt[i]);
		m_widOKVec[i].resize(m_objCnt[i]);
		m_lenVec[i].resize(m_objCnt[i]);
		m_lenOKVec[i].resize(m_objCnt[i]);
		HTuple regionRow, regionCol, regionArea, row1, col1, row2, col2;
		area_center(roiImg, &regionArea, &regionRow, &regionCol);
		smallest_rectangle1(roiImg, &row1, &col1, &row2, &col2);
		long totalArea = 0;
		for (int j = 0;j < /*objcount*/m_objCnt[i];j++) {
			if (m_param->area_width_valid[pIndex]) {
				//bool widthok1 = true;
				m_widOKVec[i][j] = true;
				m_widVec[i][j] = abs(col2[j].I() - col1[j].I())*m_param->pixSize;
				if (m_widVec[i][j] < m_param->area_width_min[pIndex] ||
					m_widVec[i][j] > m_param->area_width_max[pIndex]) {
					//widthok1 = false;
					//roiok[i] = false;
					//m_testItemsStatus[4] = 0;
					//allok = false;
					m_widOKVec[i][j] = false;
					m_RoiOK[i] = false;
					m_allok = -5;
				}
				//setDetectOutData(Width, (col2[j].I() - col1[j].I())*m_param->pixSize, i, j);
				//setDetectOutData(WidthOk, widthok1, i, j);
			}
			if (m_param->area_length_valid[pIndex]) {
				//bool lengthok1 = true;
				m_lenOKVec[i][j] = true;
				m_lenVec[i][j] = abs(row2[j].I() - row1[j].I())*m_param->pixSize;
				if (m_lenVec[i][j] < m_param->area_length_min[pIndex] ||
					m_lenVec[i][j] > m_param->area_length_max[pIndex]) {
					//lengthok1 = false;
					//roiok[i] = false;
					//m_testItemsStatus[3] = 0;
					//allok = false;
					m_lenOKVec[i][j] = false;
					m_RoiOK[i] = false;
					m_allok = -4;
				}
				//setDetectOutData(LengthOk, lengthok1, i, j);
				//setDetectOutData(Length, (row2[j].I() - row1[j].I())*m_param->pixSize, i, j);
			}
			totalArea += regionArea[j].I();
		}  //end of loop objcount j
		if (m_param->total_badArea_valid[pIndex]) {
			//bool totalok1 = true;
			m_totalBadOK[i] = true;
			m_totalBad[i] = totalArea*(m_param->pixSize*m_param->pixSize);
			if (m_totalBad[i] < m_param->total_badArea_min[pIndex] ||
				m_totalBad[i] > m_param->total_badArea_max[pIndex]) {
				//totalok1 = false;
				//roiok[i] = false;
				//m_testItemsStatus[2] = 0;
				//allok = false;
				m_totalBadOK[i] = false;
				m_RoiOK[i] = false;
				m_allok = -3;
			}
			//setDetectOutData(TotalBadOk, totalok1, i);
			//setDetectOutData(TotalBad, totalArea*(m_param->pixSize*m_param->pixSize), i);
		}
		//setDetectOutData(RoiOk, roiok[i], i);
	}   //end of loop i
	union1(outUdispRegion, &outUdispRegion);
	Hobject tmpDispReg;
	copy_obj(outUdispRegion, &tmpDispReg, 1, 1);
	HalOpenCV::hal2cvRegion(tmpDispReg, &(*p_outReg));
	//copy_obj(outUdispRegion, m_areaRegion, 1, 1);
	//setDetectOutData(DISPCONTOURPOINTER, (Hlong)m_areaRegion);
	//setDetectOutData(AllOk, allok);
	return m_allok;
}

void surfaceMultMod::mselect_shape(const Hobject & Regions, Hobject * SelectedRegions, const char * Features, const char * Operation, double Min, double Max)
{
	select_shape(Regions, SelectedRegions, Features, Operation, Min, Max >= Min ? Max : Min);
}

