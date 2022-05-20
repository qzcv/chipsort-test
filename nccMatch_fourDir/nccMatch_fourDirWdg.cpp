#include "nccMatch_fourDirWdg.h"
#include "nccMatch_fourDirMod.h"
#include "nccMatch_fourDirParam.h"
#include "ui_nccMatch_fourDirWdg.h"
#include "ImageView.h"
#include "halOpenCV.h"
#include <QMessageBox>
#include "nccmatch.h"

nccMatch_fourDirWdg::nccMatch_fourDirWdg(int i) :
	ModSetWidget(),ui(new Ui::nccMatch_fourDirWdg),m_widgetType(i)
{
	m_hasConnect = false;
	ui->setupUi(this);
	iniData();
	iniUi();
	m_param = nullptr;
	m_module = nullptr;
}

nccMatch_fourDirWdg::~nccMatch_fourDirWdg()
{
	delete m_patternView;
	delete ui;
}

void nccMatch_fourDirWdg::setModule(UnitModule *module)
{
	m_module=dynamic_cast<nccMatch_fourDirMod*>(module);
	m_param = m_module->m_param;
	m_image = m_module->m_image;
	//currentView()->setLayer(m_layerIdx);
	//updateUi();
}

void nccMatch_fourDirWdg::showEvent(QShowEvent * event)
{
	updateUi();
}

