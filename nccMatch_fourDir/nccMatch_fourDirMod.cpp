#include "nccMatch_fourDirMod.h"
#include "nccMatch_fourDirParam.h"
#include <QObject>
#include <ImageView.h>
#include <resultText.h>
#include "nccmatch.h"
#include "halOpenCV.h"
#include "transformation.h"

nccMatch_fourDirMod::nccMatch_fourDirMod()
{
	iniData();
}

nccMatch_fourDirMod::nccMatch_fourDirMod(const QString& dirPath)
{
	iniData();
	load(dirPath, StructLevel);
}

nccMatch_fourDirMod::~nccMatch_fourDirMod()
{
	delete m_param;
	delete m_image;
	delete m_outArea;
}

int nccMatch_fourDirMod::command(int cmdID, void* dataIn, void* dataOut)
{
	return 0;
}

int nccMatch_fourDirMod::afterSetProperty(MetaProperty*)
{
	return 0;
}

void nccMatch_fourDirMod::save(const QString &dirPath)
{
	m_param->readWriteParam(false, dirPath, StructLevel);
	resetParamChanged();
}

void nccMatch_fourDirMod::load(const QString &dirPath, QvsParamLevel level)
{
	m_param->readWriteParam(true,dirPath,level);

	if (level == ProductLevel)
	{
		for (int i = 0;i < MAX_PAT_NUM;i++) {
			m_detect[i]->setMaxOverlap(m_param->maxOverlap);
			m_detect[i]->setNum(m_param->findNum);
			m_detect[i]->setSimilar((double)m_param->similarity / 100.0);
			m_detect[i]->setRotateDeg(m_param->seekingAngle);
			has_set_pattern[i] = 0;

			if (m_param->patImage[i].Id() != H_EMPTY_REGION)
			{
				resetPattern(i);
				m_detect[i]->setRoi(&QRect(
					m_param->roiCol1[i], m_param->roiRow1[i], 
					m_param->roiCol2[i] - m_param->roiCol1[i], 
					m_param->roiRow2[i] - m_param->roiRow1[i]));
				has_set_pattern[i] = 1;
			}
		}
	}
	else if (level == MachineLevel)
	{
		//p_lfIdx.setData(m_param->pin1OutIdx);
		//p_rtIdx.setData(m_param->pin2OutIdx);
	}
}

