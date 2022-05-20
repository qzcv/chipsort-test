#include "edgeWarpMod.h"
#include "edgeWarpParam.h"
#include <QObject>
#include <ImageView.h>
#include <resultText.h>
#include <opencv2/imgproc.hpp>
#include "Measure1D.h"
#include "halOpenCV.h"

edgeWarpMod::edgeWarpMod()
{
	iniData();
}

edgeWarpMod::edgeWarpMod(const QString& dirPath)
{
	iniData();
	load(dirPath, StructLevel);
}

edgeWarpMod::~edgeWarpMod()
{
	for (auto i = 0;i < MAX_EDGE_LINE;++i)
		if (m_detect[i])
			delete m_detect[i];
	delete m_param;
}

int edgeWarpMod::command(int cmdID, void* dataIn, void* dataOut)
{
	return 0;
}

int edgeWarpMod::afterSetProperty(MetaProperty*)
{
	return 0;
}

void edgeWarpMod::save(const QString &dirPath)
{
	m_param->readWriteParam(false, dirPath, StructLevel);
}

void edgeWarpMod::load(const QString &dirPath, QvsParamLevel level)
{
	m_param->readWriteParam(true,dirPath,level);
}

int edgeWarpMod::run(const QString &funName)
{
// 	for (int i = 0;i < MAX_TESTITEMS_NUM;i++) {
// 		m_testItemsStatus[i] = 1;
// 	}
	if (p_im->empty())
		return -1;
	if (m_param->IsRefer&&p_homMat2d->size() != cv::Size(2, 3))
		return -1;

	cv::Mat im;
	if (p_im->channels() == 3)
		cv::cvtColor(**p_im, im, CV_BGR2GRAY);
	else
		im = *p_im;

	m_allok = 0;
	
// 	if (!m_image || m_image->Id() == H_EMPTY_REGION) {
// 		allok = false;
// 		setDetectOutData(AllOK, allok);
// 		setDetectOutData(ImageFull, false);
// 		m_testItemsStatus[0] = 0;
// 		return allok;
// 	}
// 	else {
// 		setDetectOutData(ImageFull, true);
// 	}
	for (int i = 0; i < MAX_EDGE_LINE;i++) {
		m_roiFound[i] = true;
		m_begRow[i] = m_begCol[i] = 0;
		m_endRow[i] = m_endCol[i] = 0;
		if (i < MAX_EDGE_LINE - 1)
		{
			m_leadRow[i] = m_leadCol[i] = 0;
		}
	}
	
	HTuple hommat2d;
	double rotateAngle = 0;
	if (m_param->IsRefer) {
		try {
			HalOpenCV::cv2HalHomMat2D(*p_homMat2d, &hommat2d);
			double tv = hommat2d[0].D();
		}
		catch (HException& ex) {
			hom_mat2d_identity(&hommat2d);
		}
		rotateAngle = atan((hommat2d[3].D() / hommat2d[0].D()));
	}
	double regAngle[MAX_EDGE_LINE], regRow[MAX_EDGE_LINE], regCol[MAX_EDGE_LINE];     //最终设入detector的坐标
	for (int i = 0;i < MAX_EDGE_LINE;i++) {
		regAngle[i] = m_param->roiReg[i].angle + rotateAngle;
		regRow[i] = m_param->roiReg[i].row;
		regCol[i] = m_param->roiReg[i].col;
		if (m_param->IsRefer) {
			affine_trans_pixel(hommat2d, regRow[i], regCol[i], &regRow[i], &regCol[i]);
		}
	}

	double lineBegRow[MAX_EDGE_LINE], lineBegCol[MAX_EDGE_LINE];//得到的边界线条的坐标
	double lineEndRow[MAX_EDGE_LINE], lineEndCol[MAX_EDGE_LINE];//得到的边界线条的坐标
	double pixsize;
	HTuple pixval;
	if (m_param->isInputPix) {
		try {
			//传入的顺序是中左右上下,中间像素大小视为正方形，左右适用于求站高宽度
			for (auto i = 0;i < p_pixelSize->size();++i)
				pixval[i] = p_pixelSize->at(i);
			int index;
			if (m_param->isInputPix == 1) {
				switch (m_param->leadDir) {
				case 0:
					index = 4;
					break;
				case 1:
					index = 3;
					break;
				case 2:
					index = 2;
					break;
				case 3:
					index = 1;
					break;
				default:
					index = 1;
					break;
				}
			}
			else {
				index = 0;
			}
			pixsize = pixval[index].D();
		}
		catch (HException& ex) {
			pixsize = m_param->pixSize;
		}
	}
	else {
		pixsize = m_param->pixSize;
	}

	const cv::Point2f* edgePoints;
	edgePoints = NULL;
	int edgePointsNum[2];
	edgePointsNum[0] = edgePointsNum[1] = 0;

	double warpVal[2];  //保存得出的两个翘曲值
	double leadRow[2], leadCol[2];  //找到的管脚坐标
	for (int i = 0;i < MAX_EDGE_LINE;i++)
	{
		if (m_detect[i])
			delete m_detect[i];
		float vx[2] = { 0, 0 };
		float vy[2] = { m_param->roiReg[i].length2, -m_param->roiReg[i].length2 };
		cv::Point2f p[2];
		float cosa = cos(regAngle[i]);
		float sina = sin(regAngle[i]);
		for (int j = 0;j < 2;j++) {
			p[j].y = regRow[i] + cosa*vy[j] - sina*vx[j];
			p[j].x = regCol[i] + sina*vy[j] + cosa*vx[j];
		}
		m_detect[i] = new LineDetect(p[0], p[1], 2 * m_param->roiReg[i].length1);

		m_detect[i]->setFirst(m_param->first[i]);
		m_detect[i]->setHoughFilter(true, m_param->degRange[i]);
		m_detect[i]->setRemoveDistance(m_param->rmDist[i]);
		m_detect[i]->setMaxIterator(m_param->iterateNum[i]);
		m_detect[i]->setGradStep(m_param->stepVal[i]);
		cv::Point2f line[2];
		bool ok = m_detect[i]->calucalte(im, (LineDetect::Polarity)m_param->pol[i], m_param->gradient[i], line);
		if (ok) {
			if (i > 0 && m_roiFound[0]) {
				edgePoints = m_detect[i]->getEdgePoints(edgePointsNum[i - 1]);
				if (edgePointsNum[i - 1] <= 2) {
					m_roiFound[i] = false;
					m_allok = -1;
					//m_testItemsStatus[0] = 0;
					//setDetectOutData(FindOk, roiFound[i], i);
					continue;
				}
				else {
					double biggestWarp = -10000;
					int warpIndex = -1;
					for (int j = 0;j < edgePointsNum[i - 1];j++) {
						double tmpwarp;
						distance_pl(edgePoints[j].y, edgePoints[j].x, lineBegRow[0], lineBegCol[0], lineEndRow[0], lineEndCol[0], &tmpwarp);
						double interrow, intercol;
						Hlong parrel;
						double warpdir = 1.0; //翘曲的极性
						if (m_param->leadDir == 0 || m_param->leadDir == 1) {   //判断翘曲值应该是正值还是负值
							intersection_ll(lineBegRow[0], lineBegCol[0], lineEndRow[0], lineEndCol[0],
								edgePoints[j].y, edgePoints[j].x, (edgePoints[j].y - 5) > 0 ? (edgePoints[j].y - 5) : 0, edgePoints[j].x, &interrow, &intercol, &parrel);
							if (parrel == 0) {
								if (interrow > edgePoints[j].y&&m_param->leadDir == 0)
									warpdir = -1.0;
								else if (interrow < edgePoints[j].y&&m_param->leadDir == 1)
									warpdir = -1.0;
							}
							else {
								m_roiFound[i] = false;
								m_allok = -1;
								//m_testItemsStatus[0] = 0;
								//setDetectOutData(FindOk, roiFound[i], i);
								continue;
							}
						}
						else if (m_param->leadDir == 2 || m_param->leadDir == 3) {
							intersection_ll(lineBegRow[0], lineBegCol[0], lineEndRow[0], lineEndCol[0],
								edgePoints[j].y, edgePoints[j].x, edgePoints[j].y, (edgePoints[j].x - 5) > 0 ? (edgePoints[j].x - 5) : 0, &interrow, &intercol, &parrel);
							if (parrel == 0) {
								if (intercol > edgePoints[j].x&&m_param->leadDir == 2)
									warpdir = -1.0;
								else if (intercol < edgePoints[j].y&&m_param->leadDir == 3)
									warpdir = -1.0;
							}
							else {
								m_roiFound[i] = false;
								m_allok = -1;
								//m_testItemsStatus[0] = 0;
								//setDetectOutData(FindOk, roiFound[i], i);
								continue;
							}
						}

						if (1)   //暂时只考虑目标朝向为上到下的情况
						{
							tmpwarp = tmpwarp*warpdir*pixsize + m_param->warpOffset;
							if (tmpwarp > biggestWarp) {
								biggestWarp = tmpwarp;
								warpIndex = j;
							}

						}

					}  //end of loop j
					if (warpIndex >= 0) {
						leadRow[i - 1] = edgePoints[warpIndex].y;
						leadCol[i - 1] = edgePoints[warpIndex].x;
						warpVal[i - 1] = biggestWarp;
						m_leadRow[i - 1] = leadRow[i - 1];
						m_leadCol[i - 1] = leadCol[i - 1];
						//setDetectOutData(FindOk, roiFound[i], i);
						//setDetectOutData(LeadRow, leadRow[i - 1], i - 1);
						//setDetectOutData(LeadCol, leadCol[i - 1], i - 1);
					}
					else {
						m_roiFound[i] = false;
						m_allok = -1;
						//m_testItemsStatus[0] = 0;
						//setDetectOutData(FindOk, roiFound[i], i);
						continue;
					}
				}
			} //end of if (i>0 && roiFound[0])
			else {
				//setDetectOutData(FindOk, roiFound[i], i);
				lineBegRow[i] = line[0].y;
				lineBegCol[i] = line[0].x;
				lineEndRow[i] = line[1].y;
				lineEndCol[i] = line[1].x;
				double lineAngle = -atan((line[0].y - line[1].y) / (line[0].x - line[1].x));
				if (i == 0) {
					double rotateangle = lineAngle + (m_param->lineOffset[i] > 0 ? 1 : -1)*PI / 2.0;
					lineBegRow[i] = lineBegRow[i] - sin(rotateangle)*abs(m_param->lineOffset[i]);
					lineEndRow[i] = lineEndRow[i] - sin(rotateangle)*abs(m_param->lineOffset[i]);
					lineBegCol[i] = lineBegCol[i] + cos(rotateangle)*abs(m_param->lineOffset[i]);
					lineEndCol[i] = lineEndCol[i] + cos(rotateangle)*abs(m_param->lineOffset[i]);
					m_begRow[i] = lineBegRow[i];
					m_begCol[i] = lineBegCol[i];
					m_endRow[i] = lineEndRow[i];
					m_endCol[i] = lineEndCol[i];
					//setDetectOutData(BeginRow, lineBegRow[i], i);
					//setDetectOutData(BeginCol, lineBegCol[i], i);
					//setDetectOutData(EndRow, lineEndRow[i], i);
					//setDetectOutData(EndCol, lineEndCol[i], i);
				}
			}
		}
		else {
			m_roiFound[i] = false;
			m_allok = -1;
			//m_testItemsStatus[0] = 0;
			//setDetectOutData(FindOk, roiFound[i], i);
		}
	} //for loop end


	for (int i = 0;i < MAX_EDGE_LINE;i++) {
		if (!m_roiFound[i]) {
			m_allok = -1;
			//setDetectOutData(AllOK, allok);
			//cvReleaseImage(&pImage);
			//m_testItemsStatus[0] = 0;
			return m_allok;
		}
	}

	m_warpOk[0] = m_warpOk[1] = true;
	m_warpDiffOk = true;
	if (m_param->IsErrWarp || m_param->IsErrWarpDiff) {
		if (m_roiFound[1] && m_roiFound[2]) {
			for (int i = 0;i < 2;i++) {
				if (m_param->IsErrWarp) {
					if (warpVal[i] < m_param->errWarpMin || warpVal[i] > m_param->errWarpMax) {
						m_allok = -2;
						//m_testItemsStatus[1] = 0;
						m_warpOk[i] = false;
					}
					m_warpVal[i] = warpVal[i];
				}
			}  //end of loop i<2
			double warpdiff;
			if (m_param->IsErrWarpDiff) {
				warpdiff = abs(warpVal[0] - warpVal[1]);
				if (warpdiff > m_param->errWarpDiffMax || warpdiff < m_param->errWarpDiffMin) {
					m_allok = -3;
					//m_testItemsStatus[2] = 0;
					m_warpDiffOk = false;
				}
				m_warpDiff = warpdiff;
			}
		}
	}
	
	return m_allok;
}

