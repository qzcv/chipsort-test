#include "nccMatch_outNMod.h"
#include "nccMatch_outNParam.h"
#include <QObject>
#include <ImageView.h>
#include <resultText.h>
#include "nccmatch.h"
#include "halOpenCV.h"
#include "HalconCpp.h"
#include "string.h"
#include "transformation.h"

using namespace std;

nccMatch_outNMod::nccMatch_outNMod()
{
	iniData();
}

nccMatch_outNMod::nccMatch_outNMod(const QString& dirPath)
{
	iniData();
	load(dirPath, StructLevel);
}

nccMatch_outNMod::~nccMatch_outNMod()
{
	for (int i = 0;i < MAX_TEMP_NUM;i++) {
		delete m_detect[i];
	}
	delete m_param;
	delete m_image;
}

int nccMatch_outNMod::command(int cmdID, void* dataIn, void* dataOut)
{
	return 0;
}

int nccMatch_outNMod::afterSetProperty(MetaProperty*)
{
	return 0;
}

void nccMatch_outNMod::save(const QString &dirPath)
{
	m_param->readWriteParam(false, dirPath, StructLevel);
	resetParamChanged();
}

void nccMatch_outNMod::load(const QString &dirPath, QvsParamLevel level)
{
	m_param->readWriteParam(true,dirPath,level);

	if (level == ProductLevel)
	{
		for (auto i = 0;i < MAX_TEMP_NUM;++i)
		{
			//p_homMat2D[i].setVisible(m_param->GrpEnable[i]);
			//p_outArea[i].setVisible(m_param->IsUseOutArea[i]);

			m_detect[i]->setMaxOverlap(m_param->maxOverlap[i]);
			m_detect[i]->setNum(m_param->findNum[i]);
			m_detect[i]->setSimilar((float)m_param->similarity[i] / 100.0);
			m_detect[i]->setRotateDeg(m_param->seekingAngle[i]);

			has_set_pattern[i] = 0;
			if (m_param->patImage[i].Id() != H_EMPTY_REGION) {
				resetPattern(i);
				m_detect[i]->setRoi(&QRect(m_param->roix[i], m_param->roiy[i], m_param->roiw[i], m_param->roih[i]));
				has_set_pattern[i] = 1;
			}
		}
	}
}

