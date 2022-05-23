#include "edgeLocationWdg.h"
#include "edgeLocationMod.h"
#include "edgeLocationParam.h"
#include "ui_edgeLocationWdg.h"
#include <QMessageBox>
#include "AdvanceDialog.h"
#include "ImageView.h"
#include <QComboBox>
#include "halOpenCV.h"

edgeLocationWdg::edgeLocationWdg(int i) :
	ModSetWidget(), ui(new Ui::edgeLocationWdg), m_widgetType(i)
{
	m_hasConnect = false;
	ui->setupUi(this);
	iniData();
	iniUi();
}

edgeLocationWdg::~edgeLocationWdg()
{
	delete ui;
}

void edgeLocationWdg::setModule(UnitModule *module)
{
	m_module = dynamic_cast<edgeLocationMod*>(module);
	m_param = m_module->m_param;
	m_image = m_module->m_image;
	for (auto i = 0;i < 4;++i)
		m_edgeLine[i] = m_module->m_edgeLine[i];
}

void edgeLocationWdg::showEvent(QShowEvent * event)
{
	connectSlots(false);
	setUiValue();
	connectSlots(true);
}

void edgeLocationWdg::connectSlots(bool link)
{
	if (m_hasConnect == link)
		return;
	m_hasConnect = link;

	connectOrNot(link, ui->cb_errWidth, SIGNAL(toggled(bool)), SLOT(checkBox_toggled(bool)));
	connectOrNot(link, ui->cb_errHeight, SIGNAL(toggled(bool)), SLOT(checkBox_toggled(bool)));
	connectOrNot(link, ui->cb_errHeightDiff, SIGNAL(toggled(bool)), SLOT(checkBox_toggled(bool)));
	connectOrNot(link, ui->cb_errWidthDiff, SIGNAL(toggled(bool)), SLOT(checkBox_toggled(bool)));
	connectOrNot(link, ui->cb_errAngle, SIGNAL(toggled(bool)), SLOT(checkBox_toggled(bool)));
	connectOrNot(link, ui->cb_errCenterOffset, SIGNAL(toggled(bool)), SLOT(checkBox_toggled(bool)));
	connectOrNot(link, m_advDialog->ui.cb_refer, SIGNAL(toggled(bool)), SLOT(checkBox_toggled(bool)));
	connectOrNot(link, m_advDialog->ui.cb_inputPixel, SIGNAL(toggled(bool)), SLOT(checkBox_toggled(bool)));
	connectOrNot(link, m_advDialog->ui.cb_noDetect, SIGNAL(toggled(bool)), SLOT(checkBox_toggled(bool)));
	connectOrNot(link, ui->cb_crossGray, SIGNAL(toggled(bool)), SLOT(checkBox_toggled(bool)));

	connectOrNot(link, ui->sp_crossGray, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_crossStep, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_maxAngle, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_errAngle, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->dsp_errWidthMin, SIGNAL(valueChanged(double)), SLOT(doubleSpinBox_valueChanged(double)));
	connectOrNot(link, ui->dsp_errWidthMax, SIGNAL(valueChanged(double)), SLOT(doubleSpinBox_valueChanged(double)));
	connectOrNot(link, ui->dsp_errHeightMin, SIGNAL(valueChanged(double)), SLOT(doubleSpinBox_valueChanged(double)));
	connectOrNot(link, ui->dsp_errHeightMax, SIGNAL(valueChanged(double)), SLOT(doubleSpinBox_valueChanged(double)));
	connectOrNot(link, ui->dsp_errHeightDiff, SIGNAL(valueChanged(double)), SLOT(doubleSpinBox_valueChanged(double)));
	connectOrNot(link, ui->dsp_errWidthDiff, SIGNAL(valueChanged(double)), SLOT(doubleSpinBox_valueChanged(double)));

	connectOrNot(link, ui->dsp_errCenterOffset, SIGNAL(valueChanged(double)), SLOT(doubleSpinBox_valueChanged(double)));
	connectOrNot(link, m_advDialog->ui.dsp_pixSize, SIGNAL(valueChanged(double)), SLOT(doubleSpinBox_valueChanged(double)));
	connectOrNot(link, ui->rb_blackToWhite, SIGNAL(toggled(bool)), SLOT(radioButton_toggled(bool)));
	connectOrNot(link, ui->rb_whiteToBlack, SIGNAL(toggled(bool)), SLOT(radioButton_toggled(bool)));
	connectOrNot(link, m_advDialog->ui.rb_onlyLfRt, SIGNAL(toggled(bool)), SLOT(radioButton_toggled(bool)));
	connectOrNot(link, m_advDialog->ui.rb_onlyUpDw, SIGNAL(toggled(bool)), SLOT(radioButton_toggled(bool)));
	connectOrNot(link, m_advDialog->ui.rb_onlyOneSide, SIGNAL(toggled(bool)), SLOT(radioButton_toggled(bool)));

	connectOrNot(link, ui->rb_inToOut, SIGNAL(toggled(bool)), SLOT(radioButton_toggled(bool)));
	connectOrNot(link, ui->rb_outToIn, SIGNAL(toggled(bool)), SLOT(radioButton_toggled(bool)));
	connectOrNot(link, ui->sp_extendSearchLine, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_minGrayDiff, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_searchLineGap, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_lineOffset, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_ROINum, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->dsp_factor, SIGNAL(valueChanged(double)), SLOT(doubleSpinBox_valueChanged(double)));
	//connectOrNot(link, m_advDialog->ui.cb_fixWidth,SIGNAL(toggled(bool)),SLOT(checkBox_toggled(bool)));
	connectOrNot(link, m_advDialog->ui.sp_fixWidth, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->tabWidget, SIGNAL(currentChanged(int)), SLOT(tabWidget_currentIndexChanged(int)));
	connectOrNot(link, m_advDialog->ui.cob_showRegion, SIGNAL(currentIndexChanged(int)), SLOT(comboBox_indexChanged(int)));
	connectOrNot(link, m_advDialog->ui.cob_onlyOneSide, SIGNAL(currentIndexChanged(int)), SLOT(comboBox_indexChanged(int)));

}