void edgeWarpMod::viewResult(ImageView *iv, const QString &funName, int)
{
	if (p_im->empty())
		return;
	if (m_param->IsRefer&&p_homMat2d->size() != cv::Size(2, 3))
		return;

	for (int i = 0;i < MAX_EDGE_LINE;i++) {
		if (i == 0 && m_roiFound[0] == 0) {
			iv->setColor(Qt::red);
			iv->setFilled(false);
			for (int j = 0;j < MAX_EDGE_LINE;j++) {
				Hobject roirec;
				gen_rectangle2(&roirec, m_param->roiReg[j].row, m_param->roiReg[j].col,
					m_param->roiReg[j].angle, m_param->roiReg[j].length1, m_param->roiReg[j].length2);
				if (m_param->IsRefer) {
					HTuple hm2d;
					HalOpenCV::cv2HalHomMat2D(*p_homMat2d, &hm2d);
					affine_trans_region(roirec, &roirec, hm2d, "false");
				}
				iv->setFilled(false);
				RlRegion reg;
				HalOpenCV::hal2cvRegion(roirec, &reg);
				iv->dispRegion(&reg);
			}
			return;
		}
	}

	if (m_param->IsShowRegionAfterDet) {
		for (int i = 0;i < 1/*MAX_EDGE_LINE*/;i++) {
			iv->setColor(Qt::black);
			iv->dispLine(m_begRow[i], m_begCol[i], m_endRow[i], m_endCol[i]);

			iv->dispLine(m_begRow[i], m_begCol[i] - 3, m_begRow[i], m_begCol[i] + 3);
			iv->dispLine(m_begRow[i] - 3, m_begCol[i], m_begRow[i] + 3, m_begCol[i]);
			iv->dispLine(m_endRow[i], m_endCol[i] - 3, m_endRow[i], m_endCol[i] + 3);
			iv->dispLine(m_endRow[i] - 3, m_endCol[i], m_endRow[i] + 3, m_endCol[i]);
		}
	}

	if (m_allok != 0)
		iv->setColor(Qt::red);
	else
		iv->setColor(Qt::black);
	iv->setFilled(false);

	for (int i = 0;i < 2;i++) {
		if (m_roiFound[i + 1]) {
			iv->dispLine(m_leadRow[i], m_leadCol[i] - 5, m_leadRow[i], m_leadCol[i] + 5);
			iv->dispLine(m_leadRow[i] - 5, m_leadCol[i], m_leadRow[i] + 5, m_leadCol[i]);
		}
	}

// 	HTuple imagefull;
// 	getDispOutData(ImageFull, imagefull);
// 	if (!imagefull[0].I()) {
// 		return;
// 	}
// 	HTuple findok;
// 	for (int i = 0;i < MAX_EDGE_LINE;i++) {
// 		getDispOutData(FindOk, i, findok);
// 		if (i == 0 && findok[0].I() == 0) {
// 			m_view->setColor(255, 0, 0);
// 			m_view->setFilled(false);
// 			HTuple homat;
// 			if (m_param->IsRefer) {
// 				getDispOutData(TmpHomMat, homat);
// 			}
// 			for (int j = 0;j < MAX_EDGE_LINE;j++) {
// 				Hobject roirec;
// 				gen_rectangle2(&roirec, m_param->roiReg[j].row, m_param->roiReg[j].col,
// 					m_param->roiReg[j].angle, m_param->roiReg[j].length1, m_param->roiReg[j].length2);
// 				if (m_param->IsRefer) {
// 					affine_trans_region(roirec, &roirec, homat, "false");
// 				}
// 				m_view->setFilled(false);
// 				m_view->dispRegion(roirec);
// 			}
// 			return;
// 		}
// 	}
// 	if (m_param->IsShowRegionAfterDet) {
// 		for (int i = 0;i < 1/*MAX_EDGE_LINE*/;i++) {
// 			HTuple rowbegin, colbegin, rowend, colend;
// 			getDispOutData(BeginRow, i, rowbegin);
// 			getDispOutData(BeginCol, i, colbegin);
// 			getDispOutData(EndRow, i, rowend);
// 			getDispOutData(EndCol, i, colend);
// 			m_view->setColor(0, 0, 255);
// 			m_view->dispLine(rowbegin[0].D(), colbegin[0].D(), rowend[0].D(), colend[0].D());
// 
// 			m_view->dispLine(rowbegin[0].D(), colbegin[0].D() - 3, rowbegin[0].D(), colbegin[0].D() + 3);
// 			m_view->dispLine(rowbegin[0].D() - 3, colbegin[0].D(), rowbegin[0].D() + 3, colbegin[0].D());
// 			m_view->dispLine(rowend[0].D(), colend[0].D() - 3, rowend[0].D(), colend[0].D() + 3);
// 			m_view->dispLine(rowend[0].D() - 3, colend[0].D(), rowend[0].D() + 3, colend[0].D());
// 		}
// 	}

// 	HTuple allok;
// 	getDispOutData(AllOK, allok);
// 	if (allok[0].I() == 0) {
// 		m_view->setColor(255, 0, 0);
// 	}
// 	else if (allok[0].I() == 1) {
// 		m_view->setColor(0, 255, 0);
// 	}
// 	m_view->setFilled(false);
// 
// 	HTuple leadrow[2], leadcol[2];
// 	HTuple leadfound;
// 	for (int i = 0;i < 2;i++) {
// 		getDispOutData(FindOk, i + 1, leadfound);
// 		if (leadfound[0].I()) {
// 			getDispOutData(LeadRow, i, leadrow[i]);
// 			getDispOutData(LeadCol, i, leadcol[i]);
// 			m_view->dispLine(leadrow[i][0].D(), leadcol[i][0].D() - 5, leadrow[i][0].D(), leadcol[i][0].D() + 5);
// 			m_view->dispLine(leadrow[i][0].D() - 5, leadcol[i][0].D(), leadrow[i][0].D() + 5, leadcol[i][0].D());
// 		}
// 	}
}