int nccMatch_fourDirMod::run(const QString &funName)
{
	//for (int i = 0;i < MAX_TESTITEMS_NUM;i++) {
	//	m_testItemsStatus[i] = 1;
	//}
	int res = 1;
	for (auto i = 0;i < MAX_PAT_NUM;++i)
		m_detRes[i] = noDet;
	m_allOk = 0;
	m_resOk = 1;
	//*p_chooseIdx = -1;
	p_chooseIdx.setData(-1);
	int isLF = 0;
	int isRT = 0;
	//*p_lfIdx = m_param->pin1OutIdx;
	//*p_rtIdx = m_param->pin2OutIdx;
// 	if (!m_image || m_image->Id() == H_EMPTY_REGION) {
// 		res = 0;
// 		setDetectOutData(AllOK, res);
// 		setDetectOutData(ResOk, res);
// 		hom_mat2d_identity(&m_homMat2D);
// 		setDetectOutData(HomMat2D, m_homMat2D);
// 		setDetectOutData(ImageFull, false);
// 		m_testItemsStatus[0] = 0;
// 		return false;
// 	}
// 	else {
// 		setDetectOutData(ImageFull, true);
// 	}
	cv::Mat im;
	p_im.getData(im);
	if (im.empty())
	{
// 		HTuple hm2d;
// 		hom_mat2d_identity(/*&(*p_outHomMat2d)*/&hm2d);
//		m_outHomMat2d.setData(hm2d);
		*p_outHomMat2d = qzcv::hom_mat2d_identity();
		m_detRes[0] = emptyImg;
		return -1;
	}
	HalOpenCV::cv2halImg(im, m_image, false);

	bool patok = true;
	if (m_param->Is2Dor4D == 0) {
		if (!has_set_pattern[0] || !has_set_pattern[1]) {
			patok = false;
		}
	}
	else if (m_param->Is2Dor4D == 1) {
		if (!has_set_pattern[0] || !has_set_pattern[1] || !has_set_pattern[2] || !has_set_pattern[3]) {
			patok = false;
		}
	}
	if (!patok) {
// 		QMessageBox::information(this, tr("Warning"), tr("template unset!"), QMessageBox::Yes);
// 		res = 0;
// 		setDetectOutData(AllOK, res);
// 		setDetectOutData(ResOk, res);
// 		hom_mat2d_identity(&m_homMat2D);
// 		setDetectOutData(HomMat2D, m_homMat2D);
// 		m_testItemsStatus[0] = 0;
// 		return false;
// 		HTuple hm2d;
// 		hom_mat2d_identity(/*&(*p_outHomMat2d)*/&hm2d);
// 		m_outHomMat2d.setData(hm2d);
		*p_outHomMat2d = qzcv::hom_mat2d_identity();
		m_detRes[0] = patrnLoss;
		return -1;
	}
// 	if (isVisible())
// 		m_view->finishDraw();
	if (m_param->IsUsePinDir&&m_param->IsDefaultSig) {
		isLF = 0;
		isRT = 0;
		p_lfDir.setData(isLF);
		p_rtDir.setData(isRT);
		//*p_lfDir = isLF;
		//*p_rtDir = isRT;
// 		HTuple hm2d;
// 		hom_mat2d_identity(/*&(*p_outHomMat2d)*/&hm2d);
// 		m_outHomMat2d.setData(hm2d);
		*p_outHomMat2d = qzcv::hom_mat2d_identity();
		m_detRes[0] = defaultSig;
		return 0;

// 		setDetectOutData(LFOK, isLF);
// 		setDetectOutData(RTOK, isRT);
// 		setDetectOutData(AllOK, res);
// 		setDetectOutData(ResOk, res);
// 		setDetectOutData(PINOutEnable, m_param->IsUsePinDir);
// 		setDetectOutData(PIN1OUT, m_param->pin1OutNum);
// 		setDetectOutData(PIN2OUT, m_param->pin2OutNum);
// 		setDetectOutData(CHOOSEINDEX, 0);
// 		hom_mat2d_identity(&m_homMat2D);
// 		setDetectOutData(HomMat2D, m_homMat2D);
// 		return res;
	}
	int n[4];
	HTuple hommat2d[4];
	double similar[4];
	int regOk[4];
	for (int i = 0;i < MAX_PAT_NUM;i++) {
		regOk[i] = 1;
		similar[i] = 0;
	}
	int roinum;
	if (m_param->Is2Dor4D == 0) {
		roinum = 2;
	}
	else if (m_param->Is2Dor4D == 1) {
		roinum = 4;
	}
	HTuple hommat;
	if (m_param->IsRefer) {
		try {
			//getDetectInData(InHomMat2D, hommat);
			//hommat = *p_inHomMat2d;
			//p_inHomMat2d.getData(hommat);
			HalOpenCV::cv2HalHomMat2D(*p_inHomMat2d, &hommat);
			double tv = hommat[0].D();
		}
		catch (HException& ex) {
			hom_mat2d_identity(&hommat);
		}
		//setDetectOutData(TmpInHomMat, hommat);
	}
	for (auto i = 0;i < MAX_PAT_NUM;++i)
		m_count[i] = 0;
	for (int tp = 0;tp < roinum;tp++) {
		vector<NCCMatcher::Result> result;
		int d;     //detector的指数
		d = tp;
		double len1 = m_param->roiCol2[tp] - m_param->roiCol1[tp];
		double len2 = m_param->roiRow2[tp] - m_param->roiRow1[tp];
		if (m_param->IsRefer) {
			double roirow, roicol;
			roirow = (m_param->roiRow1[tp] + m_param->roiRow2[tp]) / 2.0;
			roicol = (m_param->roiCol1[tp] + m_param->roiCol2[tp]) / 2.0;
			affine_trans_pixel(hommat, roirow, roicol, &roirow, &roicol);
			m_detect[d]->setRoi(&QRect(roicol - len1 / 2.0, roirow - len2 / 2.0, len1
				, len2));
		}
		else {
			m_detect[d]->setRoi(&QRect(
				m_param->roiCol1[tp], m_param->roiRow1[tp], 
				m_param->roiCol2[tp] - m_param->roiCol1[tp], 
				m_param->roiRow2[tp] - m_param->roiRow1[tp]));
		}
		HTuple nchannels;
		count_channels(*m_image, &nchannels);
		if (nchannels[0].I() == 1)
			result = m_detect[d]->findPattern(m_image);
		else
		{
			Hobject tmp;
			rgb1_to_gray(*m_image, &tmp);
			result = m_detect[d]->findPattern(&tmp);
		}
		NCCMatcher::Result temp;
		n[tp] = result.size();
		m_count[tp] = n[tp];
		//setDetectOutData(COUNT, n[tp], tp);
		if (n[tp] == 0) {
			regOk[tp] = 0;
			m_detRes[tp] = detFail;
			continue;
		}
		m_detRes[tp] = detSucceed;
		int sz = 1/*result.size()*/;
		m_acceptVal[tp].resize(sz);
		m_acceptOK[tp].resize(sz);
		m_relativeOK[tp].resize(sz);
		m_relativeVal[tp].resize(sz);
		for (int i = 0;i < sz;i++)
		{
			temp = result.at(i);
			if (i == 0) {
				hommat2d[tp] = temp.homMat2d;
			}
			if (m_param->acceptEnable)
			{
				similar[tp] = temp.score;
				m_acceptVal[tp][i] = temp.score;
				m_acceptOK[tp][i] = true;
				//setDetectOutData(AcceptVal, sc, tp, i);
				if (temp.score*100.0 < m_param->acceptValue) {
					//setDetectOutData(AcceptValOk, 0, tp, i);
					m_acceptOK[tp][i] = false;
					regOk[tp] = 0;
				}
// 				else {
// 					setDetectOutData(AcceptValOk, 1, tp, i);
// 				}
			}
			if (m_param->relativeEnable)
			{
				double rotate_angle = temp.angle*180.0 / PI;
				m_relativeVal[tp][i] = rotate_angle;
				m_relativeOK[tp][i] = true;
				//setDetectOutData(Relative, rotate_angle, tp, i);
				if (abs(rotate_angle) >= m_param->minRelative&&abs(rotate_angle) <= m_param->maxRelative) {
					//setDetectOutData(RelativeOk, 1, tp, i);
				}
				else {
					//setDetectOutData(RelativeOk, 0, tp, i);
					m_relativeOK[tp][i] = false;
					regOk[tp] = 0;
				}
			}
		} //end of i loop
	} //end of tp loop
	if (m_param->Is2Dor4D == 0) {
		if (regOk[0] != 1 && regOk[1] != 1) {
			//m_testItemsStatus[0] = 0;
			res = 0;
		}
	}
	else if (m_param->Is2Dor4D == 1) {
		if (regOk[0] != 1 && regOk[1] != 1 && regOk[2] != 1 && regOk[3] != 1) {
			//m_testItemsStatus[0] = 0;
			res = 0;
		}
	}
	int chooseIndex = -1;
	bool hasStart = false;
	bool matchAngleOk = true;	
	//setDetectOutData(ResOk, res);
	if (res) {
		for (int i = 0;i < roinum;i++) {
			if (similar[i] > 0) {
				if (!hasStart) {
					chooseIndex = i;
				}
				else {
					if (similar[i] > similar[chooseIndex]) {
						chooseIndex = i;
					}
				}
				hasStart = true;
			}
		}
		//setDetectOutData(CHOOSEINDEX, chooseIndex);
		//*p_chooseIdx = chooseIndex;
		p_chooseIdx.setData(chooseIndex);

		int newIdx = chooseIndex;
		if (m_param->Is2Dor4D == 0 && chooseIndex == 1)
			newIdx = 2;
		if (m_param->matchAngleEnable) {
			if (m_param->matchAngle != newIdx) {
				matchAngleOk = false;
				res = false;
			}
			m_matchAngleOk = matchAngleOk;
			//setDetectOutData(MatchAngleOk, matchAngleOk);
		}
		if (m_param->IsUsePinDir) {
			if (m_param->Is2Dor4D == 0) {
				if (chooseIndex == 0) {
					if (m_param->IsDefaultEdit) {
						isLF = 1;
						isRT = 0;
					}
					else {
						isLF = m_param->sigEdit[0][0];
						isRT = m_param->sigEdit[1][0];
					}
				}
				else if (chooseIndex == 1) {
					if (m_param->IsDefaultEdit) {
						isLF = 0;
						isRT = 1;
					}
					else {
						isLF = m_param->sigEdit[0][2];
						isRT = m_param->sigEdit[1][2];
					}
				}
			}
			else if (m_param->Is2Dor4D == 1) {
				if (m_param->IsDefaultEdit) {
					if (chooseIndex == 0) {
						isLF = 0;
						isRT = 0;
					}
					else if (chooseIndex == 1) {
						isLF = 0;
						isRT = 1;
					}
					else if (chooseIndex == 2) {
						isLF = 1;
						isRT = 0;
					}
					else if (chooseIndex == 3) {
						isLF = 1;
						isRT = 1;
					}
				}
				else {
					isLF = m_param->sigEdit[0][chooseIndex];
					isRT = m_param->sigEdit[1][chooseIndex];
				}
			}
			p_lfDir.setData(isLF);
			p_rtDir.setData(isRT);
			//*p_lfDir = isLF;
			//*p_rtDir = isRT;
			//setDetectOutData(LFOK, isLF);
			//setDetectOutData(RTOK, isRT);
		}
		else
		{
			p_lfDir.setData(0);
			p_rtDir.setData(0);
			//*p_lfDir = 0;
			//*p_rtDir = 0;
		}
	}
	if (res) {
		//setDetectOutData(HomMat2D, hommat2d[chooseIndex >= 0 ? chooseIndex : 0]);
		m_outHomMat2d = hommat2d[chooseIndex >= 0 ? chooseIndex : 0];
		HalOpenCV::hal2cvHomMat2D(m_outHomMat2d, *p_outHomMat2d);
	}
	else {
		//hom_mat2d_identity(&m_homMat2D);
		//setDetectOutData(HomMat2D, m_homMat2D);
	}
	m_allOk = res ? 0 : -1;
	//setDetectOutData(AllOK, res);
	//setDetectOutData(PINOutEnable, m_param->IsUsePinDir);
	//setDetectOutData(PIN1OUT, m_param->pin1OutNum);
	//setDetectOutData(PIN2OUT, m_param->pin2OutNum);

	if (res && m_param->IsUseOutArea) {
		Hobject outRegion;
		if (m_param->OutImg[chooseIndex].Id() != H_EMPTY_REGION) {
			threshold(m_param->OutImg[chooseIndex], &outRegion, m_param->outThreMin, m_param->outThreMax < m_param->outThreMin ? m_param->outThreMin : m_param->outThreMax);
			opening_circle(outRegion, &outRegion, 3);
			closing_circle(outRegion, &outRegion, 3);
			connection(outRegion, &outRegion);
			select_shape(outRegion, &outRegion, "area", "and", m_param->outAreaMin, m_param->outAreaMax < m_param->outAreaMin ? m_param->outAreaMin : m_param->outAreaMax);
			if (m_param->IsFillUp) {
				fill_up(outRegion, &outRegion);
			}
			union1(outRegion, &outRegion);
			affine_trans_region(outRegion, &outRegion, m_outHomMat2d/*m_homMat2D*/, "false");
			copy_obj(outRegion, m_outArea, 1, 1);
			HalOpenCV::hal2cvRegion(*m_outArea, &(*p_outArea));
			//setDetectOutData(OutAreaPtr, (Hlong)m_outArea);
			//setDetectOutObject(OutArea, outRegion);
		}
		else {
			gen_empty_obj(m_outArea);
			*p_outArea = RlRegion();
			//setDetectOutData(OutAreaPtr, (Hlong)m_outArea);
			//setDetectOutObject(OutArea, *m_outArea);
		}

	}

	return m_allOk;
}

