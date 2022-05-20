#include "edgeInterMod.h"
#include "edgeInterParam.h"
#include <QObject>
#include <ImageView.h>
#include <resultText.h>
#include "Measure1D.h"
#include <opencv2/imgproc.hpp>
#include "halOpenCV.h"

edgeInterMod::edgeInterMod()
{
	iniData();
}

edgeInterMod::edgeInterMod(const QString& dirPath)
{
	iniData();
	load(dirPath, StructLevel);
}

edgeInterMod::~edgeInterMod()
{
	delete m_param;
	for (auto i = 0;i < MAX_EDGE_LINE;++i)
	{
		if (m_detect[i])
			delete m_detect[i];
	}
}

int edgeInterMod::command(int cmdID, void* dataIn, void* dataOut)
{
	return 0;
}

int edgeInterMod::afterSetProperty(MetaProperty*)
{
	return 0;
}

void edgeInterMod::save(const QString &dirPath)
{
	m_param->readWriteParam(false, dirPath, StructLevel);
	resetParamChanged();
}

void edgeInterMod::load(const QString &dirPath, QvsParamLevel level)
{
	m_param->readWriteParam(true,dirPath,level);
}

int edgeInterMod::run(const QString &funName)
{
// 	for (int i = 0;i < MAX_TESTITEMS_NUM;i++) {
// 		m_testItemsStatus[i] = 1;
// 	}

	p_inters->clear();

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
	for (int i = 0;i < MAX_EDGE_LINE;i++)
	{
		m_roiFound[i] = true;

		if (m_detect[i])
			delete m_detect[i];
		float vx[2] = { 0, 0 };
		float vy[2] = { m_param->roiReg[i].length2, -m_param->roiReg[i].length2 };
		CvPoint2D32f p[2];
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
		m_roiFound[i] = ok;
		if (ok)
		{
			lineBegRow[i] = line[0].y;
			lineBegCol[i] = line[0].x;
			lineEndRow[i] = line[1].y;
			lineEndCol[i] = line[1].x;
			double lineAngle = atan((line[0].y - line[1].y) / (line[0].x - line[1].x));
			double rotateangle = lineAngle + (m_param->lineOffset[i] > 0 ? 1 : -1)*PI / 2.0;

			lineBegRow[i] = lineBegRow[i] - sin(rotateangle)*abs(m_param->lineOffset[i]);
			lineEndRow[i] = lineEndRow[i] - sin(rotateangle)*abs(m_param->lineOffset[i]);
			lineBegCol[i] = lineBegCol[i] + cos(rotateangle)*abs(m_param->lineOffset[i]);
			lineEndCol[i] = lineEndCol[i] + cos(rotateangle)*abs(m_param->lineOffset[i]);
		}
		else {
			m_allok = -1;
		}
	} //for loop end
	for (int i = 0;i < MAX_EDGE_LINE;i++) {
		if (!m_roiFound[i]) {
			//allok = false;
			//setDetectOutData(AllOK, allok);
			//m_testItemsStatus[0] = 0;
			//return allok;
			m_allok = -1;
			return -1;
		}
	}
	float EveryAngle[MAX_EDGE_LINE];
	for (int i = 0;i < MAX_EDGE_LINE;i++) {
		if (!m_roiFound[i]) {
			continue;
		}
		EveryAngle[i] = atan((lineEndRow[i] - lineBegRow[i]) / (lineEndCol[i] - lineBegCol[i]));
		double a = 5.0;
		double b = 5.0;
		double asd = atan((3.0 - 8.0) / (a - b));
		asd -= 1;
	}

	for (int i = 0;i < MAX_EDGE_LINE;i++) {
		if (!m_roiFound[i]) {
			continue;
		}
		m_begCol[i] = lineBegCol[i];
		m_begRow[i] = lineBegRow[i];
		m_endCol[i] = lineEndCol[i];
		m_endRow[i] = lineEndRow[i];
		//setDetectOutData(BeginRow, lineBegRow[i], i);
		//setDetectOutData(BeginCol, lineBegCol[i], i);
		//setDetectOutData(EndRow, lineEndRow[i], i);
		//setDetectOutData(EndCol, lineEndCol[i], i);
	}
	HTuple finalEndRow[2], finalEndCol[2];  //求出的交点
	HTuple isparel;
	bool interok[2];
	for (int i = 0;i < 2;i++) {
		intersection_ll(lineBegRow[0], lineBegCol[0], lineEndRow[0], lineEndCol[0],
			lineBegRow[i + 1], lineBegCol[i + 1], lineEndRow[i + 1], lineEndCol[i + 1], &finalEndRow[i], &finalEndCol[i], &isparel);
		if (isparel[0].I()) {
			interok[i] = 0;
			m_allok = -1;
			return - 1;
			//allok = false;
			//setDetectOutData(AllOK, allok);
			//m_testItemsStatus[0] = 0;
			//return allok;
		}
		else {
			interok[i] = 1;
			double x = finalEndCol[i][0].D();
			double y = finalEndRow[i][0].D();
			p_inters->push_back(cv::Point2d(x, y));

			//setDetectOutData(InterRow, finalEndRow[i], i);
			//setDetectOutData(InterCol, finalEndCol[i], i);
		}
	}
	if (m_param->IsErrX || m_param->IsErrY) {
		m_centerY = (finalEndRow[0][0].D() + finalEndRow[1][0].D()) / 2.0;
		m_centerX = (finalEndCol[0][0].D() + finalEndCol[1][0].D()) / 2.0;
// 		m_tmpX = CenterX;
// 		m_tmpY = CenterY;
// 		setDetectOutData(ErrX, CenterX);
// 		setDetectOutData(ErrY, CenterY);
	}
	if (m_param->IsErrInter) {
		m_errInterOK = true;
		double dis;
		distance_pp(finalEndRow[0][0].D(), finalEndCol[0][0].D(), finalEndRow[1][0].D(), finalEndCol[1][0].D(), &dis);
		dis *= m_param->pixSize;
		dis += m_param->interOffset;
		if (dis > m_param->errInterMax || dis < m_param->errInterMin) {
			m_errInterOK = false;
			m_allok = -2;
			//m_testItemsStatus[1] = 0;
		}
		m_errInterDist = dis;
		//setDetectOutData(ErrInterOk, errinterok);
		//setDetectOutData(ErrInterVal, dis);
	}
	if (m_param->IsErrX) {
		m_errXok = true;
		m_errX = m_centerX - m_param->referX;
		if (m_errX > m_param->errXMax || m_errX < m_param->errXMin) {
			m_errXok = false;
			m_allok = -3;
			//m_testItemsStatus[2] = 0;
		}
		//setDetectOutData(ErrXOk, errxok);
	}
	if (m_param->IsErrY) {
		m_errYok = true;
		m_errY = m_centerY - m_param->referY;
		if (m_errY > m_param->errYMax || m_errY < m_param->errYMin) {
			m_errYok = false;
			m_allok = -4;
			//m_testItemsStatus[3] = 0;
		}
		//setDetectOutData(ErrYOk, erryok);
	}
	//setDetectOutData(AllOK, allok);
	return m_allok;
}

