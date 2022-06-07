#include "segLeadWdg.h"
#include "segLeadMod.h"
#include "segLeadParam.h"
#include "ui_segLeadWdg.h"
#include "ImageView.h"
#include <QMessageBox>
#include "halOpenCV.h"
#include <opencv2/imgproc.hpp>

using namespace qzcv;


segLeadWdg::segLeadWdg(int i) :
	ModSetWidget(),ui(new Ui::segLeadWdg),m_widgetType(i)
{
	m_hasConnect = false;
	ui->setupUi(this);
	iniUi();
	m_layerIdx = 0;
	m_param = nullptr;
	m_module = nullptr;
	m_image = nullptr;
	m_leadIndex = 0;
	m_testing = 0;
}

segLeadWdg::~segLeadWdg()
{
	delete ui;
}

void segLeadWdg::setModule(UnitModule *module)
{
	m_module = dynamic_cast<segLeadMod*>(module);
	m_param = m_module->m_param;
	m_image = m_module->m_image;
	//currentView()->setLayer(m_layerIdx);
	//updateUi();
}

void segLeadWdg::showEvent(QShowEvent * event)
{
	updateUi();
}

void segLeadWdg::connectSlots(bool link)
{
	if(m_hasConnect==link)
		return;
	m_hasConnect=link;

	connectOrNot(link, ui->cb_useRefer, SIGNAL(toggled(bool)), SLOT(check_box_toggled(bool)));
	connectOrNot(link, ui->cb_edgeRefer, SIGNAL(toggled(bool)), SLOT(check_box_toggled(bool)));
	connectOrNot(link, ui->cb_inputPixel, SIGNAL(toggled(bool)), SLOT(check_box_toggled(bool)));
	connectOrNot(link, ui->cb_showAfterDet, SIGNAL(toggled(bool)), SLOT(check_box_toggled(bool)));
	connectOrNot(link, ui->cb_errWidth, SIGNAL(toggled(bool)), SLOT(check_box_toggled(bool)));
	connectOrNot(link, ui->cb_errLength, SIGNAL(toggled(bool)), SLOT(check_box_toggled(bool)));
	connectOrNot(link, ui->cb_errInter, SIGNAL(toggled(bool)), SLOT(check_box_toggled(bool)));
	connectOrNot(link, ui->cb_errEdgeDis, SIGNAL(toggled(bool)), SLOT(check_box_toggled(bool)));

	connectOrNot(link, ui->rb_white, SIGNAL(toggled(bool)), SLOT(radioButton_toggled(bool)));
	connectOrNot(link, ui->rb_black, SIGNAL(toggled(bool)), SLOT(radioButton_toggled(bool)));
	connectOrNot(link, ui->rb_hor, SIGNAL(toggled(bool)), SLOT(radioButton_toggled(bool)));
	connectOrNot(link, ui->rb_ver, SIGNAL(toggled(bool)), SLOT(radioButton_toggled(bool)));
	connectOrNot(link, ui->rb_roiCenter, SIGNAL(toggled(bool)), SLOT(radioButton_toggled(bool)));
	connectOrNot(link, ui->rb_setCenter, SIGNAL(toggled(bool)), SLOT(radioButton_toggled(bool)));
	connectOrNot(link, ui->rb_autoBlack, SIGNAL(toggled(bool)), SLOT(radioButton_toggled(bool)));
	connectOrNot(link, ui->rb_autoWhite, SIGNAL(toggled(bool)), SLOT(radioButton_toggled(bool)));

	connectOrNot(link, ui->sp_leadNum, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_AreaMin, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_AreaMax, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_openWidth, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_openHeight, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_closeWidth, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_closeHeight, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_centerX, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_centerY, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_rowWid, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_colWid, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_tol, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_minGrowArea, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_meanHeight, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_meanWidth, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_grayWhite, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_grayBlack, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_threMin, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_threMax, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_threshold, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_pinSize, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));

	connectOrNot(link, ui->dsp_pixSize, SIGNAL(valueChanged(double)), SLOT(doubleSpinBox_valueChanged(double)));
	connectOrNot(link, ui->dsp_errWidthMin, SIGNAL(valueChanged(double)), SLOT(doubleSpinBox_valueChanged(double)));
	connectOrNot(link, ui->dsp_errWidthMax, SIGNAL(valueChanged(double)), SLOT(doubleSpinBox_valueChanged(double)));
	connectOrNot(link, ui->dsp_errLengthMin, SIGNAL(valueChanged(double)), SLOT(doubleSpinBox_valueChanged(double)));
	connectOrNot(link, ui->dsp_errLengthMax, SIGNAL(valueChanged(double)), SLOT(doubleSpinBox_valueChanged(double)));
	connectOrNot(link, ui->dsp_errIntervalMin, SIGNAL(valueChanged(double)), SLOT(doubleSpinBox_valueChanged(double)));
	connectOrNot(link, ui->dsp_errIntervalMax, SIGNAL(valueChanged(double)), SLOT(doubleSpinBox_valueChanged(double)));
	connectOrNot(link, ui->dsp_errEdgeDisMin, SIGNAL(valueChanged(double)), SLOT(doubleSpinBox_valueChanged(double)));
	connectOrNot(link, ui->dsp_errEdgeDisMax, SIGNAL(valueChanged(double)), SLOT(doubleSpinBox_valueChanged(double)));

	connectOrNot(link, ui->cob_lead, SIGNAL(currentIndexChanged(int)), SLOT(comboBox_Index_Changed(int)));
	connectOrNot(link, ui->cob_testType, SIGNAL(currentIndexChanged(int)), SLOT(comboBox_Index_Changed(int)));

	connectOrNot(link, ui->hs_threshold, SIGNAL(valueChanged(int)), SLOT(slider_valueChanged(int)));
}