void nccMatch_fourDirWdg::connectSlots(bool link)
{
	if(m_hasConnect==link)
		return;
	m_hasConnect=link;

	connectOrNot(link, ui->bt_threTest, SIGNAL(toggled(bool)), SLOT(bt_toggled(bool)));
	connectOrNot(link, ui->bt_editSignal, SIGNAL(toggled(bool)), SLOT(bt_toggled(bool)));

	connectOrNot(link, ui->cb_acceptValue, SIGNAL(toggled(bool)), SLOT(checkBox_toggled(bool)));
	connectOrNot(link, ui->cb_relativeAngle, SIGNAL(toggled(bool)), SLOT(checkBox_toggled(bool)));
	connectOrNot(link, ui->cb_useDir, SIGNAL(toggled(bool)), SLOT(checkBox_toggled(bool)));
	connectOrNot(link, ui->cb_defaultSig, SIGNAL(toggled(bool)), SLOT(checkBox_toggled(bool)));
	connectOrNot(link, ui->cb_defaultEdit, SIGNAL(toggled(bool)), SLOT(checkBox_toggled(bool)));
	connectOrNot(link, ui->cb_useRefer, SIGNAL(toggled(bool)), SLOT(checkBox_toggled(bool)));
	connectOrNot(link, ui->cb_matchAngle, SIGNAL(toggled(bool)), SLOT(checkBox_toggled(bool)));

	connectOrNot(link, ui->cb_mirror, SIGNAL(toggled(bool)), SLOT(checkBox_toggled(bool)));

	connectOrNot(link, ui->cb_useOutArea, SIGNAL(toggled(bool)), SLOT(checkBox_toggled(bool)));
	connectOrNot(link, ui->cb_fillUp, SIGNAL(toggled(bool)), SLOT(checkBox_toggled(bool)));
	connectOrNot(link, ui->cb_showOutArea, SIGNAL(toggled(bool)), SLOT(checkBox_toggled(bool)));

	connectOrNot(link, ui->sb_similarity, SIGNAL(valueChanged(int)), SLOT(spinBox_ValueChanged(int)));
	connectOrNot(link, ui->sb_seekingAngle, SIGNAL(valueChanged(int)), SLOT(spinBox_ValueChanged(int)));
	connectOrNot(link, ui->sb_acceptValue, SIGNAL(valueChanged(int)), SLOT(spinBox_ValueChanged(int)));
	connectOrNot(link, ui->sb_minRelative, SIGNAL(valueChanged(int)), SLOT(spinBox_ValueChanged(int)));
	connectOrNot(link, ui->sb_maxRelative, SIGNAL(valueChanged(int)), SLOT(spinBox_ValueChanged(int)));

	connectOrNot(link, ui->sp_deleteArea, SIGNAL(valueChanged(int)), SLOT(outAreaSpinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_outArea, SIGNAL(valueChanged(int)), SLOT(outAreaSpinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_minArea, SIGNAL(valueChanged(int)), SLOT(outAreaSpinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_maxArea, SIGNAL(valueChanged(int)), SLOT(outAreaSpinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_threMin, SIGNAL(valueChanged(int)), SLOT(outAreaSpinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_threMax, SIGNAL(valueChanged(int)), SLOT(outAreaSpinBox_valueChanged(int)));

	connectOrNot(link, ui->cb_pin1, SIGNAL(currentIndexChanged(int)), SLOT(comboBox_Index_Changed(int)));
	connectOrNot(link, ui->cb_pin2, SIGNAL(currentIndexChanged(int)), SLOT(comboBox_Index_Changed(int)));
	connectOrNot(link, ui->cob_pattern, SIGNAL(currentIndexChanged(int)), SLOT(comboBox_Index_Changed(int)));
	connectOrNot(link, ui->cob_startAngle, SIGNAL(currentIndexChanged(int)), SLOT(comboBox_Index_Changed(int)));
	connectOrNot(link, ui->cob_matchAngle, SIGNAL(currentIndexChanged(int)), SLOT(comboBox_Index_Changed(int)));

	connectOrNot(link, ui->rb_2D, SIGNAL(toggled(bool)), SLOT(radioButton_toggled(bool)));
	connectOrNot(link, ui->rb_4D, SIGNAL(toggled(bool)), SLOT(radioButton_toggled(bool)));
	connectOrNot(link, ui->rb_hor, SIGNAL(toggled(bool)), SLOT(radioButton_toggled(bool)));
	connectOrNot(link, ui->rb_ver, SIGNAL(toggled(bool)), SLOT(radioButton_toggled(bool)));

	for (int i = 0;i < 2;i++) {
		for (int j = 0;j < 4;j++) {
			connectOrNot(link, sp_sig[i][j], SIGNAL(valueChanged(int)), SLOT(spinBox_ValueChanged(int)));
		}
	}
}

void nccMatch_fourDirWdg::iniUi()
{
	for (int i = 0;i < 2;i++) {
		for (int j = 0;j < 4;j++) {
			sp_sig[i][j] = findChild<QSpinBox*>(QString("sp_sig%1_%2").arg(i).arg(j));
		}
	}
}

void nccMatch_fourDirWdg::iniData()
{
	m_selectingPat = false;
	m_selectingOut = false;
	m_layerIdx = 0;
	m_patternView = new ImageView(ui->widget_pattern);
}

void nccMatch_fourDirWdg::setUiValue()
{
	if(!m_param)
		return;

	ui->sb_similarity->setValue(m_param->similarity);
	ui->sb_seekingAngle->setValue(m_param->seekingAngle);
	ui->cb_useRefer->setChecked(m_param->IsRefer);
	ui->cb_acceptValue->setChecked(m_param->acceptEnable);
	ui->cb_useDir->setChecked(m_param->IsUsePinDir);
	ui->cb_defaultSig->setChecked(m_param->IsDefaultSig);
	ui->cb_mirror->setChecked(m_param->IsMirrorDir);
	ui->sb_acceptValue->setValue(m_param->acceptValue);
	ui->sb_acceptValue->setEnabled(m_param->acceptEnable);
	ui->cb_relativeAngle->setChecked(m_param->relativeEnable);
	ui->sb_minRelative->setValue(m_param->minRelative);
	ui->sb_maxRelative->setValue(m_param->maxRelative);
	ui->sb_minRelative->setEnabled(m_param->relativeEnable);
	ui->sb_maxRelative->setEnabled(m_param->relativeEnable);

	ui->cb_pin1->setCurrentIndex(m_param->pin1OutIdx);
	ui->cb_pin2->setCurrentIndex(m_param->pin2OutIdx);
	ui->cb_pin1->setEnabled(m_param->IsUsePinDir);
	ui->cb_pin2->setEnabled(m_param->IsUsePinDir);
	ui->cob_pattern->setCurrentIndex(m_param->currentPat);
	ui->cob_startAngle->setCurrentIndex(m_param->StartAngle);

	ui->cb_matchAngle->setChecked(m_param->matchAngleEnable);
	ui->cob_matchAngle->setCurrentIndex(m_param->matchAngle);

	ui->cb_defaultEdit->setChecked(m_param->IsDefaultEdit);
	ui->stackedWidget->setCurrentIndex(0);
	if (m_param->Is2Dor4D == 0) {
		ui->rb_2D->setChecked(true);
	}
	else if (m_param->Is2Dor4D == 1) {
		ui->rb_4D->setChecked(true);
	}
	ui->grp_mirror->setVisible(m_param->IsMirrorDir);
	if (m_param->HorOrVerMir == 0) {
		ui->rb_hor->setChecked(true);
	}
	else if (m_param->HorOrVerMir == 1) {
		ui->rb_ver->setChecked(true);
	}
	for (int i = 0;i < 2;i++) {
		for (int j = 0;j < 4;j++) {
			sp_sig[i][j]->setValue(m_param->sigEdit[i][j]);
		}
	}

	ui->cb_useOutArea->setChecked(m_param->IsUseOutArea);
	if (m_param->IsUseOutArea) {
		ui->groupBox_outArea->show();
	}
	else {
		ui->groupBox_outArea->hide();
	}

	ui->sp_outArea->setValue(m_param->outRegionCount);
	ui->sp_deleteArea->setValue(m_param->outDeleteCount);
	ui->sp_minArea->setValue(m_param->outAreaMin);
	ui->sp_maxArea->setValue(m_param->outAreaMax);
	ui->sp_threMin->setValue(m_param->outThreMin);
	ui->sp_threMax->setValue(m_param->outThreMax);
	ui->cb_showOutArea->setChecked(m_param->IsShowOutArea);
	ui->cb_fillUp->setChecked(m_param->IsFillUp);
	ui->tabWidget->setCurrentIndex(0);

	for (int i = 0;i < MAX_PAT_NUM;i++) {
		if (m_param->patImage[i].Id() != H_EMPTY_REGION)
		{
 			if (m_param->currentPat == i) {
				cv::Mat im;
				HalOpenCV::hal2cvImg(m_param->patImage[(m_param->Is2Dor4D == 0 && i == 2) ? 2 : i], im, false);
 				m_patternView->dispImage(im);
 				m_patternView->update();
 				m_patternView->fitImage();
 			}
		}
	}
}

void nccMatch_fourDirWdg::drawRects()
{
	currentView()->finishDraw();
	//currentView()->clear();
	currentView()->clearLayer(m_layerIdx);
	currentView()->setDrawColor(0, 255, 0);
	double row[20], col[20], angle[20], length1[20], length2[20];
	QStringList title;
	for (int i = 0;i < m_param->outRegionCount;i++) {
		row[i] = m_param->outRegionVec[i].row;
		col[i] = m_param->outRegionVec[i].col;
		angle[i] = m_param->outRegionVec[i].angle;
		length1[i] = m_param->outRegionVec[i].length1;
		length2[i] = m_param->outRegionVec[i].length2;
		//sprintf(buf[i], "outRoi%d", i);
		//title[i] = buf[i];
		title << QString("outRoi%1").arg(i);
	}
	for (int i = 0;i < m_param->outDeleteCount;i++) {
		row[i + m_param->outRegionCount] = m_param->deleteRegionVec[i].drow;
		col[i + m_param->outRegionCount] = m_param->deleteRegionVec[i].dcol;
		angle[i + m_param->outRegionCount] = m_param->deleteRegionVec[i].dangle;
		length1[i + m_param->outRegionCount] = m_param->deleteRegionVec[i].dlength1;
		length2[i + m_param->outRegionCount] = m_param->deleteRegionVec[i].dlength2;
		//sprintf(buf[i + m_param->outRegionCount], "outDel%d", i);
		//title[i + m_param->outRegionCount] = buf[i + m_param->outRegionCount];
		title << QString("outDel%1").arg(i);
	}
	currentView()->drawRects2(m_param->outRegionCount + m_param->outDeleteCount, row, col, angle, length1, length2, title);
	currentView()->update();
}

void nccMatch_fourDirWdg::getDrawRects()
{
	currentView()->finishDraw();
	currentView()->clearLayer(m_layerIdx);
	currentView()->update();
	double row[20], col[20], angle[20], length1[20], length2[20];
	currentView()->getDrawRects2(m_param->outRegionCount + m_param->outDeleteCount, row, col, angle, length1, length2);
	for (int i = 0;i < m_param->outRegionCount;i++) {
		m_param->outRegionVec[i].row = row[i];
		m_param->outRegionVec[i].col = col[i];
		m_param->outRegionVec[i].length1 = length1[i];
		m_param->outRegionVec[i].length2 = length2[i];
		m_param->outRegionVec[i].angle = angle[i];
	}
	for (int i = 0;i < m_param->outDeleteCount;i++) {
		m_param->deleteRegionVec[i].drow = row[i + m_param->outRegionCount];
		m_param->deleteRegionVec[i].dcol = col[i + m_param->outRegionCount];
		m_param->deleteRegionVec[i].dlength1 = length1[i + m_param->outRegionCount];
		m_param->deleteRegionVec[i].dlength2 = length2[i + m_param->outRegionCount];
		m_param->deleteRegionVec[i].dangle = angle[i + m_param->outRegionCount];
	}
}

void nccMatch_fourDirWdg::showRegionXld()
{
	//currentView()->clear();
	currentView()->clearLayer(m_layerIdx);
	if (m_param->OutImg[m_param->Is2Dor4D == 0 && m_param->currentPat == 2 ? 1 : m_param->currentPat].Id() == H_EMPTY_REGION) {
		QMessageBox::information(this, tr("Warning"), tr("The template of outArea is unset!"), QMessageBox::Yes);
		return;
	}
	Hobject outRegion;
	threshold(m_param->OutImg[m_param->Is2Dor4D == 0 && m_param->currentPat == 2 ? 1 : m_param->currentPat], &outRegion
		, m_param->outThreMin, m_param->outThreMax < m_param->outThreMin ? m_param->outThreMin : m_param->outThreMax);
	connection(outRegion, &outRegion);
	select_shape(outRegion, &outRegion, "area", "and", m_param->outAreaMin, m_param->outAreaMax < m_param->outAreaMin ? m_param->outAreaMin : m_param->outAreaMax);
	//fill_up(outRegion,&outRegion);
	opening_circle(outRegion, &outRegion, 3);
	closing_circle(outRegion, &outRegion, 3);
	union1(outRegion, &outRegion);
	currentView()->setFilled(false);
	currentView()->setColor(0, 255, 255);
	RlRegion rlReg;
	HalOpenCV::hal2cvRegion(outRegion, &rlReg);
	currentView()->dispRegion(&rlReg);
	currentView()->update();
}

void nccMatch_fourDirWdg::autoSetMultPattern()
{
	if (!m_module->has_set_pattern[0]) {
		return;
	}
	int angleVal[3] = { 90,180,270 };
	for (int i = 0;i < 3;i++) {
		int mIndex = i + 1;  //
		if (m_param->Is2Dor4D == 0) {
			if (i == 0 || i == 2) {
				continue;
			}
			else if (i == 1) {
				mIndex = 1;
			}
		}
		double centerrow, centercol;
		centerrow = (m_param->RecRoiRow1 + m_param->RecRoiRow2) / 2.0;
		centercol = (m_param->RecRoiCol1 + m_param->RecRoiCol2) / 2.0;

		if (m_param->IsMirrorDir && m_param->Is2Dor4D == 0) {
			if (m_param->HorOrVerMir == 0) { //左右镜像
				mirror_image(m_param->patImage[0], &m_param->patImage[mIndex], "column");
			}
			else {
				mirror_image(m_param->patImage[0], &m_param->patImage[mIndex], "row");
			}
		}
		else {
			rotate_image(m_param->patImage[0], &m_param->patImage[mIndex], angleVal[i], "bilinear");
		}

		HTuple hommat;
		double rangle = PI*angleVal[i] / 180.0;
		vector_angle_to_rigid(centerrow, centercol, 0.0, centerrow, centercol, rangle, &hommat);  //建立旋转关系矩阵

		double prow, pcol;
		if ((i == 0 || i == 2) && !(m_param->IsMirrorDir&&m_param->Is2Dor4D == 0)) {
			m_param->patternW[mIndex] = m_param->patternH[0];
			m_param->patternH[mIndex] = m_param->patternW[0];
		}
		else {
			m_param->patternH[mIndex] = m_param->patternH[0];
			m_param->patternW[mIndex] = m_param->patternW[0];
		}
		pcol = m_param->patternX[0] + m_param->patternW[0] / 2.0;
		prow = m_param->patternY[0] + m_param->patternH[0] / 2.0;

		if (m_param->IsMirrorDir&&m_param->Is2Dor4D == 0) {
			pcol = 2.0*centercol - pcol;
			prow = 2.0*centerrow - prow;
		}
		else {
			affine_trans_pixel(hommat, prow, pcol, &prow, &pcol);
		}

		m_param->patternX[mIndex] = pcol - m_param->patternW[mIndex] / 2.0;
		m_param->patternY[mIndex] = prow - m_param->patternH[mIndex] / 2.0;
		Hlong imgwid, imghei;
		get_image_size(*m_image, &imgwid, &imghei);

		m_param->roiRow1[mIndex] = (m_param->patternY[mIndex] - m_param->patternH[mIndex] / 2.0) > 0 ? (m_param->patternY[mIndex] - m_param->patternH[mIndex] / 2.0) : 0;
		m_param->roiCol1[mIndex] = (m_param->patternX[mIndex] - m_param->patternW[mIndex] / 2.0) > 0 ? (m_param->patternX[mIndex] - m_param->patternW[mIndex] / 2.0) : 0;
		m_param->roiRow2[mIndex] = (m_param->patternY[mIndex] + m_param->patternH[mIndex] * 1.5) < imghei ? (m_param->patternY[mIndex] + m_param->patternH[mIndex] * 1.5) : imghei;
		m_param->roiCol2[mIndex] = (m_param->patternX[mIndex] + m_param->patternW[mIndex] * 1.5) < imgwid ? (m_param->patternX[mIndex] + m_param->patternW[mIndex] * 1.5) : imgwid;

		//m_param->minusArea[mIndex]=m_param->minusArea[0];
		m_module->resetPattern(mIndex);
		m_module->has_set_pattern[mIndex] = 1;
		m_module->m_detect[mIndex]->setRoi(&QRect(
			m_param->roiCol1[mIndex], m_param->roiRow1[mIndex], 
			m_param->roiCol2[mIndex] - m_param->roiCol1[mIndex], 
			m_param->roiRow2[mIndex] - m_param->roiRow1[mIndex]));
	}//end of loop i
}

void nccMatch_fourDirWdg::updateUi()
{
	connectSlots(false);
	setUiValue();
	connectSlots(true);
}

void nccMatch_fourDirWdg::on_bt_selectPattern_clicked()
{
	if (m_selectingOut) {
		return;
	}
	m_selectingPat = true;
	ui->bt_cancelSelect->setEnabled(1);
	ui->bt_ensureSelect->setEnabled(1);
	ui->bt_selectPattern->setEnabled(0);
	ui->cob_startAngle->setEnabled(0);
	ui->cob_pattern->setEnabled(0);
	double row1[2], col1[2], row2[2], col2[2];

	int mStAngle;  //起始的角度
	if (m_param->Is2Dor4D == 0) {
		mStAngle = m_param->StartAngle <= 1 ? 0 : 1;
	}
	else if (m_param->Is2Dor4D == 1) {
		mStAngle = m_param->StartAngle;
	}

	row1[0] = m_param->patternY[mStAngle];
	col1[0] = m_param->patternX[mStAngle];
	row2[0] = m_param->patternY[mStAngle] + m_param->patternH[mStAngle];
	col2[0] = m_param->patternX[mStAngle] + m_param->patternW[mStAngle];

	row1[1] = m_param->RecRoiRow1;
	col1[1] = m_param->RecRoiCol1;
	row2[1] = m_param->RecRoiRow2;
	col2[1] = m_param->RecRoiCol2;

	QStringList title{ tr("temp"), tr("ROI") };
	currentView()->clearLayer(m_layerIdx);
	currentView()->drawRects1(2, row1, col1, row2, col2, title);
	currentView()->update();
}

void nccMatch_fourDirWdg::on_bt_ensureSelect_clicked()
{
	if (m_selectingOut) {
		return;
	}
	m_selectingPat = false;

	QString str0 = ui->bt_ensureSelect->text();
	//log(LogL1, str0);
	ui->bt_cancelSelect->setEnabled(0);
	ui->bt_ensureSelect->setEnabled(0);
	ui->bt_selectPattern->setEnabled(1);
	ui->cob_startAngle->setEnabled(1);
	ui->cob_pattern->setEnabled(1);
	double row1[2], col1[2], row2[2], col2[2];
	currentView()->finishDraw();
	currentView()->clearLayer(m_layerIdx);
	currentView()->getDrawRects1(2, row1, col1, row2, col2);

	int mStAngle;  //起始的角度
	if (m_param->Is2Dor4D == 0) {
		mStAngle = m_param->StartAngle <= 1 ? 0 : 1;
	}
	else if (m_param->Is2Dor4D == 1) {
		mStAngle = m_param->StartAngle;
	}

	m_param->patternX[mStAngle] = col1[0];
	m_param->patternY[mStAngle] = row1[0];
	m_param->patternH[mStAngle] = row2[0] - row1[0];
	m_param->patternW[mStAngle] = col2[0] - col1[0];

	m_param->RecRoiRow1 = row1[1];
	m_param->RecRoiCol1 = col1[1];
	m_param->RecRoiRow2 = row2[1];
	m_param->RecRoiCol2 = col2[1];

	cv::Mat img;
	m_module->p_im.getData(img);
	if (img.empty())
		return;
	HalOpenCV::cv2halImg(img, m_image, false);

	Hobject cloneImg, roiRect;
	crop_rectangle1(*m_image, &cloneImg, m_param->patternY[mStAngle], m_param->patternX[mStAngle], m_param->patternY[mStAngle]
		+ m_param->patternH[mStAngle], m_param->patternX[mStAngle] + m_param->patternW[mStAngle]);
	HTuple nchannels;
	count_channels(*m_image, &nchannels);
	if (nchannels[0].I() == 3)
		rgb1_to_gray(cloneImg, &cloneImg);
	copy_image(cloneImg, &m_param->patImage[mStAngle]);
	if (mStAngle != 0) {
		int angleVal[3] = { -90,-180,-270 };
		int mAngleIndex = mStAngle - 1;  //
		if (m_param->Is2Dor4D == 0) {
			mAngleIndex = 1;
		}
		double centerrow, centercol;
		centerrow = (m_param->RecRoiRow1 + m_param->RecRoiRow2) / 2.0;
		centercol = (m_param->RecRoiCol1 + m_param->RecRoiCol2) / 2.0;
		if (m_param->IsMirrorDir && m_param->Is2Dor4D == 0) {
			if (m_param->HorOrVerMir == 0) { //左右镜像
				mirror_image(m_param->patImage[mStAngle], &m_param->patImage[0], "column");
			}
			else {
				mirror_image(m_param->patImage[mStAngle], &m_param->patImage[0], "row");
			}
		}
		else {
			rotate_image(m_param->patImage[mStAngle], &m_param->patImage[0], angleVal[mAngleIndex], "bilinear");
		}


		HTuple hommat;
		double rangle = PI*angleVal[mAngleIndex] / 180.0;
		vector_angle_to_rigid(centerrow, centercol, -rangle, centerrow, centercol, 0, &hommat);  //建立旋转关系矩阵
		double prow, pcol;
		if ((mAngleIndex == 0 || mAngleIndex == 2) && !(m_param->IsMirrorDir&&m_param->Is2Dor4D == 0)) {
			m_param->patternW[0] = m_param->patternH[mStAngle];
			m_param->patternH[0] = m_param->patternW[mStAngle];
		}
		else {
			m_param->patternH[0] = m_param->patternH[mStAngle];
			m_param->patternW[0] = m_param->patternW[mStAngle];
		}
		pcol = m_param->patternX[mStAngle] + m_param->patternW[mStAngle] / 2.0;
		prow = m_param->patternY[mStAngle] + m_param->patternH[mStAngle] / 2.0;
		if (m_param->IsMirrorDir&&m_param->Is2Dor4D == 0) {
			pcol = 2.0*centercol - pcol;
			prow = 2.0*centerrow - prow;
		}
		else {
			affine_trans_pixel(hommat, prow, pcol, &prow, &pcol);
		}
		m_param->patternX[0] = pcol - m_param->patternW[0] / 2.0;
		m_param->patternY[0] = prow - m_param->patternH[0] / 2.0;
	}
	cv::Mat im;
	HalOpenCV::hal2cvImg(m_param->patImage[0], im, false);
	m_patternView->dispImage(im);
	m_patternView->update();
	m_patternView->fitImage();
	m_module->resetPattern(0);
	m_module->has_set_pattern[0] = 1;
	connectSlots(false);
	ui->cob_pattern->setCurrentIndex(0);
	connectSlots(true);
	Hlong imgwid, imghei;
	get_image_size(*m_image, &imgwid, &imghei);
	m_param->roiRow1[0] = (m_param->patternY[0] - m_param->patternH[0] / 2.0) > 0 ? (m_param->patternY[0] - m_param->patternH[0] / 2.0) : 0;
	m_param->roiCol1[0] = (m_param->patternX[0] - m_param->patternW[0] / 2.0) > 0 ? (m_param->patternX[0] - m_param->patternW[0] / 2.0) : 0;
	m_param->roiRow2[0] = (m_param->patternY[0] + m_param->patternH[0] * 1.5) < imghei ? (m_param->patternY[0] + m_param->patternH[0] * 1.5) : imghei;
	m_param->roiCol2[0] = (m_param->patternX[0] + m_param->patternW[0] * 1.5) < imgwid ? (m_param->patternX[0] + m_param->patternW[0] * 1.5) : imgwid;
	m_module->m_detect[0]->setRoi(&QRect(
		m_param->roiCol1[0], m_param->roiRow1[0], 
		m_param->roiCol2[0] - m_param->roiCol1[0], 
		m_param->roiRow2[0] - m_param->roiRow1[0]));
	if (m_param->IsAutoRotate) {
		autoSetMultPattern();
	}
	m_module->setParamChanged(QvsParamLevel::ProductLevel);
}

void nccMatch_fourDirWdg::on_bt_cancelSelect_clicked()
{
	if (m_selectingOut) {
		return;
	}
	m_selectingPat = false;

	ui->bt_cancelSelect->setEnabled(0);
	ui->bt_ensureSelect->setEnabled(0);
	ui->bt_selectPattern->setEnabled(1);
	ui->cob_pattern->setEnabled(1);
	ui->cob_startAngle->setEnabled(1);
	currentView()->finishDraw();
	currentView()->clearLayer(m_layerIdx);
	currentView()->update();
}

void nccMatch_fourDirWdg::bt_toggled(bool checked)
{
	QPushButton *bt = qobject_cast<QPushButton*>(sender());
	if (ui->bt_editSignal == bt)
		bt_editSignal_toggled(checked);
	else if (ui->bt_threTest)
		bt_threTest_toggled(checked);
}

void nccMatch_fourDirWdg::bt_editSignal_toggled(bool val)
{
	if (val) {
		ui->stackedWidget->setCurrentIndex(1);
		ui->bt_editSignal->setText(tr("exit Edit"));
	}
	else {
		ui->stackedWidget->setCurrentIndex(0);
		ui->bt_editSignal->setText(tr("Edit Signal"));
	}
}

void nccMatch_fourDirWdg::comboBox_Index_Changed(int index)
{
	QComboBox* cob = qobject_cast<QComboBox*>(sender());

	QString str0 = QString("");
	QString str1 = QString("");
	if (cob == ui->cb_pin1) {
		m_param->pin1OutIdx = index;
		//m_module->p_lfIdx.setData(m_param->pin1OutIdx);
		//m_module->setParamChanged(QvsParamLevel::MachineLevel);
	}
	else if (cob == ui->cb_pin2) {
		m_param->pin2OutIdx = index;
		//m_module->p_rtIdx.setData(m_param->pin2OutIdx);
		//m_module->setParamChanged(QvsParamLevel::MachineLevel);
	}
	else if (cob == ui->cob_pattern) {
		m_param->currentPat = index;
		bool imgok = m_param->patImage[m_param->Is2Dor4D == 0 && index == 2 ? 1 : index].Id() != H_EMPTY_REGION;
		cv::Mat im;
		if (((m_param->Is2Dor4D == 0 && (index == 0 || index == 2)) && imgok) || (m_param->Is2Dor4D == 1 && imgok)) {
			int midx;
			if (m_param->Is2Dor4D == 0 && index == 2) {
				midx = 1;
			}
			else {
				midx = index;
			}
			HalOpenCV::hal2cvImg(m_param->patImage[midx], im, false);
			m_patternView->dispImage(im);
		}
		else {
			Hobject img;
			gen_image_const(&img, "byte", 100, 100);
			gen_image_proto(img, &img, 0);
			HalOpenCV::hal2cvImg(img, im, false);
			m_patternView->dispImage(im);
		}
		m_patternView->fitImage();
		m_patternView->update();
	}
	else if (cob == ui->cob_startAngle) {
		str0 = ui->label_7->text();
		str1 = tr(" change from %1 to %2").arg(cob->itemText(m_param->StartAngle)).arg(cob->itemText(index));
		m_param->StartAngle = index;
	}
	else if (cob == ui->cob_matchAngle) {
		str0 = ui->cb_matchAngle->text();
		str1 = tr(" change from %1 to %2").arg(cob->itemText(m_param->matchAngle)).arg(cob->itemText(index));
		m_param->matchAngle = index;
	}
	m_module->setParamChanged(QvsParamLevel::ProductLevel);
	//log(LogL1, str0 + str1);
}

void nccMatch_fourDirWdg::radioButton_toggled(bool val)
{
	QRadioButton* rb = qobject_cast<QRadioButton*>(sender());
	QString str0 = QString("");
	QString str1 = QString("");
	if (val) {
		if (rb == ui->rb_2D) {
			str1 = tr(" change from %1 to %2").arg(ui->rb_4D->text()).arg(ui->rb_2D->text());
			m_param->Is2Dor4D = 0;
			if(m_param->IsMirrorDir)
				ui->grp_mirror->setVisible(1);
		}
		else if (rb == ui->rb_4D) {
			str1 = tr(" change from %1 to %2").arg(ui->rb_2D->text()).arg(ui->rb_4D->text());
			m_param->Is2Dor4D = 1;
			ui->grp_mirror->setVisible(0);
		}
		if (rb == ui->rb_hor) {
			str1 = tr(" change from %1 to %2").arg(ui->rb_ver->text()).arg(ui->rb_hor->text());
			m_param->HorOrVerMir = 0;
		}
		else if (rb == ui->rb_ver) {
			str1 = tr(" change from %1 to %2").arg(ui->rb_hor->text()).arg(ui->rb_ver->text());
			m_param->HorOrVerMir = 1;
		}
		m_module->setParamChanged(QvsParamLevel::ProductLevel);
		//log(LogL1, str0 + str1);
	}
}

void nccMatch_fourDirWdg::checkBox_toggled(bool val)
{
	QCheckBox* cb = qobject_cast<QCheckBox*>(sender());
	QString str0 = QString("");
	QString str1 = cb->text();
	QString str2 = QString("");
	if (val)
		str2 = tr(" Check");
	else
		str2 = tr(" unCheck");
	if (cb == ui->cb_acceptValue) {
		ui->sb_acceptValue->setEnabled(val);
		m_param->acceptEnable = val;
	}
	else if (cb == ui->cb_relativeAngle) {
		ui->sb_minRelative->setEnabled(val);
		ui->sb_maxRelative->setEnabled(val);
		m_param->relativeEnable = val;
	}
	else if (cb == ui->cb_useRefer) {
		m_param->IsRefer = val;
	}
	else if (cb == ui->cb_useDir) {
		str0 = ui->label_12->text();
		m_param->IsUsePinDir = val;
		ui->cb_pin1->setEnabled(val);
		ui->cb_pin2->setEnabled(val);
	}
	else if (cb == ui->cb_defaultSig) {
		m_param->IsDefaultSig = val;
	}
	else if (cb == ui->cb_defaultEdit) {
		m_param->IsDefaultEdit = val;
	}
	else if (cb == ui->cb_mirror) {
		m_param->IsMirrorDir = val;
		ui->grp_mirror->setVisible(val);
	}
	else if (cb == ui->cb_showOutArea) {
		m_param->IsShowOutArea = val;
	}
	else if (cb == ui->cb_fillUp) {
		m_param->IsFillUp = val;
	}
	else if (cb == ui->cb_matchAngle) {
		m_param->matchAngleEnable = val;
	}
	else if (cb == ui->cb_useOutArea) {
		m_param->IsUseOutArea = val;
		if (val) {
			ui->groupBox_outArea->show();
		}
		else {
			ui->groupBox_outArea->hide();
		}
	}
	m_module->setParamChanged(QvsParamLevel::ProductLevel);
	//log(LogL1, str0 + str1 + str2);
}

void nccMatch_fourDirWdg::spinBox_ValueChanged(int val)
{
	QSpinBox* sp = qobject_cast<QSpinBox*>(sender());
	QString str0 = QString("");
	QString str1 = QString("");
	if (sp == ui->sb_similarity) {
		str0 = ui->label->text();
		str1 = tr(" change from %1 to %2").arg(QString::number(m_param->similarity)).arg(QString::number(val));
		m_param->similarity = val;
		for (int i = 0;i < MAX_PAT_NUM;i++) {
			m_module->m_detect[i]->setSimilar((float)val / 100.0);
		}
	}
	else if (sp == ui->sb_seekingAngle) {
		str0 = ui->label_2->text();
		str1 = tr(" change from %1 to %2").arg(QString::number(m_param->seekingAngle)).arg(QString::number(val));
		m_param->seekingAngle = val;
		for (int i = 0;i < MAX_PAT_NUM;i++) {
			m_module->m_detect[i]->setRotateDeg(val);
		}
	}
	else if (sp == ui->sb_acceptValue) {
		str0 = ui->cb_acceptValue->text();
		str1 = tr(" change from %1 to %2").arg(QString::number(m_param->acceptValue)).arg(QString::number(val));
		m_param->acceptValue = val;
	}
	else if (sp == ui->sb_minRelative) {
		if (val > m_param->maxRelative)
		{
			ui->sb_minRelative->setValue(m_param->minRelative);
			return;
		}
		str0 = ui->cb_relativeAngle->text() + tr(" min val ");
		str1 = tr(" change from %1 to %2").arg(QString::number(m_param->minRelative)).arg(QString::number(val));
		m_param->minRelative = val;
	}
	else if (sp == ui->sb_maxRelative) {
		if (val < m_param->minRelative)
		{
			ui->sb_maxRelative->setValue(m_param->maxRelative);
			return;
		}
		str0 = ui->cb_relativeAngle->text() + tr(" max val ");
		str1 = tr(" change from %1 to %2").arg(QString::number(m_param->maxRelative)).arg(QString::number(val));
		m_param->maxRelative = val;
	}
	else {
		for (int i = 0;i < 2;i++) {
			for (int j = 0;j < 4;j++) {
				if (sp == sp_sig[i][j]) {
					m_param->sigEdit[i][j] = val;
					return;
				}
			}
		}
	}
	m_module->setParamChanged(ProductLevel);
	//log(LogL1, str0 + str1);
}

void nccMatch_fourDirWdg::on_bt_selectOutPat_clicked()
{
	if (m_selectingPat) {
		return;
	}
	m_selectingOut = true;
	//log(LogL1, ui.bt_selectOutPat->text());
	ui->bt_outPatOk->setEnabled(1);
	ui->bt_cancelOutPat->setEnabled(1);
	ui->bt_selectOutPat->setEnabled(0);
	ui->bt_threTest->setEnabled(0);
	drawRects();
}

void nccMatch_fourDirWdg::on_bt_outPatOk_clicked()
{
	if (m_selectingPat) {
		return;
	}
	m_selectingOut = false;
	//log(LogL1, ui.bt_outPatOk->text());
	ui->bt_outPatOk->setEnabled(0);
	ui->bt_cancelOutPat->setEnabled(0);
	ui->bt_threTest->setEnabled(1);
	ui->bt_selectOutPat->setEnabled(1);

	getDrawRects();
	if (!m_image || m_image->Id() == H_EMPTY_REGION) {
		return;
	}
	Hobject unionRec, roiRec;
	Hobject imgRec;
	Hobject dstImg;
	HTuple wid, hei;
	get_image_size(*m_image, &wid, &hei);
	gen_rectangle1(&imgRec, 0, 0, hei - 1, wid - 1);
	for (int i = 0;i < m_param->outDeleteCount;i++) {
		gen_rectangle2(&roiRec, m_param->deleteRegionVec[i].drow, m_param->deleteRegionVec[i].dcol, m_param->deleteRegionVec[i].dangle
			, m_param->deleteRegionVec[i].dlength1, m_param->deleteRegionVec[i].dlength2);
		union2(roiRec, unionRec, &unionRec);
	}
	difference(imgRec, unionRec, &unionRec);
	reduce_domain(*m_image, unionRec, &dstImg);
	Hobject outrec, outunion;
	for (int i = 0;i < m_param->outRegionCount;i++) {
		gen_rectangle2(&outrec, m_param->outRegionVec[i].row, m_param->outRegionVec[i].col, m_param->outRegionVec[i].angle
			, m_param->outRegionVec[i].length1, m_param->outRegionVec[i].length2);
		union2(outrec, outunion, &outunion);
	}
	reduce_domain(dstImg, outunion, &dstImg);
	HTuple outrow1, outrow2, outcol1, outcol2;
	smallest_rectangle1(outunion, &outrow1, &outcol1, &outrow2, &outcol2);
	int mStAngle;  //起始的角度
	if (m_param->Is2Dor4D == 0) {
		mStAngle = m_param->StartAngle <= 1 ? 0 : 1;
	}
	else if (m_param->Is2Dor4D == 1) {
		mStAngle = m_param->StartAngle;
	}
	copy_obj(dstImg, &m_param->OutImg[mStAngle], 1, 1);
	double centerrow, centercol;
	centerrow = (m_param->RecRoiRow1 + m_param->RecRoiRow2) / 2.0;
	centercol = (m_param->RecRoiCol1 + m_param->RecRoiCol2) / 2.0;
	if (mStAngle != 0) {
		int angleVal[3] = { -90,-180,-270 };
		int mAngleIndex = mStAngle - 1;  //
		if (m_param->Is2Dor4D == 0) {
			mAngleIndex = 1;
		}

		if (m_param->IsMirrorDir && m_param->Is2Dor4D == 0) {
			char tmpbuf[32];
			if (m_param->HorOrVerMir == 0) { //左右镜像
				sprintf(tmpbuf, "column");
			}
			else {
				sprintf(tmpbuf, "row");
			}
			Hobject cropimg;
			crop_rectangle1(*m_image, &cropimg, outrow1, outcol1, outrow2, outcol2);
			Hobject mirrorImg, mirrorReg;
			double mirror1Row, mirror1Col; //
			mirror1Col = (outcol1[0].D() + outcol2[0].D()) / 2.0;
			mirror1Row = (outrow1[0].D() + outrow2[0].D()) / 2.0;
			mirror_image(cropimg, &mirrorImg, tmpbuf);
			mirror_region(outunion, &mirrorReg, tmpbuf, m_param->HorOrVerMir == 0 ? mirror1Col : mirror1Row);
			HTuple hommat;
			double mirror2Col, mirror2Row;
			if (m_param->HorOrVerMir == 0) { //左右镜像
				mirror2Row = mirror1Row;
				mirror2Col = 2 * centercol - mirror1Col;
			}
			else {
				mirror2Col = mirror1Col;
				mirror2Row = 2 * centerrow - mirror1Row;
			}
			vector_angle_to_rigid(mirror1Row, mirror1Col, 0, mirror2Row, mirror2Col, 0, &hommat);
			HTuple trow1, trow2, tcol1, tcol2;
			tuple_gen_const(2, -1, &trow1);
			trow2 = tcol1 = tcol2 = trow1;
			HTuple offsetrow = 0, offsetcol = 0;
			offsetrow[1] = outrow1[0].I();offsetcol[1] = outcol1[0].I();
			Hobject inImg, resImg;
			gen_empty_obj(&inImg);
			concat_obj(inImg, *m_image, &inImg);
			concat_obj(inImg, mirrorImg, &inImg);
			long w, h;
			get_image_size(*m_image, &w, &h);
			tile_images_offset(inImg, &resImg, offsetrow, offsetcol, trow1, tcol1, trow2, tcol2, w, h);
			affine_trans_region(mirrorReg, &mirrorReg, hommat, "false");
			affine_trans_image(resImg, &resImg, hommat, "constant", "false");
			reduce_domain(resImg, mirrorReg, &resImg);
			copy_obj(resImg, &m_param->OutImg[0], 1, 1);
		}
		else {
			HTuple hommat;
			double rangle = PI*angleVal[mAngleIndex] / 180.0;
			vector_angle_to_rigid(centerrow, centercol, -rangle, centerrow, centercol, 0, &hommat);  //建立旋转关系矩阵
			Hobject affineimg, affinereg, resimg;
			affine_trans_image(*m_image, &affineimg, hommat, "constant", "false");
			affine_trans_region(outunion, &affinereg, hommat, "false");
			reduce_domain(affineimg, affinereg, &resimg);
			copy_obj(resimg, &m_param->OutImg[0], 1, 1);
		}
	}
	if (!(m_param->IsMirrorDir && m_param->Is2Dor4D == 0)) {
		int angleVal[3] = { 90,180,270 };
		for (int i = 0;i < 3;i++) {
			int mIndex = i + 1;  //
			if (m_param->Is2Dor4D == 0) {
				if (i == 0 || i == 2) {
					continue;
				}
				else if (i == 1) {
					mIndex = 1;
				}
			}
			HTuple hommat;
			double rangle = PI*angleVal[mIndex] / 180.0;
			vector_angle_to_rigid(centerrow, centercol, 0.0, centerrow, centercol, rangle, &hommat);  //建立旋转关系矩阵
			Hobject affineimg, affinereg, resimg;
			Hobject outdomain;
			get_domain(m_param->OutImg[0], &outdomain);
			affine_trans_image(m_param->OutImg[0], &affineimg, hommat, "constant", "false");
			affine_trans_region(outdomain, &affinereg, hommat, "false");
			reduce_domain(affineimg, affinereg, &resimg);
			copy_obj(resimg, &m_param->OutImg[mIndex], 1, 1);
		}//end of loop i
	}
	m_module->setParamChanged(ProductLevel);
}

void nccMatch_fourDirWdg::on_bt_cancelOutPat_clicked()
{
	if (m_selectingPat) {
		return;
	}
	m_selectingOut = false;
	//log(LogL1, ui.bt_cancelOutPat->text());
	ui->bt_outPatOk->setEnabled(0);
	ui->bt_cancelOutPat->setEnabled(0);
	ui->bt_threTest->setEnabled(1);
	ui->bt_selectOutPat->setEnabled(1);
	//currentView()->clear();
	currentView()->clearLayer(m_layerIdx);
	currentView()->finishDraw();
	currentView()->update();
}

void nccMatch_fourDirWdg::bt_threTest_toggled(bool val)
{
	if (val) {
		if (m_selectingOut || m_selectingPat) {
			connectSlots(false);
			ui->bt_threTest->setChecked(false);
			connectSlots(true);
			return;
		}
		//log(LogL1, tr("start ") + ui.bt_threTest->text());
		currentView()->finishDraw();
		ui->bt_selectPattern->setEnabled(0);
		ui->bt_selectOutPat->setEnabled(0);
		showRegionXld();
	}
	else {
		//log(LogL1, tr("finish ") + ui.bt_threTest->text());
		currentView()->finishDraw();
		//currentView()->clear();
		currentView()->clearLayer(m_layerIdx);
		currentView()->update();
		ui->bt_selectOutPat->setEnabled(1);
		ui->bt_selectPattern->setEnabled(1);
	}
}

void nccMatch_fourDirWdg::outAreaSpinBox_valueChanged(int val)
{
	QSpinBox* sp = qobject_cast<QSpinBox*>(sender());
	QString str1, str2;
	if (sp == ui->sp_outArea) {
		str1 = QStringLiteral("传出区域目标区域数");
		if (m_selectingOut) {
			getDrawRects();
		}
		nccMatch_fourDirParam::segRegion seg;
		seg.row = seg.col = 200;
		seg.length1 = seg.length2 = 25;  //给新添加的roi固定的开始位1置
		seg.angle = 0;
		if (val >= m_param->outRegionCount) {
			for (int i = m_param->outRegionCount;i < val;i++) {
				seg.row = seg.col += 10;
				m_param->outRegionVec.push_back(seg);
			}
		}
		else if (val < m_param->outRegionCount) {
			vector<nccMatch_fourDirParam::segRegion>::iterator itbegin = m_param->outRegionVec.begin() + val;
			vector<nccMatch_fourDirParam::segRegion>::iterator itEnd = m_param->outRegionVec.end();
			m_param->outRegionVec.erase(itbegin, itEnd);
		}
		str2 = QString("change from %1 to %2").arg(m_param->outRegionCount).arg(val);
		m_param->outRegionCount = val;
		if (m_selectingOut) {
			drawRects();
		}
	}
	else if (sp == ui->sp_deleteArea) {
		str1 = QStringLiteral("传出区域排除区域数");
		if (m_selectingOut) {
			getDrawRects();
		}
		nccMatch_fourDirParam::delRegion seg;
		seg.drow = seg.dcol = 200;
		seg.dlength1 = seg.dlength2 = 25;  //给新添加的roi固定的开始位1置
		seg.dangle = 0;
		if (val >= m_param->outDeleteCount) {
			for (int i = m_param->outDeleteCount;i < val;i++) {
				seg.drow = seg.dcol += 10;
				m_param->deleteRegionVec.push_back(seg);
			}
		}
		else if (val < m_param->outDeleteCount) {
			vector<nccMatch_fourDirParam::delRegion>::iterator itbegin = m_param->deleteRegionVec.begin() + val;
			vector<nccMatch_fourDirParam::delRegion>::iterator itEnd = m_param->deleteRegionVec.end();
			m_param->deleteRegionVec.erase(itbegin, itEnd);
		}
		str2 = QString("change from %1 to %2").arg(m_param->outDeleteCount).arg(val);
		m_param->outDeleteCount = val;
		if (m_selectingOut) {
			drawRects();
		}
	}
	else if (sp == ui->sp_minArea) {
		str1 = QStringLiteral("传出区域最小面积");
		str2 = QString("change from %1 to %2").arg(m_param->outAreaMin).arg(val);
		if (val >= m_param->outAreaMax) {
			m_param->outAreaMin = m_param->outAreaMax - 1;
			connectSlots(false);
			ui->sp_minArea->setValue(m_param->outAreaMax - 1);
			connectSlots(true);
		}
		else
			m_param->outAreaMin = val;
		if (ui->bt_threTest->isChecked()) {
			showRegionXld();
		}
	}
	else if (sp == ui->sp_maxArea) {
		str1 = QStringLiteral("传出区域最大面积");
		str2 = QString("change from %1 to %2").arg(m_param->outAreaMax).arg(val);
		if (val <= m_param->outAreaMin) {
			m_param->outAreaMax = m_param->outAreaMin + 1;
			connectSlots(false);
			ui->sp_maxArea->setValue(m_param->outAreaMin + 1);
			connectSlots(true);
		}
		else
			m_param->outAreaMax = val;
		if (ui->bt_threTest->isChecked()) {
			showRegionXld();
		}
	}
	else if (sp == ui->sp_threMin) {
		str1 = QStringLiteral("传出区域阈值最小值");
		str2 = QString("change from %1 to %2").arg(m_param->outThreMin).arg(val);
		if (val >= m_param->outThreMax) {
			m_param->outThreMin = m_param->outThreMax - 1;
			connectSlots(false);
			ui->sp_threMin->setValue(m_param->outThreMax - 1);
			connectSlots(true);
		}
		else
			m_param->outThreMin = val;
		if (ui->bt_threTest->isChecked()) {
			showRegionXld();
		}
	}
	else if (sp == ui->sp_threMax) {
		str1 = QStringLiteral("传出区域阈值最大值");
		str2 = QString("change from %1 to %2").arg(m_param->outThreMax).arg(val);
		if (val <= m_param->outThreMin) {
			m_param->outThreMax = m_param->outThreMin + 1;
			connectSlots(false);
			ui->sp_threMax->setValue(m_param->outThreMin + 1);
			connectSlots(true);
		}
		else
			m_param->outThreMax = val;
		if (ui->bt_threTest->isChecked()) {
			showRegionXld();
		}
	}
	m_module->setParamChanged(ProductLevel);
	//log(LogL1, str1 + str2);
}