int nccMatch_outNMod::run(const QString &funName)
{
// 	for (int i = 0;i < MAX_TESTITEMS_NUM;i++) {
// 		m_testItemsStatus[i] = 1;
// 	}
//	int res = 1;
// 	if (!m_image || m_image->Id() == H_EMPTY_REGION) {
// 		res = 0;
// 		setDetectOutData(AllOK, res);
// 		for (int i = 0;i < MAX_TEMP_NUM;i++) {
// 			hom_mat2d_identity(&m_homMat2D[i]);
// 			setDetectOutData(m_outHomDefine[i], m_homMat2D[i]);
// 		}
// 		setDetectOutData(ImageFull, false);
// 		m_testItemsStatus[0] = 0;
// 		return false;
// 	}
// 	else {
// 		setDetectOutData(ImageFull, true);
// 	}
	for(auto i=0;i<MAX_TEMP_NUM;++i)
		m_detRes[i] = noDet;
	m_allok = 0;
	if (p_im->empty())
	{
		m_detRes[0] = emptyImg;
		for (int i = 0;i < MAX_TEMP_NUM;i++)
		{
			*p_homMat2D[i] = qzcv::hom_mat2d_identity();
			*p_outArea[i] = RlRegion();
		}
		return -1;
	}
	HalOpenCV::cv2halImg(*p_im, m_image, false);
	//bool tempok[MAX_TEMP_NUM];
	for (int t = 0;t < MAX_TEMP_NUM;t++) {   //对所有的模板进行遍历
		//tempok[t] = true;
		m_patternOK[t] = true;
		if (!m_param->GrpEnable[t]) {
			continue;
		}
		if (!has_set_pattern[t]) {
			//res = 0;
			//tempok[t] = false;
			//setDetectOutData(TempOK, tempok[t], t);
			//setDetectOutData(AllOK, res);
			//m_testItemsStatus[0] = 0;
			m_patternOK[t] = false;
			m_detRes[t] = patrnLoss;
			for (int i = 0;i < MAX_TEMP_NUM;i++)
			{
				*p_homMat2D[i] = qzcv::hom_mat2d_identity();
				*p_outArea[i] = RlRegion();
			}
			return -1;
		}
		if (m_param->OutImg[t].Id() == H_EMPTY_REGION&&m_param->IsUseOutArea[t]) {
			//QMessageBox::information(this, tr("Warning"), tr("The template%1 of outArea is unset!").arg(t + 1), QMessageBox::Yes);
			//res = 0;
			//tempok[t] = false;
			//setDetectOutData(TempOK, tempok[t], t);
			//setDetectOutData(AllOK, res);
			//m_testItemsStatus[0] = 0;
			m_patternOK[t] = false;
			m_detRes[t] = emptyOutImg;
			for (int i = 0;i < MAX_TEMP_NUM;i++)
			{
				*p_homMat2D[i] = qzcv::hom_mat2d_identity();
				*p_outArea[i] = RlRegion();
			}
			return -1;
		}
		vector<NCCMatcher::Result> result;
		HTuple nchannels;
		count_channels(*m_image, &nchannels);
		if (nchannels[0].I() == 1)
			result = m_detect[t]->findPattern(m_image);
		else {
			Hobject tmp;
			rgb1_to_gray(*m_image, &tmp);
			result = m_detect[t]->findPattern(&tmp);
		}
		NCCMatcher::Result temp;
		int n = result.size();
		//setDetectOutData(COUNT, n, t);
		if (n == 0) {
			//res = 0;
			//tempok[t] = false;
			//setDetectOutData(TempOK, tempok[t], t);
			//setDetectOutData(AllOK, res);
			//setDetectOutData(m_outHomDefine[t], m_homMat2D[t]);
			//m_testItemsStatus[0] = 0;
			m_patternOK[t] = false;
			m_detRes[t] = detFail;
			for (int i = 0;i < MAX_TEMP_NUM;i++)
			{
				*p_homMat2D[i] = qzcv::hom_mat2d_identity();
				*p_outArea[i] = RlRegion();
			}
			return -1;
		}
		m_detRes[t] = detSucceed;
// 		HTuple followRecRow, followRecCol, followCirRow, followCirCol, resultRow, resultCol;
// 		HTuple resultAngle;

		m_acceptOK[t].resize(n);
		m_offsetOK[t].resize(n);
		m_relativeOK[t].resize(n);
		m_axisOK[t].resize(n);
		m_acceptVal[t].resize(n);
		m_offsetVal[t].resize(n);
		m_relativeVal[t].resize(n);
		m_axisVal[t].resize(n);
		for (auto i = 0;i < result.size();i++)
		{
			temp = result.at(i);
			if (i == 0) {
				m_homMat2D[t] = temp.homMat2d;
				HalOpenCV::hal2cvHomMat2D(m_homMat2D[t], *p_homMat2D[t]);
				//m_homMat2D[t] = temp.homMat2d;
				//setDetectOutData(m_outHomDefine[t], temp.homMat2d);
			}
// 			resultRow[i] = temp.row;
// 			resultCol[i] = temp.col;
// 			resultAngle[i] = temp.angle;
			if (m_param->acceptEnable[t])
			{
				m_acceptVal[t][i] = temp.score;
				m_acceptOK[t][i] = true;
				//setDetectOutData(AcceptVal, sc, t, i);
				if (temp.score*100.0 < m_param->acceptValue[t]) {
					//setDetectOutData(AcceptValOk, 0, t, i);
					//res = 0;
					//m_testItemsStatus[1] = 0;
					//tempok[t] = false;
					m_acceptOK[t][i] = false;
					m_patternOK[t] = false;
					m_allok = -2;
				}
// 				else {
// 					setDetectOutData(AcceptValOk, 1, t, i);
// 				}
			}
			if (m_param->offsetEnable[t]) {
				double patternx, patterny;
				m_detect[t]->getMinusRowCol(patterny, patternx);
				int offset_value = (int)distance_of_two_point(QPointF(temp.col, temp.row),
					QPointF(patternx + m_param->patternX[t], patterny + m_param->patternY[t]));
				//setDetectOutData(OffsetVal, offset_value, t, i);
				m_offsetVal[t][i] = offset_value;
				m_offsetOK[t][i] = true;
				if (offset_value > m_param->offsetValue[t]) {
					//setDetectOutData(OffsetValOk, 0, t, i);
					//m_testItemsStatus[2] = 0;
					//res = 0;
					//tempok[t] = false;
					m_offsetOK[t][i] = false;
					m_patternOK[t] = false;
					m_allok = -3;
				}
// 				else {
// 					setDetectOutData(OffsetValOk, 1, t, i);
// 				}
			}
			if (m_param->relativeEnable[t])
			{
				double rotate_angle = temp.angle*180.0 / PI;
				//setDetectOutData(Relative, rotate_angle, t, i);
				m_relativeVal[t][i] = rotate_angle;
				m_relativeOK[t][i] = true;
				if (abs(rotate_angle) >= m_param->minRelative[t] &&
					abs(rotate_angle) <= m_param->maxRelative[t]) {
					//setDetectOutData(RelativeOk, 1, t, i);
				}
				else {
					//setDetectOutData(RelativeOk, 0, t, i);
					//m_testItemsStatus[3] = 0;
					//res = 0;
					//tempok[t] = false;
					m_relativeOK[t][i] = false;
					m_patternOK[t] = false;
					m_allok = -4;
				}
			}
			if (m_param->IsAxisAngle[t])
			{
				double rotaangle = temp.angle*180.0 / PI;
				while (!(rotaangle <= 90.0&&rotaangle >= 0)) {
					if (rotaangle > 90.0) {
						rotaangle -= 90.0;
					}
					else if (rotaangle < 0) {
						rotaangle += 90.0;
					}
				}
				double offangle = 0;
				if (rotaangle <= 45.0) {
					offangle = rotaangle - 0;
				}
				else if (rotaangle > 45.0) {
					offangle = 90.0 - rotaangle;
				}
				//setDetectOutData(AxisAngle, offangle, t, i);
				m_axisVal[t][i] = offangle;
				m_axisOK[t][i] = true;
				if (abs(offangle) <= m_param->maxAxisAngle[t]) {
					//setDetectOutData(AxisAngleOk, 1, t, i);
				}
				else {
					//setDetectOutData(AxisAngleOk, 0, t, i);
					//m_testItemsStatus[4] = 0;
					//res = 0;
					//tempok[t] = false;
					m_axisOK[t][i] = false;
					m_patternOK[t] = false;
					m_allok = -5;
				}
			}
		}
// 		setDetectOutData(ResultRow, resultRow, t);
// 		setDetectOutData(ResultCol, resultCol, t);
// 		setDetectOutData(ResultAngle, resultAngle, t);
// 		if (m_param->countEnable[t]) {
// 			int n = result.size();
// 			setDetectOutData(COUNT, n, t);
// 			if ((result.size() < m_param->minCount[t]) && 
// 				(result.size() > m_param->maxCount[t])) {
// 				setDetectOutData(COUNTOK, 0, t);
// 				//res = 0;
// 				tempok[t] = false;
// 				m_allok = -6;
// 			}
// 			else {
// 				setDetectOutData(COUNTOK, 1, t);
// 			}
// 		}
		if (/*tempok[t]*/m_patternOK[t] && m_param->IsUseOutArea[t]) {
			Hobject outRegion;

			Hobject unionRec, roiRec;
			Hobject imgRec;
			Hobject dstImg;
			HTuple wid, hei;
			get_image_size(*m_image, &wid, &hei);
			gen_rectangle1(&imgRec, 0, 0, hei - 1, wid - 1);
			for (int i = 0;i < m_param->outDeleteCount[t];i++) {
				gen_rectangle2(&roiRec, m_param->deleteRegionVec[t][i].drow, m_param->deleteRegionVec[t][i].dcol,
					m_param->deleteRegionVec[t][i].dangle, m_param->deleteRegionVec[t][i].dlength1,
					m_param->deleteRegionVec[t][i].dlength2);
				union2(roiRec, unionRec, &unionRec);
			}
			difference(imgRec, unionRec, &unionRec);
			reduce_domain(m_param->OutImg[t], unionRec, &dstImg);
			Hobject outrec, outunion;
			for (int i = 0;i < m_param->outRegionCount[t];i++) {
				gen_rectangle2(&outrec, m_param->outRegionVec[t][i].row, m_param->outRegionVec[t][i].col,
					m_param->outRegionVec[t][i].angle, m_param->outRegionVec[t][i].length1,
					m_param->outRegionVec[t][i].length2);
				union2(outrec, outunion, &outunion);
			}
			reduce_domain(dstImg, outunion, &dstImg);

			threshold(dstImg, &outRegion, m_param->outThreMin[t], m_param->outThreMax[t] < m_param->outThreMin[t] ? m_param->outThreMin[t] : m_param->outThreMax[t]);
			if (m_param->openVal[t] > 0) {
				opening_circle(outRegion, &outRegion, m_param->openVal[t]);
			}
			if (m_param->closeVal[t] > 0) {
				closing_circle(outRegion, &outRegion, m_param->closeVal[t]);
			}
			connection(outRegion, &outRegion);
			select_shape(outRegion, &outRegion, "area", "and", m_param->outAreaMin[t], m_param->outAreaMax[t] < m_param->outAreaMin[t] ? m_param->outAreaMin[t] : m_param->outAreaMax[t]);
			if (m_param->IsFillUp[t]) {
				fill_up(outRegion, &outRegion);
			}
			union1(outRegion, &outRegion);
			affine_trans_region(outRegion, &outRegion, m_homMat2D[t], "false");
			copy_obj(outRegion, &m_outArea[t], 1, 1);
			HalOpenCV::hal2cvRegion(m_outArea[t], &(*p_outArea[t]));
			//affine_trans_region(outRegion, &outRegion, m_homMat2D[t], "false");
			//copy_obj(outRegion, m_outArea[t], 1, 1);
			//setDetectOutData(m_outAreaPtr[t], (Hlong)m_outArea[t]);
			//setDetectOutObject(OutArea, outRegion, t);
		}
		//setDetectOutData(TempOK, tempok[t], t);
	}   //end of loop t of template
	bool only1 = true;    //在后续模板没有使用的情况下，使用后续的传出区域
	for (int i = 1;i < MAX_TEMP_NUM;i++) {
		if (m_param->GrpEnable[i]) {
			only1 = false;
			break;
		}
	}
	if (/*res*/m_allok == 0 && only1) {
		for (int t = 1;t < MAX_TEMP_NUM;t++) {
			if (m_param->IsUseOutArea[t]) {
				Hobject outRegion;
				if (m_param->OutImg[t].Id() == H_EMPTY_REGION) {
					gen_empty_region(&outRegion);
					copy_obj(outRegion, &m_outArea[t], 1, 1);
					HalOpenCV::hal2cvRegion(m_outArea[t], &(*p_outArea[t]));
					//copy_obj(outRegion, m_outArea[t], 1, 1);
					//setDetectOutData(m_outAreaPtr[t], (Hlong)m_outArea[t]);
					continue;
				}
				threshold(m_param->OutImg[t], &outRegion, m_param->outThreMin[t]
					, m_param->outThreMax[t] < m_param->outThreMin[t] ? m_param->outThreMin[t] : m_param->outThreMax[t]);
				if (m_param->openVal[t] > 0) {
					opening_circle(outRegion, &outRegion, m_param->openVal[t]);
				}
				if (m_param->closeVal[t] > 0) {
					closing_circle(outRegion, &outRegion, m_param->closeVal[t]);
				}
				connection(outRegion, &outRegion);
				select_shape(outRegion, &outRegion, "area", "and", m_param->outAreaMin[t], m_param->outAreaMax[t] < m_param->outAreaMin[t] ? m_param->outAreaMin[t] : m_param->outAreaMax[t]);
				if (m_param->IsFillUp[t]) {
					fill_up(outRegion, &outRegion);
				}
				union1(outRegion, &outRegion);
				affine_trans_region(outRegion, &outRegion, m_homMat2D[0], "false");
				copy_obj(outRegion, &(m_outArea[t]), 1, 1);
				//affine_trans_region(outRegion, &outRegion, m_homMat2D[0], "false");
				//copy_obj(outRegion, m_outArea[t], 1, 1);
				//setDetectOutData(m_outAreaPtr[t], (Hlong)m_outArea[t]);
			}
		}
	}
	//setDetectOutData(AllOK, res);
	return m_allok;
}