void segLeadWdg::iniUi()
{

}

void segLeadWdg::setUiValue()
{
	if(!m_param)
		return;

	ui->cb_useRefer->setChecked(m_param->referSection);
	ui->cb_edgeRefer->setChecked(m_param->IsReferEdge);
	ui->cb_inputPixel->setChecked(m_param->IsInputPixel);
	ui->dsp_pixSize->setValue(m_param->pixSize);
	ui->cb_showAfterDet->setChecked(m_param->showAfterDet);
	//ui.cb_ImgGauss->setChecked(m_param->IsGauss);
	//ui.sp_gaussSize->setEnabled(m_param->IsGauss);

	ui->cb_errWidth->setChecked(m_param->errWidthValid);
	ui->dsp_errWidthMin->setEnabled(m_param->errWidthValid);
	ui->dsp_errWidthMax->setEnabled(m_param->errWidthValid);
	ui->cb_errLength->setChecked(m_param->errLengthValid);
	ui->dsp_errLengthMin->setEnabled(m_param->errLengthValid);
	ui->dsp_errLengthMax->setEnabled(m_param->errLengthValid);
	ui->cb_errInter->setChecked(m_param->errInterValid);
	ui->dsp_errIntervalMin->setEnabled(m_param->errInterValid);
	ui->dsp_errIntervalMax->setEnabled(m_param->errInterValid);
	ui->cb_errEdgeDis->setChecked(m_param->errEdgeDisValid);
	ui->dsp_errEdgeDisMin->setEnabled(m_param->errEdgeDisValid);
	ui->dsp_errEdgeDisMax->setEnabled(m_param->errEdgeDisValid);

	ui->sp_leadNum->setValue(m_param->leadNum);
	ui->sp_AreaMin->setValue(m_param->areaMin);
	ui->sp_AreaMax->setValue(m_param->areaMax);
	ui->sp_openWidth->setValue(m_param->openWidth);
	ui->sp_openHeight->setValue(m_param->openHeight);
	ui->sp_closeWidth->setValue(m_param->closeWidth);
	ui->sp_closeHeight->setValue(m_param->closeHeight);
	ui->sp_meanWidth->setValue(m_param->meanWidth);
	ui->sp_meanHeight->setValue(m_param->meanHeight);
	ui->sp_grayWhite->setValue(m_param->grayWhite);
	ui->sp_grayBlack->setValue(m_param->grayBlack);

	ui->sp_threMin->setValue(m_param->threMin);
	ui->sp_threMax->setValue(m_param->threMax);
	ui->sp_threshold->setValue(m_param->autoLeadThre);
	ui->hs_threshold->setValue(m_param->autoLeadThre);
	ui->sp_pinSize->setValue(m_param->autoLeadSize);

	ui->sp_centerX->setValue(m_param->centerX);
	ui->sp_centerY->setValue(m_param->centerY);
	ui->sp_rowWid->setValue(m_param->rowWid);
	ui->sp_colWid->setValue(m_param->colWid);
	ui->sp_tol->setValue(m_param->thresholdTol);
	ui->sp_minGrowArea->setValue(m_param->minGrowArea);
	//ui.sp_gaussSize->setValue(m_param->GaussSize);
	if (m_param->WOrB == 0) {
		ui->rb_white->setChecked(1);
	}
	else if (m_param->WOrB == 1) {
		ui->rb_black->setChecked(1);
	}

	if (m_param->HorOrVer == 0) {
		ui->rb_hor->setChecked(1);
	}
	else if (m_param->HorOrVer == 1) {
		ui->rb_ver->setChecked(1);
	}
	if (m_param->centerType == 0) {
		ui->rb_roiCenter->setChecked(1);
	}
	else if (m_param->centerType == 1) {
		ui->rb_setCenter->setChecked(1);
	}
	ui->stackedWidget->setCurrentIndex(m_param->testType);

	if (m_param->IsAutoWhite == 0) {
		ui->rb_autoBlack->setChecked(1);
	}
	else if (m_param->IsAutoWhite == 1) {
		ui->rb_autoWhite->setChecked(1);
	}
	ui->stackedWidget_lead->setCurrentIndex(0);
	ui->bt_autoLead->setChecked(false);
	ui->cob_lead->clear();
	for (int i = 0;i < m_param->leadNum;i++) {
		ui->cob_lead->addItem(tr("lead%1").arg(i + 1));
	}
	ui->cob_lead->setCurrentIndex(m_leadIndex);
	ui->cob_testType->setCurrentIndex(m_param->testType);

	changeErrLead();
}

