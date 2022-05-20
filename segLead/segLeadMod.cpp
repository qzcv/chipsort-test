#include "segLeadMod.h"
#include "segLeadParam.h"
#include <QObject>
#include <ImageView.h>
#include <resultText.h>
#include "halOpenCV.h"
#include <opencv2/imgproc.hpp>

segLeadMod::segLeadMod()
{
	iniData();
}

segLeadMod::segLeadMod(const QString& dirPath)
{
	iniData();
	load(dirPath, StructLevel);
}

segLeadMod::~segLeadMod()
{
	delete m_param;
	delete m_image;
}

int segLeadMod::command(int cmdID, void* dataIn, void* dataOut)
{
	return 0;
}

int segLeadMod::afterSetProperty(MetaProperty*)
{
	return 0;
}

void segLeadMod::save(const QString &dirPath)
{
	m_param->readWriteParam(false, dirPath, StructLevel);
}

void segLeadMod::load(const QString &dirPath, QvsParamLevel level)
{
	m_param->readWriteParam(true,dirPath,level);
}

int segLeadMod::run(const QString &funName)
{
// 	for (int i = 0;i < MAX_TESTITEMS_NUM;i++) {
// 		m_testItemsStatus[i] = 1;
// 	}
	if (p_im->empty())
		return -1;
	if (m_param->referSection&&p_homMat2d->size() != cv::Size(2, 3))
		return -1;
	else
		HalOpenCV::cv2HalHomMat2D(*p_homMat2d, &m_homMat2d);
	if (p_im->channels() == 3)
	{
		cv::Mat im;
		cv::cvtColor(**p_im, im, CV_BGR2GRAY);
		HalOpenCV::cv2halImg(im, m_image, true);
	}
	else
		HalOpenCV::cv2halImg(*p_im, m_image, false);

	m_allok = 0;
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
// 	if (m_param->referSection) {
// 		try {
// 			HalOpenCV::cv2HalHomMat2D(*p_homMat2d, &m_homMat2d);
// 			double tv = m_homMat2d[0].D();
// 		}
// 		catch (HException& ex) {
// 			hom_mat2d_identity(&m_homMat2d);
// 		}
// 	}
	double mk;//像素比例
	if (m_param->IsInputPixel) {
// 		try {
// 			mk = (**m_pixelSize)[0].D();
// 		}
// 		catch (HException& ex) {
// 			mk = m_param->pixSize;
// 		}
		if (p_pixelSize->size())
			mk = p_pixelSize->at(0);
		else
			mk = m_param->pixSize;
	}
	else {
		mk = m_param->pixSize;
	}
	HTuple fianalrow, fianalcol;
	double referRow[2][2], referCol[2][2];   //计算时使用的基准线
	double edgeAngle;   //边界相对于正交角度的一个旋转角度
	if (m_param->IsReferEdge) {
		//getDetectInData(ReferRow, fianalrow);   //左上角顺时针
		//getDetectInData(ReferCol, fianalcol);
		//setDetectOutData(TmpReferRow, fianalrow);
		//setDetectOutData(TmpReferCol, fianalcol);
		int m = min(p_row->size(), p_col->size());
		for (auto i = 0;i < m;++i)
		{
			m_row[i] = fianalrow[i] = p_row->at(i);
			m_col[i] = fianalcol[i] = p_col->at(i);
		}
		double angle11, angle12;
		try {
			if (m_param->HorOrVer == 0) {  //0上下边界
				referRow[0][0] = fianalrow[0].D();
				referRow[0][1] = fianalrow[1].D();
				referCol[0][0] = fianalcol[0].D();
				referCol[0][1] = fianalcol[1].D();
				referRow[1][0] = fianalrow[2].D();
				referRow[1][1] = fianalrow[3].D();
				referCol[1][0] = fianalcol[2].D();
				referCol[1][1] = fianalcol[3].D();
				angle11 = -atan((referRow[0][1] - referRow[0][0]) / (referCol[0][1] - referCol[0][0]));
				angle12 = -atan((referRow[1][1] - referRow[1][0]) / (referCol[1][1] - referCol[1][0]));
				edgeAngle = (angle11 + angle12) / 2.0;
			}
			else if (m_param->HorOrVer == 1) {//1左右边界
				referRow[0][0] = fianalrow[0].D();
				referRow[0][1] = fianalrow[3].D();
				referCol[0][0] = fianalcol[0].D();
				referRow[1][0] = fianalrow[1].D();
				referCol[0][1] = fianalcol[3].D();
				referRow[1][1] = fianalrow[2].D();
				referCol[1][0] = fianalcol[1].D();
				referCol[1][1] = fianalcol[2].D();
				//angle11=PI/2.0+atan((m_referCol[0][1]-m_referCol[0][0])/(m_referRow[0][1]-m_referRow[0][0]));
				//angle12=PI/2.0+atan(((m_referCol[1][1]-m_referCol[1][0])/m_referRow[1][1]-m_referRow[1][0]));
				angle11 = -atan((referRow[0][1] - referRow[0][0]) / (referCol[0][1] - referCol[0][0]));
				angle12 = -atan((referRow[1][1] - referRow[1][0]) / (referCol[1][1] - referCol[1][0]));
				edgeAngle = (angle11 + angle12) / 2.0;
				edgeAngle -= PI / 2.0;
			}
		}
		catch (HException& ex) {
			m_row[0] = referRow[0][0] = 150;
			m_row[1] = referRow[0][1] = 100;
			m_col[0] = referCol[0][0] = 100;
			m_col[1] = referCol[0][1] = 300;
			m_row[2] = referRow[1][0] = 200;
			m_row[3] = referRow[1][1] = 150;
			m_col[2] = referCol[1][0] = 300;
			m_col[3] = referCol[1][1] = 500;
			edgeAngle = 0;
		}
	}
	else if (!m_param->IsReferEdge) {
		if (m_param->HorOrVer == 0) {  //0上下边界
			referRow[0][0] = m_param->RoiVec[0].row1;
			referRow[0][1] = m_param->RoiVec[0].row1;
			referCol[0][0] = m_param->RoiVec[0].col1;
			referCol[0][1] = m_param->RoiVec[0].col2;
			referRow[1][0] = m_param->RoiVec[0].row2;
			referRow[1][1] = m_param->RoiVec[0].row2;
			referCol[1][0] = m_param->RoiVec[0].col1;
			referCol[1][1] = m_param->RoiVec[0].col2;
		}
		else if (m_param->HorOrVer == 1) {//1左右边界
			referRow[0][0] = m_param->RoiVec[0].row1;
			referRow[0][1] = m_param->RoiVec[0].row2;
			referCol[0][0] = m_param->RoiVec[0].col1;
			referCol[0][1] = m_param->RoiVec[0].col1;
			referRow[1][0] = m_param->RoiVec[0].row1;
			referRow[1][1] = m_param->RoiVec[0].row2;
			referCol[1][0] = m_param->RoiVec[0].col2;
			referCol[1][1] = m_param->RoiVec[0].col2;
		}
		edgeAngle = 0;
		if (m_param->referSection) {
			for (int i = 0;i < 2;i++) {
				for (int j = 0;j < 2;j++) {
					affine_trans_pixel(m_homMat2d, referRow[i][j], referCol[i][j],
						&referRow[i][j], &referCol[i][j]);
				}
			}
			edgeAngle += atan((m_homMat2d[3].D() / m_homMat2d[0].D()));
		}
	}
	Hobject dstImg;
	if (m_param->IsGauss) {
		int vaaa = (m_param->GaussSize % 2) == 0 ? 1 : 0;
		gauss_image(*m_image, &dstImg, vaaa + m_param->GaussSize);
	}
	else {
		copy_image(*m_image, &dstImg);
	}
	HTuple imgheight, imgwid;
	get_image_size(*m_image, &imgwid, &imgheight);
	Hobject recroi;
	gen_rectangle1(&recroi, 
		m_param->RoiVec[0].row1, m_param->RoiVec[0].col1, 
		m_param->RoiVec[0].row2, m_param->RoiVec[0].col2);
	if (m_param->testType == 0) {   //平滑
		Hobject meanImg;
		mean_image(dstImg, &meanImg, m_param->meanWidth, m_param->meanHeight);
		char buf[32];
		if (m_param->WOrB == 0) {
			sprintf(buf, "light");
		}
		else {
			sprintf(buf, "dark");
		}
		Hobject whiteImg, blackImg;
		dyn_threshold(dstImg, meanImg, &whiteImg, m_param->grayWhite, "light");
		dyn_threshold(dstImg, meanImg, &blackImg, m_param->grayBlack, "dark");
		union2(whiteImg, blackImg, &dstImg);
		if (m_param->referSection == 1) {
			affine_trans_region(recroi, &recroi, m_homMat2d, "false");
		}
		reduce_domain(dstImg, recroi, &dstImg);
		if (m_param->closeWidth > 0 && m_param->closeHeight > 0) {
			closing_rectangle1(dstImg, &dstImg, m_param->closeWidth, m_param->closeHeight);
		}
		if (m_param->openWidth > 0 && m_param->openHeight > 0) {
			opening_rectangle1(dstImg, &dstImg, m_param->openWidth, m_param->openHeight);
		}
	}
	else if (m_param->testType == 1) {  //区域生长
		int mcenterx, mcentery;
		if (m_param->centerType == 0) {
			mcenterx = (m_param->RoiVec[0].col1 + m_param->RoiVec[0].col2) / 2;
			mcentery = (m_param->RoiVec[0].row1 + m_param->RoiVec[0].row2) / 2;
		}
		else if (m_param->centerType == 1) {
			mcenterx = m_param->centerX > (imgwid[0].I() - 1) ? (imgwid[0].I() - 1) : m_param->centerX;
			mcentery = m_param->centerY > (imgheight[0].I() - 1) ? (imgheight[0].I() - 1) : m_param->centerY;
		}
		if (m_param->referSection == 1) {
			HTuple mrow, mcol;
			mrow[0] = (double)mcentery;
			mcol[0] = (double)mcenterx;
			affine_trans_pixel(m_homMat2d, mrow, mcol, &mrow, &mcol);
			mcenterx = mcol[0].I();
			mcentery = mrow[0].I();
			affine_trans_region(recroi, &recroi, m_homMat2d, "false");
		}
		//if (m_param->IsReferEdge){
		//	gen_region_polygon_filled(&recroi,fianalrow,fianalcol);
		//}
		reduce_domain(dstImg, recroi, &dstImg);
		regiongrowing(dstImg, &dstImg, m_param->rowWid, m_param->colWid, m_param->thresholdTol, m_param->minGrowArea);
		if (m_param->closeWidth > 0 && m_param->closeHeight > 0) {
			opening_rectangle1(dstImg, &dstImg, m_param->closeWidth, m_param->closeHeight);
		}
		if (m_param->openWidth > 0 && m_param->openHeight > 0) {
			closing_rectangle1(dstImg, &dstImg, m_param->openWidth, m_param->openHeight);
		}
		//reduce_domain(dstImg,recroi,&dstImg);
		connection(dstImg, &dstImg);
		//select_region_point(dstImg,&dstImg,mcentery,mcenterx);
		union1(dstImg, &dstImg);

		//setDetectOutObject(DstImg,dstImg);

		difference(recroi, dstImg, &dstImg);
		union1(dstImg, &dstImg);
	}
	else if (m_param->testType == 2) {
		threshold(dstImg, &dstImg, m_param->threMin, m_param->threMax < m_param->threMin ? m_param->threMin : m_param->threMax);
		if (m_param->referSection == 1) {
			affine_trans_region(recroi, &recroi, m_homMat2d, "false");
		}
		reduce_domain(dstImg, recroi, &dstImg);
		if (m_param->closeWidth > 0 && m_param->closeHeight > 0) {
			closing_rectangle1(dstImg, &dstImg, m_param->closeWidth, m_param->closeHeight);
		}
		if (m_param->openWidth > 0 && m_param->openHeight > 0) {
			opening_rectangle1(dstImg, &dstImg, m_param->openWidth, m_param->openHeight);
		}
	}
	
	//bool foundok = true;   
	for (auto i = 0;i < Max_LeadNum;++i)	//是否有找到管脚
		m_foundok[i] = true;
	double recCenterRow[Max_LeadNum], recCenterCol[Max_LeadNum];
	Hobject unionReg;
	for (int i = 0;i < m_param->leadNum;i++) {
		bool leadok = true;    //管脚检测项目是否ok
		Hobject leadRegion;
		gen_rectangle1(&leadRegion, 
			m_param->RoiVec[i + 1].row1, m_param->RoiVec[i + 1].col1,
			m_param->RoiVec[i + 1].row2, m_param->RoiVec[i + 1].col2);
		if (m_param->referSection == 1) {
			affine_trans_region(leadRegion, &leadRegion, m_homMat2d, "false");
		}
		reduce_domain(dstImg, leadRegion, &leadRegion);
		connection(leadRegion, &leadRegion);
		select_shape(leadRegion, &leadRegion, "area", "and", m_param->areaMin, m_param->areaMax < m_param->areaMin ? m_param->areaMin : m_param->areaMax);
		union1(leadRegion, &leadRegion);
		Hlong num;
		count_obj(leadRegion, &num);
		if (num == 0) {
			//foundok = false;
			//setDetectOutData(leadFoundOk, foundok, i);
			m_foundok[i] = false;
			m_allok = -1;
			//m_testItemsStatus[0] = 0;
			continue;
		}
		else {
			union2(unionReg, leadRegion, &unionReg);
			m_foundok[i] = true;
			//setDetectOutData(leadFoundOk, true, i);
		}
		if (m_foundok[i] == false) {   //如果数目检测不过则无需进行后面的具体计算任务
			continue;
		}
		HTuple arow, acol, aarea;
		area_center(leadRegion, &aarea, &arow, &acol);
		HTuple hommat;
		vector_angle_to_rigid(arow[0], acol[0], edgeAngle, arow[0], acol[0], 0, &hommat);
		Hobject transReg;
		affine_trans_region(leadRegion, &transReg, hommat, "false");
		HTuple recrow1, recrow2, reccol1, reccol2;
		smallest_rectangle1(transReg, &recrow1, &reccol1, &recrow2, &reccol2);
		double recrow, reccol, reclen1, reclen2, recangle;
		recrow = (recrow1[0].D() + recrow2[0].D()) / 2.0;
		reccol = (reccol1[0].D() + reccol2[0].D()) / 2.0;
		reclen1 = (reccol2[0].D() - reccol1[0].D() + 1) / 2.0;
		reclen2 = (recrow2[0].D() - recrow1[0].D() + 1) / 2.0;
		recangle = 0;
		HTuple hommat1;
		vector_angle_to_rigid(arow[0], acol[0], 0, arow[0], acol[0], edgeAngle, &hommat1);
		affine_trans_pixel(hommat1, recrow, reccol, &recrow, &reccol);
		recCenterRow[i] = recrow;
		recCenterCol[i] = reccol;
		recangle = edgeAngle;
		double leadwidth, leadlength, leadInter;
		if (m_param->HorOrVer == 0) {  //水平
			leadwidth = reclen1*2.0;
			leadlength = reclen2*2.0;
		}
		else if (m_param->HorOrVer == 1) {
			leadwidth = reclen2*2.0;
			leadlength = reclen1*2.0;
		}
		if (i > 0) {
			distance_pp(recCenterRow[i], recCenterCol[i], recCenterRow[i - 1], recCenterCol[i - 1], &leadInter);
			double langle;
			angle_ll(recCenterRow[i], recCenterCol[i], recCenterRow[i - 1], recCenterCol[i - 1], referRow[0][0], referCol[0][0],
				referRow[0][1], referCol[0][1], &langle);
			leadInter = leadInter*abs(cos(langle));
		}
		if (m_param->errWidthValid) {
			leadwidth *= mk;
			m_leadWidOk[i] = true;
			if (leadwidth > m_param->errWidthMax[i] || leadwidth < m_param->errWidthMin[i]) {
				m_leadWidOk[i] = false;
				leadok = false;
				m_allok = -2;
				//m_testItemsStatus[1] = 0;
			}
			m_leadWid[i] = leadwidth;
			//setDetectOutData(LeadWidth, leadwidth, i);
			//setDetectOutData(LeadWidthOk, widthok, i);
		}
		if (m_param->errLengthValid) {
			leadlength *= mk;
			m_leadLenOk[i] = true;
			if (leadlength > m_param->errLengthMax[i] || leadlength < m_param->errLengthMin[i]) {
				m_leadLenOk[i] = false;
				leadok = false;
				m_allok = -3;
				//m_testItemsStatus[2] = 0;
			}
			m_leadLen[i] = leadlength;
			//setDetectOutData(LeadLength, leadlength, i);
			//setDetectOutData(LeadLengthOk, lengthok, i);
		}
		if (m_param->errInterValid &&i > 0) {
			leadInter *= mk;
			m_leadInterOk[i - 1] = true;
			if (leadInter > m_param->errInterMax[i - 1] || leadInter < m_param->errInterMin[i - 1]) {
				m_leadInterOk[i - 1] = false;
				leadok = false;
				m_allok = -4;
				//m_testItemsStatus[3] = 0;
			}
			m_leadInter[i - 1] = leadInter;
			//setDetectOutData(LeadInter, leadInter, i - 1);
			//setDetectOutData(LeadInterOk, interok, i - 1);
		}
		m_leadRow[i] = recrow;
		m_leadCol[i] = reccol;
		m_leadAng[i] = recangle;
		m_leadLen1[i] = reclen1;
		m_leadLen2[i] = reclen2;
		m_leadOk[i] = leadok;
		//setDetectOutData(LeadRow, recrow, i);
		//setDetectOutData(LeadCol, reccol, i);
		//setDetectOutData(LeadAngle, recangle, i);
		//setDetectOutData(LeadLen1, reclen1, i);
		//setDetectOutData(LeadLen2, reclen2, i);
		//setDetectOutData(LeadOk, leadok, i);
	}
	union1(unionReg, &unionReg);
	HalOpenCV::hal2cvRegion(unionReg, &m_dispReg);
	//setDetectOutObject(UnionRegion, unionReg);
	//setDetectOutData(AllOk, allok);
	return m_allok;
}

