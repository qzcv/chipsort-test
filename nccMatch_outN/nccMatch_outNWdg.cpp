#include "nccMatch_outNWdg.h"
#include "nccMatch_outNMod.h"
#include "nccMatch_outNParam.h"
#include "ui_nccMatch_outNWdg.h"
#include "ImageView.h"
#include <QMessageBox>
#include "halOpenCV.h"
#include "nccmatch.h"

nccMatch_outNWdg::nccMatch_outNWdg(int i) :
	ModSetWidget(),ui(new Ui::nccMatch_outNWdg),m_widgetType(i)
{
	m_hasConnect = false;
	ui->setupUi(this);
	iniData();
	iniUi();
	m_param = nullptr;
	m_module = nullptr;
	m_image = nullptr;
}

nccMatch_outNWdg::~nccMatch_outNWdg()
{
	delete m_patternView;
	delete ui;
}

void nccMatch_outNWdg::setModule(UnitModule *module)
{
	m_module=dynamic_cast<nccMatch_outNMod*>(module);
	m_param = m_module->m_param;
	m_image = m_module->m_image;
	//currentView()->setLayer(m_layerIdx);
	//updateUi();
}

void nccMatch_outNWdg::showEvent(QShowEvent * event)
{
	updateUi();
}

void nccMatch_outNWdg::connectSlots(bool link)
{
	if(m_hasConnect==link)
		return;
	m_hasConnect=link;

	connectOrNot(link, ui->bt_threTest, SIGNAL(toggled(bool)), SLOT(bt_toggled(bool)));

	connectOrNot(link, ui->cb_acceptValue, SIGNAL(toggled(bool)), SLOT(CheckBox_toggled(bool)));
	connectOrNot(link, ui->cb_offsetValue, SIGNAL(toggled(bool)), SLOT(CheckBox_toggled(bool)));
	connectOrNot(link, ui->cb_relativeAngle, SIGNAL(toggled(bool)), SLOT(CheckBox_toggled(bool)));
	connectOrNot(link, ui->cb_axisAngle, SIGNAL(toggled(bool)), SLOT(CheckBox_toggled(bool)));
	connectOrNot(link, ui->cb_useOutArea, SIGNAL(toggled(bool)), SLOT(CheckBox_toggled(bool)));
	connectOrNot(link, ui->cb_fillUp, SIGNAL(toggled(bool)), SLOT(CheckBox_toggled(bool)));
	connectOrNot(link, ui->cb_showOutArea, SIGNAL(toggled(bool)), SLOT(CheckBox_toggled(bool)));
	connectOrNot(link, ui->cb_showRoiAfterDet, SIGNAL(toggled(bool)), SLOT(CheckBox_toggled(bool)));
	connectOrNot(link, ui->cb_showMinusAfterDet, SIGNAL(toggled(bool)), SLOT(CheckBox_toggled(bool)));
	connectOrNot(link, ui->cb_enable, SIGNAL(toggled(bool)), SLOT(CheckBox_toggled(bool)));

	connectOrNot(link, ui->sp_similarity, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_seekingAngle, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_acceptValue, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_offsetValue, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_minRelative, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_maxRelative, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_axisAngle, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));

	connectOrNot(link, ui->cob_minusArea, SIGNAL(currentIndexChanged(int)), SLOT(comboBox_IndexChanged(int)));
	connectOrNot(link, ui->cob_temp, SIGNAL(currentIndexChanged(int)), SLOT(comboBox_IndexChanged(int)));

	connectOrNot(link, ui->sp_deleteArea, SIGNAL(valueChanged(int)), SLOT(outAreaSpinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_outArea, SIGNAL(valueChanged(int)), SLOT(outAreaSpinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_minArea, SIGNAL(valueChanged(int)), SLOT(outAreaSpinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_maxArea, SIGNAL(valueChanged(int)), SLOT(outAreaSpinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_threMin, SIGNAL(valueChanged(int)), SLOT(outAreaSpinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_threMax, SIGNAL(valueChanged(int)), SLOT(outAreaSpinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_open, SIGNAL(valueChanged(int)), SLOT(outAreaSpinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_close, SIGNAL(valueChanged(int)), SLOT(outAreaSpinBox_valueChanged(int)));
}

void nccMatch_outNWdg::iniUi()
{

}

void nccMatch_outNWdg::iniData()
{
	m_selState = UnSel;
	m_layerIdx = 0;
	m_patternView = new ImageView(ui->widget_pattern);
}

void nccMatch_outNWdg::setUiValue()
{
	if(!m_param)
		return;

	ui->cob_temp->clear();
	for (int i = 0;i < MAX_TEMP_NUM;i++) {
		ui->cob_temp->addItem(tr("temp%1").arg(i + 1));
	}
	ui->cob_temp->setCurrentIndex(m_param->currentGrp);
	if (m_selState != SelMinusRegion) {
		ui->bt_cancelMinus->setVisible(false);
		ui->bt_ensureMinus->setVisible(false);
	}
	if (m_selState != SelOutPattern) {
		ui->bt_cancelOutPat->setVisible(false);
		ui->bt_outPatOk->setVisible(false);
	}
	if (m_selState != SelPattern) {
		ui->bt_cancelSelect->setVisible(false);
		ui->bt_ensureSelect->setVisible(false);
	}
	if (m_param->minusArea[m_param->currentGrp] == 0) {
		ui->bt_drawMinus->setVisible(0);
	}
	else {
		ui->bt_drawMinus->setVisible(1);
	}
	changeTemp(m_param->currentGrp);
}

void nccMatch_outNWdg::updateUi()
{
	connectSlots(false);
	setUiValue();
	connectSlots(true);
}

void nccMatch_outNWdg::changeTemp(int index)
{
	connectSlots(false);
	ui->sp_similarity->setValue(m_param->similarity[index]);
	ui->sp_seekingAngle->setValue(m_param->seekingAngle[index]);
	ui->cob_minusArea->setCurrentIndex(m_param->minusArea[index]);
	ui->cb_acceptValue->setChecked(m_param->acceptEnable[index]);
	ui->sp_acceptValue->setValue(m_param->acceptValue[index]);
	ui->cb_offsetValue->setChecked(m_param->offsetEnable[index]);
	ui->sp_offsetValue->setValue(m_param->offsetValue[index]);
	ui->cb_relativeAngle->setChecked(m_param->relativeEnable[index]);
	ui->sp_minRelative->setValue(m_param->minRelative[index]);
	ui->sp_maxRelative->setValue(m_param->maxRelative[index]);
	ui->sp_outArea->setValue(m_param->outRegionCount[index]);
	ui->sp_deleteArea->setValue(m_param->outDeleteCount[index]);
	ui->sp_minArea->setValue(m_param->outAreaMin[index]);
	ui->sp_maxArea->setValue(m_param->outAreaMax[index]);
	ui->sp_threMin->setValue(m_param->outThreMin[index]);
	ui->sp_threMax->setValue(m_param->outThreMax[index]);
	ui->sp_open->setValue(m_param->openVal[index]);
	ui->sp_close->setValue(m_param->closeVal[index]);
	ui->cb_useOutArea->setChecked(m_param->IsUseOutArea[index]);
	if (m_param->IsUseOutArea[index]) {
		ui->groupBox_3->show();
	}
	else {
		ui->groupBox_3->hide();
	}
	ui->cb_showOutArea->setChecked(m_param->IsShowOutArea[index]);
	ui->cb_axisAngle->setChecked(m_param->IsAxisAngle[index]);
	ui->sp_axisAngle->setValue(m_param->maxAxisAngle[index]);
	//ui.sp_axisAngle->setEnabled(m_param->IsAxisAngle[index]);
	ui->cb_fillUp->setChecked(m_param->IsFillUp[index]);
	ui->cb_showRoiAfterDet->setChecked(m_param->IsShowRoiAfterDet[index]);
	ui->cb_showMinusAfterDet->setChecked(m_param->IsShowMinusAfterDet[index]);
	ui->cb_enable->setChecked(m_param->GrpEnable[index]);
	ui->tabWidget->setCurrentIndex(0);
	if (m_param->patImage[index].Id() != H_EMPTY_REGION) {
		m_patternView->setVisible(true);
		cv::Mat im;
		HalOpenCV::hal2cvImg(m_param->patImage[index], im, false);
		m_patternView->dispImage(im);
		//m_patternView->dispImage(&m_param->patImage[index]);
		m_patternView->update();
		m_patternView->fitImage();
	}
	else {
		m_patternView->setVisible(false);
	}
	connectSlots(true);
}

void nccMatch_outNWdg::drawRects()
{
	currentView()->finishDraw();
	//currentView()->clear();
	currentView()->clearLayer(m_layerIdx);
	currentView()->setDrawColor(0, 255, 0);
	double row[20], col[20], angle[20], length1[20], length2[20];
	QStringList title;
// 	const char* title[20] = {};
// 	char buf[20][32];
	for (int i = 0;i < m_param->outRegionCount[m_param->currentGrp];i++) {
		row[i] = m_param->outRegionVec[m_param->currentGrp][i].row;
		col[i] = m_param->outRegionVec[m_param->currentGrp][i].col;
		angle[i] = m_param->outRegionVec[m_param->currentGrp][i].angle;
		length1[i] = m_param->outRegionVec[m_param->currentGrp][i].length1;
		length2[i] = m_param->outRegionVec[m_param->currentGrp][i].length2;
		title << QString("outRoi%1").arg(i);
// 		sprintf(buf[i], "outRoi%d", i);
// 		title[i] = buf[i];
	}
	for (int i = 0;i < m_param->outDeleteCount[m_param->currentGrp];i++) {
		row[i + m_param->outRegionCount[m_param->currentGrp]] = m_param->deleteRegionVec[m_param->currentGrp][i].drow;
		col[i + m_param->outRegionCount[m_param->currentGrp]] = m_param->deleteRegionVec[m_param->currentGrp][i].dcol;
		angle[i + m_param->outRegionCount[m_param->currentGrp]] = m_param->deleteRegionVec[m_param->currentGrp][i].dangle;
		length1[i + m_param->outRegionCount[m_param->currentGrp]] = m_param->deleteRegionVec[m_param->currentGrp][i].dlength1;
		length2[i + m_param->outRegionCount[m_param->currentGrp]] = m_param->deleteRegionVec[m_param->currentGrp][i].dlength2;
		title << QString("outDel%1").arg(i);
		//sprintf(buf[i + m_param->outRegionCount[m_param->currentGrp]], "outDel%d", i);
		//title[i + m_param->outRegionCount[m_param->currentGrp]] = buf[i + m_param->outRegionCount[m_param->currentGrp]];
	}
	currentView()->drawRects2(m_param->outRegionCount[m_param->currentGrp] + m_param->outDeleteCount[m_param->currentGrp],
		row, col, angle, length1, length2, title);
	currentView()->update();
}

void nccMatch_outNWdg::getDrawRects()
{
	currentView()->finishDraw();
	currentView()->update();
	double row[20], col[20], angle[20], length1[20], length2[20];
	int idx = m_param->currentGrp;
	currentView()->getDrawRects2(m_param->outRegionCount[idx] + m_param->outDeleteCount[idx],
		row, col, angle, length1, length2);
	for (int i = 0;i < m_param->outRegionCount[idx];i++) {
		m_param->outRegionVec[idx][i].row = row[i];
		m_param->outRegionVec[idx][i].col = col[i];
		m_param->outRegionVec[idx][i].length1 = length1[i];
		m_param->outRegionVec[idx][i].length2 = length2[i];
		m_param->outRegionVec[idx][i].angle = angle[i];
	}
	for (int i = 0;i < m_param->outDeleteCount[idx];i++) {
		m_param->deleteRegionVec[idx][i].drow = row[i + m_param->outRegionCount[idx]];
		m_param->deleteRegionVec[idx][i].dcol = col[i + m_param->outRegionCount[idx]];
		m_param->deleteRegionVec[idx][i].dlength1 = length1[i + m_param->outRegionCount[idx]];
		m_param->deleteRegionVec[idx][i].dlength2 = length2[i + m_param->outRegionCount[idx]];
		m_param->deleteRegionVec[idx][i].dangle = angle[i + m_param->outRegionCount[idx]];
	}
}

void nccMatch_outNWdg::showRegionXld()
{
	//currentView()->clear();
	currentView()->clearLayer(m_layerIdx);
	if (m_param->OutImg[m_param->currentGrp].Id() == H_EMPTY_REGION) {
		QMessageBox::information(this, tr("Warning"), tr("The template of outArea is unset!"), QMessageBox::Yes);
		return;
	}
	Hobject outRegion;
	int t = m_param->currentGrp;
	threshold(m_param->OutImg[t], &outRegion, m_param->outThreMin[t], m_param->outThreMax[t]);
	if (m_param->openVal[t] > 0) {
		opening_circle(outRegion, &outRegion, m_param->openVal[t]);
	}
	if (m_param->closeVal[t] > 0) {
		closing_circle(outRegion, &outRegion, m_param->closeVal[t]);
	}
	connection(outRegion, &outRegion);
	select_shape(outRegion, &outRegion, "area", "and", m_param->outAreaMin[t], m_param->outAreaMax[t]);
	if (m_param->IsFillUp[t]) {
		fill_up(outRegion, &outRegion);
	}
	union1(outRegion, &outRegion);
	currentView()->setFilled(false);
	currentView()->setColor(0, 255, 255);
	RlRegion rlReg;
	HalOpenCV::hal2cvRegion(outRegion, &rlReg);
	currentView()->dispRegion(&rlReg);
	//currentView()->dispRegion(outRegion);
	currentView()->update();
}

void nccMatch_outNWdg::on_bt_selectPattern_clicked()
{
	if (m_selState != UnSel)
		return;
	m_selState = SelPattern;
	//log(LogL1, ui.bt_selectPattern->text());
	ui->bt_cancelSelect->setVisible(1);
	ui->bt_ensureSelect->setVisible(1);
	ui->bt_selectPattern->setEnabled(0);
	ui->cb_onlySearchRoi->setEnabled(0);
	//on_bt_cancelMinus_clicked();
	double row1[2], col1[2], row2[2], col2[2];
	row1[0] = m_param->patternY[m_param->currentGrp];
	row1[1] = m_param->roiy[m_param->currentGrp];
	col1[0] = m_param->patternX[m_param->currentGrp];
	col1[1] = m_param->roix[m_param->currentGrp];
	row2[0] = m_param->patternY[m_param->currentGrp] + m_param->patternH[m_param->currentGrp];
	row2[1] = m_param->roiy[m_param->currentGrp] + m_param->roih[m_param->currentGrp];
	col2[0] = m_param->patternX[m_param->currentGrp] + m_param->patternW[m_param->currentGrp];
	col2[1] = m_param->roix[m_param->currentGrp] + m_param->roiw[m_param->currentGrp];
	//const char *title[2] = { "tempRoi", "SearchRoi" };
	QStringList title = { "tempRoi", "SearchRoi" };
	currentView()->clearLayer(m_layerIdx);
	if (ui->cb_onlySearchRoi->isChecked()) {
		currentView()->drawRect1(row1[1], col1[1], row2[1], col2[1], title[1]);
	}
	else {
		currentView()->drawRects1(2, row1, col1, row2, col2, title);
	}
	currentView()->update();
}

void nccMatch_outNWdg::on_bt_ensureSelect_clicked()
{
	if (m_selState != SelPattern)
		return;
	m_selState = UnSel;
	//log(LogL1, ui.bt_ensureSelect->text());
	ui->bt_cancelSelect->setVisible(0);
	ui->bt_ensureSelect->setVisible(0);
	ui->bt_selectPattern->setEnabled(1);
	ui->cb_onlySearchRoi->setEnabled(1);
	double row1[2], col1[2], row2[2], col2[2];
	currentView()->finishDraw();
	currentView()->clearLayer(m_layerIdx);
	if (ui->cb_onlySearchRoi->isChecked()) {
		currentView()->getDrawRect1(row1[1], col1[1], row2[1], col2[1]);
	}
	else {
		currentView()->getDrawRects1(2, row1, col1, row2, col2);
	}
	int idx = m_param->currentGrp;
	if (ui->cb_onlySearchRoi->isChecked() && m_param->minusArea[m_param->currentGrp] != 0) {
		Hobject patArea, minusArea, diffArea;
		HTuple patA, diffA, arow, acol;
		gen_rectangle1(&patArea, row1[0], col1[0], row2[0], col2[0]);
		if (m_param->minusArea[idx] == 1) {
			gen_rectangle2(&minusArea, m_param->minusRow[idx], m_param->minusCol[idx], m_param->minusA[idx]
				, m_param->minusLength1[idx], m_param->minusLength2[idx]);
		}
		else {
			gen_circle(&minusArea, m_param->minusCY[idx], m_param->minusCY[idx], m_param->minusR[idx]);
		}
		difference(patArea, minusArea, &diffArea);
		area_center(patArea, &patA, &arow, &acol);
		area_center(diffArea, &diffA, &arow, &acol);
		if (diffA[0].I() < ((double)patA[0].I()*0.1)) {
			QMessageBox::warning(this, tr("warning"), tr("The exclusion area exceeds 90 % of the template area"),
				QMessageBox::Yes);
			return;
		}
	}

	m_param->roix[m_param->currentGrp] = col1[1];
	m_param->roiy[m_param->currentGrp] = row1[1];
	m_param->roih[m_param->currentGrp] = row2[1] - row1[1];
	m_param->roiw[m_param->currentGrp] = col2[1] - col1[1];
	m_module->m_detect[m_param->currentGrp]->setRoi(&QRect(
		m_param->roix[m_param->currentGrp], m_param->roiy[m_param->currentGrp],
		m_param->roiw[m_param->currentGrp], m_param->roih[m_param->currentGrp]));
	if (ui->cb_onlySearchRoi->isChecked()) {
		return;
	}
	m_param->patternX[m_param->currentGrp] = col1[0];
	m_param->patternY[m_param->currentGrp] = row1[0];
	m_param->patternH[m_param->currentGrp] = row2[0] - row1[0];
	m_param->patternW[m_param->currentGrp] = col2[0] - col1[0];
	if (m_module->p_im->empty())
		return;
	HalOpenCV::cv2halImg(*m_module->p_im, m_image, false);
	Hobject cloneImg, roiRect;
	crop_rectangle1(*m_image, &cloneImg, 
		m_param->patternY[m_param->currentGrp], 
		m_param->patternX[m_param->currentGrp],
		m_param->patternY[m_param->currentGrp] + m_param->patternH[m_param->currentGrp],
		m_param->patternX[m_param->currentGrp] + m_param->patternW[m_param->currentGrp]);
	HTuple nchannels;
	count_channels(*m_image, &nchannels);
	if (nchannels[0].I() == 3)
		rgb1_to_gray(cloneImg, &cloneImg);
	copy_image(cloneImg, &m_param->patImage[m_param->currentGrp]);
	m_patternView->setVisible(true);
	cv::Mat im;
	HalOpenCV::hal2cvImg(m_param->patImage[m_param->currentGrp], im, false);
	m_patternView->dispImage(im);
	//m_patternView->dispImage(&m_param->patImage[m_param->currentGrp]);
	m_patternView->fitImage();
	m_patternView->update();

	m_module->resetPattern(m_param->currentGrp);
	m_module->has_set_pattern[m_param->currentGrp] = 1;

	m_module->setParamChanged(QvsParamLevel::ProductLevel);
}

void nccMatch_outNWdg::on_bt_cancelSelect_clicked()
{
	if (m_selState != SelPattern)
		return;
	m_selState = UnSel;
	//log(LogL1, ui.bt_cancelSelect->text());
	ui->bt_cancelSelect->setVisible(0);
	ui->bt_ensureSelect->setVisible(0);
	ui->bt_selectPattern->setEnabled(1);
	ui->cb_onlySearchRoi->setEnabled(1);
	currentView()->finishDraw();
	currentView()->clearLayer(m_layerIdx);
}

void nccMatch_outNWdg::on_bt_drawMinus_clicked()
{
	if (m_selState != UnSel || m_param->minusArea[m_param->currentGrp] == 0)
		return;
	m_selState = SelMinusRegion;
	//log(LogL1, ui.bt_drawMinus->text());
	ui->bt_cancelMinus->setVisible(1);
	ui->bt_ensureMinus->setVisible(1);
	ui->bt_drawMinus->setEnabled(0);
	currentView()->clearLayer(m_layerIdx);
	if (m_param->minusArea[m_param->currentGrp] == 1) {
		currentView()->drawRect2(
			m_param->minusRow[m_param->currentGrp], m_param->minusCol[m_param->currentGrp], 
			m_param->minusA[m_param->currentGrp],
			m_param->minusLength1[m_param->currentGrp], m_param->minusLength2[m_param->currentGrp], "minus");
	}
	else {
		currentView()->drawCircle(m_param->minusCY[m_param->currentGrp], m_param->minusCX[m_param->currentGrp], 
			m_param->minusR[m_param->currentGrp], "minus");
	}
}

void nccMatch_outNWdg::on_bt_ensureMinus_clicked()
{
	if (m_selState != SelMinusRegion)
		return;
	m_selState = UnSel;
	//log(LogL1, ui.bt_ensureMinus->text());
	ui->bt_cancelMinus->setVisible(0);
	ui->bt_ensureMinus->setVisible(0);
	ui->bt_drawMinus->setEnabled(1);
	int idx = m_param->currentGrp;
	currentView()->finishDraw();
	currentView()->clearLayer(m_layerIdx);
	double tmpR, tmpC, tmpA, tmpLen1, tmpLen2, tmpCY, tmpCX, tmpCR;
	if (m_param->minusArea[idx] == 1) {
		currentView()->getDrawRect2(tmpR, tmpC, tmpA, tmpLen1, tmpLen2);
	}
	else {
		currentView()->getDrawCircle(tmpCY, tmpCX, tmpCR);
	}
	if (m_module->has_set_pattern[idx]) {

		if (m_param->minusArea[idx] != 0) {
			Hobject patArea, minusArea, diffArea;
			HTuple patA, diffA, arow, acol;
			gen_rectangle1(&patArea, m_param->patternY[idx], m_param->patternX[idx], 
				m_param->patternY[idx] + m_param->patternH[idx], m_param->patternX[idx] + m_param->patternW[idx]);
			if (m_param->minusArea[idx] == 1) {
				gen_rectangle2(&minusArea, tmpR, tmpC, tmpA, tmpLen1, tmpLen2);
			}
			else {
				gen_circle(&minusArea, tmpCY, tmpCX, tmpCR);
			}
			difference(patArea, minusArea, &diffArea);
			area_center(patArea, &patA, &arow, &acol);
			area_center(diffArea, &diffA, &arow, &acol);
			if (diffA[0].I() < ((double)patA[0].I()*0.1)) {
				QMessageBox::warning(this, tr("warning"), tr("The exclusion area exceeds 90 % of the template area"),
					QMessageBox::Yes);
				return;
			}
		}
		m_module->resetPattern(m_param->currentGrp);
	}
	if (m_param->minusArea[m_param->currentGrp] == 1) {
		currentView()->getDrawRect2(m_param->minusRow[idx], m_param->minusCol[idx], m_param->minusA[idx]
			, m_param->minusLength1[idx], m_param->minusLength2[idx]);
	}
	else {
		currentView()->getDrawCircle(m_param->minusCY[idx], m_param->minusCX[idx], m_param->minusR[idx]);
	}
	m_module->setParamChanged(QvsParamLevel::ProductLevel);
}

void nccMatch_outNWdg::on_bt_cancelMinus_clicked()
{
	if (m_selState != SelMinusRegion)
		return;
	m_selState = UnSel;
	//log(LogL1, ui.bt_cancelMinus->text());
	ui->bt_cancelMinus->setVisible(0);
	ui->bt_ensureMinus->setVisible(0);
	ui->bt_drawMinus->setEnabled(1);
	currentView()->finishDraw();
	currentView()->clearLayer(m_layerIdx);
}

void nccMatch_outNWdg::comboBox_IndexChanged(int value)
{
	QComboBox* cob = qobject_cast<QComboBox*>(sender());
	QString str1, str2;
	if (cob == ui->cob_minusArea) {
		if (m_selState == SelMinusRegion) {
			connectSlots(false);
			ui->cob_minusArea->setCurrentIndex(m_param->minusArea[m_param->currentGrp]);
			connectSlots(true);
			return;
		}
		str1 = cob->itemText(m_param->minusArea[m_param->currentGrp]);
		str2 = cob->itemText(value);
		if (value == 0) {
			ui->bt_drawMinus->setVisible(0);
			ui->bt_cancelMinus->setVisible(0);
			ui->bt_ensureMinus->setVisible(0);
		}
		else {
			ui->bt_drawMinus->setVisible(1);
		}

		m_param->minusArea[m_param->currentGrp] = value;
		//log(LogL1, ui.label_11->text() + tr(" change from %1 to %2").arg(str1).arg(str2));
	}
	else if (cob == ui->cob_temp) {
		if (m_selState != UnSel) {
			connectSlots(false);
			ui->cob_temp->setCurrentIndex(m_param->currentGrp);
			connectSlots(true);
			return;
		}
		m_param->currentGrp = value;
		changeTemp(value);
	}
	m_module->setParamChanged(QvsParamLevel::ProductLevel);
}

void nccMatch_outNWdg::on_bt_selectOutPat_clicked()
{
	if (m_selState != UnSel)
		return;
	m_selState = SelOutPattern;
	//log(LogL1, ui.bt_selectOutPat->text());
	ui->bt_outPatOk->setEnabled(1);
	ui->bt_cancelOutPat->setEnabled(1);
	ui->bt_outPatOk->setVisible(1);
	ui->bt_cancelOutPat->setVisible(1);

	ui->bt_selectOutPat->setEnabled(0);
	drawRects();
}

void nccMatch_outNWdg::on_bt_outPatOk_clicked()
{
	if (m_selState != SelOutPattern)
		return;
	m_selState = UnSel;
	//log(LogL1, ui.bt_outPatOk->text());
	ui->bt_outPatOk->setEnabled(0);
	ui->bt_cancelOutPat->setEnabled(0);
	ui->bt_outPatOk->setVisible(0);
	ui->bt_cancelOutPat->setVisible(0);

	ui->bt_selectOutPat->setEnabled(1);
	getDrawRects();
	if (m_module->p_im->empty()) {
		return;
	}
	HalOpenCV::cv2halImg(*m_module->p_im, m_image, false);
	Hobject unionRec, roiRec;
	Hobject imgRec;
	Hobject dstImg;
	HTuple wid, hei;
	get_image_size(*m_image, &wid, &hei);
	gen_rectangle1(&imgRec, 0, 0, hei - 1, wid - 1);
	for (int i = 0;i < m_param->outDeleteCount[m_param->currentGrp];i++) {
		gen_rectangle2(&roiRec, m_param->deleteRegionVec[m_param->currentGrp][i].drow, m_param->deleteRegionVec[m_param->currentGrp][i].dcol,
			m_param->deleteRegionVec[m_param->currentGrp][i].dangle, m_param->deleteRegionVec[m_param->currentGrp][i].dlength1,
			m_param->deleteRegionVec[m_param->currentGrp][i].dlength2);
		union2(roiRec, unionRec, &unionRec);
	}
	difference(imgRec, unionRec, &unionRec);
	reduce_domain(*m_image, unionRec, &dstImg);
	Hobject outrec, outunion;
	for (int i = 0;i < m_param->outRegionCount[m_param->currentGrp];i++) {
		gen_rectangle2(&outrec, m_param->outRegionVec[m_param->currentGrp][i].row, m_param->outRegionVec[m_param->currentGrp][i].col,
			m_param->outRegionVec[m_param->currentGrp][i].angle, m_param->outRegionVec[m_param->currentGrp][i].length1,
			m_param->outRegionVec[m_param->currentGrp][i].length2);
		union2(outrec, outunion, &outunion);
	}
	reduce_domain(dstImg, outunion, &dstImg);
	copy_obj(dstImg, &m_param->OutImg[m_param->currentGrp], 1, 1);
	m_module->setParamChanged(QvsParamLevel::ProductLevel);
}

void nccMatch_outNWdg::on_bt_cancelOutPat_clicked()
{
	if (m_selState != SelOutPattern)
		return;
	m_selState = UnSel;
	//log(LogL1, ui.bt_cancelOutPat->text());
	ui->bt_outPatOk->setEnabled(0);
	ui->bt_cancelOutPat->setEnabled(0);
	ui->bt_outPatOk->setVisible(0);
	ui->bt_cancelOutPat->setVisible(0);

	ui->bt_selectOutPat->setEnabled(1);
	//currentView()->clear();
	currentView()->clearLayer(m_layerIdx);
	currentView()->finishDraw();
}

void nccMatch_outNWdg::bt_toggled(bool val)
{
	QPushButton *bt = qobject_cast<QPushButton *>(sender());

	if (ui->bt_threTest == bt)
		bt_threTest_toggled(val);
}

void nccMatch_outNWdg::bt_threTest_toggled(bool val)
{
	if ((m_selState != UnSel && val) || (m_selState != threTest && !val)) {
		connectSlots(false);
		ui->bt_threTest->setChecked(!val);
		connectSlots(true);
		return;
	}
	if (val) {
		m_selState = threTest;
		//log(LogL1, tr("start ") + ui.bt_threTest->text());
		currentView()->finishDraw();
		showRegionXld();
	}
	else {
		m_selState = UnSel;
		//log(LogL1, tr("finish ") + ui.bt_threTest->text());
		currentView()->finishDraw();
		//currentView()->clear();
		currentView()->clearLayer(m_layerIdx);
		currentView()->update();
	}
}

void nccMatch_outNWdg::outAreaSpinBox_valueChanged(int val)
{
	QSpinBox* sp = qobject_cast<QSpinBox*>(sender());
	QString str1, str2;
	if (sp == ui->sp_outArea) {
		str1 = ui->label_8->text();
		if (m_selState == SelOutPattern) {
			getDrawRects();
		}
		nccMatch_outNParam::segRegion seg;
		seg.row = seg.col = 200;
		seg.length1 = seg.length2 = 25;  //给新添加的roi固定的开始位1置
		seg.angle = 0;
		if (val >= m_param->outRegionCount[m_param->currentGrp]) {
			for (int i = m_param->outRegionCount[m_param->currentGrp];i < val;i++) {
				seg.row = seg.col += 10;
				m_param->outRegionVec[m_param->currentGrp].push_back(seg);
			}
		}
		else if (val < m_param->outRegionCount[m_param->currentGrp]) {
			vector<nccMatch_outNParam::segRegion>::iterator itbegin = m_param->outRegionVec[m_param->currentGrp].begin() + val;
			vector<nccMatch_outNParam::segRegion>::iterator itEnd = m_param->outRegionVec[m_param->currentGrp].end();
			m_param->outRegionVec[m_param->currentGrp].erase(itbegin, itEnd);
		}
		str2 = tr(" change from %1 to %2").arg(m_param->outRegionCount[m_param->currentGrp]).arg(val);
		m_param->outRegionCount[m_param->currentGrp] = val;
		if (m_selState == SelOutPattern) {
			drawRects();
		}
	}
	else if (sp == ui->sp_deleteArea) {
		str1 = ui->label_5->text();
		if (m_selState == SelOutPattern) {
			getDrawRects();
		}
		nccMatch_outNParam::delRegion seg;
		seg.drow = seg.dcol = 200;
		seg.dlength1 = seg.dlength2 = 25;  //给新添加的roi固定的开始位1置
		seg.dangle = 0;
		if (val >= m_param->outDeleteCount[m_param->currentGrp]) {
			for (int i = m_param->outDeleteCount[m_param->currentGrp];i < val;i++) {
				seg.drow = seg.dcol += 10;
				m_param->deleteRegionVec[m_param->currentGrp].push_back(seg);
			}
		}
		else if (val < m_param->outDeleteCount[m_param->currentGrp]) {
			vector<nccMatch_outNParam::delRegion>::iterator itbegin = m_param->deleteRegionVec[m_param->currentGrp].begin() + val;
			vector<nccMatch_outNParam::delRegion>::iterator itEnd = m_param->deleteRegionVec[m_param->currentGrp].end();
			m_param->deleteRegionVec[m_param->currentGrp].erase(itbegin, itEnd);
		}
		str2 = tr(" change from %1 to %2").arg(m_param->outDeleteCount[m_param->currentGrp]).arg(val);
		m_param->outDeleteCount[m_param->currentGrp] = val;
		if (m_selState == SelOutPattern) {
			drawRects();
		}
	}
	else if (sp == ui->sp_minArea) {
		str1 = ui->label_12->text() + tr(" min val");
		str2 = tr(" change from %1 to %2").arg(m_param->outAreaMin[m_param->currentGrp]).arg(val);
		if (val >= m_param->outAreaMax[m_param->currentGrp]) {
			m_param->outAreaMin[m_param->currentGrp] = m_param->outAreaMax[m_param->currentGrp] - 1;
			connectSlots(false);
			ui->sp_minArea->setValue(m_param->outAreaMax[m_param->currentGrp] - 1);
			connectSlots(true);
		}
		else
			m_param->outAreaMin[m_param->currentGrp] = val;
		if (ui->bt_threTest->isChecked()) {
			showRegionXld();
		}
	}
	else if (sp == ui->sp_maxArea) {
		str1 = ui->label_12->text() + tr(" max val");
		str2 = tr(" change from %1 to %2").arg(m_param->outAreaMax[m_param->currentGrp]).arg(val);
		if (val <= m_param->outAreaMin[m_param->currentGrp]) {
			m_param->outAreaMax[m_param->currentGrp] = m_param->outAreaMin[m_param->currentGrp] + 1;
			connectSlots(false);
			ui->sp_maxArea->setValue(m_param->outAreaMin[m_param->currentGrp] + 1);
			connectSlots(true);
		}
		else
			m_param->outAreaMax[m_param->currentGrp] = val;
		if (ui->bt_threTest->isChecked()) {
			showRegionXld();
		}
	}
	else if (sp == ui->sp_threMin) {
		str1 = tr("threshold") + tr(" min val");
		str2 = tr(" change from %1 to %2").arg(m_param->outThreMin[m_param->currentGrp]).arg(val);
		if (val >= m_param->outThreMax[m_param->currentGrp]) {
			m_param->outThreMin[m_param->currentGrp] = m_param->outThreMax[m_param->currentGrp] - 1;
			connectSlots(false);
			ui->sp_threMin->setValue(m_param->outThreMax[m_param->currentGrp] - 1);
			connectSlots(true);
		}
		else
			m_param->outThreMin[m_param->currentGrp] = val;
		if (ui->bt_threTest->isChecked()) {
			showRegionXld();
		}
	}
	else if (sp == ui->sp_threMax) {
		str1 = tr("threshold") + tr(" max val");
		str2 = tr(" change from %1 to %2").arg(m_param->outThreMax[m_param->currentGrp]).arg(val);
		if (val <= m_param->outThreMin[m_param->currentGrp]) {
			m_param->outThreMax[m_param->currentGrp] = m_param->outThreMin[m_param->currentGrp] + 1;
			connectSlots(false);
			ui->sp_threMax->setValue(m_param->outThreMin[m_param->currentGrp] + 1);
			connectSlots(true);
		}
		else
			m_param->outThreMax[m_param->currentGrp] = val;
		if (ui->bt_threTest->isChecked()) {
			showRegionXld();
		}
	}
	else if (sp == ui->sp_open) {
		str1 = ui->label_13->text();
		str2 = tr(" change from %1 to %2").arg(m_param->openVal[m_param->currentGrp]).arg(val);
		m_param->openVal[m_param->currentGrp] = val;
		if (ui->bt_threTest->isChecked()) {
			showRegionXld();
		}
	}
	else if (sp == ui->sp_close) {
		str1 = ui->label_14->text();
		str2 = tr(" change from %1 to %2").arg(m_param->closeVal[m_param->currentGrp]).arg(val);
		m_param->closeVal[m_param->currentGrp] = val;
		if (ui->bt_threTest->isChecked()) {
			showRegionXld();
		}
	}
	m_module->setParamChanged(QvsParamLevel::ProductLevel);
	//log(LogL1, str1 + str2);
}

void nccMatch_outNWdg::spinBox_valueChanged(int value)
{
	QSpinBox* sp = qobject_cast<QSpinBox*>(sender());
	QString str1, str2;
	if (sp == ui->sp_similarity) {
		str1 = ui->label->text();
		if (value == m_param->similarity[m_param->currentGrp])
			return;
		str2 = tr(" change from %1 to %2").arg(m_param->similarity[m_param->currentGrp]).arg(value);
		m_param->similarity[m_param->currentGrp] = value;
		m_module->m_detect[m_param->currentGrp]->setSimilar((float)value / 100.0);
	}
	else if (sp == ui->sp_seekingAngle) {
		str1 = ui->label_2->text();
		str2 = tr(" change from %1 to %2").arg(m_param->seekingAngle[m_param->currentGrp]).arg(value);
		if (value == m_param->seekingAngle[m_param->currentGrp])
			return;
		m_param->seekingAngle[m_param->currentGrp] = value;
		m_module->m_detect[m_param->currentGrp]->setRotateDeg(value);
	}
	else if (sp == ui->sp_acceptValue) {
		str1 = ui->cb_acceptValue->text();
		str2 = tr(" change from %1 to %2").arg(m_param->acceptValue[m_param->currentGrp]).arg(value);
		if (value == m_param->acceptValue[m_param->currentGrp])
			return;
		m_param->acceptValue[m_param->currentGrp] = value;
	}
	else if (sp == ui->sp_offsetValue) {
		str1 = ui->cb_offsetValue->text();
		str2 = tr(" change from %1 to %2").arg(m_param->offsetValue[m_param->currentGrp]).arg(value);
		if (value == m_param->offsetValue[m_param->currentGrp])
			return;
		m_param->offsetValue[m_param->currentGrp] = value;
	}
	else if (sp == ui->sp_minRelative) {
		str1 = ui->cb_relativeAngle->text() + tr(" min val");
		str2 = tr(" change from %1 to %2").arg(m_param->minRelative[m_param->currentGrp]).arg(value);
		if (value == m_param->minRelative[m_param->currentGrp])
			return;
		if (value > m_param->maxRelative[m_param->currentGrp]) {
			ui->sp_minRelative->setValue(m_param->minRelative[m_param->currentGrp]);
			return;
		}
		m_param->minRelative[m_param->currentGrp] = value;
	}
	else if (sp == ui->sp_maxRelative) {
		str1 = ui->cb_relativeAngle->text() + tr(" max val");
		str2 = tr(" change from %1 to %2").arg(m_param->maxRelative[m_param->currentGrp]).arg(value);
		if (value == m_param->maxRelative[m_param->currentGrp])
			return;
		if (value < m_param->minRelative[m_param->currentGrp]) {
			ui->sp_maxRelative->setValue(m_param->maxRelative[m_param->currentGrp]);
			return;
		}
		m_param->maxRelative[m_param->currentGrp] = value;
	}
	else if (sp == ui->sp_axisAngle) {
		str1 = ui->cb_axisAngle->text();
		str2 = tr(" change from %1 to %2").arg(m_param->maxAxisAngle[m_param->currentGrp]).arg(value);
		m_param->maxAxisAngle[m_param->currentGrp] = value;
	}
	m_module->setParamChanged(QvsParamLevel::ProductLevel);
	//log(LogL1, str1 + str2);
}

void nccMatch_outNWdg::CheckBox_toggled(bool flag)
{
	QCheckBox* cb = qobject_cast<QCheckBox*>(sender());
	QString str;
	if (flag)
		str = cb->text() + " Check";
	else
		str = cb->text() + " UnCheck";
	//log(LogL1, str);
	if (cb == ui->cb_acceptValue) {
		//ui.sp_acceptValue->setEnabled(flag);
		if ((int)flag == m_param->acceptEnable[m_param->currentGrp])
			return;
		m_param->acceptEnable[m_param->currentGrp] = flag;
	}
	else if (cb == ui->cb_relativeAngle) {
		//ui.sp_minRelative->setEnabled(flag);
		//ui.sp_maxRelative->setEnabled(flag);
		if ((int)flag == m_param->relativeEnable[m_param->currentGrp])
			return;
		m_param->relativeEnable[m_param->currentGrp] = (int)flag;
	}
	else if (cb == ui->cb_offsetValue) {
		//ui.sp_offsetValue->setEnabled(flag);
		if ((int)flag == m_param->offsetEnable[m_param->currentGrp])
			return;
		m_param->offsetEnable[m_param->currentGrp] = (int)flag;
	}
	else if (cb == ui->cb_axisAngle) {
		//ui.sp_axisAngle->setEnabled(flag);
		if ((int)flag == m_param->IsAxisAngle[m_param->currentGrp])
			return;
		m_param->IsAxisAngle[m_param->currentGrp] = flag;
	}
	else if (cb == ui->cb_showOutArea) {
		m_param->IsShowOutArea[m_param->currentGrp] = flag;
	}
	else if (cb == ui->cb_fillUp) {
		m_param->IsFillUp[m_param->currentGrp] = flag;
	}
	else if (cb == ui->cb_useOutArea) {
		m_param->IsUseOutArea[m_param->currentGrp] = flag;
//		m_module->p_outArea[m_param->currentGrp].setVisible(flag);
		if (flag) {
			ui->groupBox_3->show();
		}
		else {
			ui->groupBox_3->hide();
		}
	}
	else if (cb == ui->cb_showRoiAfterDet) {
		m_param->IsShowRoiAfterDet[m_param->currentGrp] = flag;
	}
	else if (cb == ui->cb_showMinusAfterDet) {
		m_param->IsShowMinusAfterDet[m_param->currentGrp] = flag;
	}
	else if (cb == ui->cb_enable) {
		m_param->GrpEnable[m_param->currentGrp] = flag;
	}
	m_module->setParamChanged(QvsParamLevel::ProductLevel);
}