void segLeadWdg::updateUi()
{
	connectSlots(false);
	setUiValue();
	connectSlots(true);
}

void segLeadWdg::drawRect()
{
	double row1[1 + Max_LeadNum], col1[1 + Max_LeadNum], row2[1 + Max_LeadNum], col2[1 + Max_LeadNum];
	QStringList title = { tr("SearchRoi") };
	for (int i = 0;i < 1 + m_param->leadNum;i++) {
		row1[i] = m_param->RoiVec[i].row1;
		col1[i] = m_param->RoiVec[i].col1;
		row2[i] = m_param->RoiVec[i].row2;
		col2[i] = m_param->RoiVec[i].col2;
		if (i > 0)
			title << tr("lead%1").arg(i);
	}
	currentView()->finishDraw();
	//currentView()->clear();
	currentView()->clearLayer(m_layerIdx);
	currentView()->setDrawColor(0, 255, 0);
	currentView()->drawRects1(1 + m_param->leadNum, row1, col1, row2, col2, title);
	currentView()->update();
}

void segLeadWdg::changeErrLead()
{
	ui->dsp_errWidthMin->setValue(m_param->errWidthMin[m_leadIndex]);
	ui->dsp_errWidthMax->setValue(m_param->errWidthMax[m_leadIndex]);
	ui->dsp_errLengthMin->setValue(m_param->errLengthMin[m_leadIndex]);
	ui->dsp_errLengthMax->setValue(m_param->errLengthMax[m_leadIndex]);
	ui->dsp_errIntervalMin->setValue(m_param->errInterMin[m_leadIndex]);
	ui->dsp_errIntervalMax->setValue(m_param->errInterMax[m_leadIndex]);
}

void segLeadWdg::autoThreshold()
{
	HalOpenCV::cv2halImg(*m_module->p_im, m_image, false);

	Hobject threRect, threRectRt, tmpImage, tmpImageRt;
	gen_rectangle1(&threRect, 
		m_param->RoiVec[0].row1, m_param->RoiVec[0].col1,
		m_param->RoiVec[0].row2, m_param->RoiVec[0].col2);
	currentView()->finishDraw();
	currentView()->setColor(0, 0, 0);
	currentView()->setFilled(true);
	currentView()->dispRect1(
		m_param->RoiVec[0].row1, m_param->RoiVec[0].col1,
		m_param->RoiVec[0].row2, m_param->RoiVec[0].col2);  //把背景涂黑
	reduce_domain(*m_image, threRect, &tmpImage);
	if (m_param->IsAutoWhite) {
		threshold(tmpImage, &tmpImage, m_param->autoLeadThre, 255);
	}
	else {
		threshold(tmpImage, &tmpImage, 0, m_param->autoLeadThre);
	}
	currentView()->setColor(255, 255, 255);
	Hlong num;
	count_obj(tmpImage, &num);

	connection(tmpImage, &tmpImage);
	select_shape(tmpImage, &tmpImage, "area", "and", m_param->areaMin, m_param->areaMax < m_param->areaMin ? m_param->areaMin : m_param->areaMax);
	Hlong regionCount;
	count_obj(tmpImage, &regionCount);
	if (regionCount > 0) {
		RlRegion reg;
		union1(tmpImage, &tmpImage);
		HalOpenCV::hal2cvRegion(tmpImage, &reg);
		currentView()->dispRegion(&reg);
		connection(tmpImage, &tmpImage);
	}
	if (regionCount > Max_LeadNum) {
		currentView()->update();
		return;
	}
	else {
		m_param->leadNum = regionCount;
	}
	HTuple lfRow1, lfCol1, lfRow2, lfCol2;
	smallest_rectangle1(tmpImage, &lfRow1, &lfCol1, &lfRow2, &lfCol2);
	HTuple rowsort, sortindex;
	//tuple_sort(lfRow1,&rowsort);
	if (m_param->HorOrVer == 1) {    //上下排布
		tuple_sort_index(lfRow1, &sortindex);
	}
	else {
		tuple_sort_index(lfCol1, &sortindex);
	}
	HTuple mrow1, mrow2, mcol1, mcol2;
	for (int e = 0;e < regionCount;e++) {
		mrow1[e] = lfRow1[sortindex[e].I()].D();
		mrow2[e] = lfRow2[sortindex[e].I()].D();
		mcol1[e] = lfCol1[sortindex[e].I()].D();
		mcol2[e] = lfCol2[sortindex[e].I()].D();
	}
	vector<segLeadParam::roiRegion>::iterator itbegin = m_param->RoiVec.begin() + 1;
	vector<segLeadParam::roiRegion>::iterator itEnd = m_param->RoiVec.end();
	m_param->RoiVec.erase(itbegin, itEnd);

	segLeadParam::roiRegion seg;
	for (int i = 0;i < m_param->leadNum;i++)
	{
		seg.row1 = mrow1[i].D() - m_param->autoLeadSize / 3;
		seg.row2 = mrow2[i].D() + m_param->autoLeadSize / 3;
		seg.col1 = mcol1[i].D() - m_param->autoLeadSize / 3;
		seg.col2 = mcol2[i].D() + m_param->autoLeadSize / 3;
		m_param->RoiVec.push_back(seg);
	}

	currentView()->setColor(255, 255, 0);
	currentView()->setFilled(false);
	currentView()->dispRect1(
		m_param->RoiVec[0].row1, m_param->RoiVec[0].col1,
		m_param->RoiVec[0].row2, m_param->RoiVec[0].col2, "roi");
	int DrawCount = m_param->leadNum;
	double DrawRow1[Max_LeadNum], DrawRow2[Max_LeadNum], DrawCol1[Max_LeadNum], DrawCol2[Max_LeadNum];
	QStringList title;
	for (int i = 0;i < DrawCount;i++) {
		DrawRow1[i] = m_param->RoiVec[i + 1].row1;
		DrawCol1[i] = m_param->RoiVec[i + 1].col1;
		DrawRow2[i] = m_param->RoiVec[i + 1].row2;
		DrawCol2[i] = m_param->RoiVec[i + 1].col2;
		title << QString("%1").arg(i + 1);
	}
	currentView()->setColor(255, 60, 255);
	currentView()->drawRects1(DrawCount, DrawRow1, DrawCol1, DrawRow2, DrawCol2, title);
	currentView()->setFilled(false);
	currentView()->setColor(255, 255, 0);
	currentView()->update();

	connectSlots(false);
	ui->sp_leadNum->setValue(m_param->leadNum);
	connectSlots(true);
}