void segLeadMod::viewResult(ImageView *iv, const QString &funName, int)
{
	if (p_im->empty())
		return;
	if (m_param->referSection&&p_homMat2d->size() != cv::Size(2, 3))
		return;

	bool FoundOK = true;
	for (int i = 0;i < m_param->leadNum;i++) {
		if (!m_foundok[i]) {
			FoundOK = false;
		}
	}
	if (!FoundOK || m_param->showAfterDet) {
		iv->setFilled(false);
		for (int i = 0;i < m_param->leadNum + 1;i++) {
			Hobject rec;
			gen_rectangle1(&rec, m_param->RoiVec[i].row1, m_param->RoiVec[i].col1, m_param->RoiVec[i].row2, m_param->RoiVec[i].col2);
			if (m_param->referSection) {
				affine_trans_region(rec, &rec, m_homMat2d, "false");
			}
			if (!FoundOK&&i == 0) {
				iv->setColor(Qt::red);
			}
			else if (i > 0 && !m_foundok[i - 1]) {
				iv->setColor(Qt::red);
			}
			else {
				iv->setColor(Qt::green);
			}
			char buf[32];
			if (i > 0) {
				sprintf(buf, "%d", i);
			}
			else {
				sprintf(buf, "");
			}
			RlRegion reg;
			HalOpenCV::hal2cvRegion(rec, &reg);
			iv->dispRegion(&reg, buf);
		}
	}
	if (!FoundOK) {
		return;
	}

	Hobject leadregion;
	iv->setFilled(false);
	iv->setColor(255, 255, 0);
	iv->dispRegion(&m_dispReg);

	if (m_param->IsReferEdge) {
		HTuple referrow, refercol;
		referrow = m_row;
		refercol = m_col;
		iv->setColor(0, 255, 255);
		if (m_param->HorOrVer == 0) {
			iv->dispLine(referrow[0].D(), refercol[0].D(), referrow[1].D(), refercol[1].D());
			iv->dispLine(referrow[2].D(), refercol[2].D(), referrow[3].D(), refercol[3].D());
		}
		else if (m_param->HorOrVer == 1) {
			iv->dispLine(referrow[0].D(), refercol[0].D(), referrow[3].D(), refercol[3].D());
			iv->dispLine(referrow[2].D(), refercol[2].D(), referrow[1].D(), refercol[1].D());
		}
	}
	for (int i = 0;i < m_param->leadNum;i++) {
		if (m_leadOk[i] == 0)
			iv->setColor(Qt::red);
		else
			iv->setColor(Qt::green);
		iv->dispRect2(m_leadRow[i], m_leadCol[i], m_leadAng[i], m_leadLen1[i], m_leadLen2[i]);
	}
// 	HTuple imagefull;
// 	getDispOutData(ImageFull, imagefull);
// 	if (!imagefull[0].I()) {
// 		return;
// 	}
// 	bool leadfoundOk[Max_LeadNum];
// 	HTuple leadok;
// 	bool FoundOK = true;
//
//m_view->setFilled(false);
//m_view->setColor(255,255,0);
//m_view->dispRegion(*getDispObject(DstImg));
//
// 	for (int i = 0;i < m_param->leadNum;i++) {
// 		getDispOutData(leadFoundOk, i, leadok);
// 		leadfoundOk[i] = leadok[0].I();
// 		if (!leadfoundOk[i]) {
// 			FoundOK = false;
// 		}
// 	}
// 	HTuple tmphom;
// 	if (m_param->referSection) {
// 		getDispOutData(TmpHomMat, tmphom);
// 	}
// 	if (!FoundOK || m_param->showAfterDet) {
// 		m_view->setFilled(false);
// 		for (int i = 0;i < m_param->leadNum + 1;i++) {
// 			Hobject rec;
// 			gen_rectangle1(&rec, m_param->RoiVec[i].row1, m_param->RoiVec[i].col1, m_param->RoiVec[i].row2, m_param->RoiVec[i].col2);
// 			if (m_param->referSection) {
// 				affine_trans_region(rec, &rec, tmphom, "false");
// 			}
// 			if (!FoundOK&&i == 0) {
// 				m_view->setColor(255, 0, 0);
// 			}
// 			else if (i > 0 && !leadfoundOk[i - 1]) {
// 				m_view->setColor(255, 0, 0);
// 			}
// 			else {
// 				m_view->setColor(0, 255, 0);
// 			}
// 			char buf[32];
// 			if (i > 0) {
// 				sprintf(buf, "%d", i);
// 			}
// 			else {
// 				sprintf(buf, "");
// 			}
// 			m_view->dispRegion(rec, buf);
// 		}
// 	}
// 	if (!FoundOK) {
// 		return;
// 	}
// 	Hobject leadregion;
// 	m_view->setFilled(false);
// 	m_view->setColor(255, 255, 0);
// 	m_view->dispRegion(*getDispObject(UnionRegion));
// 
// 	//Hobject leadregion1;
// 	//m_view->setFilled(true);
// 	//m_view->setColor(255,0,255);
// 	//m_view->dispRegion(*getDispObject(DstImg));
// 	if (m_param->IsReferEdge) {
// 		HTuple referrow, refercol;
// 		getDispOutData(TmpReferRow, referrow);
// 		getDispOutData(TmpReferCol, refercol);
// 		m_view->setColor(0, 255, 255);
// 		if (m_param->HorOrVer == 0) {
// 			m_view->dispLine(referrow[0].D(), refercol[0].D(), referrow[1].D(), refercol[1].D());
// 			m_view->dispLine(referrow[2].D(), refercol[2].D(), referrow[3].D(), refercol[3].D());
// 		}
// 		else if (m_param->HorOrVer == 1) {
// 			m_view->dispLine(referrow[0].D(), refercol[0].D(), referrow[3].D(), refercol[3].D());
// 			m_view->dispLine(referrow[2].D(), refercol[2].D(), referrow[1].D(), refercol[1].D());
// 		}
// 	}
// 	for (int i = 0;i < m_param->leadNum;i++) {
// 		getDispOutData(LeadOk, i, leadok);
// 		HTuple recrow, reccol, recangle, reclen1, reclen2;
// 		getDispOutData(LeadRow, i, recrow);
// 		getDispOutData(LeadCol, i, reccol);
// 		getDispOutData(LeadAngle, i, recangle);
// 		getDispOutData(LeadLen1, i, reclen1);
// 		getDispOutData(LeadLen2, i, reclen2);
// 		HTuple oklead;
// 		getDispOutData(LeadOk, i, oklead);
// 		if (oklead[0].I() == 0) {
// 			m_view->setColor(255, 0, 0);
// 		}
// 		else {
// 			m_view->setColor(0, 255, 0);
// 		}
// 		m_view->dispRect2(recrow[0].D(), reccol[0].D(), recangle[0].D(), reclen1[0].D(), reclen2[0].D());
// 	}
}