void nccMatch_outNMod::viewResult(ImageView *iv, const QString &funName, int)
{
	if (m_detRes[0] == emptyImg)
		return;
	for (int t = 0;t < MAX_TEMP_NUM;t++) {
		if (m_detRes[t] == noDet)
			continue;
		else if (m_detRes[t] == patrnLoss)
			return;
		else if (m_detRes[t] == emptyOutImg)
			return;
		else if (m_detRes[t] == detFail)
		{
			iv->setFilled(false);
			iv->setColor(Qt::red);
			iv->dispRect1(m_param->roiy[t], m_param->roix[t], 
				m_param->roiy[t] + m_param->roih[t], m_param->roix[t] + m_param->roiw[t], QString("%1").arg(t + 1));
			return;
		}
		else if (m_detRes[t] == detSucceed)
		{
			iv->setFilled(false);
			if (m_patternOK[t])
				iv->setColor(0, 255, 255);
			else 
				iv->setColor(Qt::red);

			if (m_param->IsShowRoiAfterDet[t]) {
				Hobject patternRoi;
				HTuple homMat2d;
				gen_rectangle2(&patternRoi,
					m_param->patternY[t] + m_param->patternH[t] / 2,
					m_param->patternX[t] + m_param->patternW[t] / 2, 0,
					m_param->patternW[t] / 2, m_param->patternH[t] / 2);
				affine_trans_region(patternRoi, &patternRoi, m_homMat2D[t], "false");
				iv->setFilled(false);
				RlRegion rlReg;
				HalOpenCV::hal2cvRegion(patternRoi, &rlReg);
				iv->dispRegion(&rlReg, QString("%1").arg(t + 1));
			}
			if (m_param->IsShowMinusAfterDet[t]) {
				if (m_param->minusArea[t] == 1) {                  //暂时不显示排除区域?
					Hobject minusrec;
					gen_rectangle2(&minusrec, m_param->minusRow[t], m_param->minusCol[t],
						m_param->minusA[t], m_param->minusLength1[t], m_param->minusLength2[t]);
					affine_trans_region(minusrec, &minusrec, m_homMat2D[t], "false");
					iv->setFilled(false);
					RlRegion rlReg;
					HalOpenCV::hal2cvRegion(minusrec, &rlReg);
					iv->dispRegion(&rlReg, QString("%1").arg(t + 1));

				}
				else if (m_param->minusArea[t] == 2) {
					double cy, cx;
					affine_trans_pixel(m_homMat2D[t], m_param->minusCY[t], m_param->minusCX[t], &cy, &cx);
					iv->dispCircle(cy, cx, m_param->minusR[t], QString("%1").arg(t + 1));
				}
			}
		}
		
		if (m_param->IsUseOutArea[t] && m_param->IsShowOutArea[t] && m_patternOK[0]) {
			Hobject outarea;
			iv->setFilled(false);
			iv->setColor(0, 255, 255);
			iv->dispRegion(&(*p_outArea[t]), QString("%1").arg(t + 1));
		}
	}
// 	HTuple imagefull;
// 	getDispOutData(ImageFull, imagefull);
// 	if (!imagefull[0].I()) {
// 		return;
// 	}
// 	for (int t = 0;t < MAX_TEMP_NUM;t++) {
// 		if (!m_param->GrpEnable[t])
// 			continue;
// 		if (!has_set_pattern[t])
// 			return;
// 		if (m_param->OutImg[t].Id() == H_EMPTY_REGION&&m_param->IsUseOutArea[t])
// 			return;
// 		HTuple targetCount;
// 		getDispOutData(COUNT, t, targetCount);
// 		char buf[32];
// 		sprintf(buf, "%d", t + 1);
// 		if (targetCount[0].I() == 0) {
// 			m_view->setFilled(false);
// 			m_view->setColor(255, 0, 0);
// 			m_view->dispRect1(m_param->roiy[t], m_param->roix[t], m_param->roiy[t] + m_param->roih[t], m_param->roix[t] + m_param->roiw[t], buf);
// 			return;
// 		}
// 		m_view->setFilled(false);
// 		m_view->setColor(0, 0, 255);
// 		HTuple  resultRow, resultCol, resultAngle, followRecRow, followRecCol, followCirRow, followCirCol;
// 		getDispOutData(ResultRow, t, resultRow);
// 		getDispOutData(ResultCol, t, resultCol);
// 		getDispOutData(ResultAngle, t, resultAngle);
// 		double anggle = resultAngle[0].D()*180.0 / PI;
// 		HTuple tempok;
// 		getDispOutData(TempOK, t, tempok);
// 		if (tempok[0].I()) {
// 			m_view->setColor(0, 255, 255);
// 		}
// 		else {
// 			m_view->setColor(255, 0, 0);
// 		}
// 		HTuple ImgWidth, ImgHeight;
// 		get_image_size(m_param->patImage[t], &ImgWidth, &ImgHeight);
// 		if (targetCount[0].I() > 0) {
// 			for (int i = 0;i < targetCount[0].I();i++) {
// 				if (i == 0) {
// 					Hobject patternRoi;
// 					HTuple homMat2d;
// 					getDispOutData(m_outHomDefine[t], homMat2d);
// 					gen_rectangle2(&patternRoi, m_param->patternY[t] + m_param->patternH[t] / 2, m_param->patternX[t] + m_param->patternW[t] / 2, 0, m_param->patternW[t] / 2, m_param->patternH[t] / 2);
// 					affine_trans_region(patternRoi, &patternRoi, homMat2d, "false");
// 					m_view->setFilled(false);
// 					if (m_param->IsShowRoiAfterDet[t]) {
// 						m_view->dispRegion(patternRoi, buf);
// 					}
// 					if (m_param->IsShowMinusAfterDet[t]) {
// 						if (m_param->minusArea[t] == 1) {                  //暂时不显示排除区域?
// 							Hobject minusrec;
// 							gen_rectangle2(&minusrec, m_param->minusRow[t], m_param->minusCol[t], m_param->minusA[t], m_param->minusLength1[t], m_param->minusLength2[t]);
// 							affine_trans_region(minusrec, &minusrec, homMat2d, "false");
// 							m_view->setFilled(false);
// 							m_view->dispRegion(minusrec, buf);
// 
// 						}
// 						else if (m_param->minusArea[t] == 2) {
// 							double cy, cx;
// 							affine_trans_pixel(homMat2d, m_param->minusCY[t], m_param->minusCX[t], &cy, &cx);
// 							m_view->dispCircle(cy, cx, m_param->minusR[t], buf);
// 						}
// 					}
// 				}
// 			}
// 		}
// 		if (m_param->IsUseOutArea[t] && m_param->IsShowOutArea[t] && tempok[0].I()) {
// 			Hobject outarea;
// 			m_view->setFilled(false);
// 			m_view->setColor(0, 255, 255);
// 			m_view->dispRegion(*getDispObject(OutArea, t), buf);
// 		}
// 	}
}