void segLeadWdg::testVal()
{
	//currentView()->clear();
	currentView()->clearLayer(m_layerIdx);
	currentView()->setFilled(true);
	if (m_module->p_im->empty())
		return;

	HTuple homMat2d;
	if (m_param->referSection&&m_module->p_homMat2d->size() == cv::Size(3, 2))
		HalOpenCV::cv2HalHomMat2D(*m_module->p_homMat2d, &homMat2d);
	else
		hom_mat2d_identity(&homMat2d);

	if (m_module->p_im->channels() == 3)
	{
		cv::Mat im;
		cv::cvtColor(**m_module->p_im, im, CV_BGR2GRAY);
		HalOpenCV::cv2halImg(im, m_image, true);
	}
	else
		HalOpenCV::cv2halImg(*m_module->p_im, m_image, false);

	Hobject dstImg;
	if (m_param->IsGauss) {
		int vaaa = (m_param->GaussSize % 2) == 0 ? 1 : 0;
		gauss_image(*m_image, &dstImg, vaaa + m_param->GaussSize);
	}
	else {
		copy_image(*m_image, &dstImg);
	}
//	if (m_param->referSection) {
// 		try {
// 			double tv = (**m_module->m_homMat2d)[0].D();
// 			homMat2d = *m_module->m_homMat2d;
// 		}
// 		catch (HException& ex) {
// 			hom_mat2d_identity(&homMat2d);
// 		}
//	}
	HTuple imgheight, imgwid;
	get_image_size(*m_image, &imgwid, &imgheight);
	Hobject recroi;
	gen_rectangle1(&recroi, m_param->RoiVec[0].row1, m_param->RoiVec[0].col1, m_param->RoiVec[0].row2, m_param->RoiVec[0].col2);

	if (m_param->testType == 0) {  //平滑
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
			affine_trans_region(recroi, &recroi, homMat2d, "false");
		}
		reduce_domain(dstImg, recroi, &dstImg);
		if (m_param->closeWidth > 0 && m_param->closeHeight > 0) {
			closing_rectangle1(dstImg, &dstImg, m_param->closeWidth, m_param->closeHeight);
		}
		if (m_param->openWidth > 0 && m_param->openHeight > 0) {
			opening_rectangle1(dstImg, &dstImg, m_param->openWidth, m_param->openHeight);
		}
	}
	else if (m_param->testType == 1) { //regiongrowing
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
			affine_trans_pixel(homMat2d, mrow, mcol, &mrow, &mcol);
			mcenterx = mcol[0].I();
			mcentery = mrow[0].I();
			affine_trans_region(recroi, &recroi, homMat2d, "false");
		}
		//if (m_param->IsReferEdge){
		//	gen_region_points(&recroi,fianalrow,fianalcol);
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
		//difference(recroi,dstImg,&dstImg);
		//union1(dstImg,&dstImg);
	}
	else if (m_param->testType == 2) {
		threshold(dstImg, &dstImg, m_param->threMin, m_param->threMax < m_param->threMin ? m_param->threMin : m_param->threMax);
		if (m_param->referSection == 1) {
			affine_trans_region(recroi, &recroi, homMat2d, "false");
		}
		reduce_domain(dstImg, recroi, &dstImg);
		if (m_param->closeWidth > 0 && m_param->closeHeight > 0) {
			closing_rectangle1(dstImg, &dstImg, m_param->closeWidth, m_param->closeHeight);
		}
		if (m_param->openWidth > 0 && m_param->openHeight > 0) {
			opening_rectangle1(dstImg, &dstImg, m_param->openWidth, m_param->openHeight);
		}
	}

	currentView()->setColor(0, 255, 0);
	for (int i = 0;i < m_param->leadNum;i++) {
		Hobject leadRegion;
		gen_rectangle1(&leadRegion, m_param->RoiVec[i + 1].row1, m_param->RoiVec[i + 1].col1,
			m_param->RoiVec[i + 1].row2, m_param->RoiVec[i + 1].col2);
		reduce_domain(dstImg, leadRegion, &leadRegion);
		connection(leadRegion, &leadRegion);
		select_shape(leadRegion, &leadRegion, "area", "and", m_param->areaMin, m_param->areaMax < m_param->areaMin ? m_param->areaMin : m_param->areaMax);
		union1(leadRegion, &leadRegion);
		Hlong num;
		count_obj(leadRegion, &num);
		if (num != 0) {
			RlRegion reg;
			HalOpenCV::hal2cvRegion(leadRegion, &reg);
			currentView()->dispRegion(&reg);
		}
	}
	currentView()->update();
}