void segLeadMod::textResult(ResultText *text, const QString &funName)
{
	if (p_im->empty())
	{
		text->setTextColor(Qt::red);
		text->append(QObject::tr("Empty image!"));
		return;
	}
	if (m_param->referSection&&p_homMat2d->size() != cv::Size(2, 3))
	{
		text->setTextColor(Qt::red);
		text->append(QObject::tr("Input matrix's size !=6"));
		return;
	}
	bool FoundOK = true;
	for (int i = 0;i < m_param->leadNum;i++) {
		if (!m_foundok[i]) {
			FoundOK = false;
			text->setTextColor(Qt::red);
			text->append(QObject::tr("lead %1 found failed").arg(i + 1));
		}
	}
	if (!FoundOK) {
		return;
	}

	for (int i = 0;i < m_param->leadNum;i++) {
		if (m_leadOk[i] == 0)
			text->setTextColor(Qt::red);
		else
			text->setTextColor(Qt::black);
		text->append(QObject::tr("Lead%1:").arg(i + 1));

		if (m_param->errWidthValid) {
			if (m_leadWidOk[i] == 0)
				text->setTextColor(Qt::red);
			else
				text->setTextColor(Qt::black);
			text->append(QObject::tr(" Width:%1(%2,%3)")
				.arg(QString::number(m_leadWid[i], 'g', 6))
				.arg(QString::number(m_param->errWidthMin[i]))
				.arg(QString::number(m_param->errWidthMax[i])));
		}
		if (m_param->errLengthValid) {
			if (m_leadLenOk[i] == 0)
				text->setTextColor(Qt::red);
			else
				text->setTextColor(Qt::black);
			text->append(QObject::tr(" Length:%1(%2,%3)")
				.arg(QString::number(m_leadLen[i], 'g', 6))
				.arg(QString::number(m_param->errLengthMin[i]))
				.arg(QString::number(m_param->errLengthMax[i])));
		}
		if (m_param->errInterValid&&i < m_param->leadNum - 1) {
			if (m_leadInterOk[i] == 0)
				text->setTextColor(Qt::red);
			else
				text->setTextColor(Qt::black);
			text->append(QObject::tr(" Interval:%1(%2,%3)")
				.arg(QString::number(m_leadInter[i], 'g', 6))
				.arg(QString::number(m_param->errInterMin[i]))
				.arg(QString::number(m_param->errInterMax[i])));
		}
	}

// 	HTuple imagefull;
// 	getDispOutData(ImageFull, imagefull);
// 	if (!imagefull[0].I()) {
// 		return;
// 	}
// 	HTuple leadok;
// 	bool FoundOK = true;
// 	for (int i = 0;i < m_param->leadNum;i++) {
// 		getDispOutData(leadFoundOk, i, leadok);
// 		if (!leadok[0].I()) {
// 			FoundOK = false;
// 			m_text->setTextColor(QColor(255, 0, 0));
// 			m_text->append(tr("lead %1 found failed").arg(i + 1));
// 		}
// 	}
// 	if (!FoundOK) {
// 		return;
// 	}
//
// 	for (int i = 0;i < m_param->leadNum;i++) {
// 		getDispOutData(LeadOk, i, leadok);
// 		if (leadok[0].I() == 0) {
// 			m_text->setTextColor(QColor(255, 0, 0));
// 		}
// 		else {
// 			m_text->setTextColor(QColor(0, 0, 0));
// 		}
// 		m_text->append(tr("Lead%1:").arg(i + 1));
// 		if (m_param->errWidthValid) {
// 			HTuple widthOk, width;
// 			getDispOutData(LeadWidthOk, i, widthOk);
// 			getDispOutData(LeadWidth, i, width);
// 			if (widthOk[0].I() == 0) {
// 				m_text->setTextColor(QColor(255, 0, 0));
// 			}
// 			else {
// 				m_text->setTextColor(QColor(0, 0, 0));
// 			}
// 			m_text->append(tr(" Width:%1(%2,%3)").arg(QString::number(width[0].D(), 'g', 6))
// 				.arg(QString::number(m_param->errWidthMin[i])).arg(QString::number(m_param->errWidthMax[i])));
// 		}
// 		if (m_param->errLengthValid) {
// 			HTuple lengthOk, length;
// 			getDispOutData(LeadLengthOk, i, lengthOk);
// 			getDispOutData(LeadLength, i, length);
// 			if (lengthOk[0].I() == 0) {
// 				m_text->setTextColor(QColor(255, 0, 0));
// 			}
// 			else {
// 				m_text->setTextColor(QColor(0, 0, 0));
// 			}
// 			m_text->append(tr(" Length:%1(%2,%3)").arg(QString::number(length[0].D(), 'g', 6))
// 				.arg(QString::number(m_param->errLengthMin[i])).arg(QString::number(m_param->errLengthMax[i])));
// 		}
// 		if (m_param->errInterValid&&i < m_param->leadNum - 1) {
// 			HTuple interOk, inter;
// 			getDispOutData(LeadInterOk, i, interOk);
// 			getDispOutData(LeadInter, i, inter);
// 			if (interOk[0].I() == 0) {
// 				m_text->setTextColor(QColor(255, 0, 0));
// 			}
// 			else {
// 				m_text->setTextColor(QColor(0, 0, 0));
// 			}
// 			m_text->append(tr(" Interval:%1(%2,%3)").arg(QString::number(inter[0].D(), 'g', 6))
// 				.arg(QString::number(m_param->errInterMin[i])).arg(QString::number(m_param->errInterMax[i])));
// 		}
// 	}
}

void segLeadMod::iniData()
{
	m_param = new segLeadParam;
	m_image = new Hobject;
	m_allok = 0;
	for (auto i = 0;i < Max_LeadNum;++i)
	{
		m_foundok[i] = true;

		m_leadWid[i] = 0;
		m_leadWidOk[i] = true;

		m_leadLen[i] = 0;
		m_leadLenOk[i] = true;

		m_leadRow[i] = 100;
		m_leadCol[i] = 100;
		m_leadAng[i] = 0;
		m_leadLen1[i] = 50;
		m_leadLen2[i] = 50;
		m_leadOk[i] = true;

		if (i < Max_LeadNum - 1)
		{
			m_leadInter[i] = 0;
			m_leadInterOk[i] = true;
		}
	}
	createPins();
}

void segLeadMod::createPins()
{
	addPin(&p_im, "im");
	addPin(&p_homMat2d, "hm2d");
	addPin(&p_row, "row");
	addPin(&p_col, "col");
	addPin(&p_pixelSize, "sz");

	p_row.setVisible(false);
	p_col.setVisible(false);
}