void edgeInterMod::viewResult(ImageView *iv, const QString &funName, int)
{
	if (p_im->empty())
		return;
	if (m_param->IsRefer&&p_homMat2d->size() != cv::Size(2, 3))
		return;

	for (int i = 0;i < MAX_EDGE_LINE;i++) {
		if (m_roiFound[i] == 0) {
			iv->setColor(Qt::red);
			iv->setFilled(false);

			for (int j = 0;j < MAX_EDGE_LINE;j++) {
				Hobject roirec;
				gen_rectangle2(&roirec, m_param->roiReg[j].row, m_param->roiReg[j].col,
					m_param->roiReg[j].angle, m_param->roiReg[j].length1, m_param->roiReg[j].length2);
				if (m_param->IsRefer) {
					HTuple hommat2d;
					HalOpenCV::cv2HalHomMat2D(*p_homMat2d, &hommat2d);
					affine_trans_region(roirec, &roirec, hommat2d, "false");
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
		for (int i = 0;i < MAX_EDGE_LINE;i++) {
			iv->setColor(Qt::blue);
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
		iv->setColor(Qt::green);
	iv->setFilled(false);

	for (int i = 0;i < p_inters->size();i++) {
		double r = p_inters->at(i).y;
		double c = p_inters->at(i).x;
		iv->dispLine(r, c - 3, r, c + 3);
		iv->dispLine(r - 3, c, r + 3, c);
	}
	if (p_inters->size() != 2)
		return;
	iv->dispLine(
		p_inters->at(0).y, p_inters->at(0).x, 
		p_inters->at(1).y, p_inters->at(1).x);

	if (m_param->IsErrX || m_param->IsErrY) {
		iv->dispLine(m_centerY - 10, m_centerX, m_centerY + 10, m_centerX);
		iv->dispLine(m_centerY, m_centerX - 10, m_centerY, m_centerX + 10);
	}

// 	HTuple imagefull;
// 	getDispOutData(ImageFull, imagefull);
// 	if (!imagefull[0].I()) {
// 		return;
// 	}
// 	HTuple findok;
// 	for (int i = 0;i < MAX_EDGE_LINE;i++) {
// 		getDispOutData(FindOk, i, findok);
// 		if (findok[0].I() == 0) {
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
// 		for (int i = 0;i < MAX_EDGE_LINE;i++) {
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
// 	HTuple interrow[2], intercol[2];
// 	for (int i = 0;i < 2;i++) {
// 
// 		getDispOutData(InterRow, i, interrow[i]);
// 		getDispOutData(InterCol, i, intercol[i]);
// 		m_view->dispLine(interrow[i][0].D(), intercol[i][0].D() - 3, interrow[i][0].D(), intercol[i][0].D() + 3);
// 		m_view->dispLine(interrow[i][0].D() - 3, intercol[i][0].D(), interrow[i][0].D() + 3, intercol[i][0].D());
// 	}
// 	m_view->dispLine(interrow[0][0].D(), intercol[0][0].D(), interrow[1][0].D(), intercol[1][0].D());
// 
// 	if (m_param->IsErrX || m_param->IsErrY) {
// 		HTuple centerx, centery;
// 		getDispOutData(ErrX, centerx);
// 		getDispOutData(ErrY, centery);
// 		double centerxx = centerx[0].D();
// 		m_view->dispLine(centery[0].D() - 10, centerx[0].D(), centery[0].D() + 10, centerx[0].D());
// 		m_view->dispLine(centery[0].D(), centerx[0].D() - 10, centery[0].D(), centerx[0].D() + 10);
// 	}
}

void edgeInterMod::textResult(ResultText *text, const QString &funName)
{
	if (p_im->empty())
	{
		text->setTextColor(Qt::red);
		text->append(QObject::tr("Empty image"));
		return;
	}
	if (m_param->IsRefer&&p_homMat2d->size() != cv::Size(2, 3))
	{
		text->setTextColor(Qt::red);
		text->append(QObject::tr("Input matrix's size != 2*3!"));
		return;
	}
	for (int i = 0;i < MAX_EDGE_LINE;i++) {
		if (m_roiFound[i] == 0) {
			text->setTextColor(Qt::red);
			text->append(QObject::tr(" edge%1 found failed").arg(i + 1));
			return;
		}
	}

	if (m_param->IsErrInter) {
		if (m_errInterOK == 0) {
			text->setTextColor(Qt::red);
		}
		else {
			text->setTextColor(Qt::black);
		}
		text->append(QObject::tr("Intersection distance= (%1),(%2,%3)")
			.arg(QString::number(m_errInterDist, 'g', 6))
			.arg(QString::number(m_param->errInterMin))
			.arg(QString::number(m_param->errInterMax)));
	}

	if (m_param->IsErrX) {
		if (m_errXok == 0) {
			text->setTextColor(Qt::red);
		}
		else {
			text->setTextColor(Qt::black);
		}
		text->append(QObject::tr("X= (%1),(%2,%3)")
			.arg(QString::number(m_errX, 'g', 6))
			.arg(QString::number(m_param->errXMin))
			.arg(QString::number(m_param->errXMax)));
	}
	if (m_param->IsErrY) {
		if (m_errYok == 0) {
			text->setTextColor(Qt::red);
		}
		else {
			text->setTextColor(Qt::black);
		}
		text->append(QObject::tr("Y= (%1),(%2,%3)")
			.arg(QString::number(m_errY, 'g', 6))
			.arg(QString::number(m_param->errYMin))
			.arg(QString::number(m_param->errYMax)));
	}
// 	HTuple imagefull;
// 	getDispOutData(ImageFull, imagefull);
// 	if (!imagefull[0].I()) {
// 		return;
// 	}
// 	HTuple found;
// 	for (int i = 0;i < MAX_EDGE_LINE;i++) {
// 		getDispOutData(FindOk, i, found);
// 		if (found[0].I() == 0) {
// 			m_text->setTextColor(QColor(255, 0, 0));
// 			m_text->append(tr(" edge%1 found failed").arg(i + 1));
// 			return;
// 		}
// 	}
// 	if (m_param->IsErrInter) {
// 		HTuple errinterok, interval;
// 		getDispOutData(ErrInterOk, errinterok);
// 		getDispOutData(ErrInterVal, interval);
// 
// 		if (errinterok[0].I() == 0) {
// 			m_text->setTextColor(QColor(255, 0, 0));
// 		}
// 		else {
// 			m_text->setTextColor(QColor(0, 0, 0));
// 		}
// 		m_text->append(tr("Intersection distance= (%1),(%2,%3)").arg(QString::number(interval[0].D(), 'g', 6)).
// 			arg(QString::number(m_param->errInterMin)).arg(QString::number(m_param->errInterMax)));
// 	}
// 	if (m_param->IsErrX) {
// 		HTuple errxok, xval;
// 		getDispOutData(ErrXOk, errxok);
// 		getDispOutData(ErrX, xval);
// 		if (errxok[0].I() == 0) {
// 			m_text->setTextColor(QColor(255, 0, 0));
// 		}
// 		else {
// 			m_text->setTextColor(QColor(0, 0, 0));
// 		}
// 		m_text->append(tr("X= (%1),(%2,%3)").arg(QString::number(xval[0].D() - m_param->referX, 'g', 6)).
// 			arg(QString::number(m_param->errXMin)).arg(QString::number(m_param->errXMax)));
// 	}
// 	if (m_param->IsErrY) {
// 		HTuple erryok, yval;
// 		getDispOutData(ErrYOk, erryok);
// 		getDispOutData(ErrY, yval);
// 		if (erryok[0].I() == 0) {
// 			m_text->setTextColor(QColor(255, 0, 0));
// 		}
// 		else {
// 			m_text->setTextColor(QColor(0, 0, 0));
// 		}
// 		m_text->append(tr("Y= (%1),(%2,%3)").arg(QString::number(yval[0].D() - m_param->referY, 'g', 6)).
// 			arg(QString::number(m_param->errYMin)).arg(QString::number(m_param->errYMax)));
// 	}
// 	m_text->setTextColor(QColor(0, 0, 0));
}

void edgeInterMod::iniData()
{
	m_param = new edgeInterParam;
	m_centerX = m_centerY = 0;
	for (auto i = 0;i < MAX_EDGE_LINE;++i)
	{
		m_detect[i] = nullptr;
		m_roiFound[i] = true;
		m_begRow[i] = m_begCol[i] = 0;
		m_endRow[i] = m_endCol[i] = 0;
	}
	m_errInterOK = true;
	m_errInterDist = 0;
	m_errXok = m_errYok = true;
	m_errX = m_errY = 0;
	m_allok = 0;
	createPins();
}

void edgeInterMod::createPins()
{
	addPin(&p_im, "im");
	addPin(&p_homMat2d, "hm2d");

	addPin(&p_inters, "inters");
}