void nccMatch_fourDirMod::viewResult(ImageView *iv, const QString &funName, int)
{
	if (m_detRes[0] == emptyImg)
		return;
	else if (m_detRes[0] == patrnLoss)
		return;
	else if (m_detRes[0] == defaultSig)
	{
		iv->setColor(0, 255, 255);
		iv->dispText(400, 50, QObject::tr("direction of 0"));
		return;
	}
// 	HTuple allok, resok, matchok;
// 	getDispOutData(AllOK, allok);
// 	getDispOutData(ResOk, resok);
	if (m_resOk == 0) {
		iv->setFilled(false);
		iv->setColor(Qt::red);
		iv->dispRect1(m_param->roiRow1[0], m_param->roiCol1[0], m_param->roiRow2[0], m_param->roiCol2[0]);
		iv->dispRect1(m_param->roiRow1[1], m_param->roiCol1[1], m_param->roiRow2[1], m_param->roiCol2[1]);
		if (m_param->Is2Dor4D == 1) {
			iv->dispRect1(m_param->roiRow1[2], m_param->roiCol1[2], m_param->roiRow2[2], m_param->roiCol2[2]);
			iv->dispRect1(m_param->roiRow1[3], m_param->roiCol1[3], m_param->roiRow2[3], m_param->roiCol2[3]);
		}
		return;
	}
	iv->setFilled(false);
	iv->setColor(Qt::red);
	HTuple chooseindex;
	if (m_allOk == 0) {
		if (m_param->matchAngleEnable && !m_matchAngleOk) {
			iv->setColor(255, 0, 0);
		}
		else {
			iv->setColor(0, 255, 255);
		}
		Hobject patternRoi;
		HTuple homMat2d = m_outHomMat2d;
// 		HTuple tmpIdx;
// 		p_chooseIdx.getData(tmpIdx);
// 		int dindex = tmpIdx[0].I();
		int dindex;
		p_chooseIdx.getData(dindex);
		//int dindex = (*p_chooseIdx)[0].I();
		if (dindex == -1) {
			return;
		}
		if (m_param->IsUsePinDir) {
			iv->setColor(0, 255, 255);
			if (m_param->Is2Dor4D == 0) {
				iv->dispText(400, 50, QObject::tr("direction of %1").arg(180 * dindex));
			}
			else {
				iv->dispText(400, 50, QObject::tr("direction of %1").arg(90 * dindex));
			}
		}
		if (m_param->matchAngleEnable && !m_matchAngleOk) {
			return;
		}
		gen_rectangle2(&patternRoi, 
			m_param->patternY[dindex] + m_param->patternH[dindex] / 2,
			m_param->patternX[dindex] + m_param->patternW[dindex] / 2, 
			0, m_param->patternW[dindex] / 2, m_param->patternH[dindex] / 2);
		affine_trans_region(patternRoi, &patternRoi, homMat2d, "false");
		iv->setFilled(false);
		RlRegion rlReg;
		HalOpenCV::hal2cvRegion(patternRoi, &rlReg);
		iv->dispRegion(&rlReg);
		if (m_param->minusArea[dindex] == 1) {                  //暂时不显示排除区域?
			Hobject minusrec;
			gen_rectangle2(&minusrec, m_param->minusRow[dindex], m_param->minusCol[dindex], m_param->minusA[dindex], m_param->minusLength1[dindex], m_param->minusLength2[dindex]);
			affine_trans_region(minusrec, &minusrec, homMat2d, "false");
			iv->setFilled(false);
			RlRegion r;
			HalOpenCV::hal2cvRegion(minusrec, &r);
			iv->dispRegion(&r);
		}
		else if (m_param->minusArea[dindex] == 2) {
			double cy, cx;
			affine_trans_pixel(homMat2d, m_param->minusCY[dindex], m_param->minusCX[dindex], &cy, &cx);
			iv->dispCircle(cy, cx, m_param->minusR[dindex]);
		}
	}

	if (m_allOk == 0 && m_param->IsUseOutArea&&m_param->IsShowOutArea) {
		Hobject outarea;
		iv->setFilled(false);
		iv->setColor(0, 255, 255);
		iv->dispRegion(&(*p_outArea));
	}
// 	HTuple imagefull;
// 	getDispOutData(ImageFull, imagefull);
// 	if (!imagefull[0].I()) {
// 		return;
// 	}
// 	if (m_param->Is2Dor4D == 0) {
// 		if (!has_set_pattern[0] || !has_set_pattern[1]) {
// 			return;
// 		}
// 	}
// 	else if (m_param->Is2Dor4D == 1) {
// 		if (!has_set_pattern[0] || !has_set_pattern[1] || !has_set_pattern[2] || !has_set_pattern[3]) {
// 			return;
// 		}
// 	}
// 	if (m_param->IsDefaultSig&&m_param->IsUsePinDir) {
// 		m_view->setColor(0, 255, 255);
// 		m_view->dispText(400, 50, "方向0度");
// 		return;
// 	}
// 	HTuple allok, resok, matchok;
// 	getDispOutData(AllOK, allok);
// 	getDispOutData(ResOk, resok);
// 	if (m_param->matchAngleEnable) {
// 		getDispOutData(MatchAngleOk, matchok);
// 	}
// 	if (resok[0].I() == 0) {
// 		m_view->setFilled(false);
// 		m_view->setColor(255, 0, 0);
// 		m_view->dispRect1(m_param->roiRow1[0], m_param->roiCol1[0], m_param->roiRow2[0], m_param->roiCol2[0]);
// 		m_view->dispRect1(m_param->roiRow1[1], m_param->roiCol1[1], m_param->roiRow2[1], m_param->roiCol2[1]);
// 		if (m_param->Is2Dor4D == 1) {
// 			m_view->dispRect1(m_param->roiRow1[2], m_param->roiCol1[2], m_param->roiRow2[2], m_param->roiCol2[2]);
// 			m_view->dispRect1(m_param->roiRow1[3], m_param->roiCol1[3], m_param->roiRow2[3], m_param->roiCol2[3]);
// 		}
// 		return;
// 	}
// 	m_view->setFilled(false);
// 	m_view->setColor(0, 0, 255);
// 	HTuple  followRecRow, followRecCol, followCirRow, followCirCol;
// 	HTuple chooseindex;
// 	if (allok[0].I()) {
// 		if (m_param->matchAngleEnable && !matchok[0].I()) {
// 			m_view->setColor(255, 0, 0);
// 		}
// 		else {
// 			m_view->setColor(0, 255, 255);
// 		}
// 		getDispOutData(CHOOSEINDEX, chooseindex);
// 		Hobject patternRoi;
// 		HTuple homMat2d;
// 		getDispOutData(HomMat2D, homMat2d);
// 		int dindex;
// 		dindex = chooseindex[0].I();
// 		if (dindex == -1) {
// 			return;
// 		}
// 		if (m_param->IsUsePinDir) {
// 			m_view->setColor(0, 255, 255);
// 			if (m_param->Is2Dor4D == 0) {
// 				if (dindex == 0)
// 					m_view->dispText(400, 50, "方向0度");
// 				else if (dindex == 1)
// 					m_view->dispText(400, 50, "方向180度");
// 			}
// 			else {
// 				if (dindex == 0)
// 					m_view->dispText(400, 50, "方向0度");
// 				else if (dindex == 1)
// 					m_view->dispText(400, 50, "方向90度");
// 				else if (dindex == 2)
// 					m_view->dispText(400, 50, "方向180度");
// 				else if (dindex == 3)
// 					m_view->dispText(400, 50, "方向270度");
// 			}
// 		}
// 		if (m_param->matchAngleEnable && !matchok[0].I()) {
// 			return;
// 		}
// 		gen_rectangle2(&patternRoi, m_param->patternY[dindex] + m_param->patternH[dindex] / 2, m_param->patternX[dindex] + m_param->patternW[dindex] / 2, 0, m_param->patternW[dindex] / 2, m_param->patternH[dindex] / 2);
// 		affine_trans_region(patternRoi, &patternRoi, homMat2d, "false");
// 		m_view->setFilled(false);
// 		m_view->dispRegion(patternRoi);
// 		if (m_param->minusArea[dindex] == 1) {                  //暂时不显示排除区域?
// 			Hobject minusrec;
// 			gen_rectangle2(&minusrec, m_param->minusRow[dindex], m_param->minusCol[dindex], m_param->minusA[dindex], m_param->minusLength1[dindex], m_param->minusLength2[dindex]);
// 			affine_trans_region(minusrec, &minusrec, homMat2d, "false");
// 			m_view->setFilled(false);
// 			m_view->dispRegion(minusrec);
// 		}
// 		else if (m_param->minusArea[dindex] == 2) {
// 			double cy, cx;
// 			affine_trans_pixel(homMat2d, m_param->minusCY[dindex], m_param->minusCX[dindex], &cy, &cx);
// 			m_view->dispCircle(cy, cx, m_param->minusR[dindex]);
// 		}
// 	}
// 
// 	if (allok[0].I() && m_param->IsUseOutArea&&m_param->IsShowOutArea) {
// 		Hobject outarea;
// 		m_view->setFilled(false);
// 		m_view->setColor(0, 255, 255);
// 		m_view->dispRegion(*getDispObject(OutArea));
// 	}
}