void segLeadWdg::check_box_toggled(bool check)
{
	QCheckBox* cb = qobject_cast<QCheckBox*>(sender());
	QString str0 = cb->text();
	QString str1 = QString("");
	if (check)
		str1 = tr(" Check");
	else
		str1 = tr(" Uncheck");
	if (cb == ui->cb_useRefer) {
		m_param->referSection = check;
	}
	else if (cb == ui->cb_edgeRefer) {
		m_param->IsReferEdge = check;
	}
	else if (cb == ui->cb_inputPixel) {
		m_param->IsInputPixel = check;
	}
	else if (cb == ui->cb_showAfterDet) {
		m_param->showAfterDet = check;
	}
	//else if(cb==ui.cb_ImgGauss){
	//	ui.sp_gaussSize->setEnabled(check);
	//	m_param->IsGauss=check;
	//	if (m_testing){
	//		testVal();
	//	}
	//}
	else if (cb == ui->cb_errWidth) {
		ui->dsp_errWidthMin->setEnabled(check);
		ui->dsp_errWidthMax->setEnabled(check);
		m_param->errWidthValid = check;
	}
	else if (cb == ui->cb_errLength) {
		ui->dsp_errLengthMin->setEnabled(check);
		ui->dsp_errLengthMax->setEnabled(check);
		m_param->errLengthValid = check;
	}
	else if (cb == ui->cb_errInter) {
		ui->dsp_errIntervalMin->setEnabled(check);
		ui->dsp_errIntervalMax->setEnabled(check);
		m_param->errInterValid = check;
	}
	else if (cb == ui->cb_errEdgeDis) {
		ui->dsp_errEdgeDisMin->setEnabled(check);
		ui->dsp_errEdgeDisMax->setEnabled(check);
		m_param->errEdgeDisValid = check;
	}
	m_module->setParamChanged(ProductLevel);
	//log(LogL1, str0 + str1);
}