void nccMatch_outNMod::textResult(ResultText *text, const QString &funName)
{
	if (m_detRes[0] == emptyImg)
	{
		text->setTextColor(Qt::red);
		text->append(QObject::tr("Empty image!"));
		return;
	}
	
	for (int t = 0;t < MAX_TEMP_NUM;t++) {
		if (m_detRes[t] == noDet)
			continue;
		else if (m_detRes[t] == patrnLoss)
		{
			text->setTextColor(Qt::red);
			text->append(QObject::tr("template%1 unset!").arg(t + 1));
			return;
		}
		else if (m_detRes[t] == emptyOutImg)
		{
			text->setTextColor(Qt::red);
			text->append(QObject::tr("The template%1 of outArea is unset!").arg(t + 1));
			return;
		}
		text->setTextColor(Qt::black);
		text->append(QObject::tr("template%1:").arg(t + 1));
		if (m_detRes[t] == detFail)
		{
			text->setTextColor(Qt::red);
			text->append(QObject::tr("detected failed"));
			return;
		}
		else if (m_detRes[t] == detSucceed)
		{
			int n = m_acceptVal[t].size();
			for (int i = 0;i < n;i++) {
				text->setTextColor(Qt::blue);
				text->append(QObject::tr("detection result:%1").arg(i + 1));
				QString str1, str2;
				if (m_param->acceptEnable[t])
				{
					text->append(QObject::tr("accepted value: "));
					if (m_acceptOK[t][i] == 0) {
						text->setTextColor(Qt::red);
						str1 = QString("<");
						str2 = QString("Fail");
					}
					else {
						str1 = QString(" >=");
						str2 = QString("Pass");
					}
					text->insertPlainText(QString::number(100 * m_acceptVal[t][i], 'i', 0));
					text->setTextColor(Qt::blue);
					text->insertPlainText(QString(" %1  (%2) %3").arg(str1).arg(m_param->acceptValue[t]).arg(str2));
				}
				if (m_param->offsetEnable[t])
				{
					text->append(QObject::tr("offset: "));
					if (m_offsetOK[t][i] == 0) {
						text->setTextColor(Qt::red);
						str1 = QString(">");
						str2 = QString("Fail");
					}
					else {
						str1 = QString(" <=");
						str2 = QString("Pass");
					}
					text->insertPlainText(QString::number(m_offsetVal[t][i]));
					text->setTextColor(Qt::blue);
					text->insertPlainText(QString(" %1  (%2) %3").arg(str1).arg(m_param->offsetValue[t]).arg(str2));
				}
				if (m_param->relativeEnable[t])
				{
					text->append(QObject::tr("relative angle:"));
					if (m_relativeOK[t][i] == 1) {
						str1 = QString(" in");
						str2 = QString("Pass");
					}
					else {
						text->setTextColor(Qt::red);
						str1 = QString(" out");
						str2 = QString("Fail");
					}
					text->insertPlainText(QString::number(m_relativeVal[t][i], 'g', 4));
					text->setTextColor(Qt::blue);
					text->insertPlainText(QString(" %1  (%2,%3) %4").arg(str1)
						.arg(m_param->minRelative[t]).arg(m_param->maxRelative[t]).arg(str2));
				}
				if (m_param->IsAxisAngle[t])
				{
					text->append(QObject::tr("Orthogonal angle:"));
					if (m_axisOK[t][i] == 1) {
						str1 = QString(" in");
						str2 = QString("Pass");
					}
					else {
						text->setTextColor(Qt::red);
						str1 = QString(" out");
						str2 = QString("Fail");
					}
					text->insertPlainText(QString::number(m_axisVal[t][i], 'g', 4));
					text->setTextColor(Qt::blue);
					text->insertPlainText(QString(" %1  (%2) %3").arg(str1).arg(m_param->maxAxisAngle[t]).arg(str2));
				}
			}

			if (m_patternOK[t] == 0) {
				text->setTextColor(Qt::red);
				text->append(QObject::tr("template%1 detected failed").arg(t + 1));
			}
			else {
				text->setTextColor(Qt::green);
				text->append(QObject::tr("template%1 detected successfully").arg(t + 1));
			}
		}
// 		if (!m_param->GrpEnable[t])
// 			continue;
// 		if (!has_set_pattern[t])
// 			return;
// 		if (m_param->OutImg[t].Id() == H_EMPTY_REGION&&m_param->IsUseOutArea[t])
// 			return;
//		m_text->setTextColor(Qt::black);
//		m_text->append(tr("template%1:").arg(t + 1));
// 		HTuple targetCount;
// 		getDispOutData(COUNT, t, targetCount);
// 		if (targetCount[0].I() == 0) {
// 			m_text->setTextColor(Qt::red);
// 			m_text->append(tr("detected failed"));
// 			return;
// 		}
// 		for (int i = 0;i < targetCount[0].I();i++) {
// 			m_text->setTextColor(Qt::blue);
// 			m_text->append(tr("detection result:%1").arg(QString::number(i + 1)));
// 			QString str1, str2;
// 			if (m_param->acceptEnable[t])
// 			{
// 				HTuple acceptOK, accepVal;
// 				getDispOutData(AcceptValOk, t, i, acceptOK);
// 				getDispOutData(AcceptVal, t, i, accepVal);
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
// 				m_text->insertPlainText(QString(" %1  (%2) %3").arg(str1).arg(QString::number(m_param->acceptValue[t])).arg(str2));
// 			}
// 			if (m_param->offsetEnable[t])
// 			{
// 				HTuple offsetOK, offsetVal;
// 				getDispOutData(OffsetValOk, t, i, offsetOK);
// 				getDispOutData(OffsetVal, t, i, offsetVal);
// 				m_text->append(tr("offset: "));
// 				if (offsetOK[0].I() == 0) {
// 					m_text->setTextColor(Qt::red);
// 					str1 = QString(">");
// 					str2 = QString("Fail");
// 				}
// 				else {
// 					str1 = QString(" <=");
// 					str2 = QString("Pass");
// 				}
// 				m_text->insertPlainText(QString::number(offsetVal[0].D()));
// 				m_text->setTextColor(Qt::blue);
// 				m_text->insertPlainText(QString(" %1  (%2) %3").arg(str1).arg(QString::number(m_param->offsetValue[t])).arg(str2));
// 			}
// 			if (m_param->relativeEnable[t])
// 			{
// 				HTuple relativeOK, relativeVal;
// 				getDispOutData(Relative, t, i, relativeVal);
// 				getDispOutData(RelativeOk, t, i, relativeOK);
// 				m_text->append(tr("relative angle:"));
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
// 					.arg(QString::number(m_param->minRelative[t])).arg(QString::number(m_param->maxRelative[t])).arg(str2));
// 			}
// 			if (m_param->IsAxisAngle[t])
// 			{
// 				HTuple axisOK, axisVal;
// 				getDispOutData(AxisAngle, t, i, axisVal);
// 				getDispOutData(AxisAngleOk, t, i, axisOK);
// 				m_text->append(tr("Orthogonal angle:"));
// 				if (axisOK[0].I() == 1) {
// 					str1 = QString(" in");
// 					str2 = QString("Pass");
// 				}
// 				else {
// 					m_text->setTextColor(Qt::red);
// 					str1 = QString(" out");
// 					str2 = QString("Fail");
// 				}
// 				m_text->insertPlainText(QString::number(axisVal[0].D(), 'g', 4));
// 				m_text->setTextColor(Qt::blue);
// 				m_text->insertPlainText(QString(" %1  (%2) %3").arg(str1)
// 					.arg(QString::number(m_param->maxAxisAngle[t])).arg(str2));
// 			}
// 		}
// 		HTuple tempok;
// 		getDispOutData(TempOK, t, tempok);
// 		if (tempok[0].I() == 0) {
// 			m_text->setTextColor(Qt::red);
// 			m_text->append(tr("template%1 detected failed").arg(t + 1));
// 		}
// 		else {
// 			m_text->setTextColor(Qt::green);
// 			m_text->append(tr("template%1 detected successfully").arg(t + 1));
// 		}
	}
}

