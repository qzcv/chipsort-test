#include "mark1Wdg.h"
#include "mark1Mod.h"
#include "mark1Param.h"
#include "ui_mark1Wdg.h"
#include <QMessageBox>
#include <QFileDialog>
#include "ImageView.h"
#include "halOpenCV.h"
#include "thresholdwidget.h"
#include "orientEditDialog.h"
#include "AdvanceDialog.h"
#include "showWidget.h"

mark1Wdg::mark1Wdg(int i) :
	ModSetWidget(), ui(new Ui::mark1Wdg), m_widgetType(i)
{
	m_hasConnect = false;
	ui->setupUi(this);
	iniData();
	iniUi();
}

mark1Wdg::~mark1Wdg()
{
	delete m_showWidget;
	delete m_thresholdWidget;
	delete m_advanceDialog;
	delete m_orientDialog;
	delete ui;
}

void mark1Wdg::setModule(UnitModule *module)
{
	m_module = dynamic_cast<mark1Mod*>(module);
	m_param = m_module->m_param;
	m_image = m_module->m_image;
	m_orientDialog->setParam(m_param);
	m_advanceDialog->setParam(m_param);
	m_showWidget->setParam(m_param);
	//currentView()->setLayer(m_layerIdx);
	//updateUi();
}

void mark1Wdg::showEvent(QShowEvent * event)
{
	updateUi();
}

void mark1Wdg::connectSlots(bool link)
{
	if (m_hasConnect == link)
		return;
	m_hasConnect = link;

	connectOrNot(link, ui->cb_autoSegmen, SIGNAL(toggled(bool)), SLOT(check_box_toggled(bool)));
	connectOrNot(link, ui->cb_whiteChar, SIGNAL(toggled(bool)), SLOT(check_box_toggled(bool)));
	connectOrNot(link, ui->cb_searchAngle, SIGNAL(toggled(bool)), SLOT(check_box_toggled(bool)));
	connectOrNot(link, ui->cb_charOffSet, SIGNAL(toggled(bool)), SLOT(check_box_toggled(bool)));
	connectOrNot(link, ui->cb_errsAngle, SIGNAL(toggled(bool)), SLOT(check_box_toggled(bool)));
	connectOrNot(link, ui->cb_errOffSet, SIGNAL(toggled(bool)), SLOT(check_box_toggled(bool)));
	connectOrNot(link, ui->cb_crack, SIGNAL(toggled(bool)), SLOT(check_box_toggled(bool)));
	connectOrNot(link, ui->cb_excess, SIGNAL(toggled(bool)), SLOT(check_box_toggled(bool)));
	connectOrNot(link, ui->cb_edgeDisX, SIGNAL(toggled(bool)), SLOT(check_box_toggled(bool)));
	connectOrNot(link, ui->cb_edgeDisY, SIGNAL(toggled(bool)), SLOT(check_box_toggled(bool)));

	connectOrNot(link, ui->cb_errArea, SIGNAL(toggled(bool)), SLOT(check_box_toggled(bool)));
	connectOrNot(link, m_orientDialog->ui.cb_useDir, SIGNAL(toggled(bool)), SLOT(check_box_toggled(bool)));

	connectOrNot(link, m_advanceDialog->ui.cb_useMult, SIGNAL(toggled(bool)), SLOT(check_box_toggled(bool)));
	connectOrNot(link, m_advanceDialog->ui.cb_inputPatIndex, SIGNAL(toggled(bool)), SLOT(check_box_toggled(bool)));
	connectOrNot(link, m_advanceDialog->ui.cb_inputRatio, SIGNAL(toggled(bool)), SLOT(check_box_toggled(bool)));

	connectOrNot(link, m_advanceDialog->ui.cb_DirPattern, SIGNAL(toggled(bool)), SLOT(check_box_toggled(bool)));
	connectOrNot(link, m_advanceDialog->ui.cb_fillUp, SIGNAL(toggled(bool)), SLOT(check_box_toggled(bool)));
	connectOrNot(link, m_advanceDialog->ui.cb_referTempAngle, SIGNAL(toggled(bool)), SLOT(check_box_toggled(bool)));
	connectOrNot(link, m_advanceDialog->ui.cb_ocr, SIGNAL(toggled(bool)), SLOT(check_box_toggled(bool)));

	connectOrNot(link, m_advanceDialog->ui.bt_clearTmp, SIGNAL(clicked()), SLOT(bt_clearTmp_clicked()));
	connectOrNot(link, m_advanceDialog->ui.bt_testCharThreshold, SIGNAL(toggled(bool)), SLOT(bt_testCharThreshold_toggled(bool)));
	connectOrNot(link, m_advanceDialog->ui.bt_saveImageDir, SIGNAL(clicked()), SLOT(bt_saveImageDir_clicked()));

	connectOrNot(link, m_advanceDialog->ui.rb_2Angle, SIGNAL(toggled(bool)), SLOT(radioButton_toggled(bool)));
	connectOrNot(link, m_advanceDialog->ui.rb_4Angle, SIGNAL(toggled(bool)), SLOT(radioButton_toggled(bool)));

	connectOrNot(link, m_advanceDialog->ui.rb_font, SIGNAL(toggled(bool)), SLOT(radioButton_toggled(bool)));
	connectOrNot(link, m_advanceDialog->ui.rb_layer, SIGNAL(toggled(bool)), SLOT(radioButton_toggled(bool)));

	connectOrNot(link, ui->cob_multPattern, SIGNAL(currentIndexChanged(int)), SLOT(comboBox_Index_Changed(int)));
	connectOrNot(link, m_advanceDialog->ui.cob_charIndex, SIGNAL(currentIndexChanged(int)), SLOT(comboBox_Index_Changed(int)));
	connectOrNot(link, m_advanceDialog->ui.cob_refer, SIGNAL(currentIndexChanged(int)), SLOT(comboBox_Index_Changed(int)));
	connectOrNot(link, m_advanceDialog->ui.cob_area, SIGNAL(currentIndexChanged(int)), SLOT(comboBox_Index_Changed(int)));


	connectOrNot(link, ui->sp_charCounts, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_charOffset, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_searchAngle, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_acceptSimilar, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_errAngle, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_errOffset, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_edgeDisXMin, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_edgeDisYMin, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_edgeDisXMax, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_edgeDisYMax, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_errArea, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));

	connectOrNot(link, ui->dsp_crack, SIGNAL(valueChanged(double)), SLOT(doubleSpinBox_valueChanged(double)));
	connectOrNot(link, ui->dsp_excess, SIGNAL(valueChanged(double)), SLOT(doubleSpinBox_valueChanged(double)));

	connectOrNot(link, m_advanceDialog->ui.sp_AutoRegionLarge, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, m_advanceDialog->ui.sp_charThreMin, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, m_advanceDialog->ui.sp_charThreMax, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, m_advanceDialog->ui.sp_defectTOL, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, m_advanceDialog->ui.sp_patternNum, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, m_advanceDialog->ui.dsp_ratio, SIGNAL(valueChanged(double)), SLOT(doubleSpinBox_valueChanged(double)));

	connectOrNot(link, m_thresholdWidget, SIGNAL(areaAndThreChanged(int, int, int, int, int, int)), SLOT(thresholdWidget_thresholdAndArea_changed(int, int, int, int, int, int)));
	connectOrNot(link, m_thresholdWidget, SIGNAL(accepted()), SLOT(thresholdWidgetAccept()));
	connectOrNot(link, m_thresholdWidget, SIGNAL(rejected()), SLOT(thresholdWidgetReject()));
	connectOrNot(link, currentView(), SIGNAL(mousePressed(int, int, int, double, double)), SLOT(viewMousePressed(int, int, int, double, double)));
}