void segLeadWdg::spinBox_valueChanged(int val)
{
	QSpinBox* sp = qobject_cast<QSpinBox*>(sender());
	QString str0 = QString("");
	QString str1 = QString("");
	if (sp == ui->sp_AreaMin) {
		str0 = ui->label_17->text() + tr(" min val");
		str1 = tr(" change from %1 to %2").arg(m_param->areaMin).arg(val);
		m_param->areaMin = val;
	}
	else if (sp == ui->sp_AreaMax) {
		str0 = ui->label_17->text() + tr(" max val");
		str1 = tr(" change from %1 to %2").arg(m_param->areaMax).arg(val);
		m_param->areaMax = val;
	}
	else if (sp == ui->sp_openWidth) {
		str0 = ui->label_21->text() + ui->label_15->text();
		str1 = tr(" change from %1 to %2").arg(m_param->openWidth).arg(val);
		m_param->openWidth = val;
	}
	else if (sp == ui->sp_openHeight) {
		str0 = ui->label_21->text() + ui->label_18->text();
		str1 = tr(" change from %1 to %2").arg(m_param->openHeight).arg(val);
		m_param->openHeight = val;
	}
	else if (sp == ui->sp_closeWidth) {
		str0 = ui->label_19->text() + ui->label_20->text();
		str1 = tr(" change from %1 to %2").arg(m_param->closeWidth).arg(val);
		m_param->closeWidth = val;
	}
	else if (sp == ui->sp_closeHeight) {
		str0 = ui->label_21->text() + ui->label_16->text();
		str1 = tr(" change from %1 to %2").arg(m_param->openHeight).arg(val);
		m_param->closeHeight = val;

	}
	else if (sp == ui->sp_meanWidth) {
		str0 = ui->label_3->text() + ui->label_2->text();
		str1 = tr(" change from %1 to %2").arg(m_param->meanWidth).arg(val);
		m_param->meanWidth = val;
	}
	else if (sp == ui->sp_meanHeight) {
		str0 = ui->label_3->text() + ui->label_23->text();
		str1 = tr(" change from %1 to %2").arg(m_param->meanHeight).arg(val);
		m_param->meanHeight = val;
	}
	else if (sp == ui->sp_grayWhite) {
		str0 = ui->label_5->text() + ui->label_22->text();
		str1 = tr(" change from %1 to %2").arg(m_param->grayWhite).arg(val);
		m_param->grayWhite = val;
	}
	else if (sp == ui->sp_grayBlack) {
		str0 = ui->label_5->text() + ui->label_24->text();
		str1 = tr(" change from %1 to %2").arg(m_param->grayBlack).arg(val);
		m_param->grayBlack = val;
	}
	else if (sp == ui->sp_threMin) {
		str0 = ui->label_8->text() + tr(" min val");
		str1 = tr(" change from %1 to %2").arg(m_param->threMin).arg(val);
		m_param->threMin = val;
	}
	else if (sp == ui->sp_threMax) {
		str0 = ui->label_8->text() + tr(" max val");
		str1 = tr(" change from %1 to %2").arg(m_param->threMax).arg(val);
		m_param->threMax = val;
	}
	else if (sp == ui->sp_centerX) {
		str0 = ui->label_13->text();
		str1 = tr(" change from %1 to %2").arg(m_param->centerX).arg(val);
		m_param->centerX = val;
	}
	else if (sp == ui->sp_centerY) {
		str0 = ui->label_14->text();
		str1 = tr(" change from %1 to %2").arg(m_param->centerY).arg(val);
		m_param->centerY = val;
	}
	else if (sp == ui->sp_rowWid) {
		str0 = ui->label_9->text();
		str1 = tr(" change from %1 to %2").arg(m_param->rowWid).arg(val);
		m_param->rowWid = val;
	}
	else if (sp == ui->sp_colWid) {
		str0 = ui->label_10->text();
		str1 = tr(" change from %1 to %2").arg(m_param->colWid).arg(val);
		m_param->colWid = val;
	}
	else if (sp == ui->sp_tol) {
		str0 = ui->label_11->text();
		str1 = tr(" change from %1 to %2").arg(m_param->thresholdTol).arg(val);
		m_param->thresholdTol = val;
	}
	else if (sp == ui->sp_minGrowArea) {
		str0 = ui->label_12->text();
		str1 = tr(" change from %1 to %2").arg(m_param->minGrowArea).arg(val);
		m_param->minGrowArea = val;
	}
	else if (sp == ui->sp_pinSize) {
		str0 = ui->label_25->text();
		str1 = tr(" change from %1 to %2").arg(m_param->autoLeadSize).arg(val);
		m_param->autoLeadSize = val;
	}
	else if (sp == ui->sp_leadNum) {
		segLeadParam::roiRegion seg;
		seg.row1 = seg.col1 = 125;
		seg.col2 = seg.row2 = 225;  //给新添加的roi固定的开始位1置
		if (val >= m_param->leadNum) {
			for (int i = m_param->leadNum;i < val;i++) {
				seg.row1 = seg.col1 += 10;
				seg.row2 = seg.col2 += 10;
				m_param->RoiVec.push_back(seg);
			}
		}
		else if (val < m_param->leadNum) {
			vector<segLeadParam::roiRegion>::iterator itbegin = m_param->RoiVec.begin() + val + 1;
			vector<segLeadParam::roiRegion>::iterator itEnd = m_param->RoiVec.end();
			m_param->RoiVec.erase(itbegin, itEnd);
		}
		str0 = ui->label_4->text();
		str1 = tr(" change from %1 to %2").arg(m_param->leadNum).arg(val);
		m_param->leadNum = val;
		connectSlots(false);
		ui->cob_lead->clear();
		for (int i = 0;i < m_param->leadNum;i++) {
			ui->cob_lead->addItem(tr("lead%1").arg(i + 1));
		}
		m_leadIndex = 0;
		ui->cob_lead->setCurrentIndex(m_leadIndex);
		connectSlots(true);
		if (ui->bt_selectRoi->isChecked()) {
			drawRect();
		}
	}
	else if (sp == ui->sp_threshold) {
		str0 = ui->label_26->text();
		str1 = tr(" change from %1 to %2").arg(m_param->autoLeadThre).arg(val);
		m_param->autoLeadThre = val;
		ui->hs_threshold->setValue(val);
	}
	if (m_testing) {
		testVal();
	}
	m_module->setParamChanged(ProductLevel);
	//log(LogL1, str0 + str1);
}

