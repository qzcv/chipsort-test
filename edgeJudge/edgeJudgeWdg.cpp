#include "edgeJudgeWdg.h"
#include "edgeJudgeMod.h"
#include "edgeJudgeParam.h"
#include "ui_edgeJudgeWdg.h"
#include <QMessageBox>
#include "ImageView.h"
#include "chipMod.h"

edgeJudgeWdg::edgeJudgeWdg(int i) :
	ModSetWidget(),ui(new Ui::edgeJudgeWdg),m_widgetType(i)
{
	m_hasConnect = false;
	ui->setupUi(this);
	iniUi();
	m_param = nullptr;
	m_module = nullptr;
	m_layerIdx = 0;
	m_IsSelecting = false;
	m_nowGrpIndex = 0;
}

edgeJudgeWdg::~edgeJudgeWdg()
{
	delete ui;
}

void edgeJudgeWdg::setModule(UnitModule *module)
{
	m_module=dynamic_cast<edgeJudgeMod*>(module);
	m_param = m_module->m_param;
// 	connectSlots(false);
// 	setUiValue();
// 	connectSlots(true);
}

void edgeJudgeWdg::showEvent(QShowEvent * event)
{
	connectSlots(false);
	setUiValue();
	connectSlots(true);
}

void edgeJudgeWdg::on_bt_setOrigin_clicked()
{
	HTuple centerRow, centerCol;
	m_module->getDispOutData(CenterRow, centerRow);
	m_module->getDispOutData(CenterCol, centerCol);
	if (centerRow.Num() == 0) {
		return;
	}

	if (QMessageBox::information(this, tr("Hint"), 
		tr("Are you sure to change the current center point:(%1,%2) to the template center?")
		.arg(centerCol[0].D(), 0, 'f', 3).arg(centerRow[0].D(), 0, 'f', 3),
		QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
			m_param->TmpCenterRow = centerRow[0].D();
			m_param->TmpCenterCol = centerCol[0].D();
			m_module->setParamChanged(ProductLevel);
	}
}

void edgeJudgeWdg::radioButton_toggled(bool check)
{
	QRadioButton* rb = qobject_cast<QRadioButton*>(sender());
	if (rb == ui->rb_blackToWhite) {
		m_param->whiteBlackDir[m_nowGrpIndex] = !check;
	}
	else if (rb == ui->rb_whiteToBlack) {
		m_param->whiteBlackDir[m_nowGrpIndex] = check;
	}
	if (rb == ui->rb_inToOut) {
		m_param->searchDir[m_nowGrpIndex] = !check;
	}
	else if (rb == ui->rb_outToIn) {
		m_param->searchDir[m_nowGrpIndex] = check;
	}
	m_module->setParamChanged(ProductLevel);
}

void edgeJudgeWdg::checkBox_toggled(bool check)
{
	QCheckBox* cb = qobject_cast<QCheckBox*>(sender());
	if (cb == ui->cb_errWidth) {
		m_param->IsErrWidth = check;
		//ui->dsp_errWidthMin->setEnabled(check);
		//ui->dsp_errWidthMax->setEnabled(check);
	}
	else if (cb == ui->cb_errHeight) {
		m_param->IsErrHeight = check;
		//ui->dsp_errHeightMin->setEnabled(check);
		//ui->dsp_errHeightMax->setEnabled(check);
	}
	else if (cb == ui->cb_errAngle) {
		m_param->IsErrAngle = check;
		//ui->sp_errAngle->setEnabled(check);
	}
	else if (cb == ui->cb_errCenterOffset) {
		m_param->IsErrCenterOffset = check;
		//ui->dsp_errCenterOffset->setEnabled(check);
	}
	else if (cb == ui->cb_refer) {
		m_param->IsRefer = check;
	}
	else if (cb == ui->cb_showRegionAfterDet) {
		m_param->IsShowRegionAfterDet = check;
	}
	else if (cb == ui->cb_inputPixel) {
		m_param->IsInputPixel = check;
	}
	else if (cb == ui->cb_fixWidth) {
		m_param->IsFixWidth[m_nowGrpIndex] = check;
		//ui->sp_fixWidth->setEnabled(check);
	}
	else if (cb == ui->cb_UseInput) {
		m_param->IsUseCompare = check;
	}
	else if (cb == ui->cb_offArea) {
		m_param->IsOffArea = check;
		//ui->sp_errOffAreaMin->setEnabled(check);
	}
	else if (cb == ui->cb_showSegDiff) {
		m_param->IsShowSegDiff = check;
	}
	m_module->setParamChanged(ProductLevel);
}

void edgeJudgeWdg::doubleSpinBox_valueChanged(double val)
{
	QDoubleSpinBox* dsp = qobject_cast<QDoubleSpinBox*>(sender());
	if (dsp == ui->dsp_errWidthMin) {
		m_param->errWidthMin = val;
	}
	else if (dsp == ui->dsp_errWidthMax) {
		m_param->errWidthMax = val;
	}
	else if (dsp == ui->dsp_errHeightMin) {
		m_param->errHeightMin = val;
	}
	else if (dsp == ui->dsp_errHeightMax) {
		m_param->errHeightmax = val;
	}
	else if (dsp == ui->dsp_errCenterOffset) {
		m_param->errCenterOffset = val;
	}
	else if (dsp == ui->dsp_pixSize) {
		m_param->pixSize = val;
	}
	else if (dsp == ui->dsp_factor) {
		m_param->clipfactor[m_nowGrpIndex] = val;
	}
	m_module->setParamChanged(ProductLevel);
}

void edgeJudgeWdg::spinBox_valueChanged(int val)
{
	QSpinBox* sp = qobject_cast<QSpinBox*>(sender());
	if (sp == ui->sp_maxAngle) {
		m_param->maxAngle = val;
	}
	else if (sp == ui->sp_errAngle) {
		m_param->errAngle = val;
	}
	else if (sp == ui->sp_extendSearchLine) {
		m_param->extendSearchLine[m_nowGrpIndex] = val;
	}
	else if (sp == ui->sp_minGrayDiff) {
		m_param->minGrayDiff[m_nowGrpIndex] = val;
	}
	else if (sp == ui->sp_searchLineGap) {
		m_param->searchLineGap[m_nowGrpIndex] = val;
	}
	else if (sp == ui->sp_lineOffset) {
		m_param->lineOffset[m_nowGrpIndex] = val;
	}
	else if (sp == ui->sp_ROINum) {
		if (m_IsSelecting) {
			return;
		}
		int beginIndex = 1;  //前边有一个搜索区域
		for (int j = 0;j < m_nowGrpIndex;j++) {
			beginIndex += m_param->roiNum[j];
		}
		if (val > m_param->roiNum[m_nowGrpIndex]) {
			edgeJudgeParam::RoiRegion seg;
			seg.row1 = seg.col1 = 40;
			seg.row2 = seg.col2 = 90;
			vector<edgeJudgeParam::RoiRegion>::iterator itbegin = m_param->RoiRegionVector.begin() + beginIndex + m_param->roiNum[m_nowGrpIndex];
			m_param->RoiRegionVector.insert(itbegin, val - m_param->roiNum[m_nowGrpIndex], seg);
		}
		else if (val < m_param->roiNum[m_nowGrpIndex]) {
			vector<edgeJudgeParam::RoiRegion>::iterator itbegin = m_param->RoiRegionVector.begin() + beginIndex + val;
			vector<edgeJudgeParam::RoiRegion>::iterator itEnd = m_param->RoiRegionVector.begin() + beginIndex + m_param->roiNum[m_nowGrpIndex];
			m_param->RoiRegionVector.erase(itbegin, itEnd);
		}
		m_param->roiNum[m_nowGrpIndex] = val;
	}
	else if (sp == ui->sp_fixWidth) {
		m_param->fixWidth[m_nowGrpIndex] = val;
	}
	else if (sp == ui->sp_paintGray) {
		m_param->backGray = val;
	}
	else if (sp == ui->sp_inputLarge) {
		m_param->inputRegionLarge = val;
	}
	else if (sp == ui->sp_offThickMin) {
		m_param->offThickMin = val;
	}
	else if (sp == ui->sp_errOffAreaMin) {
		m_param->errOffAreaMin = val;
	}
	m_module->setParamChanged(ProductLevel);
}

void edgeJudgeWdg::on_bt_selectRoi_toggled(bool check)
{
	int allnum = 1;
	for (int i = 0;i < 4;i++) {
		allnum += m_param->roiNum[i];
	}
	if (check) {
		m_IsSelecting = true;
		//currentView()->clear();
		currentView()->clearLayer(m_layerIdx);
		double row1[21], col1[21], row2[21], col2[21];  //1+4*5
		QStringList title;

		int beforeNum[4];  //前面一组一共有多少个roi
		beforeNum[0] = 1;
		const char* regionName[4] = { "up","right","down","left" };
		for (int i = 0;i < 4;i++) {
			if (i > 0) {
				beforeNum[i] = beforeNum[i - 1] + m_param->roiNum[i - 1];
			}
		}
		int grpIndex = 0; //隶属哪一个组别
		for (int i = 0;i < allnum;i++) {
			row1[i] = m_param->RoiRegionVector[i].row1;
			col1[i] = m_param->RoiRegionVector[i].col1;
			row2[i] = m_param->RoiRegionVector[i].row2;
			col2[i] = m_param->RoiRegionVector[i].col2;
			if (i == 0) {
				//sprintf(buf[i],"搜索区域",i+1);
				title << tr("SearchRoi");
				//sprintf(buf[i], tr("SearchRoi").toLocal8Bit().constData());
			}
			//else if(i>0&&i<m_param->roiNum[0]){
			//	sprintf(buf[i],"上",i+1);
			//}
			//else if(i==2){
			//	sprintf(buf[i],"右",i+1);
			//}
			//else if(i==3){
			//	sprintf(buf[i],"下",i+1);
			//}
			//else if(i==4){
			//	sprintf(buf[i],"左",i+1);
			//}
			else if (i > 0) {
				if ((i > beforeNum[grpIndex] - 1) && (i < (beforeNum[grpIndex] + m_param->roiNum[grpIndex]))) {
					title << tr("%s_%d").arg(regionName[grpIndex]).arg(i - beforeNum[grpIndex]);
					//sprintf(buf[i], "%s_%d", tr(regionName[grpIndex]).toLocal8Bit().constData(), i - beforeNum[grpIndex] + 1);
					if (i == (beforeNum[grpIndex] + m_param->roiNum[grpIndex] - 1)) {
						grpIndex++;
					}
				}
			}
			//title[i] = buf[i];
		}
		currentView()->setColor(255, 0, 128);
		currentView()->drawRects1(allnum, row1, col1, row2, col2, title);
		currentView()->update();
	}
	else {
		m_IsSelecting = false;
		currentView()->finishDraw();
		double row1[21], col1[21], row2[21], col2[21];
		currentView()->getDrawRects1(allnum, row1, col1, row2, col2);
		for (int i = 0;i < allnum;i++) {
			m_param->RoiRegionVector[i].row1 = row1[i];
			m_param->RoiRegionVector[i].col1 = col1[i];
			m_param->RoiRegionVector[i].row2 = row2[i];
			m_param->RoiRegionVector[i].col2 = col2[i];
		}
		//currentView()->clear();
		currentView()->clearLayer(m_layerIdx);
		currentView()->setColor(0, 255, 0);
		currentView()->setFilled(false);
		for (int i = 0;i < allnum;i++) {
			currentView()->dispRect1(row1[i], col1[i], row2[i], col2[i]);
		}
		currentView()->update();

		m_module->setParamChanged(ProductLevel);
	}
}

void edgeJudgeWdg::on_bt_advance_toggled(bool val)
{
	if (val) {
		ui->bt_advance->setText(tr("exit advance"));
	}
	else {
		ui->bt_advance->setText(tr("advance set"));
	}
	ui->grp_advance->setVisible(val);
}

void edgeJudgeWdg::tabWidget_currentIndexChanged(int index)
{
	QTabWidget* tab = qobject_cast<QTabWidget*>(sender());
	if (tab == ui->tabWidget) {
		m_nowGrpIndex = index;
		connectSlots(false);
		changeGroupIndex(index);
		connectSlots(true);
	}
}

void edgeJudgeWdg::connectSlots(bool link)
{
	if(m_hasConnect==link)
		return;
	m_hasConnect=link;

	connectOrNot(link, ui->cb_showRegionAfterDet, SIGNAL(toggled(bool)), SLOT(checkBox_toggled(bool)));
	connectOrNot(link, ui->cb_errWidth, SIGNAL(toggled(bool)), SLOT(checkBox_toggled(bool)));
	connectOrNot(link, ui->cb_errHeight, SIGNAL(toggled(bool)), SLOT(checkBox_toggled(bool)));
	connectOrNot(link, ui->cb_errAngle, SIGNAL(toggled(bool)), SLOT(checkBox_toggled(bool)));
	connectOrNot(link, ui->cb_errCenterOffset, SIGNAL(toggled(bool)), SLOT(checkBox_toggled(bool)));
	connectOrNot(link, ui->cb_refer, SIGNAL(toggled(bool)), SLOT(checkBox_toggled(bool)));
	connectOrNot(link, ui->cb_inputPixel, SIGNAL(toggled(bool)), SLOT(checkBox_toggled(bool)));

	connectOrNot(link, ui->sp_maxAngle, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_errAngle, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->dsp_errWidthMin, SIGNAL(valueChanged(double)), SLOT(doubleSpinBox_valueChanged(double)));
	connectOrNot(link, ui->dsp_errWidthMax, SIGNAL(valueChanged(double)), SLOT(doubleSpinBox_valueChanged(double)));
	connectOrNot(link, ui->dsp_errHeightMin, SIGNAL(valueChanged(double)), SLOT(doubleSpinBox_valueChanged(double)));
	connectOrNot(link, ui->dsp_errHeightMax, SIGNAL(valueChanged(double)), SLOT(doubleSpinBox_valueChanged(double)));
	connectOrNot(link, ui->dsp_errCenterOffset, SIGNAL(valueChanged(double)), SLOT(doubleSpinBox_valueChanged(double)));
	connectOrNot(link, ui->dsp_pixSize, SIGNAL(valueChanged(double)), SLOT(doubleSpinBox_valueChanged(double)));
	connectOrNot(link, ui->rb_blackToWhite, SIGNAL(toggled(bool)), SLOT(radioButton_toggled(bool)));
	connectOrNot(link, ui->rb_whiteToBlack, SIGNAL(toggled(bool)), SLOT(radioButton_toggled(bool)));
	connectOrNot(link, ui->rb_inToOut, SIGNAL(toggled(bool)), SLOT(radioButton_toggled(bool)));
	connectOrNot(link, ui->rb_outToIn, SIGNAL(toggled(bool)), SLOT(radioButton_toggled(bool)));
	connectOrNot(link, ui->sp_extendSearchLine, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_minGrayDiff, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_searchLineGap, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_lineOffset, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_ROINum, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->dsp_factor, SIGNAL(valueChanged(double)), SLOT(doubleSpinBox_valueChanged(double)));
	connectOrNot(link, ui->cb_fixWidth, SIGNAL(toggled(bool)), SLOT(checkBox_toggled(bool)));
	connectOrNot(link, ui->sp_fixWidth, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->tabWidget, SIGNAL(currentChanged(int)), SLOT(tabWidget_currentIndexChanged(int)));

	connectOrNot(link, ui->cb_UseInput, SIGNAL(toggled(bool)), SLOT(checkBox_toggled(bool)));

	connectOrNot(link, ui->cb_showSegDiff, SIGNAL(toggled(bool)), SLOT(checkBox_toggled(bool)));
	connectOrNot(link, ui->cb_offArea, SIGNAL(toggled(bool)), SLOT(checkBox_toggled(bool)));
	connectOrNot(link, ui->sp_paintGray, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_inputLarge, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_offThickMin, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_errOffAreaMin, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
}

void edgeJudgeWdg::iniUi()
{

}

void edgeJudgeWdg::setUiValue()
{
	if(!m_param)
		return;

	ui->sp_maxAngle->setValue(m_param->maxAngle);
	changeGroupIndex(m_nowGrpIndex);
	ui->dsp_pixSize->setValue(m_param->pixSize);
	ui->cb_errWidth->setChecked(m_param->IsErrWidth == 1);
	ui->dsp_errWidthMin->setValue(m_param->errWidthMin);
	//ui->dsp_errWidthMin->setEnabled(m_param->IsErrWidth==1);
	ui->dsp_errWidthMax->setValue(m_param->errWidthMax);
	//ui->dsp_errWidthMax->setEnabled(m_param->IsErrWidth==1);

	ui->cb_errHeight->setChecked(m_param->IsErrHeight == 1);
	ui->dsp_errHeightMin->setValue(m_param->errHeightMin);
	//ui->dsp_errHeightMin->setEnabled(m_param->IsErrHeight==1);
	ui->dsp_errHeightMax->setValue(m_param->errHeightmax);
	//ui->dsp_errHeightMax->setEnabled(m_param->IsErrHeight==1);

	ui->cb_errAngle->setChecked(m_param->IsErrAngle == 1);
	ui->sp_errAngle->setValue(m_param->errAngle);
	//ui->sp_errAngle->setEnabled(m_param->IsErrAngle==1);

	ui->cb_errCenterOffset->setChecked(m_param->IsErrCenterOffset == 1);
	ui->dsp_errCenterOffset->setValue(m_param->errCenterOffset);
	//ui->dsp_errCenterOffset->setEnabled(m_param->IsErrCenterOffset==1);

	ui->cb_refer->setChecked(m_param->IsRefer);
	ui->cb_showRegionAfterDet->setChecked(m_param->IsShowRegionAfterDet);
	ui->cb_inputPixel->setChecked(m_param->IsInputPixel);
	if (ui->bt_advance->isChecked()) {
		ui->bt_advance->setChecked(false);
	}
	ui->grp_advance->setVisible(false);
	ui->tabWidget->setCurrentIndex(0);

	ui->cb_showSegDiff->setChecked(m_param->IsShowSegDiff);
	ui->cb_UseInput->setChecked(m_param->IsUseCompare);
	ui->sp_paintGray->setValue(m_param->backGray);
	ui->sp_inputLarge->setValue(m_param->inputRegionLarge);
	ui->sp_offThickMin->setValue(m_param->offThickMin);
	ui->cb_offArea->setChecked(m_param->IsOffArea);
	ui->sp_errOffAreaMin->setValue(m_param->errOffAreaMin);
	//ui->sp_errOffAreaMin->setEnabled(m_param->IsOffArea);
}

void edgeJudgeWdg::changeGroupIndex(int index)
{
	if (m_param->whiteBlackDir[index] == 0) {
		ui->rb_blackToWhite->setChecked(true);
	}
	else if (m_param->whiteBlackDir[index] == 1) {
		ui->rb_whiteToBlack->setChecked(true);
	}
	if (m_param->searchDir[index] == 0) {
		ui->rb_inToOut->setChecked(true);
	}
	else if (m_param->searchDir[index] == 1) {
		ui->rb_outToIn->setChecked(true);
	}
	ui->sp_extendSearchLine->setValue(m_param->extendSearchLine[index]);
	ui->sp_searchLineGap->setValue(m_param->searchLineGap[index]);
	ui->sp_minGrayDiff->setValue(m_param->minGrayDiff[index]);
	ui->sp_lineOffset->setValue(m_param->lineOffset[index]);

	ui->sp_ROINum->setValue(m_param->roiNum[index]);
	ui->dsp_factor->setValue(m_param->clipfactor[index]);
	ui->cb_fixWidth->setChecked(m_param->IsFixWidth[index]);
	//ui->sp_fixWidth->setEnabled(m_param->IsFixWidth[index]);
	ui->sp_fixWidth->setValue(m_param->fixWidth[index]);
}