void nccMatch_outNMod::iniData()
{
	m_param = new nccMatch_outNParam;
	createPins();
	m_image = new Hobject;
	m_allok = -1;
	for (int i = 0;i < MAX_TEMP_NUM;i++) {
		hom_mat2d_identity(&m_homMat2D[i]);
		gen_empty_region(&m_outArea[i]);

		m_detect[i] = new NCCMatcher;
		m_detect[i]->setMaxOverlap(m_param->maxOverlap[i]);
		m_detect[i]->setNum(m_param->findNum[i]);
		m_detect[i]->setSimilar((float)m_param->similarity[i] / 100.0);
		m_detect[i]->setRotateDeg(m_param->seekingAngle[i]);

		has_set_pattern[i] = 0;

		m_detRes[i] = noDet;
		m_patternOK[i] = true;

		m_acceptOK[i].clear();
		m_acceptVal[i].clear();

		m_offsetOK[i].clear();
		m_offsetVal[i].clear();

		m_relativeOK[i].clear();
		m_relativeVal[i].clear();

		m_axisOK[i].clear();
		m_axisVal[i].clear();
	}
}

void nccMatch_outNMod::createPins()
{
	addPin(&p_im, "im");

	for (auto i = 0;i < MAX_TEMP_NUM;++i)
	{
		addPin(&p_homMat2D[i], QString("hm2d%1").arg(i));
		addPin(&p_outArea[i], QString("area%1").arg(i));

		*p_homMat2D[i] = qzcv::hom_mat2d_identity();

		p_homMat2D[i].setVisible(false);
		p_outArea[i].setVisible(false);
	}
}

void nccMatch_outNMod::resetPattern(int index)
{
	if (m_param->minusArea[index] == 0)
		m_detect[index]->setPattern(m_param->patImage[index],
			m_param->patternX[index], m_param->patternY[index]);
	else {
		NCCMatcher::Region region;
		region.isCircle = (m_param->minusArea[index] == 2);
		region.angle = m_param->minusA[index];
		region.w = m_param->minusLength1[index] * 2;
		region.h = m_param->minusLength2[index] * 2;
		region.R = m_param->minusR[index];
		if (region.isCircle) {
			region.cx = m_param->minusCX[index] - m_param->patternX[index];
			region.cy = m_param->minusCY[index] - m_param->patternY[m_param->currentGrp];
		}
		else {
			region.cx = m_param->minusCol[m_param->currentGrp] - m_param->patternX[index];
			region.cy = m_param->minusRow[index] - m_param->patternY[index];
		}
		m_detect[index]->setPattern(m_param->patImage[index],
			m_param->patternX[index], m_param->patternY[index], &region);
	}
}

double nccMatch_outNMod::distance_of_two_point(QPointF point1, QPointF point2)
{
	return sqrt(pow(point1.x() - point2.x(), 2) + pow(point1.y() - point2.y(), 2));
}