void mark1Wdg::iniData()
{
	m_param = nullptr;
	m_module = nullptr;
	m_image = nullptr;
	m_layerIdx = 0;

	m_selectState = UnSel;
	m_showPatIndex = 0;
	m_connectView = false;

	m_thresholdWidget = new thresholdWidget(this);
	m_orientDialog = new OrientEditDialog(this);
	m_advanceDialog = new AdvanceSetDialog(this);
	m_showWidget = new ShowWidget(this);

	m_tmpTol = m_tmpMinArea = m_tmpThreshold = 0;
	m_tmpHorOrVer = 0;
	m_tmpCharGap = 50;
	m_tmpMinHole = 0;
	m_tmpConns.clear();
	m_tmpHoles.clear();
	m_sPoint.clear();
}

void mark1Wdg::iniUi()
{

}

void mark1Wdg::setUiValue()
{
	if (!m_param)
		return;

	ui->cb_errsAngle->setChecked(m_param->errAngleValid);
	//ui.sp_errAngle->setEnabled(m_param->errAngleValid);
	ui->cb_errOffSet->setChecked(m_param->errOffSetValid);
	//ui.sp_errOffset->setEnabled(m_param->errOffSetValid);

	ui->cb_errArea->setChecked(m_param->errAreaValid);
	ui->cb_edgeDisX->setChecked(m_param->errOffXValid);
	ui->cb_edgeDisY->setChecked(m_param->errOffYValid);
	//ui.sp_errArea->setEnabled(m_param->errAreaValid);
	ui->sp_errArea->setValue(m_param->errAreaPercent);
	ui->cb_excess->setChecked(m_param->errExcessValid);
	ui->cb_crack->setChecked(m_param->errCrackValid);

	m_advanceDialog->ui.cb_fillUp->setChecked(m_param->IsFillUp);
	m_advanceDialog->ui.cb_DirPattern->setChecked(m_param->IsDirPattern);
	m_advanceDialog->ui.cb_referTempAngle->setChecked(m_param->IsReferTempAngle);
	m_advanceDialog->ui.cb_ocr->setChecked(m_param->OcrValid);

	m_advanceDialog->ui.cb_inputRatio->setChecked(m_param->IsInputRatio);

	m_advanceDialog->ui.grp_dirPattern->setVisible(m_param->IsDirPattern);

	m_advanceDialog->ui.cob_refer->setCurrentIndex(m_param->IsReferSection);
	m_advanceDialog->ui.cob_charIndex->clear();
	for (int i = 0;i < m_param->charCounts[m_param->NowPatternNum];i++) {
		m_advanceDialog->ui.cob_charIndex->addItem(tr("Char%1").arg(i + 1));
	}
	m_showPatIndex = 0;
	if (m_advanceDialog->ui.cob_charIndex->count() != 0)
		m_advanceDialog->ui.cob_charIndex->setCurrentIndex(m_showPatIndex);
	m_advanceDialog->ui.cob_area->setCurrentIndex(m_param->areaType);
	if (m_param->angleType == 0) {
		m_advanceDialog->ui.rb_2Angle->setChecked(true);
	}
	else if (m_param->angleType == 1) {
		m_advanceDialog->ui.rb_4Angle->setChecked(true);
	}

	if (m_param->IsFontOrLayer == 0) {
		m_advanceDialog->ui.rb_font->setChecked(true);
	}
	else if (m_param->IsFontOrLayer == 1) {
		m_advanceDialog->ui.rb_layer->setChecked(true);
	}
	//ui.
	//学习模块
	ui->cb_autoSegmen->setChecked(m_param->autoSegment);
	ui->bt_studyTmp->setVisible(m_param->autoSegment);
	ui->bt_selectPattern->setVisible(!m_param->autoSegment);
	ui->bt_selectOk->setVisible(0);
	ui->bt_selectCancel->setVisible(0);


	ui->bt_setCharCounts->setVisible(!m_param->autoSegment);

	ui->cb_whiteChar->setChecked(m_param->whiteChar);
	ui->cb_charOffSet->setChecked(m_param->charOffsetValid);
	if (m_param->searchAngleValid) {
		ui->cb_searchAngle->setChecked(true);
		//ui.sp_searchAngle->setEnabled(true);
	}

	m_advanceDialog->ui.sp_charThreMin->setValue(m_param->charThreMin);
	m_advanceDialog->ui.sp_charThreMax->setValue(m_param->charThreMax);
	m_advanceDialog->ui.dsp_ratio->setValue(m_param->pixelSize);

	ui->sp_charCounts->setValue(m_param->charCounts[m_param->NowPatternNum]);
	ui->sp_charOffset->setValue(m_param->charOffset);
	ui->sp_searchAngle->setValue(m_param->searchAngle);
	ui->sp_acceptSimilar->setValue(m_param->acceptSimilar);
	ui->sp_errAngle->setValue(m_param->errAngle);
	ui->sp_errOffset->setValue(m_param->errOffSet);
	ui->sp_edgeDisXMin->setValue(m_param->errOffXMin);
	ui->sp_edgeDisXMax->setValue(m_param->errOffXMax);
	ui->sp_edgeDisYMin->setValue(m_param->errOffYMin);
	ui->sp_edgeDisYMax->setValue(m_param->errOffYMax);
	ui->dsp_excess->setValue(m_param->errExcess);
	ui->dsp_crack->setValue(m_param->errCrack);

	m_orientDialog->ui.cb_useDir->setChecked(m_param->IsUsePinDirOut);

	m_advanceDialog->ui.sp_AutoRegionLarge->setValue(m_param->autoRegionLarge);
	m_advanceDialog->ui.sp_defectTOL->setValue(m_param->CharDefectTOL);
	m_advanceDialog->ui.sp_patternNum->setValue(m_param->PatternNum);

	m_advanceDialog->ui.cb_useMult->setChecked(m_param->IsMult);
	m_advanceDialog->ui.cb_inputPatIndex->setChecked(m_param->IsInputPatIndex);
	m_advanceDialog->ui.grp_multPat->setVisible(m_param->IsMult);
	//ui.cob_multPattern->setEnabled(m_param->IsMult);

	ui->cob_multPattern->clear();
	for (int i = 0;i < m_param->PatternNum;i++) {
		ui->cob_multPattern->addItem(tr("template%1").arg(i + 1));
	}
	ui->cob_multPattern->setCurrentIndex(m_param->NowPatternNum);
	ui->cob_multPattern->setVisible(m_param->IsMult);
	ui->bt_high->setChecked(false);
	ui->bt_high->setText(tr("Advanced"));
	if (m_param->MultPatternEnable[m_param->NowPatternNum]) {
		ui->lb_patternState->setText(tr("template set"));
	}
	else {
		ui->lb_patternState->setText(tr("template unset"));
	}
}

void mark1Wdg::updateUi()
{
	connectSlots(false);
	setUiValue();
	connectSlots(true);
}

void mark1Wdg::showCharRegion()
{
	//currentView()->clear();
	currentView()->clearLayer(m_layerIdx);
	currentView()->setColor(255, 255, 0);
	Hobject singleUnion, allUnion;
	int j = m_param->NowPatternNum;
	for (int i = 0;i < m_param->charCounts[j];i++)
	{
		Hobject segroi;
		Hobject patimg;
		crop_rectangle1(m_param->SegImg[j], &patimg, 
			m_param->segRegionVector[j][i].row1 - m_param->RegionRoi[j].row1,
			m_param->segRegionVector[j][i].col1 - m_param->RegionRoi[j].col1,
			m_param->segRegionVector[j][i].row2 - m_param->RegionRoi[j].row1,
			m_param->segRegionVector[j][i].col2 - m_param->RegionRoi[j].col1);
		
		threshold(patimg, &segroi, m_param->charThreMin, m_param->charThreMax >= m_param->charThreMin ? m_param->charThreMax : m_param->charThreMin);

		//closing_rectangle1(segroi,&segroi,3,3);
		//opening_rectangle1(segroi,&segroi,3,3);
		connection(segroi, &segroi);
		select_shape(segroi, &segroi, "area", "and", m_param->minArea, 100000000);
		HTuple recmat;
		vector_angle_to_rigid(
			(m_param->segRegionVector[m_param->NowPatternNum][i].row2 - m_param->segRegionVector[m_param->NowPatternNum][i].row1) / 2.0,
			(m_param->segRegionVector[m_param->NowPatternNum][i].col2 - m_param->segRegionVector[m_param->NowPatternNum][i].col1) / 2.0, 0,
			(m_param->segRegionVector[m_param->NowPatternNum][i].row2 + m_param->segRegionVector[m_param->NowPatternNum][i].row1) / 2.0,
			(m_param->segRegionVector[m_param->NowPatternNum][i].col2 + m_param->segRegionVector[m_param->NowPatternNum][i].col1) / 2.0, 0,
			&recmat);
		affine_trans_region(segroi, &segroi, recmat, "false");
		if (m_param->IsFillUp) {
			fill_up(segroi, &segroi);
		}
		if (i == 0) {
			union1(segroi, &allUnion);
		}
		else {
			union1(segroi, &singleUnion);
			union2(singleUnion, allUnion, &allUnion);
		}
	}
	currentView()->setFilled(false);
	RlRegion reg;
	HalOpenCV::hal2cvRegion(allUnion, &reg);
	currentView()->dispRegion(&reg);
	currentView()->update();
}