void edgeWarpMod::textResult(ResultText *text, const QString &funName)
{
	if (p_im->empty())
	{
		text->setTextColor(Qt::red);
		text->append(QObject::tr("Empty image!"));
		return;
	}
	if (m_param->IsRefer&&p_homMat2d->size() != cv::Size(2, 3))
	{
		text->setTextColor(Qt::red);
		text->append(QObject::tr("Input matrix's size != 2*3!"));
		return;
	}

	QStringList name = { QObject::tr("baseLine"), QObject::tr("leadSide 1"), QObject::tr("leadSide 2") };
	for (int i = 0;i < MAX_EDGE_LINE;i++) {
		if (m_roiFound[i] == 0) {
			text->setTextColor(Qt::red);
			text->append(QObject::tr("%1 edge found failed").arg(name[i]));
			return;
		}
	}

	if (m_param->IsErrWarp) {
		for (int i = 0;i < 2;i++) {
			if (m_warpOk[i] == 0) {
				text->setTextColor(Qt::red);
			}
			else {
				text->setTextColor(Qt::black);
			}
			text->append(QObject::tr("lead %4 warp= (%1),(%2,%3)")
				.arg(QString::number(m_warpVal[i], 'g', 6))
				.arg(QString::number(m_param->errWarpMin))
				.arg(QString::number(m_param->errWarpMax)).arg(i + 1));
		}
	}
	if (m_param->IsErrWarpDiff) {
		if (m_warpDiffOk == 0) {
			text->setTextColor(Qt::red);
		}
		else {
			text->setTextColor(Qt::black);
		}
		text->append(QObject::tr("lead coplanar= (%1),(%2,%3)")
			.arg(QString::number(m_warpDiff, 'g', 6))
			.arg(QString::number(m_param->errWarpDiffMin))
			.arg(QString::number(m_param->errWarpDiffMax)));
	}

// 	HTuple imagefull;
// 	getDispOutData(ImageFull, imagefull);
// 	if (!imagefull[0].I()) {
// 		return;
// 	}
// 	HTuple found;
// 	QString name[3];
// 	name[0] = tr("baseLine"); name[1] = tr("leadSide 1");name[2] = tr("leadSide 2");
// 	for (int i = 0;i < MAX_EDGE_LINE;i++) {
// 		getDispOutData(FindOk, i, found);
// 		if (found[0].I() == 0) {
// 			m_text->setTextColor(QColor(255, 0, 0));
// 			m_text->append(tr(" %1 edge found failed").arg(name[i]));
// 			return;
// 		}
// 	}
// 	HTuple warpval[2];
// 	HTuple leadfound[2];
// 	for (int i = 0;i < 2;i++) {
// 		getDispOutData(FindOk, i + 1, leadfound[i]);
// 		if (!leadfound[i][0].I()) {
// 			m_text->setTextColor(QColor(255, 0, 0));
// 			m_text->append(tr(" %1 point found failed").arg(name[i + 1]));
// 		}
// 	}
// 	if (!leadfound[0][0].I() || !leadfound[1][0].I()) {
// 		return;
// 	}
// 	if (m_param->IsErrWarp) {
// 		for (int i = 0;i < 2;i++) {
// 			HTuple errwarpok, warpval;
// 			getDispOutData(ErrWarpOk, i, errwarpok);
// 			getDispOutData(ErrWarpVal, i, warpval);
// 
// 			if (errwarpok[0].I() == 0) {
// 				m_text->setTextColor(QColor(255, 0, 0));
// 			}
// 			else {
// 				m_text->setTextColor(QColor(0, 0, 0));
// 			}
// 			m_text->append(tr("lead %4 warp= (%1),(%2,%3)").arg(QString::number(warpval[0].D(), 'g', 6)).
// 				arg(QString::number(m_param->errWarpMin)).arg(QString::number(m_param->errWarpMax)).arg(i + 1));
// 		}
// 
// 	}
// 	if (m_param->IsErrWarpDiff) {
// 		HTuple errdiffok, warpdiffval;
// 		getDispOutData(ErrWarpDiffOk, errdiffok);
// 		getDispOutData(ErrWarpDiffVal, warpdiffval);
// 
// 		if (errdiffok[0].I() == 0) {
// 			m_text->setTextColor(QColor(255, 0, 0));
// 		}
// 		else {
// 			m_text->setTextColor(QColor(0, 0, 0));
// 		}
// 		m_text->append(tr("lead coplanar= (%1),(%2,%3)").arg(QString::number(warpdiffval[0].D(), 'g', 6)).
// 			arg(QString::number(m_param->errWarpDiffMin)).arg(QString::number(m_param->errWarpDiffMax)));
// 	}
}

void edgeWarpMod::iniData()
{
	m_param = new edgeWarpParam;
	for (auto i = 0;i < MAX_EDGE_LINE;++i)
	{
		m_detect[i] = nullptr;
		m_roiFound[i] = true;
		m_begRow[i] = m_begCol[i] = 0;
		m_endRow[i] = m_endCol[i] = 0;
		if (i < MAX_EDGE_LINE - 1)
		{
			m_leadCol[i] = m_leadRow[i] = 0;
			m_warpVal[i] = 0;
			m_warpOk[i] = true;
		}
	}
	m_warpDiffOk = true;
	m_warpDiff = 0;
	m_allok = 0;
	createPins();
}

void edgeWarpMod::createPins()
{
	addPin(&p_im, "im");
	addPin(&p_homMat2d, "hm2d");
	addPin(&p_pixelSize, "sz");
}