void nccMatch_fourDirMod::textResult(ResultText *text, const QString &funName)
{
	int roinum = (m_param->Is2Dor4D == 0) ? 2 : 4;

	if (m_detRes[0] == emptyImg)
	{
		text->setTextColor(Qt::red);
		text->append(QObject::tr("Empty image"));
		return;
	}
	else if (m_detRes[0] == patrnLoss)
	{
		text->setTextColor(Qt::red);
		text->append(QObject::tr("Template unset!"));
		return;
	}
	else if (m_detRes[0] == defaultSig)
	{
		text->setTextColor(Qt::green);
		text->append(QObject::tr("direction of 0"));
		return;
	}
	
	bool isSucceed = false;
	for (auto i = 0;i < roinum;++i)
		if (m_detRes[i] == detSucceed)
		{
			isSucceed = true;
			break;
		}
	if (!isSucceed)
	{
		text->setTextColor(Qt::red);
		text->append(QObject::tr("detected failed"));
		return;
	}

	for (int j = 0;j < roinum;j++) {
		int sz = m_count[j] ? 1 : 0;
		for (int i = 0;i < sz;i++) {
			text->setTextColor(Qt::black);
			text->append(QObject::tr("region%1: ").arg(j + 1));
			text->setTextColor(Qt::blue);
			text->append(QObject::tr("detection result:%1").arg(i + 1));
			QString str1, str2;
			if (m_param->acceptEnable) {
				text->append(QObject::tr("accepted value: "));
				if (m_acceptOK[j][i] == 0) {
					text->setTextColor(Qt::red);
					str1 = QString("<");
					str2 = QString("Fail");
				}
				else {
					str1 = QString(" >=");
					str2 = QString("Pass");
				}
				text->insertPlainText(QString::number(100 * m_acceptVal[j][i], 'i', 0));
				text->setTextColor(Qt::blue);
				text->insertPlainText(QString(" %1  (%2) %3").arg(str1).arg(m_param->acceptValue).arg(str2));
			}
			if (m_param->relativeEnable)
			{
				text->append(QObject::tr("relative angle: "));
				if (m_relativeOK[j][i] == true) {
					str1 = QString(" in");
					str2 = QString("Pass");
				}
				else {
					text->setTextColor(Qt::red);
					str1 = QString(" out");
					str2 = QString("Fail");
				}
				text->insertPlainText(QString::number(m_relativeVal[j][i], 'g', 4));
				text->setTextColor(Qt::blue);
				text->insertPlainText(QString(" %1  (%2,%3) %4").arg(str1)
					.arg(m_param->minRelative).arg(m_param->maxRelative).arg(str2));
			}
		}
	}
	if (m_allOk == 0) {
		text->setTextColor(Qt::green);
		if (m_param->IsUsePinDir) {
// 			HTuple tmpIdx;
// 			p_chooseIdx.getData(tmpIdx);
// 			int dindex = tmpIdx[0].I();
//			int dindex = (*p_chooseIdx)[0].I();
			int dindex;
			p_chooseIdx.getData(dindex);
			if (m_param->Is2Dor4D == 0) {
				text->append(QObject::tr("direction of %1").arg(180 * dindex));
			}
			else if (m_param->Is2Dor4D == 1) {
				text->append(QObject::tr("direction of %1").arg(90 * dindex));
			}
		}
		text->setTextColor(Qt::green);
		text->append(QObject::tr("detected succesfully"));
	}
	else {
		if (m_param->matchAngleEnable && m_resOk == 0) {
			if (!m_matchAngleOk) {
				text->setTextColor(Qt::red);
				text->append(QObject::tr("matching angle failed "));
			}
		}
	}
// 	HTuple imagefull;
// 	getDispOutData(ImageFull, imagefull);
// 	if (!imagefull[0].I()) {
// 		return;
// 	}
// 	int roinum;
// 	if (m_param->Is2Dor4D == 0) {
// 		roinum = 2;
// 		if (!has_set_pattern[0] || !has_set_pattern[1]) {
// 			return;
// 		}
// 	}
// 	else if (m_param->Is2Dor4D == 1) {
// 		roinum = 4;
// 		if (!has_set_pattern[0] || !has_set_pattern[1] || !has_set_pattern[2] || !has_set_pattern[3]) {
// 			return;
// 		}
// 	}
// 	if (m_param->IsDefaultSig) {
// 		m_text->setTextColor(Qt::green);
// 		m_text->append(tr("direction of 0"));
// 		return;
// 	}
// 	HTuple allok;
// 	getDispOutData(AllOK, allok);
// 	HTuple targetCount[4];
// 	getDispOutData(COUNT, 0, targetCount[0]);
// 	getDispOutData(COUNT, 1, targetCount[1]);
// 	if (m_param->Is2Dor4D == 1) {
// 		getDispOutData(COUNT, 2, targetCount[2]);
// 		getDispOutData(COUNT, 3, targetCount[3]);
// 	}
// 	if (m_param->Is2Dor4D == 0) {
// 		if (targetCount[0][0].I() == 0 && targetCount[1][0].I() == 0) {
// 			m_text->setTextColor(Qt::red);
// 			m_text->append(tr("detected failed"));
// 			return;
// 		}
// 	}
// 	else {
// 		if (targetCount[0][0].I() == 0 && targetCount[1][0].I() == 0 && targetCount[2][0].I() == 0 && targetCount[3][0].I() == 0) {
// 			m_text->setTextColor(Qt::red);
// 			m_text->append(tr("detected failed"));
// 			return;
// 		}
// 	}
// 	for (int j = 0;j < roinum;j++) {
// 		for (int i = 0;i < targetCount[j][0].I();i++) {
// 			m_text->setTextColor(Qt::black);
// 			m_text->append(tr("region%1: ").arg(QString::number(j + 1)));
// 			m_text->setTextColor(Qt::blue);
// 			m_text->append(tr("detection result:%1").arg(QString::number(i + 1)));
// 			QString str1, str2;
// 			if (m_param->acceptEnable) {
// 				HTuple acceptOK, accepVal;
// 				getDispOutData(AcceptValOk, j, i, acceptOK);
// 				getDispOutData(AcceptVal, j, i, accepVal);
// 				m_text->append(tr("accepted value: "));
// 				if (acceptOK[0].I() == 0) {
// 					m_text->setTextColor(Qt::red);
// 					str1 = QString("<");
// 					str2 = QString("Fail");
// 				}
// 				else {
// 					str1 = QString(" >=");
// 					str2 = QString("Pass");
// 				}
// 				m_text->insertPlainText(QString::number(100 * accepVal[0].D(), 'i', 0));
// 				m_text->setTextColor(Qt::blue);
// 				m_text->insertPlainText(QString(" %1  (%2) %3").arg(str1).arg(QString::number(m_param->acceptValue)).arg(str2));
// 			}
// 			if (m_param->relativeEnable)
// 			{
// 				HTuple relativeOK, relativeVal;
// 				getDispOutData(Relative, j, i, relativeVal);
// 				getDispOutData(RelativeOk, j, i, relativeOK);
// 				m_text->append(tr("relative angle: "));
// 				if (relativeOK[0].I() == 1) {
// 					str1 = QString(" in");
// 					str2 = QString("Pass");
// 				}
// 				else {
// 					m_text->setTextColor(Qt::red);
// 					str1 = QString(" out");
// 					str2 = QString("Fail");
// 				}
// 				m_text->insertPlainText(QString::number(relativeVal[0].D(), 'g', 4));
// 				m_text->setTextColor(Qt::blue);
// 				m_text->insertPlainText(QString(" %1  (%2,%3) %4").arg(str1)
// 					.arg(QString::number(m_param->minRelative)).arg(QString::number(m_param->maxRelative)).arg(str2));
// 			}
// 		}
// 	}
// 	if (allok[0].I() != 0) {
// 		m_text->setTextColor(Qt::green);
// 		HTuple islf, isrt;
// 		if (m_param->IsUsePinDir) {
// 			getDispOutData(LFOK, islf);
// 			getDispOutData(RTOK, isrt);
// 			HTuple chooseindex;
// 			getDispOutData(CHOOSEINDEX, chooseindex);
// 			int dindex = chooseindex[0].I();
// 			if (m_param->Is2Dor4D == 0) {
// 				if (dindex == 0) {
// 					m_text->append(tr("direction of 0"));
// 				}
// 				else if (dindex == 1) {
// 					m_text->append(tr("direction of 180"));
// 				}
// 			}
// 			else if (m_param->Is2Dor4D == 1) {
// 				if (dindex == 0) {
// 					m_text->append(tr("direction of 0"));
// 				}
// 				else if (dindex == 1) {
// 					m_text->append(tr("direction of 90"));
// 				}
// 				else if (dindex == 2) {
// 					m_text->append(tr("direction of 180"));
// 				}
// 				else if (dindex == 3) {
// 					m_text->append(tr("direction of 270"));
// 				}
// 			}
// 		}
// 		m_text->setTextColor(Qt::green);
// 		m_text->append(tr("detected succesfully"));
// 	}
// 	else {
// 		HTuple resok;
// 		getDispOutData(ResOk, resok);
// 		if (m_param->matchAngleEnable && resok[0].I()) {
// 			HTuple matchok;
// 			getDispOutData(MatchAngleOk, matchok);
// 			if (!matchok[0].I()) {
// 				m_text->setTextColor(Qt::red);
// 				m_text->append(tr("matching angle failed "));
// 			}
// 		}
// 	}
}