bool mark1Wdg::checkPointInside(double recrow1, double reccol1, double recrow2, double reccol2, double pointrow, double pointcol)
{
	bool isInside = false;
	if ((pointrow<recrow2 && pointrow>recrow1) && (pointcol<reccol2 && pointcol>reccol1)) {
		isInside = true;
	}
	return isInside;
}

void mark1Wdg::on_bt_selectRoi_clicked()
{
	if (m_selectState != UnSel) {
		return;
	}
	m_selectState = SelRoiRegion;
	//currentView()->clear();
	currentView()->clearLayer(m_layerIdx);
	currentView()->setDrawColor(0, 255, 0);
	ui->bt_selectRoi->setEnabled(false);
	ui->bt_selectOk->setVisible(1);
	ui->bt_selectCancel->setVisible(1);
	m_advanceDialog->ui.bt_testCharThreshold->setEnabled(false);
	double row1, col1, row2, col2;
	int index = m_param->NowPatternNum;
	row1 = m_param->RegionRoi[index].row1;
	col1 = m_param->RegionRoi[index].col1;
	row2 = m_param->RegionRoi[index].row2;
	col2 = m_param->RegionRoi[index].col2;

	const char *buf = { "searchRoi" };
	currentView()->drawRect1(row1, col1, row2, col2, buf);
	currentView()->update();

	QString str1 = ui->bt_selectRoi->text();
	//log(LogL1, str1);
}

void mark1Wdg::on_bt_selectPattern_clicked()
{
	if (m_param->charCounts[m_param->NowPatternNum] <= 0 || m_param->charCounts[m_param->NowPatternNum] > MAX_CHAR_NUM) {
		return;
	}
	if (m_selectState != UnSel)
		return;
	m_selectState = SelPattern;
	m_advanceDialog->ui.bt_testCharThreshold->setEnabled(false);
	ui->bt_selectPattern->setEnabled(false);
	ui->bt_selectOk->setVisible(1);
	ui->bt_selectCancel->setVisible(1);
	//currentView()->clear();
	currentView()->clearLayer(m_layerIdx);
	currentView()->setColor(255, 255, 0);
	int index = m_param->NowPatternNum;
	currentView()->dispRect1(
		m_param->RegionRoi[index].row1, m_param->RegionRoi[index].col1,
		m_param->RegionRoi[index].row2, m_param->RegionRoi[index].col2, "searchROI");
	currentView()->setDrawColor(0, 255, 0);
	double row1[MAX_CHAR_NUM], col1[MAX_CHAR_NUM], row2[MAX_CHAR_NUM], col2[MAX_CHAR_NUM];
	QStringList title;
	for (int i = 0;i < m_param->charCounts[index];i++) {
		row1[i] = m_param->segRegionVector[index][i].row1;
		row2[i] = m_param->segRegionVector[index][i].row2;
		col1[i] = m_param->segRegionVector[index][i].col1;
		col2[i] = m_param->segRegionVector[index][i].col2;
		title << QString("%1").arg(i + 1);
	}
	currentView()->drawRects1(m_param->charCounts[index], row1, col1, row2, col2, title);
	currentView()->update();
}

void mark1Wdg::on_bt_selectOk_clicked()
{
	if (m_selectState == UnSel)
		return;
	if (m_selectState == SelPattern) {
		m_selectState = UnSel;
		ui->bt_selectOk->setVisible(0);
		ui->bt_selectCancel->setVisible(0);
		ui->bt_selectPattern->setEnabled(true);
		m_advanceDialog->ui.bt_testCharThreshold->setEnabled(true);
		currentView()->finishDraw();
		//currentView()->clear();
		currentView()->clearLayer(m_layerIdx);
		currentView()->update();
		int index = m_param->NowPatternNum;
		if (m_param->SegImg[index].Id() == H_EMPTY_REGION) {
			QMessageBox::warning(this, tr("Warning"), tr("no image for setting template!"),
				QMessageBox::Yes);
			return;
		}
		double row1[MAX_CHAR_NUM], col1[MAX_CHAR_NUM], row2[MAX_CHAR_NUM], col2[MAX_CHAR_NUM];
		currentView()->getDrawRects1(m_param->charCounts[index], row1, col1, row2, col2);
		for (int i = 0;i < m_param->charCounts[index];i++) {
			if (row1[i]<m_param->RegionRoi[index].row1 || row2[i]>m_param->RegionRoi[index].row2 ||
				col1[i]<m_param->RegionRoi[index].col1 || col2[i]>m_param->RegionRoi[index].col2) {
				QMessageBox::warning(this, tr("Warning"), 
					tr("char region out of search Region! please reset template."), QMessageBox::Yes);
				return;
			}
			m_param->segRegionVector[index][i].row1 = row1[i];
			m_param->segRegionVector[index][i].col1 = col1[i];
			m_param->segRegionVector[index][i].row2 = row2[i];
			m_param->segRegionVector[index][i].col2 = col2[i];
		}
		m_module->creatNccTemp(false, index);
		m_advanceDialog->ui.cob_charIndex->setCurrentIndex(0);
		if (m_param->MultPatternEnable[index]) {
			m_module->createRegionTemp(index);
			ui->lb_patternState->setText(tr("template set"));
		}
		else
			ui->lb_patternState->setText(tr("template unset"));
		if (ui->bt_viewTemp->isChecked()) {
			m_showWidget->showTemp();
		}
		//log(LogL1,ui->bt_selectOk->text());
	}
	else if (m_selectState == SelRoiRegion) {
		m_selectState = UnSel;
		ui->bt_selectOk->setVisible(0);
		ui->bt_selectCancel->setVisible(0);
		ui->bt_selectRoi->setEnabled(true);

		currentView()->finishDraw();
		currentView()->update();
		m_advanceDialog->ui.bt_testCharThreshold->setEnabled(true);

		int index = m_param->NowPatternNum;
		double row1, col1, row2, col2;
		currentView()->getDrawRect1(row1, col1, row2, col2);
		m_param->RegionRoi[index].row1 = row1;
		m_param->RegionRoi[index].col1 = col1;
		m_param->RegionRoi[index].row2 = row2;
		m_param->RegionRoi[index].col2 = col2;

		if (!m_module->p_im->empty()) {
			HalOpenCV::cv2halImg(*m_module->p_im, m_image, false);
			crop_rectangle1(*m_image, &m_param->SegImg[index], row1, col1, row2, col2);
			if (index == 0) {
				HalOpenCV::hal2cvImg(m_param->SegImg[0], *m_module->p_segImg, true);
				//setDetectOutData(OutSegImg, (Hlong)&m_param->SegImg[0]);
			}
		}
		else {
			gen_empty_obj(&m_param->SegImg[index]);
			QMessageBox::warning(this, tr("Warning"), tr("no image for setting template!"),
				QMessageBox::Yes);
		}
		m_param->MultPatternEnable[index] = false;
		ui->lb_patternState->setText(tr("template unset"));
		QString str1 = ui->bt_selectRoi->text();
		//log(LogL1, str1);
	}
	m_module->setParamChanged(ProductLevel);
}

