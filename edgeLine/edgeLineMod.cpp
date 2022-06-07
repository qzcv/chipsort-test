#include "edgeLineMod.h"
#include "edgeLineParam.h"
#include <QObject>
#include <ImageView.h>
#include <resultText.h>
#include "Measure1D.h"
#include <opencv2/imgproc.hpp>
#include "halOpenCV.h"

edgeLineMod::edgeLineMod()
{
	iniData();
}

edgeLineMod::edgeLineMod(const QString& dirPath)
{
	iniData();
	load(dirPath, StructLevel);
}

edgeLineMod::~edgeLineMod()
{
	for (auto i = 0;i < 4;++i)
	{
		if (m_detect[i])
			delete m_detect[i];
	}

	delete m_param;
}

int edgeLineMod::command(int cmdID, void* dataIn, void* dataOut)
{
	return 0;
}

int edgeLineMod::afterSetProperty(MetaProperty*)
{
	return 0;
}

void edgeLineMod::save(const QString &dirPath)
{
	m_param->readWriteParam(false, dirPath, StructLevel);
	resetParamChanged();
}

void edgeLineMod::load(const QString &dirPath, QvsParamLevel level)
{
	m_param->readWriteParam(true,dirPath,level);
}

int edgeLineMod::run(const QString &funName)
{
// 	for (int i = 0;i < MAX_TESTITEMS_NUM;i++) {
// 		m_testItemsStatus[i] = 1;
// 	}
	p_corners->clear();

	if (m_dispReg)
		delete m_dispReg;

	if (p_im->empty())
		return -1;
	if (m_param->IsRefer&&p_homMat2d->size() != cv::Size(3, 2))
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
	int enableNum = 0;
	for (int i = 0;i < 4;i++) {
		if (!m_param->IsFixWidth[i])
			enableNum++;
	}
	if (enableNum <= 1) {
		m_allok = -1;
		return -1;
		//setDetectOutData(AllOK, allok);
		//setDetectOutData(ImageFull, false);
		//m_testItemsStatus[0] = 0;
		//return allok;
	}
// 	else {
// 		setDetectOutData(ImageFull, true);
// 	}
	
	for (int i = 0; i < 4;i++) {
		m_roiFound[i] = true;
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
	double regAngle[4], regRow[4], regCol[4];     //最终设入detector的坐标
	for (int i = 0;i < 4;i++) {
		regAngle[i] = m_param->roiReg[i].angle + rotateAngle;
		regRow[i] = m_param->roiReg[i].row;
		regCol[i] = m_param->roiReg[i].col;
		if (m_param->IsRefer) {
			affine_trans_pixel(hommat2d, regRow[i], regCol[i], &regRow[i], &regCol[i]);
		}
	}

	double lineBegRow[4], lineBegCol[4];//得到的边界线条的坐标
	double lineEndRow[4], lineEndCol[4];//得到的边界线条的坐标
	for (int i = 0;i < 4;i++)
	{
		if (m_param->IsFixWidth[i]) {
			m_roiFound[i] = true;
			continue;
		}
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
		m_roiFound[i] = ok;
		if (ok) {
			lineBegRow[i] = line[0].y;
			lineBegCol[i] = line[0].x;
			lineEndRow[i] = line[1].y;
			lineEndCol[i] = line[1].x;
			if (i == 0) {
				//printf("0-X=%.3f,0-Y=%.3f,1-X=%.3f,1-Y=%.3f\n",line[0].x,line[0].y,line[1].x,line[1].y);
			}
			if (i == 0 || i == 2) {
				lineBegRow[i] += m_param->lineOffset[i];
				lineEndRow[i] += m_param->lineOffset[i];
			}
			else if (i == 1 || i == 3) {
				lineBegCol[i] += m_param->lineOffset[i];
				lineEndCol[i] += m_param->lineOffset[i];
			}
			//printf("lineoff_%d=%d\n",i,m_param->lineOffset[i]);
			if (((lineEndRow[i] < lineBegRow[i]) && (i == 1 || i == 3)) || 
				((lineEndCol[i] < lineBegCol[i]) && (i == 2 || i == 0))) {   
				//调转开头点和结尾点，使全部符合图片上的正向顺1序
				//double tmp;
				//tmp = lineEndCol[i];
				//lineEndCol[i] = lineBegCol[i];
				//lineBegCol[i] = tmp;
				//tmp = lineEndRow[i];
				//lineEndRow[i] = lineBegRow[i];
				//lineBegRow[i] = tmp;
				std::swap(lineBegCol[i], lineEndCol[i]);
				std::swap(lineBegRow[i], lineEndRow[i]);
			}
		}
		else {
			m_allok = -1;
		}
	} //for loop end

	for (int i = 0;i < 4;i++) {
		if (!m_roiFound[i]) {
			m_allok = -1;
			return -1;
			//setDetectOutData(AllOK, allok);
			//m_testItemsStatus[0] = 0;
			//return allok;
		}
	}
	float EveryAngle[4];
	for (int i = 0;i < 4;i++) {
		if (m_param->IsFixWidth[i]) {
			continue;
		}
		if (!m_roiFound[i]) {
			continue;
		}
		if (i == 1 || i == 3) {
			EveryAngle[i] = PI / 2 + atan((lineEndCol[i] - lineBegCol[i]) / (lineEndRow[i] - lineBegRow[i]));
		}
		else {
			EveryAngle[i] = -atan((lineEndRow[i] - lineBegRow[i]) / (lineEndCol[i] - lineBegCol[i]));
		}
	}
	if ((m_param->IsFixWidth[0] && m_param->IsFixWidth[2])) {   // 上下
		lineBegRow[0] = lineBegRow[3];
		lineEndRow[0] = lineBegRow[1];
		lineBegCol[0] = lineBegCol[3];
		lineEndCol[0] = lineBegCol[1];

		lineBegRow[2] = lineEndRow[3];
		lineEndRow[2] = lineEndRow[1];
		lineBegCol[2] = lineEndCol[3];
		lineEndCol[2] = lineEndCol[1];
	}
	else if (m_param->IsFixWidth[1] && m_param->IsFixWidth[3]) {  //左右
		lineBegRow[1] = lineEndRow[0];
		lineEndRow[1] = lineEndRow[2];
		lineBegCol[1] = lineEndCol[0];
		lineEndCol[1] = lineEndCol[2];

		lineBegRow[3] = lineBegRow[0];
		lineEndRow[3] = lineBegRow[2];
		lineBegCol[3] = lineBegCol[0];
		lineEndCol[3] = lineBegCol[2];
	}
	else {
		if (m_param->IsFixWidth[0]) {
			lineBegRow[0] = lineBegRow[2] - m_param->fixWidth[0];
			lineEndRow[0] = lineEndRow[2] - m_param->fixWidth[0];
			lineBegCol[0] = lineBegCol[2];
			lineEndCol[0] = lineEndCol[2];
		}
		if (m_param->IsFixWidth[2]) {
			lineBegRow[2] = lineBegRow[0] + m_param->fixWidth[2];
			lineEndRow[2] = lineEndRow[0] + m_param->fixWidth[2];
			lineBegCol[2] = lineBegCol[0];
			lineEndCol[2] = lineEndCol[0];
		}
		if (m_param->IsFixWidth[1]) {
			lineBegRow[1] = lineBegRow[3];
			lineEndRow[1] = lineEndRow[3];
			lineBegCol[1] = lineBegCol[3] + m_param->fixWidth[1];
			lineEndCol[1] = lineEndCol[3] + m_param->fixWidth[1];
		}
		if (m_param->IsFixWidth[3]) {
			lineBegRow[3] = lineBegRow[1];
			lineEndRow[3] = lineEndRow[1];
			lineBegCol[3] = lineBegCol[1] - m_param->fixWidth[3];
			lineEndCol[3] = lineEndCol[1] - m_param->fixWidth[3];
		}
	}
	for (int i = 0;i < 4;i++) {
		if (!m_roiFound[i]) {
			continue;
		}
		m_begRow[i] = lineBegRow[i];
		m_begCol[i] = lineBegCol[i];
		m_endRow[i] = lineEndRow[i];
		m_endCol[i] = lineEndCol[i];
		//setDetectOutData(BeginRow, lineBegRow[i], i);
		//setDetectOutData(BeginCol, lineBegCol[i], i);
		//setDetectOutData(EndRow, lineEndRow[i], i);
		//setDetectOutData(EndCol, lineEndCol[i], i);
	}
	HTuple finalEndRow[4], finalEndCol[4];
	HTuple isparel;
	bool interok[4];
	for (int i = 0;i < 4;i++) {
		if (i != 3) {
			intersection_ll(lineBegRow[i], lineBegCol[i], lineEndRow[i], lineEndCol[i],
				lineBegRow[i + 1], lineBegCol[i + 1], lineEndRow[i + 1], lineEndCol[i + 1], &finalEndRow[i], &finalEndCol[i], &isparel);
		}
		else {
			intersection_ll(lineBegRow[3], lineBegCol[3], lineEndRow[3], lineEndCol[3],
				lineBegRow[0], lineBegCol[0], lineEndRow[0], lineEndCol[0], &finalEndRow[i], &finalEndCol[i], &isparel);
		}
		if (isparel[0].I()) {
			interok[i] = 0;
			m_allok = -1;
			return -1;
			//setDetectOutData(AllOK, allok);
			//cvReleaseImage(&pImage);
			//m_testItemsStatus[0] = 0;
			//return allok;
		}
		else {
			interok[i] = 1;
		}
	}

	HTuple centerRoww, centerColl;
	intersection_ll(
		finalEndRow[0], finalEndCol[0], finalEndRow[2], finalEndCol[2],
		finalEndRow[1], finalEndCol[1], finalEndRow[3], finalEndCol[3], &centerRoww, &centerColl, &isparel);
	if (isparel[0].I()) {
		m_allok = -1;
		return -1;
		//setDetectOutData(AllOK, allok);
		//cvReleaseImage(&pImage);
		//m_testItemsStatus[0] = 0;
		//return allok;
	}
	m_centerRow = centerRoww[0].D();
	m_centerCol = centerColl[0].D();
	//setDetectOutData(CenterRow, centerRoww);
	//setDetectOutData(CenterCol, centerColl);

	HTuple fnalRow, fnalCol;
	for (int i = 0;i < 4;i++) {
		if (i != 3) {
			fnalRow[i + 1] = finalEndRow[i];
			fnalCol[i + 1] = finalEndCol[i];
		}
		else if (i == 3) {
			fnalRow[0] = finalEndRow[3];
			fnalCol[0] = finalEndCol[3];
		}
	}
	fnalRow[4] = fnalRow[0].D();
	fnalCol[4] = fnalCol[0].D();

	for (auto i = 0;i < 5;++i)	//左上角顺时针
	{
		double x = fnalCol[i].D();
		double y = fnalRow[i].D();
		p_corners->push_back(cv::Point2d(x,y));
	}

	set_system("clip_region", "false");
	Hobject reg;
	gen_region_polygon_filled(&reg, fnalRow, fnalCol);
	m_dispReg = new RlRegion();
	HalOpenCV::hal2cvRegion(reg, m_dispReg);

	//setDetectOutData(FinalRow, fnalRow);  //左上角顺时针
	//setDetectOutData(FinalCol, fnalCol);

	float tmpAngle[4];
	if (m_param->IsErrAngle)
	{
		m_errAngleOk = true;
		float maxangle = 0;

		for (int i = 0;i < 4;i++) {
			if (i > 1) {
				tmpAngle[i] = PI / 2 + atan((fnalCol[2 * i - 3] - fnalCol[6 - 2 * i]) / (fnalRow[2 * i - 3] - fnalRow[6 - 2 * i]));//1,2   3,0
			}
			else {
				tmpAngle[i] = -atan((fnalRow[2 * i] - fnalRow[2 * i + 1]) / (fnalCol[2 * i] - fnalCol[2 * i + 1]));   //0,1     2,3
			}
		}
		for (int i = 0;i < 4;i++) {
			if (((tmpAngle[i] * 180.0) / PI < (44 + 90)) && 
				((tmpAngle[i] * 180.0) / PI > (-44 + 90))) {
				tmpAngle[i] -= 90.0*PI / 180.0;
			}
			if (abs(tmpAngle[i]) > maxangle) {
				maxangle = abs(tmpAngle[i]);
			}
		}
		if (maxangle > m_param->errAngle) {
			m_errAngleOk = false;
			m_allok = -4;
			//m_testItemsStatus[3] = 0;
		}
		m_maxAngle = maxangle / PI * 180;
		//setDetectOutData(ErrAngleOk, errAngleOk);
		//setDetectOutData(ErrAngle, maxangle / PI * 180);
	}
	if (m_param->IsErrWidth)
	{
		m_errWidOk = true;
		float maxwidth = 0;
		float minwidth = 10000;
		float dis;
		dis = sqrt(
			(finalEndRow[0][0].D() - finalEndRow[3][0].D())*(finalEndRow[0][0].D() - finalEndRow[3][0].D()) +
			(finalEndCol[0][0].D() - finalEndCol[3][0].D())*(finalEndCol[0][0].D() - finalEndCol[3][0].D()));
		if (maxwidth < dis)
			maxwidth = dis;
		if (minwidth > dis)
			minwidth = dis;
		dis = sqrt(
			(finalEndRow[1][0].D() - finalEndRow[2][0].D())*(finalEndRow[1][0].D() - finalEndRow[2][0].D()) +
			(finalEndCol[1][0].D() - finalEndCol[2][0].D())*(finalEndCol[1][0].D() - finalEndCol[2][0].D()));
		if (maxwidth < dis)
			maxwidth = dis;
		if (minwidth > dis)
			minwidth = dis;
		if (maxwidth > m_param->errWidthMax || minwidth < m_param->errWidthMin) {
			m_errWidOk = false;
			m_allok = -2;
			//m_testItemsStatus[1] = 0;
		}
		m_maxWid = maxwidth;
		m_minWid = minwidth;
		//setDetectOutData(ErrWidthOk, errwidthok);
		//setDetectOutData(ErrWidthMax, maxwidth);
		//setDetectOutData(ErrWidthMin, minwidth);
	}
	if (m_param->IsErrHeight)
	{
		m_errHeiOk = true;
		float maxheight = 0;
		float minheight = 10000;
		float dis;
		dis = sqrt(
			(finalEndRow[0][0].D() - finalEndRow[1][0].D())*(finalEndRow[0][0].D() - finalEndRow[1][0].D()) +
			(finalEndCol[0][0].D() - finalEndCol[1][0].D())*(finalEndCol[0][0].D() - finalEndCol[1][0].D()));
		if (maxheight < dis)
			maxheight = dis;
		if (minheight > dis)
			minheight = dis;
		dis = sqrt(
			(finalEndRow[2][0].D() - finalEndRow[3][0].D())*(finalEndRow[2][0].D() - finalEndRow[3][0].D()) +
			(finalEndCol[2][0].D() - finalEndCol[3][0].D())*(finalEndCol[2][0].D() - finalEndCol[3][0].D()));
		if (maxheight < dis)
			maxheight = dis;
		if (minheight > dis)
			minheight = dis;
		if (maxheight > m_param->errHeightmax || minheight < m_param->errHeightMin) {
			m_errHeiOk = false;
			m_allok = -3;
			//m_testItemsStatus[2] = 0;
		}
		m_maxHei = maxheight;
		m_minHei = minheight;
		//setDetectOutData(ErrHeightOk, errheightok);
		//setDetectOutData(ErrHeightMax, maxheight);
		//setDetectOutData(ErrHeightMin, minheight);
	}
	if (m_param->IsErrCenterOffset)
	{
		m_centerOk = true;
		m_centerShift = sqrt(
			pow((centerRoww[0].D() - m_param->TmpCenterRow), 2) + 
			pow((centerColl[0].D() - m_param->TmpCenterCol), 2));
		if (m_centerShift > m_param->errCenterOffset) {
			m_allok = -5;
			m_centerOk = false;
			//m_testItemsStatus[4] = 0;
		}
		//setDetectOutData(ErrCenterShift, Centershift);
		//setDetectOutData(ErrCenterShiftOk, centerok);
	}
	//setDetectOutData(AllOK, allok);
	return m_allok;
}

void edgeLineMod::viewResult(ImageView *iv, const QString &funName, int)
{
	if (p_im->empty())
		return;
	if (m_param->IsRefer&&p_homMat2d->size() != cv::Size(3, 2))
		return;

	for (int i = 0;i < 4;i++) {
		if (m_roiFound[i] == 0) {
			iv->setColor(Qt::red);
			iv->setFilled(false);
			for (int j = 0;j < 4;j++) {
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
		for (int i = 0;i < 4;i++) {
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

	if (m_dispReg)
	{
		iv->setFilled(false);
		if (m_param->IsShowRegionAfterDet) {
			iv->dispRegion(m_dispReg);
		}
	}
	iv->dispLine(m_centerRow, m_centerCol - 3, m_centerRow, m_centerCol + 3);
	iv->dispLine(m_centerRow - 3, m_centerCol, m_centerRow + 3, m_centerCol);

// 	HTuple imagefull;
// 	getDispOutData(ImageFull, imagefull);
// 	if (!imagefull[0].I()) {
// 		return;
// 	}
// 	HTuple findok;
// 	for (int i = 0;i < 4;i++) {
// 		getDispOutData(FindOk, i, findok);
// 		if (findok[0].I() == 0) {
// 			m_view->setColor(255, 0, 0);
// 			m_view->setFilled(false);
// 			HTuple homat;
// 			if (m_param->IsRefer) {
// 				getDispOutData(TmpHomMat, homat);
// 			}
// 			for (int j = 0;j < 4;j++) {
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
// 		for (int i = 0;i < 4;i++) {
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
//
// 	HTuple allok;
// 	getDispOutData(AllOK, allok);
// 	if (allok[0].I() == 0) {
// 		m_view->setColor(255, 0, 0);
// 	}
// 	else if (allok[0].I() == 1) {
// 		m_view->setColor(0, 255, 0);
// 	}
// 	HTuple polyRow, polyCol;
// 	getDispOutData(FinalRow, polyRow);
// 	getDispOutData(FinalCol, polyCol);
// 	polyRow[4] = polyRow[0];
// 	polyCol[4] = polyCol[0];
// 	Hobject polyRec;
// 	gen_contour_polygon_xld(&polyRec, polyRow, polyCol);
// 	gen_region_contour_xld(polyRec, &polyRec, "filled");
// 	m_view->setFilled(false);
// 	if (m_param->IsShowRegionAfterDet) {
// 		m_view->dispRegion(polyRec);
// 	}
// 	HTuple centerRow, centerCol;
// 	getDispOutData(CenterRow, centerRow);
// 	getDispOutData(CenterCol, centerCol);
// 	m_view->dispLine(centerRow[0].D(), centerCol[0].D() - 3, centerRow[0].D(), centerCol[0].D() + 3);
// 	m_view->dispLine(centerRow[0].D() - 3, centerCol[0].D(), centerRow[0].D() + 3, centerCol[0].D());
}

void edgeLineMod::textResult(ResultText *text, const QString &funName)
{
	if (p_im->empty())
	{
		text->setTextColor(Qt::red);
		text->append(QObject::tr("Empty image!"));
		return;
	}
	if (m_param->IsRefer&&p_homMat2d->size() != cv::Size(3, 2))
	{
		text->setTextColor(Qt::red);
		text->append(QObject::tr("Input matrix's size != 2*3!"));
		return;
	}

	QStringList t = { QObject::tr("Top"),QObject::tr("Right"),QObject::tr("Bottom"),QObject::tr("Left") };
	for (int i = 0;i < 4;i++) {
		if (m_roiFound[i] == 0) {
			text->setTextColor(Qt::red);
			text->append(t[i]);
			text->insertPlainText(QObject::tr(" edge found failed"));
			return;
		}
	}
	if (m_param->IsErrWidth) {
		if (m_errWidOk == 0)
			text->setTextColor(Qt::red);
		else
			text->setTextColor(Qt::black);
		text->append(QObject::tr("Body Width= (%1,%2),(%3,%4)")
			.arg(QString::number(m_minWid, 'g', 6))
			.arg(QString::number(m_maxWid, 'g', 6))
			.arg(QString::number(m_param->errWidthMin))
			.arg(QString::number(m_param->errWidthMax)));
	}
	if (m_param->IsErrHeight) {
		if (m_errHeiOk == 0)
			text->setTextColor(Qt::red);
		else
			text->setTextColor(Qt::black);
		text->append(QObject::tr(" Body Height= (%1,%2),(%3,%4)")
			.arg(QString::number(m_minHei, 'g', 6))
			.arg(QString::number(m_maxHei, 'g', 6))
			.arg(QString::number(m_param->errHeightMin))
			.arg(QString::number(m_param->errHeightmax)));
	}
	if (m_param->IsErrAngle) {
		if (m_errAngleOk == 0)
			text->setTextColor(Qt::red);
		else
			text->setTextColor(Qt::black);
		text->append(QObject::tr(" Body Angle= %1(%2)")
			.arg(QString::number(m_maxAngle, 'g', 6))
			.arg(QString::number(m_param->errAngle)));
	}
	if (m_param->IsErrCenterOffset) {
		if (m_centerOk == 0)
			text->setTextColor(Qt::red);
		else
			text->setTextColor(Qt::black);
		text->append(QObject::tr(" Body Shift= %1(%2)")
			.arg(QString::number(m_centerShift, 'g', 6))
			.arg(QString::number(m_param->errCenterOffset)));
	}

// 	HTuple imagefull;
// 	getDispOutData(ImageFull, imagefull);
// 	if (!imagefull[0].I()) {
// 		return;
// 	}
// 	HTuple found;
// 	char charbuf[4][32] = { "Top","Right","Bottom","Left" };
// 	for (int i = 0;i < 4;i++) {
// 		getDispOutData(FindOk, i, found);
// 		if (found[0].I() == 0) {
// 			m_text->setTextColor(QColor(255, 0, 0));
// 			m_text->append(QString::fromLocal8Bit(charbuf[i]));
// 			m_text->insertPlainText(tr(" edge found failed"));
// 			return;
// 		}
// 	}
// 	if (m_param->IsErrWidth) {
// 		HTuple errwidthok, errwidthmin, errwidthmax;
// 		getDispOutData(ErrWidthOk, errwidthok);
// 		getDispOutData(ErrWidthMax, errwidthmax);
// 		getDispOutData(ErrWidthMin, errwidthmin);
// 		if (errwidthok[0].I() == 0) {
// 			m_text->setTextColor(QColor(255, 0, 0));
// 		}
// 		else {
// 			m_text->setTextColor(QColor(0, 0, 0));
// 		}
// 		m_text->append(tr("Body Width= (%1,%2),(%3,%4)").arg(QString::number(errwidthmin[0].D(), 'g', 6)).arg(QString::number(errwidthmax[0].D(), 'g', 6)).
// 			arg(QString::number(m_param->errWidthMin)).arg(QString::number(m_param->errWidthMax)));
// 		//m_text->append(QString::fromLocal8Bit("本体宽度 = (%1,%2),(%3,%4)").arg(QString::number(errwidthmin[0].D(),'g',6)).arg(QString::number(errwidthmax[0].D(),'g',6)).
// 		//	arg(QString::number(m_param->errWidthMin)).arg(QString::number(m_param->errWidthMax)));
// 	}
// 	if (m_param->IsErrHeight) {
// 		HTuple errheightok, errheightmax, errheightmin;
// 		getDispOutData(ErrHeightOk, errheightok);
// 		getDispOutData(ErrHeightMax, errheightmax);
// 		getDispOutData(ErrHeightMin, errheightmin);
// 		if (errheightok[0].I() == 0) {
// 			m_text->setTextColor(QColor(255, 0, 0));
// 		}
// 		else {
// 			m_text->setTextColor(QColor(0, 0, 0));
// 		}
// 		m_text->append(tr(" Body Height= (%1,%2),(%3,%4)").arg(QString::number(errheightmin[0].D(), 'g', 6)).arg(QString::number(errheightmax[0].D(), 'g', 6)).
// 			arg(QString::number(m_param->errHeightMin)).arg(QString::number(m_param->errHeightmax)));
// 	}
// 	if (m_param->IsErrAngle) {
// 		HTuple errangleok, errangle;
// 		getDispOutData(ErrAngleOk, errangleok);
// 		getDispOutData(ErrAngle, errangle);
// 		if (errangleok[0].I() == 0) {
// 			m_text->setTextColor(QColor(255, 0, 0));
// 		}
// 		else {
// 			m_text->setTextColor(QColor(0, 0, 0));
// 		}
// 		m_text->append(tr(" Body Angle= %1(%2)").arg(QString::number(errangle[0].D(), 'g', 6)).
// 			arg(QString::number(m_param->errAngle)));
// 	}
// 	if (m_param->IsErrCenterOffset) {
// 		HTuple errcenterok, errcenter;
// 		getDispOutData(ErrCenterShiftOk, errcenterok);
// 		getDispOutData(ErrCenterShift, errcenter);
// 		if (errcenterok[0].I() == 0) {
// 			m_text->setTextColor(QColor(255, 0, 0));
// 		}
// 		else {
// 			m_text->setTextColor(QColor(0, 0, 0));
// 		}
// 		m_text->append(tr(" Body Shift= %1(%2)").arg(QString::number(errcenter[0].D(), 'g', 6)).
// 			arg(QString::number(m_param->errCenterOffset)));
// 	}
// 	m_text->setTextColor(QColor(0, 0, 0));
}

void edgeLineMod::iniData()
{
	m_param = new edgeLineParam;
	for (auto i = 0;i < 4;++i)
	{
		m_detect[i] = nullptr;
		m_roiFound[i] = true;

		m_begRow[i] = m_begCol[i] = 0;
		m_endRow[i] = m_endCol[i] = 0;
	}
	m_dispReg = nullptr;

	m_allok = 0;

	m_centerRow = m_centerCol = 0;

	m_errAngleOk = true;
	m_maxAngle = 0;

	m_errWidOk = true;
	m_minWid = m_maxWid = 0;
	m_errHeiOk = true;
	m_minWid = m_maxHei = 0;
	m_centerOk = true;
	m_centerShift = 0;
	createPins();
}

void edgeLineMod::createPins()
{
	addPin(&p_im, "im");
	addPin(&p_homMat2d, "hm2d");

	addPin(&p_corners, "corners");
}