void nccMatch_fourDirMod::iniData()
{
	m_param = new nccMatch_fourDirParam;
	createPins();
	createEvents();
	m_image = new Hobject;
	m_outArea = new Hobject;

	for (int i = 0;i < MAX_PAT_NUM;i++) {
		m_detect[i] = new NCCMatcher;
		m_detect[i]->setMaxOverlap(m_param->maxOverlap);
		m_detect[i]->setNum(m_param->findNum);
		m_detect[i]->setSimilar((double)m_param->similarity / 100.0);
		m_detect[i]->setRotateDeg(m_param->seekingAngle);
		has_set_pattern[i] = 0;

		if (m_param->patImage[i].Id() != H_EMPTY_REGION)
		{
			resetPattern(i);
			m_detect[i]->setRoi(&QRect(m_param->roiCol1[i], m_param->roiRow1[i], 
				m_param->roiCol2[i] - m_param->roiCol1[i], m_param->roiRow2[i] - m_param->roiRow1[i]));
			has_set_pattern[i] = 1;
		}

		m_detRes[i] = noDet;

		m_count[i] = 0;

		m_acceptOK[i].clear();
		m_acceptVal[i].clear();

		m_relativeOK[i].clear();
		m_relativeVal[i].clear();
	}

	m_allOk = 0;
	m_resOk = 0;
	m_matchAngleOk = true;
}