void segLeadWdg::doubleSpinBox_valueChanged(double val)
{
	QDoubleSpinBox* dsp = qobject_cast<QDoubleSpinBox*>(sender());
	QString str0 = QString("");
	QString str1 = QString("");
	if (dsp == ui->dsp_errWidthMin) {
		str0 = ui->cob_lead->currentText() + QString(" ") + ui->cb_errWidth->text() + tr(" min val");
		str1 = tr(" change from %1 to %2")
			.arg(QString::number(m_param->errWidthMin[m_leadIndex], 'f', 3))
			.arg(QString::number(val, 'f', 3));
		m_param->errWidthMin[m_leadIndex] = val;
	}
	else if (dsp == ui->dsp_errWidthMax) {
		str0 = ui->cob_lead->currentText() + QString(" ") + ui->cb_errWidth->text() + tr(" max val");
		str1 = tr(" change from %1 to %2")
			.arg(QString::number(m_param->errWidthMax[m_leadIndex], 'f', 3))
			.arg(QString::number(val, 'f', 3));
		m_param->errWidthMax[m_leadIndex] = val;
	}
	else if (dsp == ui->dsp_errLengthMin) {
		str0 = ui->cob_lead->currentText() + QString(" ") + ui->cb_errLength->text() + tr(" min val");
		str1 = tr(" change from %1 to %2")
			.arg(QString::number(m_param->errLengthMin[m_leadIndex], 'f', 3))
			.arg(QString::number(val, 'f', 3));
		m_param->errLengthMin[m_leadIndex] = val;
	}
	else if (dsp == ui->dsp_errLengthMax) {
		str0 = ui->cob_lead->currentText() + QString(" ") + ui->cb_errLength->text() + tr(" max val");
		str1 = tr(" change from %1 to %2")
			.arg(QString::number(m_param->errLengthMax[m_leadIndex], 'f', 3))
			.arg(QString::number(val, 'f', 3));
		m_param->errLengthMax[m_leadIndex] = val;
	}
	else if (dsp == ui->dsp_errIntervalMin) {
		str0 = ui->cob_lead->currentText() + QString(" ") + ui->cb_errInter->text() + tr(" min val");
		str1 = tr(" change from %1 to %2")
			.arg(QString::number(m_param->errInterMin[m_leadIndex], 'f', 3))
			.arg(QString::number(val, 'f', 3));
		m_param->errInterMin[m_leadIndex] = val;
	}
	else if (dsp == ui->dsp_errIntervalMax) {
		str0 = ui->cob_lead->currentText() + QString(" ") + ui->cb_errInter->text() + tr(" max val");
		str1 = tr(" change from %1 to %2")
			.arg(QString::number(m_param->errInterMax[m_leadIndex], 'f', 3))
			.arg(QString::number(val, 'f', 3));
		m_param->errInterMax[m_leadIndex] = val;
	}
	else if (dsp == ui->dsp_errEdgeDisMin) {
		str0 = ui->cob_lead->currentText() + QString(" ") + ui->cb_errEdgeDis->text() + tr(" min val");
		str1 = tr(" change from %1 to %2")
			.arg(QString::number(m_param->errEdgeDisMin[m_leadIndex], 'f', 3))
			.arg(QString::number(val, 'f', 3));
		m_param->errEdgeDisMin[m_leadIndex] = val;
	}
	else if (dsp == ui->dsp_errEdgeDisMax) {
		str0 = ui->cob_lead->currentText() + QString(" ") + ui->cb_errEdgeDis->text() + tr(" max val");
		str1 = tr(" change from %1 to %2")
			.arg(QString::number(m_param->errEdgeDisMax[m_leadIndex], 'f', 3))
			.arg(QString::number(val, 'f', 3));
		m_param->errEdgeDisMax[m_leadIndex] = val;
	}
	else if (dsp == ui->dsp_pixSize) {
		str0 = ui->label_28->text();
		str1 = tr(" change from %1 to %2")
			.arg(QString::number(m_param->pixSize, 'f', 3))
			.arg(QString::number(val, 'f', 3));
		m_param->pixSize = val;
	}
	m_module->setParamChanged(ProductLevel);
	//log(LogL1, str0 + str1);
}

void segLeadWdg::comboBox_Index_Changed(int index)
{
	QComboBox* cob = qobject_cast<QComboBox*>(sender());
	QString str0 = QString("");
	QString str1 = QString("");
	if (cob == ui->cob_lead) {
		str1 = tr(" change from %1 to %2").arg(cob->itemText(m_leadIndex)).arg(cob->itemText(index));
		m_leadIndex = index;
		connectSlots(false);
		changeErrLead();
		connectSlots(true);
	}
	else if (cob == ui->cob_testType) {
		str1 = tr(" change from %1 to %2").arg(cob->itemText(m_param->testType)).arg(cob->itemText(index));
		m_param->testType = index;
		ui->stackedWidget->setCurrentIndex(index);
		if (m_testing) {
			testVal();
		}
	}

	//log(LogL1, str0 + str1);
	m_module->setParamChanged(ProductLevel);
}

void segLeadWdg::on_bt_selectRoi_toggled(bool check)
{
	if (check) {
		if (ui->bt_autoLead->isChecked()) {
			ui->bt_autoLead->setChecked(false);
		}
		ui->bt_selectRoi->setText(tr("ROI OK"));

		drawRect();
	}
	else {
		//log(LogL1, ui.bt_selectRoi->text());
		ui->bt_selectRoi->setText(tr("Sel ROI"));
		currentView()->finishDraw();
		currentView()->update();
		double row1[1 + Max_LeadNum], col1[1 + Max_LeadNum], row2[1 + Max_LeadNum], col2[1 + Max_LeadNum];
		currentView()->getDrawRects1(1 + m_param->leadNum, row1, col1, row2, col2);
		for (int i = 0;i < 1 + m_param->leadNum;i++) {
			m_param->RoiVec[i].row1 = row1[i];
			m_param->RoiVec[i].col1 = col1[i];
			m_param->RoiVec[i].row2 = row2[i];
			m_param->RoiVec[i].col2 = col2[i];
		}
		m_module->setParamChanged(ProductLevel);
	}
}