void mark1Wdg::on_bt_selectCancel_clicked()
{
	if (m_selectState == UnSel)
		return;
	if (m_selectState == SelPattern) {
		ui->bt_selectPattern->setEnabled(true);
	}
	else if (m_selectState == SelRoiRegion) {
		ui->bt_selectRoi->setEnabled(true);
	}
	m_selectState = UnSel;
	m_advanceDialog->ui.bt_testCharThreshold->setEnabled(true);
	ui->bt_selectOk->setVisible(0);
	ui->bt_selectCancel->setVisible(0);
	currentView()->finishDraw();
	//currentView()->clear();
	currentView()->clearLayer(m_layerIdx);
	currentView()->update();
}

void mark1Wdg::on_bt_studyTmp_clicked()
{
	if (!m_param->autoSegment) {
		return;
	}
	if (m_selectState != UnSel)
		return;
	bool imgok = true;
	if (m_param->SegImg[m_param->NowPatternNum].Id() == H_EMPTY_REGION) {
		imgok = false;
	}
	else {
		Hlong imglen;
		count_obj(m_param->SegImg[m_param->NowPatternNum], &imglen);
		if (imglen == 0) {
			imgok = false;
		}
	}
	if (!imgok) {
		QMessageBox::information(this, tr("Hint"), tr("The current template is not available, please set the template first! "),
			QMessageBox::Yes);
		return;
	}
	if (ui->bt_viewTemp->isChecked()) {
		ui->bt_viewTemp->setChecked(0);
	}
	m_thresholdWidget->IsLoading(true);
	m_thresholdWidget->setMinArea(m_param->minArea);
	m_thresholdWidget->setCharCounts(m_param->charCounts[m_param->NowPatternNum]);
	m_thresholdWidget->setThreshold(m_param->threshold);
	m_thresholdWidget->setCharTOL(m_param->CharDefectTOL);
	m_thresholdWidget->setCharDir(m_param->HorOrVer);
	m_thresholdWidget->setCharGap(m_param->charGap);
	m_thresholdWidget->setMinHoles(m_param->minHoles);
	m_thresholdWidget->IsLoading(false);
	if (currentView())
		//currentView()->clear();
		currentView()->clearLayer(m_layerIdx);
	currentView()->setColor(255, 255, 0);
	int index = m_param->NowPatternNum;
	currentView()->dispRect1(
		m_param->RegionRoi[index].row1, m_param->RegionRoi[index].col1,
		m_param->RegionRoi[index].row2, m_param->RegionRoi[index].col2, "searchROI");
	currentView()->update();
	m_thresholdWidget->show();
	m_sPoint.clear();
	thresholdWidget_thresholdAndArea_changed(m_param->threshold, m_param->minArea, m_param->CharDefectTOL, m_param->HorOrVer, m_param->charGap, m_param->minHoles);
// 	if (!m_connectView) {
// 		m_connectView = true;
// 		connectOrNot(link, currentView(), SIGNAL(mousePressed(int, int, int, double, double)), SLOT(viewMousePressed(int, int, int, double, double)));
// 	}
	m_selectState = StudyTmp;
}