void edgeLocationWdg::iniUi()
{

}

void edgeLocationWdg::iniData()
{
	m_layerIdx = 0;
	m_nowGrpIndex = 0;
	m_IsSelecting = false;
	m_advDialog = new AdvancedDialog(this);
	m_image = nullptr;

	m_param = nullptr;
	m_module = nullptr;
	for (auto i = 0;i < 4;++i)
		m_edgeLine[i] = nullptr;
}

void edgeLocationWdg::setUiValue()
{
	if (!m_param)
		return;

	ui->sp_maxAngle->setValue(m_param->maxAngle);
	changeGroupIndex(m_nowGrpIndex);
	m_advDialog->ui.dsp_pixSize->setValue(m_param->pixSize);
	ui->cb_errWidth->setChecked(m_param->IsErrWidth == 1);
	ui->dsp_errWidthMin->setValue(m_param->errWidthMin);
	//ui.dsp_errWidthMin->setEnabled(m_param->IsErrWidth==1);
	ui->dsp_errWidthMax->setValue(m_param->errWidthMax);
	//ui.dsp_errWidthMax->setEnabled(m_param->IsErrWidth==1);

	ui->cb_errHeight->setChecked(m_param->IsErrHeight == 1);
	ui->dsp_errHeightMin->setValue(m_param->errHeightMin);
	//ui.dsp_errHeightMin->setEnabled(m_param->IsErrHeight==1);
	ui->dsp_errHeightMax->setValue(m_param->errHeightmax);
	//ui.dsp_errHeightMax->setEnabled(m_param->IsErrHeight==1);

	ui->cb_errHeightDiff->setChecked(m_param->IsErrHeightDiff);
	ui->dsp_errHeightDiff->setValue(m_param->errHeightDiff);
	//ui.dsp_errHeightDiff->setEnabled(m_param->IsErrHeightDiff);
	ui->cb_errWidthDiff->setChecked(m_param->IsErrWidthDiff);
	ui->dsp_errWidthDiff->setValue(m_param->errWidthDiff);
	//ui.dsp_errWidthDiff->setEnabled(m_param->IsErrWidthDiff);

	ui->cb_errAngle->setChecked(m_param->IsErrAngle == 1);
	ui->sp_errAngle->setValue(m_param->errAngle);
	//ui.sp_errAngle->setEnabled(m_param->IsErrAngle==1);

	ui->cb_errCenterOffset->setChecked(m_param->IsErrCenterOffset == 1);
	ui->dsp_errCenterOffset->setValue(m_param->errCenterOffset);
	//ui.dsp_errCenterOffset->setEnabled(m_param->IsErrCenterOffset==1);

	m_advDialog->ui.cb_refer->setChecked(m_param->IsRefer);
	m_advDialog->ui.cob_showRegion->setCurrentIndex(m_param->IsShowRegionAfterDet);
	m_advDialog->ui.cb_inputPixel->setChecked(m_param->IsInputPixel);
	m_advDialog->ui.cb_noDetect->setChecked(m_param->IsSideNoDetect);
	m_advDialog->ui.grp_noDetect->setVisible(m_param->IsSideNoDetect);
	m_advDialog->ui.sp_fixWidth->setValue(m_param->fixWidth);
	if (m_param->SideDetectType == 0)
	{
		m_advDialog->ui.rb_onlyUpDw->setChecked(1);
	}
	else if (m_param->SideDetectType == 1)
	{
		m_advDialog->ui.rb_onlyLfRt->setChecked(1);
	}
	else if (m_param->SideDetectType == 2)
	{
		m_advDialog->ui.rb_onlyOneSide->setChecked(1);
	}
	m_advDialog->ui.cob_onlyOneSide->setCurrentIndex(m_param->onlyOneSideIndex);

	if (ui->bt_advance->isChecked()) {
		ui->bt_advance->setChecked(false);
	}
	ui->grp_advance->setVisible(false);
	ui->tabWidget->setCurrentIndex(0);

}