void nccMatch_fourDirMod::createPins()
{
	addPin(&p_im, "im");
	addPin(&p_inHomMat2d, "iHm2d");

	addPin(&p_outHomMat2d, "oHm2d");
	addPin(&p_chooseIdx, "dirIdx");
	//addPin(&p_lfIdx, "lfIdx");
	//addPin(&p_rtIdx, "rtIdx");
	addPin(&p_lfDir, "lf");
	addPin(&p_rtDir, "rt");
	addPin(&p_outArea, "area");

	*p_outHomMat2d = qzcv::hom_mat2d_identity();

	p_inHomMat2d.setVisible(false);
	//p_lfIdx.setVisible(false);
	//p_rtIdx.setVisible(false);
	p_outArea.setVisible(false);
}

void nccMatch_fourDirMod::createEvents()
{

}

void nccMatch_fourDirMod::resetPattern(int index)
{
	if (m_param->minusArea[index] == 0)
		m_detect[index]->setPattern(m_param->patImage[index], m_param->patternX[index], m_param->patternY[index]);
	else
	{
		NCCMatcher::Region region;
		region.isCircle = (m_param->minusArea[index] == 2);
		region.angle = m_param->minusA[index];
		region.w = m_param->minusLength1[index] * 2;
		region.h = m_param->minusLength2[index] * 2;
		region.R = m_param->minusR[index];
		if (region.isCircle) {
			region.cx = m_param->minusCX[index] - m_param->patternX[index];
			region.cy = m_param->minusCY[index] - m_param->patternY[index];
		}
		else {
			region.cx = m_param->minusCol[index] - m_param->patternX[index];
			region.cy = m_param->minusRow[index] - m_param->patternY[index];
		}
		m_detect[index]->setPattern(m_param->patImage[index], m_param->patternX[index], m_param->patternY[index], &region);
	}
}