void mark1Wdg::bt_clearTmp_clicked()
{
	if (QMessageBox::warning(this, tr("warning"), tr("Are you sure to delete all templates for the current group?"),
		QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
	{
		m_module->clearTemp(m_param->NowPatternNum);
	}
}

void mark1Wdg::on_bt_setCharCounts_clicked()
{
	if (m_param->NowPatternNum != 0 && ((!m_param->IsMult || (m_param->IsMult && m_param->IsFontOrLayer == 0)))) {
		QMessageBox::warning(this, tr("Hint"),
			tr("Only allowed to set the number of characters at template 1!"),
			QMessageBox::Yes);
		return;
	}
	mark1Param::segRegion seg;
	seg.row1 = seg.col1 = 100;
	seg.row2 = seg.col2 = 150;  //给新添加的roi固定的开始位1置
	int holenum = 0;
	int connnum = 1;
	int stindex = m_param->NowPatternNum;
	int endindex = m_param->NowPatternNum + 1;
	if (m_param->IsMult&&m_param->IsFontOrLayer == 0) {
		stindex = 0;endindex = m_param->PatternNum;
	}
	if (ui->sp_charCounts->value() >= m_param->charCounts[m_param->NowPatternNum]) {
		for (int i = m_param->charCounts[m_param->NowPatternNum];i < ui->sp_charCounts->value();i++) {
			seg.row1 = seg.col1 += 10;
			seg.row2 = seg.col2 += 10;
			for (int j = stindex;j < endindex;j++) {
				m_param->segRegionVector[j].push_back(seg);
			}
			m_advanceDialog->ui.cob_charIndex->addItem(tr("Char%1").arg(i + 1));
		}
	}
	else if (ui->sp_charCounts->value() < m_param->charCounts[m_param->NowPatternNum]) {
		for (int j = stindex;j < endindex;j++) {
			vector<mark1Param::segRegion>::iterator itbegin = m_param->segRegionVector[j].begin() + ui->sp_charCounts->value();
			vector<mark1Param::segRegion>::iterator itEnd = m_param->segRegionVector[j].end();
			m_param->segRegionVector[j].erase(itbegin, itEnd);
		}
		connectSlots(false);
		for (int i = m_param->charCounts[m_param->NowPatternNum];i > ui->sp_charCounts->value();i--) {
			m_advanceDialog->ui.cob_charIndex->removeItem(i - 1);
		}
		connectSlots(true);
	}
	for (int i = stindex;i < endindex;i++) {
		QString str0 = ui->label_21->text();
		QString str1 = QString("%1").arg(QString::number(m_param->charCounts[i]));
		m_param->charCounts[i] = ui->sp_charCounts->value();
		QString str2 = QString("%1").arg(QString::number(m_param->charCounts[i]));
		//log(LogL1, str0 + str1 + QString("->") + str2);
	}
	if (m_param->charCounts[m_param->NowPatternNum] > 0) {
		m_advanceDialog->ui.cob_charIndex->setCurrentIndex(0);
		//m_advanceDialog->ui.sp_conns->setEnabled(1);
		//m_advanceDialog->ui.sp_holes->setEnabled(1);
	}
	else if (m_param->charCounts[m_param->NowPatternNum] == 0) {
		//m_advanceDialog->ui.sp_conns->setEnabled(0);
		//m_advanceDialog->ui.sp_holes->setEnabled(0);
	}
	for (int i = stindex;i < endindex;i++) {
		m_param->MultPatternEnable[i] = 0;
	}
	if (m_selectState == SelPattern)
	{
		currentView()->finishDraw();
		//currentView()->clear();
		currentView()->clearLayer(m_layerIdx);
		currentView()->setColor(255, 255, 0);
		currentView()->dispRect1(
			m_param->RegionRoi[m_param->NowPatternNum].row1, m_param->RegionRoi[m_param->NowPatternNum].col1,
			m_param->RegionRoi[m_param->NowPatternNum].row2, m_param->RegionRoi[m_param->NowPatternNum].col2, "搜寻ROI");
		currentView()->setDrawColor(0, 255, 0);
		double row1[30], col1[30], row2[30], col2[30];
		QStringList title;
		for (int i = 0;i < m_param->charCounts[m_param->NowPatternNum];i++) {
			row1[i] = m_param->segRegionVector[m_param->NowPatternNum][i].row1;
			row2[i] = m_param->segRegionVector[m_param->NowPatternNum][i].row2;
			col1[i] = m_param->segRegionVector[m_param->NowPatternNum][i].col1;
			col2[i] = m_param->segRegionVector[m_param->NowPatternNum][i].col2;
			title << QString("%1").arg(i + 1);
		}
		currentView()->drawRects1(m_param->charCounts[m_param->NowPatternNum], row1, col1, row2, col2, title);
		currentView()->update();
	}
	m_module->setParamChanged(ProductLevel);
}

void mark1Wdg::bt_testCharThreshold_toggled(bool check)
{
	if (check) {
		m_advanceDialog->ui.bt_testCharThreshold->setText(tr("exit test"));
		showCharRegion();
	}
	else {
		m_advanceDialog->ui.bt_testCharThreshold->setText(tr("test char"));
		//currentView()->clear();
		currentView()->clearLayer(m_layerIdx);
		currentView()->update();
	}
}

void mark1Wdg::bt_saveImageDir_clicked()
{
	QString dir = QFileDialog::getExistingDirectory(this, QString(), 
		QString::fromLocal8Bit(m_param->AutoSaveTmpImgDir));
	if (dir.size() > 0) {
		sprintf(m_param->AutoSaveTmpImgDir, dir.toLocal8Bit().constData());
		//log(m_permission, tr("Change image directory to") + dir);
	}
}

void mark1Wdg::on_bt_high_clicked(bool check)
{
	if (m_advanceDialog->exec() == QDialog::Accepted) {
	}
}

void mark1Wdg::on_bt_dir_clicked()
{
	if (m_orientDialog->exec() == QDialog::Accepted) {
	}
}

void mark1Wdg::on_bt_viewTemp_clicked(bool val)
{
	m_showWidget->show();
	m_showWidget->showTemp();
}

void mark1Wdg::check_box_toggled(bool check)
{
	QCheckBox* cb = qobject_cast<QCheckBox*>(sender());
	if (cb == ui->cb_autoSegmen)
	{
		if (m_selectState != UnSel) {
			connectSlots(false);
			ui->cb_autoSegmen->setChecked(m_param->autoSegment);
			connectSlots(true);
			return;
		}
		m_param->autoSegment = check;
		//ui.bt_studyTmp->setEnabled(check);
		ui->bt_studyTmp->setVisible(check);
		//ui.sp_charCounts->setEnabled(!check);
		//ui.bt_setCharCounts->setEnabled(!check);
		ui->bt_selectPattern->setVisible(!m_param->autoSegment);
		ui->bt_setCharCounts->setVisible(!m_param->autoSegment);

	}
	else if (cb == ui->cb_whiteChar)
	{
		m_param->whiteChar = check;
	}
	else if (cb == ui->cb_charOffSet)
	{
		m_param->charOffsetValid = check;
	}
	else if (cb == ui->cb_searchAngle)
	{
		m_param->searchAngleValid = check;
		//ui.sp_searchAngle->setEnabled(check);
	}
	else if (cb == ui->cb_errsAngle) {
		m_param->errAngleValid = check;
		//ui.sp_errAngle->setEnabled(check);
	}
	else if (cb == ui->cb_errOffSet) {
		m_param->errOffSetValid = check;
		//ui.sp_errOffset->setEnabled(check);
	}
	else if (cb == ui->cb_edgeDisX) {
		m_param->errOffXValid = check;
		//ui.sp_errOffset->setEnabled(check);
	}
	else if (cb == ui->cb_edgeDisY) {
		m_param->errOffYValid = check;
		//ui.sp_errOffset->setEnabled(check);
	}
	else if (cb == ui->cb_errArea) {
		m_param->errAreaValid = check;
		//ui.sp_errArea->setEnabled(check);
	}
	else if (cb == m_advanceDialog->ui.cb_useMult) {
		m_param->IsMult = check;
		ui->cob_multPattern->setVisible(check);
		ui->cob_multPattern->setCurrentIndex(0);
		m_advanceDialog->ui.grp_multPat->setVisible(check);
	}
	else if (cb == m_advanceDialog->ui.cb_DirPattern) {
		m_param->IsDirPattern = check;
		m_advanceDialog->ui.grp_dirPattern->setVisible(check);
		m_module->creatNccTemp(true);
		for (int i = 0;i < m_param->PatternNum;i++) {
			if (m_param->MultPatternEnable[i]) {
				m_module->createRegionTemp(i);
			}
		}
	}
	else if (cb == m_advanceDialog->ui.cb_fillUp) {
		m_param->IsFillUp = check;
	}
	else if (cb == m_advanceDialog->ui.cb_inputRatio) {
		m_param->IsInputRatio = check;
	}
	else if (cb == m_advanceDialog->ui.cb_referTempAngle) {
		m_param->IsReferTempAngle = check;
	}
	else if (cb == m_advanceDialog->ui.cb_ocr) {
		m_param->OcrValid = check;
		m_advanceDialog->ui.cb_useMult->setChecked(false);
	}
	else if (cb == m_orientDialog->ui.cb_useDir) {
		m_param->IsUsePinDirOut = check;
		//ui.grp_dir->setVisible(check);
		if (check) {
			if (!m_param->IsDirPattern) {
				m_advanceDialog->ui.cb_DirPattern->setChecked(1);
			}
			if (m_param->IsReferSection != 2) {
				m_advanceDialog->ui.cob_refer->setCurrentIndex(2);
			}
		}
	}
	else if (cb == m_advanceDialog->ui.cb_inputPatIndex) {
		m_param->IsInputPatIndex = check;
	}
	else if (cb == ui->cb_errArea) {
		m_param->errAreaValid = check;
	}
	else if (cb == ui->cb_excess) {
		m_param->errExcessValid = check;
	}
	else if (cb == ui->cb_crack) {
		m_param->errCrackValid = check;
	}
	QString str1 = cb->text();
	QString str2 = QString("");
	if (check)
		str2 = tr("Check");
	else
		str2 = tr("unCheck");
	//log(LogL1, str1 + str2);
	m_module->setParamChanged(ProductLevel);
}

void mark1Wdg::doubleSpinBox_valueChanged(double val)
{
	QDoubleSpinBox* dsp = qobject_cast<QDoubleSpinBox*>(sender());
	QString str1;
	QString str2;
	if (dsp == ui->dsp_crack) {
		str1 = ui->cb_crack->text();
		str2 = tr(" change from %1 to %2").arg(QString::number(m_param->errCrack)).arg(QString::number(val));
		m_param->errCrack = val;
	}
	else if (dsp == ui->dsp_excess) {
		str1 = ui->cb_excess->text();
		str2 = tr(" change from %1 to %2").arg(QString::number(m_param->errExcess)).arg(QString::number(val));
		m_param->errExcess = val;
	}
	else if (dsp == m_advanceDialog->ui.dsp_ratio) {
		str1 = m_advanceDialog->ui.cb_inputRatio->text();
		str2 = tr(" change from %1 to %2").arg(QString::number(m_param->pixelSize)).arg(QString::number(val));
		m_param->pixelSize = val;
	}
	//log(LogL1, str1 + str2);
	m_module->setParamChanged(ProductLevel);
}

void mark1Wdg::spinBox_valueChanged(int val)
{
	QSpinBox* sp = qobject_cast<QSpinBox*>(sender());
	QString str1;
	QString str2;
	if (sp == ui->sp_charOffset) {
		str1 = ui->cb_charOffSet->text();
		str2 = tr(" change from %1 to %2").arg(QString::number(m_param->charOffset)).arg(QString::number(val));
		m_param->charOffset = val;
	}
	else if (sp == ui->sp_searchAngle) {
		str1 = ui->cb_searchAngle->text();
		str2 = tr(" change from %1 to %2").arg(QString::number(m_param->searchAngle)).arg(QString::number(val));
		m_param->searchAngle = val;
	}
	else if (sp == ui->sp_acceptSimilar) {
		str1 = ui->label_22->text();
		str2 = tr(" change from %1 to %2").arg(QString::number(m_param->acceptSimilar)).arg(QString::number(val));
		m_param->acceptSimilar = val;
	}
	else if (sp == ui->sp_errAngle) {
		str1 = ui->cb_errsAngle->text();
		str2 = tr(" change from %1 to %2").arg(QString::number(m_param->errAngle)).arg(QString::number(val));
		m_param->errAngle = val;
	}
	else if (sp == ui->sp_errOffset) {
		str1 = ui->cb_errOffSet->text();
		str2 = tr(" change from %1 to %2").arg(QString::number(m_param->errOffSet)).arg(QString::number(val));
		m_param->errOffSet = val;
	}
	else if (sp == ui->sp_edgeDisXMin) {
		str1 = ui->cb_edgeDisX->text();
		str2 = tr(" min value change from %1 to %2").arg(QString::number(m_param->errOffXMin)).arg(QString::number(val));
		m_param->errOffXMin = val;
	}
	else if (sp == ui->sp_edgeDisYMin) {
		str1 = ui->cb_edgeDisY->text();
		str2 = tr(" min value change from %1 to %2").arg(QString::number(m_param->errOffYMin)).arg(QString::number(val));
		m_param->errOffYMin = val;
	}
	else if (sp == ui->sp_edgeDisXMax) {
		str1 = ui->cb_edgeDisX->text();
		str2 = tr(" max value change from %1 to %2").arg(QString::number(m_param->errOffXMax)).arg(QString::number(val));
		m_param->errOffXMax = val;
	}
	else if (sp == ui->sp_edgeDisYMax) {
		str1 = ui->cb_edgeDisY->text();
		str2 = tr(" max value change from %1 to %2").arg(QString::number(m_param->errOffYMax)).arg(QString::number(val));
		m_param->errOffYMax = val;
	}
	else if (sp == m_advanceDialog->ui.sp_AutoRegionLarge) {
		str1 = m_advanceDialog->ui.label_23->text();
		str2 = tr(" change from %1 to %2").arg(QString::number(m_param->autoRegionLarge)).arg(QString::number(val));
		m_param->autoRegionLarge = val;
	}
	else if (sp == m_advanceDialog->ui.sp_charThreMin) {
		str1 = m_advanceDialog->ui.label_24->text();
		str2 = tr(" change from %1 to %2").arg(QString::number(m_param->charThreMin)).arg(QString::number(val));
		m_param->charThreMin = val;
		if (m_advanceDialog->ui.bt_testCharThreshold->isChecked()) {
			showCharRegion();
		}
	}
	else if (sp == m_advanceDialog->ui.sp_charThreMax) {
		str1 = m_advanceDialog->ui.label_24->text();
		str2 = tr(" change from %1 to %2").arg(QString::number(m_param->charThreMax)).arg(QString::number(val));
		m_param->charThreMax = val;
		if (m_advanceDialog->ui.bt_testCharThreshold->isChecked()) {
			showCharRegion();
		}
	}
	else if (sp == m_advanceDialog->ui.sp_defectTOL) {
		str1 = m_advanceDialog->ui.label_3->text();
		str2 = tr(" change from %1 to %2").arg(QString::number(m_param->CharDefectTOL)).arg(QString::number(val));
		m_param->CharDefectTOL = val;
	}
	else if (sp == ui->sp_errArea) {
		str1 = ui->cb_errArea->text();
		str2 = tr(" change from %1 to %2").arg(QString::number(m_param->errAreaPercent)).arg(QString::number(val));
		m_param->errAreaPercent = val;
	}
	else if (sp == m_advanceDialog->ui.sp_patternNum) {
		str1 = m_advanceDialog->ui.label_4->text();
		str2 = tr(" change from %1 to %2").arg(QString::number(m_param->PatternNum)).arg(QString::number(val));
		if (m_param->PatternNum >= val) {
			for (int i = val;i < m_param->PatternNum;i++) {
				m_param->segRegionVector[i].clear();
				m_param->MultPatternEnable[i] = false;
			}
		}
		else {
			int holenum = 0;
			int connnum = 1;
			mark1Param::segRegion seg;
			seg.row1 = seg.col1 = 100;
			seg.row2 = seg.col2 = 150;
			for (int i = m_param->PatternNum;i < val;i++) {
				m_param->charCounts[i] = m_param->charCounts[0];
				seg.row1 = seg.col1 += 10;
				seg.row2 = seg.col2 += 10;
				for (int j = 0;j < m_param->charCounts[i];j++) {
					m_param->segRegionVector[i].push_back(seg);
				}
				m_param->MultPatternEnable[i] = false;
			}
		}
		m_param->PatternNum = val;
		connectSlots(false);
		ui->cob_multPattern->clear();
		for (int i = 0;i < val;i++) {
			ui->cob_multPattern->addItem(QString::fromLocal8Bit("模板%1").arg(i + 1));
		}
		connectSlots(true);
		ui->cob_multPattern->setCurrentIndex(0);
	}
	m_module->setParamChanged(ProductLevel);
	//if (sp !=ui->sp_charCounts)  //字符在点“设入”时，才算记录
	//	log(LogL1, str1 + str2);
}

void mark1Wdg::comboBox_Index_Changed(int index)
{
	QComboBox* cob = qobject_cast<QComboBox*>(sender());
	if (cob == ui->cob_multPattern) {
		if (ui->bt_selectRoi->isChecked() || m_selectState == SelPattern || m_selectState == StudyTmp) {
			connectSlots(false);
			ui->cob_multPattern->setCurrentIndex(m_param->NowPatternNum);
			connectSlots(true);
			return;
		}
		m_param->NowPatternNum = index;
		if (m_param->MultPatternEnable[m_param->NowPatternNum]) {
			ui->lb_patternState->setText(tr("template set"));
		}
		else {
			ui->lb_patternState->setText(tr("template unset"));
		}
		connectSlots(false);
		ui->sp_charCounts->setValue(m_param->charCounts[m_param->NowPatternNum]);
		m_advanceDialog->ui.cob_charIndex->clear();
		for (int i = 0;i < m_param->charCounts[index];i++) {
			m_advanceDialog->ui.cob_charIndex->addItem(tr("Char%1").arg(i + 1));
		}
		connectSlots(true);
		if (m_showWidget->isVisible()) {
			m_showWidget->showTemp();
		}
		m_advanceDialog->ui.cob_charIndex->setCurrentIndex(0);
	}
	else if (cob == m_advanceDialog->ui.cob_charIndex) {
		m_showPatIndex = index;
	}
	else if (cob == m_advanceDialog->ui.cob_refer) {
		if ((m_param->IsUsePinDirOut&&index != 2) || (m_param->IsMult&&m_param->IsFontOrLayer == 1 && index != 2)) {
			connectSlots(false);
			m_advanceDialog->ui.cob_refer->setCurrentIndex(2);
			connectSlots(true);
			return;
		}

		QString str0 = m_advanceDialog->ui.label_25->text();
		QString str1 = tr(" change from %1 to %2")
			.arg(m_advanceDialog->ui.cob_refer->itemText(m_param->IsReferSection))
			.arg(m_advanceDialog->ui.cob_refer->currentText());
		m_param->IsReferSection = index;

		//log(LogL1, str0 + str1);
	}
	else if (cob == m_advanceDialog->ui.cob_area) {
		QString str0 = m_advanceDialog->ui.label_6->text();
		QString str1 = tr(" change from %1 to %2")
			.arg(m_advanceDialog->ui.cob_area->itemText(m_param->areaType))
			.arg(m_advanceDialog->ui.cob_area->currentText());
		m_param->areaType = index;
		//log(LogL1, str0 + str1);
	}
	m_module->setParamChanged(ProductLevel);
}

void mark1Wdg::radioButton_toggled(bool check)
{
	QRadioButton* rb = qobject_cast<QRadioButton*>(sender());

	QString str1 = QString("");
	QString str2 = QString("");
	if (check) {
		str1 = rb->text();
		if (rb == m_advanceDialog->ui.rb_2Angle) {
			str2 = tr(" change from %1 to %2")
				.arg(m_advanceDialog->ui.rb_4Angle->text())
				.arg(m_advanceDialog->ui.rb_2Angle->text());
			m_param->angleType = 0;
			if (m_param->IsDirPattern) {
				m_module->creatNccTemp(true);
				for (int i = 0;i < m_param->PatternNum;i++) {
					if (m_param->MultPatternEnable[i]) {
						m_module->createRegionTemp(i);
					}
				}
			}
			if (m_param->IsUsePinDirOut) {
				if (m_param->Is2Dor4D != m_param->angleType) {
					//ui.rb_2D->setChecked(1);
				}

			}
		}
		else if (rb == m_advanceDialog->ui.rb_4Angle) {
			str2 = tr(" change from %1 to %2")
				.arg(m_advanceDialog->ui.rb_2Angle->text())
				.arg(m_advanceDialog->ui.rb_4Angle->text());
			m_param->angleType = 1;
			if (m_param->IsDirPattern) {
				m_module->creatNccTemp(true);
				for (int i = 0;i < m_param->PatternNum;i++) {
					if (m_param->MultPatternEnable[i]) {
						m_module->createRegionTemp(i);
					}
				}
			}
			if (m_param->IsUsePinDirOut) {
				if (m_param->Is2Dor4D != m_param->angleType) {
					//ui.rb_4D->setChecked(1);
				}

			}
		}
		else if (rb == m_advanceDialog->ui.rb_font) {
			str2 = tr(" change from %1 to %2")
				.arg(m_advanceDialog->ui.rb_layer->text())
				.arg(m_advanceDialog->ui.rb_font->text());
			m_param->IsFontOrLayer = 0;
		}
		else if (rb == m_advanceDialog->ui.rb_layer) {
			str2 = tr(" change from %1 to %2")
				.arg(m_advanceDialog->ui.rb_font->text())
				.arg(m_advanceDialog->ui.rb_layer->text());
			m_param->IsFontOrLayer = 1;
			if (m_param->IsMult) {
				if (m_param->IsReferSection != 2) {
					m_advanceDialog->ui.cob_refer->setCurrentIndex(2);
				}
			}
		}
		m_module->setParamChanged(ProductLevel);
		//log(LogL1, str1 + str2);
	}
}

void mark1Wdg::thresholdWidget_thresholdAndArea_changed(int thre, int minarea, int tol, int horOrVer, int charGap, int minHole)
{
	//currentView()->clear();
	currentView()->clearLayer(m_layerIdx);
	m_tmpThreshold = thre;
	m_tmpMinArea = minarea;
	m_tmpTol = tol;
	m_tmpHorOrVer = horOrVer;
	m_tmpCharGap = charGap;
	m_tmpMinHole = minHole;

	if (m_module->p_im->empty())
		return;
	HalOpenCV::cv2halImg(*m_module->p_im, m_image, false);

	Hobject threRect, tmpImage, tmpResult;
	int index = m_param->NowPatternNum;
	gen_rectangle1(&threRect, m_param->RegionRoi[index].row1, m_param->RegionRoi[index].col1,
		m_param->RegionRoi[index].row2, m_param->RegionRoi[index].col2);
	reduce_domain(*m_image, threRect, &tmpImage);
	if (m_param->whiteChar) {
		threshold(tmpImage, &tmpImage, m_tmpThreshold, 255);
	}
	else {
		threshold(tmpImage, &tmpImage, 0, m_tmpThreshold);
	}
	if (m_tmpTol > 0) {
		closing_rectangle1(tmpImage, &tmpImage, m_tmpTol, m_tmpTol);
	}
	if (m_tmpCharGap) {
		if (m_tmpHorOrVer == 0) {   //水平字符
			closing_rectangle1(tmpImage, &tmpImage, m_tmpCharGap, 1);
		}
		else if (m_tmpHorOrVer == 1) { //竖直字符
			closing_rectangle1(tmpImage, &tmpImage, 1, m_tmpCharGap);
		}
	}
	connection(tmpImage, &tmpImage);
	char tmpbuf[32];
	if (m_tmpHorOrVer == 1) {  //垂直
		sprintf(tmpbuf, "column");
	}
	else {
		sprintf(tmpbuf, "row");
	}
	sort_region(tmpImage, &tmpImage, "character", "true", tmpbuf);
	Hlong recnum;
	count_obj(tmpImage, &recnum);
	HTuple urow1, ucol1, urow2, ucol2;
	smallest_rectangle1(tmpImage, &urow1, &ucol1, &urow2, &ucol2);
	Hobject urec1;
	if (recnum == 0) {
		currentView()->update();
		return;
	}
	gen_rectangle1(&urec1, urow1, ucol1, urow2, ucol2);
	int recCount = 0;
	HTuple row1, col1, row2, col2;
	HTuple conns, holes;
	Hobject munionrec;
	for (int i = 0;i < recnum;i++) {
		Hobject recreg;
		gen_rectangle1(&recreg, urow1[i].D(), ucol1[i].D(), urow2[i].D(), ucol2[i].D());
		reduce_domain(*m_image, recreg, &recreg);
		if (m_param->whiteChar) {
			threshold(recreg, &recreg, m_tmpThreshold, 255);
		}
		else {
			threshold(recreg, &recreg, 0, m_tmpThreshold);
		}
		if (m_tmpTol > 0) {
			closing_rectangle1(tmpImage, &tmpImage, m_tmpTol, m_tmpTol);
		}
		connection(recreg, &recreg);
		select_shape(recreg, &recreg, "area", "and", m_tmpMinArea, 1000000);
		if (m_tmpMinHole) {
			fill_up_shape(recreg, &recreg, "area", 0, m_tmpMinHole);
		}
		HTuple tcon, tho;
		connect_and_holes(recreg, &tcon, &tho);
		union2(munionrec, recreg, &munionrec);
		char buf[32];
		if (m_tmpHorOrVer == 1) {  //垂直
			sprintf(buf, "row");
		}
		else {
			sprintf(buf, "column");
		}
		sort_region(recreg, &recreg, "character", "true", buf);
		HTuple slrow1, slrow2, slcol1, slcol2;
		smallest_rectangle1(recreg, &slrow1, &slcol1, &slrow2, &slcol2);
		Hlong unum;
		count_obj(recreg, &unum);
		tuple_concat(row1, slrow1, &row1);
		tuple_concat(col1, slcol1, &col1);
		tuple_concat(row2, slrow2, &row2);
		tuple_concat(col2, slcol2, &col2);
		tuple_concat(conns, tcon, &conns);
		tuple_concat(holes, tho, &holes);
		recCount += unum;
	}
	currentView()->setColor(0, 0, 0);
	currentView()->setFilled(true);
	currentView()->dispRect1(
		m_param->RegionRoi[index].row1, m_param->RegionRoi[index].col1,
		m_param->RegionRoi[index].row2, m_param->RegionRoi[index].col2);
	currentView()->setFilled(false);
	currentView()->setColor(255, 255, 0);
	currentView()->dispRect1(
		m_param->RegionRoi[index].row1, m_param->RegionRoi[index].col1,
		m_param->RegionRoi[index].row2, m_param->RegionRoi[index].col2, "searchROI");
	currentView()->setFilled(true);
	currentView()->setColor(255, 255, 255);

	Hlong imagenun;
	count_obj(munionrec, &imagenun);

	m_tmpHoles.clear();
	if (imagenun > 0) {
		Hobject showimg;
		union1(munionrec, &showimg);
		RlRegion reg;
		HalOpenCV::hal2cvRegion(showimg, &reg);
		currentView()->dispRegion(&reg);
	}
	int countNum = 0;
	m_smallRecRow1.Reset();
	m_smallRecRow2.Reset();
	m_smallRecCol1.Reset();
	m_smallRecCol2.Reset();
	for (int i = 0;i < row1.Num();i++) {
		bool thisFit = false;
		for (int j = 0;j < m_sPoint.size();j++) {
			bool pok = checkPointInside(row1[i].D(), col1[i].D(), row2[i].D(), col2[i].D(), m_sPoint[j].srow, m_sPoint[j].scol);
			if (pok) {
				thisFit = pok;
				break;
			}
		}
		if (!thisFit) {  //不在区域内
			m_smallRecRow1[countNum] = row1[i].D();
			m_smallRecCol1[countNum] = col1[i].D();
			m_smallRecRow2[countNum] = row2[i].D();
			m_smallRecCol2[countNum] = col2[i].D();
			countNum++;

			int holenum = holes[i].I();
			int conn = 1;
			m_tmpHoles.push_back(holenum);
			m_tmpConns.push_back(conn);

		}
	}
	m_thresholdWidget->setCharCounts(countNum);
	currentView()->setFilled(false);
	currentView()->setColor(255, 255, 0);

	for (int i = 0;i < countNum;i++) {
		char buf[32];
		sprintf(buf, "%d", i + 1);
		currentView()->dispRect1(m_smallRecRow1[i].D(), m_smallRecCol1[i].D(), m_smallRecRow2[i].D(), m_smallRecCol2[i].D(), buf);
	}
	currentView()->update();
}

void mark1Wdg::thresholdWidgetAccept()
{
	m_selectState = UnSel;
	int thre, area, tol, hororver, chargap, minholes;
	int index = m_param->NowPatternNum;
	m_thresholdWidget->getThresholdAndArea(thre, area, tol, hororver, chargap, minholes);
	if (!m_param->IsMult || (m_param->IsMult&&m_param->NowPatternNum == 0) || (m_param->IsMult&&m_param->IsFontOrLayer == 1)) {
		m_param->threshold = thre;
		m_param->minArea = area;
		m_param->CharDefectTOL = tol;
		m_param->charGap = chargap;
		m_param->minHoles = minholes;
		m_param->HorOrVer = hororver;
		m_advanceDialog->ui.sp_defectTOL->setValue(m_param->CharDefectTOL);
	}
	int regionCount;
	regionCount = m_smallRecRow1.Num();
	if (m_param->IsMult&&m_param->NowPatternNum != 0 && m_param->IsFontOrLayer == 0 && (m_param->charCounts[0] != regionCount)) {
		//if(QMessageBox::information(this, tr("Hint"), tr("字符数与模板一字符数不符，请重新设置！"),
		if (QMessageBox::information(this, tr("Hint"), tr("In Multiple Fonts Model Now.The number of characters doesn't match the number of characters in the template 1.Please reset!"),
			QMessageBox::Yes) == QMessageBox::Yes)
			return;
		return;
	}
	m_param->charCounts[index] = regionCount > MAX_CHAR_NUM ? MAX_CHAR_NUM : regionCount;
	if (m_param->IsMult && m_param->IsFontOrLayer == 0 && m_param->NowPatternNum == 0) {
		for (int i = 0;i < m_param->PatternNum;i++) {
			m_param->charCounts[i] = regionCount > MAX_CHAR_NUM ? MAX_CHAR_NUM : regionCount;
		}
	}
	m_advanceDialog->ui.cob_charIndex->setCurrentIndex(0);
	if (!m_param->IsMult || (m_param->IsMult&&m_param->NowPatternNum == 0 && m_param->IsFontOrLayer == 0)) {
		for (int i = 0;i < m_param->PatternNum;i++) {
			vector<mark1Param::segRegion>::iterator itbegin = m_param->segRegionVector[i].begin();
			vector<mark1Param::segRegion>::iterator itEnd = m_param->segRegionVector[i].end();
			m_param->segRegionVector[i].erase(itbegin, itEnd);
		}
	}
	else {
		vector<mark1Param::segRegion>::iterator itbegin = m_param->segRegionVector[m_param->NowPatternNum].begin();
		vector<mark1Param::segRegion>::iterator itEnd = m_param->segRegionVector[m_param->NowPatternNum].end();
		m_param->segRegionVector[m_param->NowPatternNum].erase(itbegin, itEnd);
	}

	if (m_param->charCounts[index] >= ui->sp_charCounts->value()) {
		connectSlots(false);
		for (int i = ui->sp_charCounts->value();i < m_param->charCounts[index];i++) {
			m_advanceDialog->ui.cob_charIndex->addItem(tr("Char%1").arg(i + 1));
		}
		connectSlots(true);
	}
	else if (m_param->charCounts[index] < ui->sp_charCounts->value()) {
		connectSlots(false);
		for (int i = ui->sp_charCounts->value();i > m_param->charCounts[index];i--) {
			m_advanceDialog->ui.cob_charIndex->removeItem(i - 1);
		}
		connectSlots(true);
	}
	if (m_param->charCounts[index] > 0) {
		m_advanceDialog->ui.cob_charIndex->setCurrentIndex(0);
		//m_advanceDialog->ui.sp_conns->setEnabled(1);
		//m_advanceDialog->ui.sp_holes->setEnabled(1);
	}
	else if (m_param->charCounts[index] == 0) {
		//m_advanceDialog->ui.sp_conns->setEnabled(0);
		//m_advanceDialog->ui.sp_holes->setEnabled(0);
	}
	ui->sp_charCounts->setValue(m_param->charCounts[index]);


	mark1Param::segRegion seg;
	for (int i = 0;i < m_param->charCounts[index];i++) {
		int lg = m_param->autoRegionLarge;
		seg.row1 = (m_smallRecRow1[i].D() - lg) < m_param->RegionRoi[m_param->NowPatternNum].row1 ? m_param->RegionRoi[m_param->NowPatternNum].row1 : (m_smallRecRow1[i].D() - lg);
		seg.col1 = (m_smallRecCol1[i].D() - lg) < m_param->RegionRoi[m_param->NowPatternNum].col1 ? m_param->RegionRoi[m_param->NowPatternNum].col1 : (m_smallRecCol1[i].D() - lg);
		seg.row2 = (m_smallRecRow2[i].D() + lg) > m_param->RegionRoi[m_param->NowPatternNum].row2 ? m_param->RegionRoi[m_param->NowPatternNum].row2 : (m_smallRecRow2[i].D() + lg);
		seg.col2 = (m_smallRecCol2[i].D() + lg) > m_param->RegionRoi[m_param->NowPatternNum].col2 ? m_param->RegionRoi[m_param->NowPatternNum].col2 : (m_smallRecCol2[i].D() + lg);;
		if (!m_param->IsMult || (m_param->IsMult&&m_param->NowPatternNum == 0 && m_param->IsFontOrLayer == 0)) {
			for (int j = 0;j < m_param->PatternNum;j++) {
				m_param->segRegionVector[j].push_back(seg);
			}
		}
		else {
			m_param->segRegionVector[m_param->NowPatternNum].push_back(seg);
		}
	}
	//currentView()->clear();
	currentView()->clearLayer(m_layerIdx);
	currentView()->setFilled(false);
	currentView()->setColor(255, 255, 0);
	Hobject rec;
	for (int i = 0;i < m_param->charCounts[m_param->NowPatternNum];i++) {
		char buf[32];
		sprintf(buf, "%d", i + 1);
		currentView()->dispRect1(m_smallRecRow1[i].D(), m_smallRecCol1[i].D(), m_smallRecRow2[i].D(), m_smallRecCol2[i].D(), buf);
	}
	currentView()->update();
	m_module->creatNccTemp(false, index);
	m_advanceDialog->ui.cob_charIndex->setCurrentIndex(0);
	if (m_param->MultPatternEnable[index]) {
		m_module->createRegionTemp(index);
	}
	if (ui->bt_viewTemp->isChecked()) {
		m_showWidget->showTemp();
	}

	m_module->setParamChanged(ProductLevel);
}

void mark1Wdg::thresholdWidgetReject()
{
	m_selectState = UnSel;
}

void mark1Wdg::viewMousePressed(int button, int wRow, int wCol, double iRow, double iCol)
{
	if (m_selectState != StudyTmp || button != 2) {//右键2才起效
		return;
	}
	bool isIn = false;
	for (int i = 0;i < m_smallRecRow1.Num();i++) {
		if (checkPointInside(m_smallRecRow1[i].D(), m_smallRecCol1[i].D(), m_smallRecRow2[i].D(), m_smallRecCol2[i].D(), iRow, iCol)) {
			isIn = true;
			sPoint sp;
			sp.srow = iRow;
			sp.scol = iCol;
			m_sPoint.push_back(sp);
			break;
		}
	}
	thresholdWidget_thresholdAndArea_changed(m_tmpThreshold, m_tmpMinArea, m_tmpTol, m_tmpHorOrVer, m_tmpCharGap, m_tmpMinHole);
}