void edgeLocationWdg::changeGroupIndex(int index)
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
	ui->cb_crossGray->setChecked(m_param->CrossGrayValid[index]);
	ui->sp_crossGray->setValue(m_param->crossGray[index]);
	ui->sp_crossStep->setValue(m_param->crossStep[index]);
}

void edgeLocationWdg::changeFixWidthValid()
{
	int fixvalid[4] = { 0,0,0,0 };
	if (m_param->IsSideNoDetect)
	{
		if (m_param->SideDetectType == 0)
		{
			fixvalid[1] = fixvalid[3] = 1;
		}
		else if (m_param->SideDetectType == 1) {
			fixvalid[0] = fixvalid[2] = 1;
		}
		else {
			fixvalid[m_param->onlyOneSideIndex] = 1;
		}
	}
	for (int i = 0;i < 4;i++) {
		m_param->IsFixWidth[i] = fixvalid[i];
	}
}

void edgeLocationWdg::on_bt_setOrigin_clicked()
{
	HTuple centerRow, centerCol;
	m_module->getDispOutData(CenterRow, centerRow);
	m_module->getDispOutData(CenterCol, centerCol);
	if (centerRow.Num() != 0)
	{
		if (QMessageBox::information(this, tr("Hint"),
			tr("Are you sure to change the current center point(%1,%2) to the template center?")
			.arg(centerCol[0].D(), 0, 'f', 3).arg(centerRow[0].D(), 0, 'f', 3),
			QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
		{
			m_param->TmpCenterRow = centerRow[0].D();
			m_param->TmpCenterCol = centerCol[0].D();
		}
	}
}

void edgeLocationWdg::radioButton_toggled(bool check)
{
	QString str0 = QString("");
	QString str1 = QString("");
	QString str2 = QString("");

	QRadioButton* rb = qobject_cast<QRadioButton*>(sender());
	bool matched = false;
	if (check)
	{
		m_module->setParamChanged(ProductLevel);

		if (rb == m_advDialog->ui.rb_onlyUpDw) {
			m_param->SideDetectType = 0;
			changeFixWidthValid();
			matched = true;
		}
		else if (rb == m_advDialog->ui.rb_onlyLfRt) {
			m_param->SideDetectType = 1;
			changeFixWidthValid();
			matched = true;
		}
		else if (rb == m_advDialog->ui.rb_onlyOneSide) {
			m_param->SideDetectType = 2;
			changeFixWidthValid();
			matched = true;
		}
		if (matched)
		{
			str1 = tr(" change to %1 ").arg(rb->text());
			//log(LogL1, str1);
			return;
		}
		if (rb == ui->rb_blackToWhite) {
			str0 = ui->label_20->text();
			str1 = tr(" change to %1 ").arg(rb->text());
			str2 = ui->tabWidget->tabText(m_nowGrpIndex);
			m_param->whiteBlackDir[m_nowGrpIndex] = !check;
		}
		else if (rb == ui->rb_whiteToBlack) {
			str0 = ui->label_20->text();
			str1 = tr(" change to %1 ").arg(rb->text());
			str2 = ui->tabWidget->tabText(m_nowGrpIndex);
			m_param->whiteBlackDir[m_nowGrpIndex] = check;
		}
		if (rb == ui->rb_inToOut) {
			str0 = ui->label_27->text();
			str1 = tr(" change to %1 ").arg(rb->text());
			str2 = ui->tabWidget->tabText(m_nowGrpIndex);
			m_param->searchDir[m_nowGrpIndex] = !check;
		}
		else if (rb == ui->rb_outToIn) {
			str0 = ui->label_27->text();
			str1 = tr(" change to %1 ").arg(rb->text());
			str2 = ui->tabWidget->tabText(m_nowGrpIndex);
			m_param->searchDir[m_nowGrpIndex] = check;
		}
		m_edgeLine[m_nowGrpIndex]->setPolarAndOrient((fitEdgeLine::Polarity)m_param->whiteBlackDir[m_nowGrpIndex], m_dir[m_param->searchDir[m_nowGrpIndex]][m_nowGrpIndex]);
		//log(LogL1, str2 + str0 + str1);
	}
}

void edgeLocationWdg::checkBox_toggled(bool check)
{
	QCheckBox* cb = qobject_cast<QCheckBox*>(sender());

	QString str0 = cb->text();
	QString str1;
	if (check)
		str1 = tr(" Check");
	else
		str1 = tr(" unCheck");
	QString str2 = QString("");

	if (cb == ui->cb_errWidth) {
		m_param->IsErrWidth = check;
		//ui.dsp_errWidthMin->setEnabled(check);
		//ui.dsp_errWidthMax->setEnabled(check);
	}
	else if (cb == ui->cb_errHeight) {
		m_param->IsErrHeight = check;
		//ui.dsp_errHeightMin->setEnabled(check);
		//ui.dsp_errHeightMax->setEnabled(check);
	}
	else if (cb == ui->cb_errHeightDiff) {
		m_param->IsErrHeightDiff = check;
		//ui.dsp_errHeightDiff->setEnabled(check);
	}
	else if (cb == ui->cb_errWidthDiff) {
		m_param->IsErrWidthDiff = check;
		//ui.dsp_errWidthDiff->setEnabled(check);
	}
	else if (cb == ui->cb_errAngle) {
		m_param->IsErrAngle = check;
		//ui.sp_errAngle->setEnabled(check);
	}
	else if (cb == ui->cb_errCenterOffset) {
		m_param->IsErrCenterOffset = check;
		//ui.dsp_errCenterOffset->setEnabled(check);
	}
	else if (cb == m_advDialog->ui.cb_refer) {
		m_param->IsRefer = check;
	}
	else if (cb == m_advDialog->ui.cb_inputPixel) {
		m_param->IsInputPixel = check;
	}
	else if (cb == m_advDialog->ui.cb_noDetect) {
		m_param->IsSideNoDetect = check;
		m_advDialog->ui.grp_noDetect->setVisible(check);
		changeFixWidthValid();
	}
	//else if (cb==m_advDialog->ui.cb_fixWidth){
	//	str2=ui.tabWidget->tabText(m_nowGrpIndex);
	//	m_param->IsFixWidth[m_nowGrpIndex]=check;
	//	//ui.sp_fixWidth->setEnabled(check);
	//}
	else if (cb == ui->cb_crossGray) {
		m_param->CrossGrayValid[m_nowGrpIndex] = check;
		//ui.sp_fixWidth->setEnabled(check);
		m_edgeLine[m_nowGrpIndex]->setCrossGrayValid(m_param->CrossGrayValid[m_nowGrpIndex], m_param->crossGray[m_nowGrpIndex], m_param->crossStep[m_nowGrpIndex]);
	}
	m_module->setParamChanged(ProductLevel);
		//log(LogL1, str2 + str0 + str1);
}

void edgeLocationWdg::doubleSpinBox_valueChanged(double val)
{
	QDoubleSpinBox* dsp = qobject_cast<QDoubleSpinBox*>(sender());

	QString str0 = QString("");
	QString str1 = QString("");
	QString str2 = QString("");
	if (dsp == ui->dsp_errWidthMin) {
		str0 = ui->cb_errWidth->text() + tr(" min val ");
		str1 = tr(" change from %1 to %2").arg(QString::number(m_param->errWidthMin, 'f', 3)).arg(QString::number(val, 'f', 3));
		m_param->errWidthMin = val;
	}
	else if (dsp == ui->dsp_errWidthMax) {
		str0 = ui->cb_errWidth->text() + tr(" max val ");
		str1 = tr(" change from %1 to %2").arg(QString::number(m_param->errWidthMax, 'f', 3)).arg(QString::number(val, 'f', 3));
		m_param->errWidthMax = val;
	}
	else if (dsp == ui->dsp_errHeightMin) {
		str0 = ui->cb_errHeight->text() + tr(" min val ");
		str1 = tr(" change from %1 to %2").arg(QString::number(m_param->errHeightMin, 'f', 3)).arg(QString::number(val, 'f', 3));
		m_param->errHeightMin = val;
	}
	else if (dsp == ui->dsp_errHeightMax) {
		str0 = ui->cb_errHeight->text() + tr(" max val ");
		str1 = tr(" change from %1 to %2").arg(QString::number(m_param->errHeightmax, 'f', 3)).arg(QString::number(val, 'f', 3));
		m_param->errHeightmax = val;
	}
	else if (dsp == ui->dsp_errHeightDiff) {
		str0 = ui->cb_errHeightDiff->text() + tr(" val ");
		str1 = tr(" change from %1 to %2").arg(QString::number(m_param->errHeightDiff, 'f', 3)).arg(QString::number(val, 'f', 3));
		m_param->errHeightDiff = val;
	}
	else if (dsp == ui->dsp_errWidthDiff) {
		str0 = ui->cb_errWidthDiff->text() + tr(" val ");
		str1 = tr(" change from %1 to %2").arg(QString::number(m_param->errWidthDiff, 'f', 3)).arg(QString::number(val, 'f', 3));
		m_param->errWidthDiff = val;
	}
	else if (dsp == ui->dsp_errCenterOffset) {
		str0 = ui->cb_errCenterOffset->text();
		str1 = tr(" change from %1 to %2").arg(QString::number(m_param->errCenterOffset, 'f', 3)).arg(QString::number(val, 'f', 3));
		m_param->errCenterOffset = val;
	}
	else if (dsp == m_advDialog->ui.dsp_pixSize) {
		str0 = m_advDialog->ui.label_35->text();
		str1 = tr(" change from %1 to %2").arg(QString::number(m_param->pixSize, 'f', 3)).arg(QString::number(val, 'f', 3));
		m_param->pixSize = val;
	}
	else if (dsp == ui->dsp_factor) {
		str2 = ui->tabWidget->tabText(m_nowGrpIndex);
		str0 = ui->label_14->text();
		str1 = tr(" change from %1 to %2").arg(QString::number(m_param->clipfactor[m_nowGrpIndex], 'f', 3)).arg(QString::number(val, 'f', 3));
		m_param->clipfactor[m_nowGrpIndex] = val;
		m_edgeLine[m_nowGrpIndex]->setFitFactor(m_param->clipfactor[m_nowGrpIndex]);
	}
	m_module->setParamChanged(ProductLevel);
	//log(LogL1, str2 + str0 + str1);
}

void edgeLocationWdg::spinBox_valueChanged(int val)
{
	QString str0 = QString("");
	QString str1 = QString("");
	QString str2 = QString("");

	QSpinBox* sp = qobject_cast<QSpinBox*>(sender());
	if (sp == ui->sp_maxAngle) {
		str0 = ui->label->text();
		str1 = tr(" change from %1 to %2").arg(QString::number(m_param->maxAngle)).arg(QString::number(val));
		m_param->maxAngle = val;
	}
	else if (sp == ui->sp_errAngle) {
		str0 = ui->cb_errAngle->text();
		str1 = tr(" change from %1 to %2").arg(QString::number(m_param->errAngle)).arg(QString::number(val));
		m_param->errAngle = val;
	}
	else if (sp == m_advDialog->ui.sp_fixWidth) {
		str0 = m_advDialog->ui.lb_fixWidth->text();
		str1 = tr(" change from %1 to %2").arg(QString::number(m_param->fixWidth)).arg(QString::number(val));
		m_param->fixWidth = val;
	}
	else {
		str2 = ui->tabWidget->tabText(m_nowGrpIndex);
		if (sp == ui->sp_extendSearchLine) {
			str0 = ui->label_21->text();
			str1 = tr(" change from %1 to %2").arg(QString::number(m_param->extendSearchLine[m_nowGrpIndex])).arg(QString::number(val));
			m_param->extendSearchLine[m_nowGrpIndex] = val;
			m_edgeLine[m_nowGrpIndex]->setSearchParam(m_param->searchLineGap[m_nowGrpIndex], m_param->extendSearchLine[m_nowGrpIndex]);
		}
		else if (sp == ui->sp_minGrayDiff) {
			str0 = ui->label_25->text();
			str1 = tr(" change from %1 to %2").arg(QString::number(m_param->minGrayDiff[m_nowGrpIndex])).arg(QString::number(val));
			m_param->minGrayDiff[m_nowGrpIndex] = val;
			m_edgeLine[m_nowGrpIndex]->setThreshold(m_param->minGrayDiff[m_nowGrpIndex]);
		}
		else if (sp == ui->sp_searchLineGap) {
			str0 = ui->label_22->text();
			str1 = tr(" change from %1 to %2").arg(QString::number(m_param->searchLineGap[m_nowGrpIndex])).arg(QString::number(val));
			m_param->searchLineGap[m_nowGrpIndex] = val;
			m_edgeLine[m_nowGrpIndex]->setSearchParam(m_param->searchLineGap[m_nowGrpIndex], m_param->extendSearchLine[m_nowGrpIndex]);
		}
		else if (sp == ui->sp_lineOffset) {
			str0 = ui->label_24->text();
			str1 = tr(" change from %1 to %2").arg(QString::number(m_param->lineOffset[m_nowGrpIndex])).arg(QString::number(val));
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
				edgeLocationParam::RoiRegion seg;
				seg.row1 = seg.col1 = 40;
				seg.row2 = seg.col2 = 90;
				vector<edgeLocationParam::RoiRegion>::iterator itbegin = m_param->RoiRegionVector.begin() + beginIndex + m_param->roiNum[m_nowGrpIndex];
				m_param->RoiRegionVector.insert(itbegin, val - m_param->roiNum[m_nowGrpIndex], seg);
			}
			else if (val < m_param->roiNum[m_nowGrpIndex]) {
				vector<edgeLocationParam::RoiRegion>::iterator itbegin = m_param->RoiRegionVector.begin() + beginIndex + val;
				vector<edgeLocationParam::RoiRegion>::iterator itEnd = m_param->RoiRegionVector.begin() + beginIndex + m_param->roiNum[m_nowGrpIndex];
				m_param->RoiRegionVector.erase(itbegin, itEnd);
			}

			str0 = ui->label_46->text();
			str1 = tr(" change from %1 to %2").arg(QString::number(m_param->roiNum[m_nowGrpIndex])).arg(QString::number(val));
			m_param->roiNum[m_nowGrpIndex] = val;
			int roiCount = 1;
			for (int i = 0;i < m_nowGrpIndex;i++) {
				roiCount += m_param->roiNum[i];
			}
			int drawRow1[20], drawRow2[20], drawCol1[20], drawCol2[20];  //4*5
			for (int j = 0;j < m_param->roiNum[m_nowGrpIndex];j++) {
				drawRow1[j] = m_param->RoiRegionVector[j + roiCount].row1;
				drawRow2[j] = m_param->RoiRegionVector[j + roiCount].row2;
				drawCol1[j] = m_param->RoiRegionVector[j + roiCount].col1;
				drawCol2[j] = m_param->RoiRegionVector[j + roiCount].col2;
			}
			m_edgeLine[m_nowGrpIndex]->setRoi(m_param->roiNum[m_nowGrpIndex], drawRow1, drawCol1, drawRow2, drawCol2);
		}
		else if (sp == ui->sp_crossGray) {
			str0 = ui->cb_crossGray->text();
			str1 = tr(" change from %1 to %2").arg(QString::number(m_param->crossGray[m_nowGrpIndex])).arg(QString::number(val));
			m_param->crossGray[m_nowGrpIndex] = val;
			m_edgeLine[m_nowGrpIndex]->setCrossGrayValid(m_param->CrossGrayValid[m_nowGrpIndex], m_param->crossGray[m_nowGrpIndex], m_param->crossStep[m_nowGrpIndex]);
		}
		else if (sp == ui->sp_crossStep) {
			str0 = ui->lb_crossStep->text();
			str1 = tr(" change from %1 to %2").arg(QString::number(m_param->crossStep[m_nowGrpIndex])).arg(QString::number(val));
			m_param->crossStep[m_nowGrpIndex] = val;
			m_edgeLine[m_nowGrpIndex]->setCrossGrayValid(m_param->CrossGrayValid[m_nowGrpIndex], m_param->crossGray[m_nowGrpIndex], m_param->crossStep[m_nowGrpIndex]);
		}
	}
	m_module->setParamChanged(ProductLevel);
	//log(LogL1, str2 + str0 + str1);
}

void edgeLocationWdg::comboBox_indexChanged(int index)
{
	QString str0 = QString("");
	QString str1 = QString("");

	QComboBox* cb = qobject_cast<QComboBox *>(sender());
	if (cb == m_advDialog->ui.cob_showRegion) {
		str1 = tr(" change from %1 to %2")
			.arg(m_advDialog->ui.cob_showRegion->itemText(m_param->IsShowRegionAfterDet))
			.arg(m_advDialog->ui.cob_showRegion->currentText());
		m_param->IsShowRegionAfterDet = index;
	}
	else if (cb == m_advDialog->ui.cob_onlyOneSide) {
		str1 = tr(" change from %1 to %2")
			.arg(m_advDialog->ui.cob_onlyOneSide->itemText(m_param->onlyOneSideIndex))
			.arg(m_advDialog->ui.cob_onlyOneSide->currentText());
		m_param->onlyOneSideIndex = index;
		changeFixWidthValid();
	}
	m_module->setParamChanged(ProductLevel);
	//log(LogL1, str0 + str1);
}

void edgeLocationWdg::on_bt_selectRoi_toggled(bool checked)
{
	int roiNum = 0;
	for (size_t i = 0;i < 4;++i)
		roiNum += m_param->roiNum[i];

	if (checked)
	{
		if (ui->bt_HelpRect->isChecked())
			ui->bt_HelpRect->setChecked(false);

		m_IsSelecting = true;
		double drawRow1[20], drawRow2[20], drawCol1[20], drawCol2[20];  //4*5
		QStringList title;
		int beforeNum = 0;  //前面一组一共有多少个roi
		int grpIndex = 0; //隶属哪一个组别
		QStringList regionName = { tr("up"),tr("right"),tr("down"),tr("left") };

		for (size_t i = 0;i < roiNum;++i)
		{
			drawRow1[i] = m_param->RoiRegionVector[i + 1].row1;
			drawRow2[i] = m_param->RoiRegionVector[i + 1].row2;
			drawCol1[i] = m_param->RoiRegionVector[i + 1].col1;
			drawCol2[i] = m_param->RoiRegionVector[i + 1].col2;

			if ((i >= beforeNum) && (i < (beforeNum + m_param->roiNum[grpIndex])))
			{
				title << QString("%1_%2").arg(regionName[grpIndex]).arg(i - beforeNum);
				//sprintf(buf[i], "%s_%d", tr(regionName[grpIndex]).toLocal8Bit().constData(), i - beforeNum);
				if (i == (beforeNum + m_param->roiNum[grpIndex] - 1))
				{
					beforeNum += m_param->roiNum[grpIndex];
					grpIndex++;
				}
			}
		}
		currentView()->drawRects1(roiNum, drawRow1, drawCol1, drawRow2, drawCol2, title);
		currentView()->update();
	}
	else
	{
		m_IsSelecting = false;
		currentView()->finishDraw();
		double row1[20], col1[20], row2[20], col2[20];
		currentView()->getDrawRects1(roiNum, row1, col1, row2, col2);
		for (int i = 0;i < roiNum;i++) {
			m_param->RoiRegionVector[i + 1].row1 = row1[i];
			m_param->RoiRegionVector[i + 1].col1 = col1[i];
			m_param->RoiRegionVector[i + 1].row2 = row2[i];
			m_param->RoiRegionVector[i + 1].col2 = col2[i];
		}
		//currentView()->clear();
		currentView()->clearLayer(m_layerIdx);
		currentView()->setColor(0, 255, 0);
		currentView()->setFilled(false);
		for (int i = 0;i < roiNum;i++) {
			currentView()->dispRect1(row1[i], col1[i], row2[i], col2[i]);
		}
		currentView()->update();

		int roiCount = 1;
		for (int i = 0;i < 4;i++) {
			// up right dw left
			int drawRow1[20], drawRow2[20], drawCol1[20], drawCol2[20];  //4*5
			for (int j = 0;j < m_param->roiNum[i];j++) {
				drawRow1[j] = m_param->RoiRegionVector[j + roiCount].row1;
				drawRow2[j] = m_param->RoiRegionVector[j + roiCount].row2;
				drawCol1[j] = m_param->RoiRegionVector[j + roiCount].col1;
				drawCol2[j] = m_param->RoiRegionVector[j + roiCount].col2;
			}
			m_edgeLine[i]->setRoi(m_param->roiNum[i], drawRow1, drawCol1, drawRow2, drawCol2);
			roiCount += m_param->roiNum[i];
		}
		m_module->setParamChanged(ProductLevel);
		//log(LogL1, ui->bt_selectRoi->text());
	}
}

void edgeLocationWdg::on_bt_HelpRect_toggled(bool checked)
{
	if (checked) {
		if (ui->bt_selectRoi->isChecked())
			ui->bt_selectRoi->setChecked(false);

		double row1 = m_param->RoiRegionVector[0].row1;
		double col1 = m_param->RoiRegionVector[0].col1;
		double row2 = m_param->RoiRegionVector[0].row2;
		double col2 = m_param->RoiRegionVector[0].col2;
		//currentView()->clear();
		currentView()->clearLayer(m_layerIdx);
		currentView()->setColor(255, 0, 128);
		currentView()->drawRect1(row1, col1, row2, col2, tr("HelpRect"));
		currentView()->update();
	}
	else {
		HalOpenCV::cv2halImg(*m_module->p_im, m_image, false);

		double row1, col1, row2, col2;
		currentView()->finishDraw();
		currentView()->getDrawRect1(row1, col1, row2, col2);
		//currentView()->clear();
		currentView()->clearLayer(m_layerIdx);
		currentView()->setColor(0, 255, 0);
		currentView()->setFilled(false);
		currentView()->dispRect1(row1, col1, row2, col2);

		m_param->RoiRegionVector[0].row1 = row1;
		m_param->RoiRegionVector[0].col1 = col1;
		m_param->RoiRegionVector[0].row2 = row2;
		m_param->RoiRegionVector[0].col2 = col2;

		//生成四个位置的框
		double centerRow = (row1 + row2) / 2;
		double centerCol = (col1 + col2) / 2;

		HTuple imgH, imgW;
		get_image_size(*m_image, &imgW, &imgH);
		int imgh = imgH[0].I();
		int imgw = imgW[0].I();

		int wid = (col2 - col1) / 10;         //ROI区域的宽
		wid = max(wid, 12);
		int beforeNum = 1;
		for (size_t i = 0;i < 4;++i)
		{
			double totalRow1, totalCol1, totalRow2, totalCol2;        //整体的框，具体ROI区域，根据ROI数目平分
			if (i == 0 || i == 2)       //0:up 2:down
			{
				totalRow1 = (i == 0) ? (row1 - wid) : (row2 - wid);
				totalRow2 = (i == 0) ? (row1 + wid) : (row2 + wid);
				totalCol1 = col1;
				totalCol2 = col2;

				if (i == 0)//up
					totalRow1 = max(totalRow1, 0.0);
				else if (i == 2)//down
					totalRow2 = min(totalRow2, (double)imgh);

				//根据ROI数平分ROI区域
				for (size_t j = 0;j < m_param->roiNum[i];++j)
				{
					m_param->RoiRegionVector[j + beforeNum].row1 = totalRow1;
					m_param->RoiRegionVector[j + beforeNum].row2 = totalRow2;
					m_param->RoiRegionVector[j + beforeNum].col1 = totalCol1 + j*(totalCol2 - totalCol1) / m_param->roiNum[i];
					m_param->RoiRegionVector[j + beforeNum].col2 = totalCol1 + (j + 1)*(totalCol2 - totalCol1) / m_param->roiNum[i];
				}
			}
			else if (i == 1 || i == 3)  //1:right 3:left
			{
				totalRow1 = row1;
				totalRow2 = row2;
				totalCol1 = (i == 3) ? (col1 - wid) : (col2 - wid);
				totalCol2 = (i == 3) ? (col1 + wid) : (col2 + wid);

				if (i == 3)//left
					totalCol1 = max(totalCol1, 0.0);
				else if (i == 1)//right
					totalCol2 = min(totalCol2, (double)imgw);

				//根据ROI数平分ROI区域
				for (size_t j = 0;j < m_param->roiNum[i];++j)
				{
					m_param->RoiRegionVector[j + beforeNum].row1 = totalRow1 + j*(totalRow2 - totalRow1) / m_param->roiNum[i];
					m_param->RoiRegionVector[j + beforeNum].row2 = totalRow1 + (j + 1)*(totalRow2 - totalRow1) / m_param->roiNum[i];
					m_param->RoiRegionVector[j + beforeNum].col1 = totalCol1;
					m_param->RoiRegionVector[j + beforeNum].col2 = totalCol2;
				}
			}
			beforeNum += m_param->roiNum[i];
		}
		ui->bt_selectRoi->setChecked(true);
		int roiCount = 1;
		for (int i = 0;i < 4;i++) {
			// up right dw left
			int drawRow1[20], drawRow2[20], drawCol1[20], drawCol2[20];  //4*5
			for (int j = 0;j < m_param->roiNum[i];j++) {
				drawRow1[j] = m_param->RoiRegionVector[j + roiCount].row1;
				drawRow2[j] = m_param->RoiRegionVector[j + roiCount].row2;
				drawCol1[j] = m_param->RoiRegionVector[j + roiCount].col1;
				drawCol2[j] = m_param->RoiRegionVector[j + roiCount].col2;
			}
			m_edgeLine[i]->setRoi(m_param->roiNum[i], drawRow1, drawCol1, drawRow2, drawCol2);
			roiCount += m_param->roiNum[i];
		}
	}
}

void edgeLocationWdg::on_bt_advance_toggled(bool val)
{
	if (val) {
		ui->bt_advance->setText(tr("exit advance"));
	}
	else {
		ui->bt_advance->setText(tr("advance set"));
	}
	ui->grp_advance->setVisible(val);
}

void edgeLocationWdg::on_bt_AdvDialog_clicked()
{
	m_advDialog->exec();
}

void edgeLocationWdg::tabWidget_currentIndexChanged(int index)
{
	QTabWidget* tab = qobject_cast<QTabWidget*>(sender());
	if (tab == ui->tabWidget) {
		m_nowGrpIndex = index;
		connectSlots(false);
		changeGroupIndex(index);
		connectSlots(true);
	}
}