void segLeadWdg::radioButton_toggled(bool val)
{
	QRadioButton* rb = qobject_cast<QRadioButton*>(sender());
	if (val) {
		QString str0 = QString("");
		QString str1 = QString("");
		if (rb == ui->rb_white) {
			str0 = ui->label_6->text();
			str1 = tr(" change from %1 to %2").arg(ui->rb_black->text()).arg(ui->rb_white->text());
			m_param->WOrB = 0;
		}
		else if (rb == ui->rb_black) {
			str0 = ui->label_6->text();
			str1 = tr(" change from %1 to %2").arg(ui->rb_white->text()).arg(ui->rb_black->text());
			m_param->WOrB = 1;
		}
		else if (rb == ui->rb_hor) {
			str0 = ui->label_7->text();
			str1 = tr(" change from %1 to %2").arg(ui->rb_ver->text()).arg(ui->rb_hor->text());
			m_param->HorOrVer = 0;
		}
		else if (rb == ui->rb_ver) {
			str0 = ui->label_7->text();
			str1 = tr(" change from %1 to %2").arg(ui->rb_hor->text()).arg(ui->rb_ver->text());
			m_param->HorOrVer = 1;
		}
		else if (rb == ui->rb_roiCenter) {
			str1 = tr(" change from %1 to %2").arg(ui->rb_setCenter->text()).arg(ui->rb_roiCenter->text());
			m_param->centerType = 0;
		}
		else if (rb == ui->rb_setCenter) {
			str1 = tr(" change from %1 to %2").arg(ui->rb_roiCenter->text()).arg(ui->rb_setCenter->text());
			m_param->centerType = 1;
		}
		else if (rb == ui->rb_autoWhite) {
			str1 = tr(" change from %1 to %2").arg(ui->rb_autoBlack->text()).arg(ui->rb_autoWhite->text());
			m_param->IsAutoWhite = 1;
		}
		else if (rb == ui->rb_autoBlack) {
			str1 = tr(" change from %1 to %2").arg(ui->rb_autoWhite->text()).arg(ui->rb_autoBlack->text());
			m_param->IsAutoWhite = 0;
		}
		if (m_testing) {
			testVal();
		}
		m_module->setParamChanged(ProductLevel);
		//log(LogL1, str0 + str1);
	}
}

void segLeadWdg::on_bt_useLead1_clicked()
{
	if (QMessageBox::warning(this, tr("Warning!"),
		tr("make sure to set all leads' param the same as lead 1?"), 
		QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
		for (int j = 1;j < Max_LeadNum;j++) {
			m_param->errLengthMin[j] = m_param->errLengthMin[0];
			m_param->errLengthMax[j] = m_param->errLengthMax[0];
			m_param->errWidthMin[j] = m_param->errWidthMin[0];
			m_param->errWidthMax[j] = m_param->errWidthMax[0];
			m_param->errInterMin[j] = m_param->errInterMin[0];
			m_param->errInterMax[j] = m_param->errInterMax[0];
		}
		connectSlots(false);
		changeErrLead();
		connectSlots(true);

		m_module->setParamChanged(ProductLevel);
		//log(LogL1, ui.bt_useLead1->text());
	}
}

void segLeadWdg::on_bt_test_toggled(bool val)
{
	m_testing = val;
	if (val) {
		testVal();
	}
	else {
		//currentView()->clear();
		currentView()->clearLayer(m_layerIdx);
		currentView()->update();
	}
}

void segLeadWdg::on_bt_autoLead_toggled(bool val)
{
	if (val) {
		if (ui->bt_selectRoi->isChecked()) {
			ui->bt_selectRoi->setChecked(false);
		}
		ui->stackedWidget_lead->setCurrentIndex(1);
		ui->bt_autoLead->setText(tr("exit seg"));
		autoThreshold();
	}
	else {
		ui->stackedWidget_lead->setCurrentIndex(0);
		ui->bt_autoLead->setText(tr("auto seg"));
		currentView()->finishDraw();
		currentView()->update();
		double row1[Max_LeadNum], col1[Max_LeadNum], row2[Max_LeadNum], col2[Max_LeadNum];
		currentView()->getDrawRects1(m_param->leadNum, row1, col1, row2, col2);
		for (int i = 0;i < m_param->leadNum;i++) {
			m_param->RoiVec[i + 1].row1 = row1[i];
			m_param->RoiVec[i + 1].col1 = col1[i];
			m_param->RoiVec[i + 1].row2 = row2[i];
			m_param->RoiVec[i + 1].col2 = col2[i];
		}
		m_module->setParamChanged(ProductLevel);
	}
}

void segLeadWdg::slider_valueChanged(int val)
{
	m_param->autoLeadThre = val;
	connectSlots(false);
	ui->sp_threshold->setValue(val);
	connectSlots(true);
	if (ui->bt_autoLead->isChecked()) {
		autoThreshold();
	}
}
