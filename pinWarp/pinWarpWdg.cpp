#include "pinWarpWdg.h"
#include "pinWarpMod.h"
#include "pinWarpParam.h"
#include "ui_pinWarpWdg.h"
#include "ImageView.h"

pinWarpWdg::pinWarpWdg(int i) :
	ModSetWidget(),ui(new Ui::pinWarpWdg),m_widgetType(i)
{
	m_hasConnect = false;
	ui->setupUi(this);
	iniUi();
	m_layerIdx = 0;
	m_param = nullptr;
	m_module = nullptr;
}

pinWarpWdg::~pinWarpWdg()
{
	delete ui;
}

void pinWarpWdg::setModule(UnitModule *module)
{
	m_module=dynamic_cast<pinWarpMod*>(module);
	m_param = m_module->m_param;
	//currentView()->setLayer(m_layerIdx);
	//updateUi();
}

void pinWarpWdg::showEvent(QShowEvent * event)
{
	updateUi();
}

void pinWarpWdg::radioButton_toggled(bool check)
{
	QRadioButton* rb = qobject_cast<QRadioButton*>(sender());
	QString str0 = QString("");
	QString str1 = QString("");
	if (check) {

		QRadioButton *RB = NULL;
		if (m_param->searchDir[m_param->currentGrp] == 0)
			RB = ui->rb_LfToRt;
		else if (m_param->searchDir[m_param->currentGrp] == 1)
			RB = ui->rb_RtToLf;
		else if (m_param->searchDir[m_param->currentGrp] == 2)
			RB = ui->rb_UpToDw;
		else if (m_param->searchDir[m_param->currentGrp] == 3)
			RB = ui->rb_DwToUp;

		if (rb == ui->rb_blackToWhite) {
			str0 = ui->cob_currentGrp->currentText() + QString(" ") + ui->label_20->text();
			str1 = tr(" change from %1 to %2").arg(ui->rb_whiteToBlack->text()).arg(ui->rb_blackToWhite->text());
			m_param->whiteBlackDir[m_param->currentGrp] = 0;
		}
		else if (rb == ui->rb_whiteToBlack) {
			str0 = ui->cob_currentGrp->currentText() + QString(" ") + ui->label_20->text();
			str1 = tr(" change from %1 to %2").arg(ui->rb_blackToWhite->text()).arg(ui->rb_whiteToBlack->text());
			m_param->whiteBlackDir[m_param->currentGrp] = 1;
		}
		else if (rb == ui->rb_LfToRt) {
			str0 = ui->cob_currentGrp->currentText() + QString(" ") + ui->label_26->text();
			str1 = tr(" change from %1 to %2").arg(RB->text()).arg(rb->text());
			m_param->searchDir[m_param->currentGrp] = 0;
		}
		else if (rb == ui->rb_RtToLf) {
			str0 = ui->cob_currentGrp->currentText() + QString(" ") + ui->label_26->text();
			str1 = tr(" change from %1 to %2").arg(RB->text()).arg(rb->text());
			m_param->searchDir[m_param->currentGrp] = 1;
		}
		else if (rb == ui->rb_UpToDw) {
			str0 = ui->cob_currentGrp->currentText() + QString(" ") + ui->label_26->text();
			str1 = tr(" change from %1 to %2").arg(RB->text()).arg(rb->text());
			m_param->searchDir[m_param->currentGrp] = 2;
		}
		else if (rb == ui->rb_DwToUp) {
			str0 = ui->cob_currentGrp->currentText() + QString(" ") + ui->label_26->text();
			str1 = tr(" change from %1 to %2").arg(RB->text()).arg(rb->text());
			m_param->searchDir[m_param->currentGrp] = 3;
		}
		else if (rb == ui->rb_detectBase) {
			str1 = tr(" change from %1 to %2").arg(ui->rb_noBase->text()).arg(ui->rb_detectBase->text());
			m_param->IsDetBase[m_param->currentGrp] = 1;
		}
		else if (rb == ui->rb_noBase) {
			str1 = tr(" change from %1 to %2").arg(ui->rb_detectBase->text()).arg(ui->rb_noBase->text());
			m_param->IsDetBase[m_param->currentGrp] = 0;
			for (int i = 0;i < 4;i++) {
				cb_errWarp[i]->setChecked(false);
			}
		}
		m_module->setParamChanged(ProductLevel);
		//log(LogL1, str0 + str1);
	}
}

void pinWarpWdg::checkBox_toggled(bool check)
{
	QCheckBox* cb = qobject_cast<QCheckBox*>(sender());
	QString str0 = QString("");
	QString str1 = QString("");
	if (check)
		str1 = tr(" Check");
	else
		str1 = tr(" Uncheck");
	if (cb == ui->cb_enableGrp) {
		str0 = ui->cob_currentGrp->currentText() + QString(" ") + cb->text();
		m_param->grpEnable[m_param->currentGrp] = check;
	}
	else if (cb == ui->cb_whitePin) {
		str0 = ui->cob_currentGrp->currentText() + QString(" ") + cb->text();
		m_param->IsWhitePin[m_param->currentGrp] = check;
	}
	else if (cb == ui->cb_baseSameWithLead) {
		str0 = ui->cob_currentGrp->currentText() + QString(" ") + cb->text();
		m_param->IsBaseSameWithLead[m_param->currentGrp] = check;
	}
	else if (cb == ui->cb_inputRatio) {
		str0 = ui->cob_currentGrp->currentText() + QString(" ") + cb->text();
		m_param->IsInputRatio[m_param->currentGrp] = check;
	}
	else {
		for (int i = 0;i < GRP_NUM;i++) {
			if (cb == cb_totalWidth[i]) {
				str0 = ui->tabWidget->tabText(i) + QString(" ") + cb->text();
				m_param->errWidthValid[i] = check;
				dsp_TotalWidthMin[i]->setEnabled(check);
				dsp_TotalWidthMax[i]->setEnabled(check);
				break;
			}
			else if (cb == cb_errWarp[i]) {
				str0 = ui->tabWidget->tabText(i) + QString(" ") + cb->text();
				m_param->errWarpValid[i] = check;
				dsp_ErrWarp[i]->setEnabled(check);
				break;
			}
		}
	}
	m_module->setParamChanged(ProductLevel);
	//log(LogL1, str0 + str1);
}

void pinWarpWdg::doubleSpinBox_valueChanged(double val)
{
	QDoubleSpinBox* dsp = qobject_cast<QDoubleSpinBox*>(sender());
	QString str0 = QString("");
	QString str1 = QString("");
	if (dsp == ui->dsp_factor) {
		str0 = ui->cob_currentGrp->currentText() + QString(" ") + ui->label_14->text();
		str1 = tr(" change from %1 to %2")
			.arg(QString::number(m_param->clipfactor[m_param->currentGrp], 'f', 1))
			.arg(QString::number(val, 'f', 1));
		m_param->clipfactor[m_param->currentGrp] = val;
	}
	else if (dsp == ui->dsp_ratio) {
		str0 = ui->cob_currentGrp->currentText() + QString(" ") + ui->label_23->text();
		str1 = tr(" change from %1 to %2")
			.arg(QString::number(m_param->sizeRatio[m_param->currentGrp], 'f', 3))
			.arg(QString::number(val, 'f', 3));
		m_param->sizeRatio[m_param->currentGrp] = val;
	}
	else if (dsp == ui->dsp_normalOffMin) {
		str0 = ui->cob_currentGrp->currentText() + QString(" ") + ui->label->text() + tr(" min val");
		str1 = tr(" change from %1 to %2")
			.arg(QString::number(m_param->normalOffMin[m_param->currentGrp], 'f', 3))
			.arg(QString::number(val, 'f', 3));
		m_param->normalOffMin[m_param->currentGrp] = val;
	}
	else if (dsp == ui->dsp_normalOffMax) {
		str0 = ui->cob_currentGrp->currentText() + QString(" ") + ui->label->text() + tr(" max val");
		str1 = tr(" change from %1 to %2")
			.arg(QString::number(m_param->normalOffMax[m_param->currentGrp], 'f', 3))
			.arg(QString::number(val, 'f', 3));
		m_param->normalOffMax[m_param->currentGrp] = val;
	}
	else {
		for (int i = 0;i < GRP_NUM;i++) {
			if (dsp == dsp_TotalWidthMin[i]) {
				str0 = ui->tabWidget->tabText(i) + QString(" ") + cb_totalWidth[i]->text() + tr(" min val");
				str1 = tr(" change from %1 to %2")
					.arg(QString::number(m_param->errWidthMin[i], 'f', 3))
					.arg(QString::number(val, 'f', 3));
				m_param->errWidthMin[i] = val;
				break;
			}
			else if (dsp == dsp_TotalWidthMax[i]) {
				str0 = ui->tabWidget->tabText(i) + QString(" ") + cb_totalWidth[i]->text() + tr(" max val");
				str1 = tr(" change from %1 to %2")
					.arg(QString::number(m_param->errWidthMax[i], 'f', 3))
					.arg(QString::number(val, 'f', 3));
				m_param->errWidthMax[i] = val;
				break;
			}
			else if (dsp == dsp_ErrWarp[i]) {
				str0 = ui->tabWidget->tabText(i) + QString(" ") + cb_errWarp[i]->text();
				str1 = tr(" change from %1 to %2")
					.arg(QString::number(m_param->errWarp[i], 'f', 3))
					.arg(QString::number(val, 'f', 3));
				m_param->errWarp[i] = val;
				break;
			}
		}
	}
	m_module->setParamChanged(ProductLevel);
	//log(LogL1, str0 + str1);
}

void pinWarpWdg::spinBox_valueChanged(int val)
{
	QSpinBox* sp = qobject_cast<QSpinBox*>(sender());
	QString str0 = QString("");
	QString str1 = QString("");
	if (sp == ui->sp_extendSearchLine) {
		str0 = ui->cob_currentGrp->currentText() + QString(" ") + ui->label_21->text();
		str1 = tr(" change from %1 to %2").arg(m_param->extendSearchLine[m_param->currentGrp]).arg(val);
		m_param->extendSearchLine[m_param->currentGrp] = val;
	}
	else if (sp == ui->sp_leadSearchLength) {
		str0 = ui->cob_currentGrp->currentText() + QString(" ") + ui->label_29->text();
		str1 = tr(" change from %1 to %2").arg(m_param->leadSearchLength[m_param->currentGrp]).arg(val);
		m_param->leadSearchLength[m_param->currentGrp] = val;
	}
	else if (sp == ui->sp_leadGrayDiff) {
		str0 = ui->cob_currentGrp->currentText() + QString(" ") + ui->label_25->text();
		str1 = tr(" change from %1 to %2").arg(m_param->leadGrayDiff[m_param->currentGrp]).arg(val);
		m_param->leadGrayDiff[m_param->currentGrp] = val;
	}
	else if (sp == ui->sp_baseGrayDiff) {
		str0 = ui->cob_currentGrp->currentText() + QString(" ") + ui->label_28->text();
		str1 = tr(" change from %1 to %2").arg(m_param->baseGrayDiff[m_param->currentGrp]).arg(val);
		m_param->baseGrayDiff[m_param->currentGrp] = val;
	}
	else if (sp == ui->sp_searchLineGap) {
		str0 = ui->cob_currentGrp->currentText() + QString(" ") + ui->label_22->text();
		str1 = tr(" change from %1 to %2").arg(m_param->searchLineGap[m_param->currentGrp]).arg(val);
		m_param->searchLineGap[m_param->currentGrp] = val;
	}
	else if (sp == ui->sp_deleteEdge) {
		str0 = ui->cob_currentGrp->currentText() + QString(" ") + ui->label_27->text();
		str1 = tr(" change from %1 to %2").arg(m_param->deleteEdge[m_param->currentGrp]).arg(val);
		m_param->deleteEdge[m_param->currentGrp] = val;
	}
	else if (sp == ui->sp_noiseMax) {
		str0 = ui->cob_currentGrp->currentText() + QString(" ") + ui->label_4->text();
		str1 = tr(" change from %1 to %2").arg(m_param->noiseMax[m_param->currentGrp]).arg(val);
		m_param->noiseMax[m_param->currentGrp] = val;
	}
	m_module->setParamChanged(ProductLevel);
	//log(LogL1, str0 + str1);
}

void pinWarpWdg::on_bt_selectRoi_toggled(bool check)
{
	if (check) {
		ui->cob_currentGrp->setEnabled(false);
		//currentView()->clear();
		currentView()->clearLayer(m_layerIdx);
		double row1[2], col1[2], row2[2], col2[2];
		QStringList title;
		for (int i = 0;i < 2;i++) {
			row1[i] = m_param->RoiReg[m_param->currentGrp][i].row1;
			col1[i] = m_param->RoiReg[m_param->currentGrp][i].col1;
			row2[i] = m_param->RoiReg[m_param->currentGrp][i].row2;
			col2[i] = m_param->RoiReg[m_param->currentGrp][i].col2;
			if (i == 0) {
				title << tr("base line");
			}
			else {
				title << tr("lead%1").arg(i);
			}
		}
		currentView()->setColor(255, 0, 128);
		currentView()->drawRects1(2, row1, col1, row2, col2, title);
		currentView()->update();
	}
	else {
		m_module->setParamChanged(ProductLevel);
		//log(LogL1, ui->bt_selectRoi->text());

		ui->cob_currentGrp->setEnabled(true);
		currentView()->finishDraw();
		double row1[2], col1[2], row2[2], col2[2];
		currentView()->getDrawRects1(2, row1, col1, row2, col2);
		for (int i = 0;i < 2;i++) {
			m_param->RoiReg[m_param->currentGrp][i].row1 = row1[i];
			m_param->RoiReg[m_param->currentGrp][i].col1 = col1[i];
			m_param->RoiReg[m_param->currentGrp][i].row2 = row2[i];
			m_param->RoiReg[m_param->currentGrp][i].col2 = col2[i];
		}
		//currentView()->clear();
		currentView()->clearLayer(m_layerIdx);
		currentView()->update();
	}
}

void pinWarpWdg::comboBox_IndexChanged(int index)
{
	QComboBox* cob = qobject_cast<QComboBox*>(sender());
	QString str1 = QString("");
	if (cob == ui->cob_currentGrp) {
		str1 = tr(" change from %1 to %2").arg(cob->itemText(m_param->currentGrp)).arg(cob->itemText(index));
		m_param->currentGrp = index;
		connectSlots(false);
		setCurrentGrp(index);
		connectSlots(true);
	}
	m_module->setParamChanged(ProductLevel);
	//log(LogL1, str1);
}

void pinWarpWdg::connectSlots(bool link)
{
	if(m_hasConnect==link)
		return;
	m_hasConnect=link;

	connectOrNot(link, ui->cob_currentGrp, SIGNAL(currentIndexChanged(int)), SLOT(comboBox_IndexChanged(int)));

	connectOrNot(link, ui->cb_enableGrp, SIGNAL(toggled(bool)), SLOT(checkBox_toggled(bool)));
	connectOrNot(link, ui->cb_whitePin, SIGNAL(toggled(bool)), SLOT(checkBox_toggled(bool)));
	connectOrNot(link, ui->cb_baseSameWithLead, SIGNAL(toggled(bool)), SLOT(checkBox_toggled(bool)));
	connectOrNot(link, ui->cb_inputRatio, SIGNAL(toggled(bool)), SLOT(checkBox_toggled(bool)));

	connectOrNot(link, ui->rb_blackToWhite, SIGNAL(toggled(bool)), SLOT(radioButton_toggled(bool)));
	connectOrNot(link, ui->rb_whiteToBlack, SIGNAL(toggled(bool)), SLOT(radioButton_toggled(bool)));

	connectOrNot(link, ui->rb_noBase, SIGNAL(toggled(bool)), SLOT(radioButton_toggled(bool)));
	connectOrNot(link, ui->rb_detectBase, SIGNAL(toggled(bool)), SLOT(radioButton_toggled(bool)));

	connectOrNot(link, ui->rb_LfToRt, SIGNAL(toggled(bool)), SLOT(radioButton_toggled(bool)));
	connectOrNot(link, ui->rb_RtToLf, SIGNAL(toggled(bool)), SLOT(radioButton_toggled(bool)));
	connectOrNot(link, ui->rb_UpToDw, SIGNAL(toggled(bool)), SLOT(radioButton_toggled(bool)));
	connectOrNot(link, ui->rb_DwToUp, SIGNAL(toggled(bool)), SLOT(radioButton_toggled(bool)));

	connectOrNot(link, ui->sp_extendSearchLine, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_searchLineGap, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_leadGrayDiff, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_baseGrayDiff, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_leadSearchLength, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_deleteEdge, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_noiseMax, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));

	connectOrNot(link, ui->dsp_factor, SIGNAL(valueChanged(double)), SLOT(doubleSpinBox_valueChanged(double)));
	connectOrNot(link, ui->dsp_ratio, SIGNAL(valueChanged(double)), SLOT(doubleSpinBox_valueChanged(double)));
	connectOrNot(link, ui->dsp_normalOffMin, SIGNAL(valueChanged(double)), SLOT(doubleSpinBox_valueChanged(double)));
	connectOrNot(link, ui->dsp_normalOffMax, SIGNAL(valueChanged(double)), SLOT(doubleSpinBox_valueChanged(double)));

	for (int i = 0;i < GRP_NUM;i++) {
		connectOrNot(link, cb_totalWidth[i], SIGNAL(toggled(bool)), SLOT(checkBox_toggled(bool)));
		connectOrNot(link, cb_errWarp[i], SIGNAL(toggled(bool)), SLOT(checkBox_toggled(bool)));

		connectOrNot(link, dsp_TotalWidthMin[i], SIGNAL(valueChanged(double)), SLOT(doubleSpinBox_valueChanged(double)));
		connectOrNot(link, dsp_TotalWidthMax[i], SIGNAL(valueChanged(double)), SLOT(doubleSpinBox_valueChanged(double)));
		connectOrNot(link, dsp_ErrWarp[i], SIGNAL(valueChanged(double)), SLOT(doubleSpinBox_valueChanged(double)));
	}
}

void pinWarpWdg::iniUi()
{
	for (int i = 0;i < GRP_NUM;i++) {
		cb_totalWidth[i] = findChild<QCheckBox*>(QString("cb_totalWidth_%1").arg(i));
		cb_errWarp[i] = findChild<QCheckBox*>(QString("cb_errWarp_%1").arg(i));

		dsp_TotalWidthMin[i] = findChild<QDoubleSpinBox*>(QString("dsp_TotalWidthMin_%1").arg(i));
		dsp_TotalWidthMax[i] = findChild<QDoubleSpinBox*>(QString("dsp_TotalWidthMax_%1").arg(i));
		dsp_ErrWarp[i] = findChild<QDoubleSpinBox*>(QString("dsp_ErrWarp_%1").arg(i));
	}
}

void pinWarpWdg::setUiValue()
{
	if(!m_param)
		return;

	for (int i = 0;i < GRP_NUM;i++) {
		cb_totalWidth[i]->setChecked(m_param->errWidthValid[i]);
		dsp_TotalWidthMin[i]->setEnabled(m_param->errWidthValid[i]);
		dsp_TotalWidthMax[i]->setEnabled(m_param->errWidthValid[i]);
		dsp_TotalWidthMin[i]->setValue(m_param->errWidthMin[i]);
		dsp_TotalWidthMax[i]->setValue(m_param->errWidthMax[i]);

		cb_errWarp[i]->setChecked(m_param->errWarpValid[i]);
		dsp_ErrWarp[i]->setEnabled(m_param->errWarpValid[i]);
		dsp_ErrWarp[i]->setValue(m_param->errWarp[i]);
	}
	setCurrentGrp(m_param->currentGrp);
}

void pinWarpWdg::updateUi()
{
	connectSlots(false);
	setUiValue();
	connectSlots(true);
}

void pinWarpWdg::setCurrentGrp(int index)
{
	ui->cob_currentGrp->setCurrentIndex(index);
	if (m_param->IsDetBase[index] == 0) {
		ui->rb_noBase->setChecked(true);
	}
	else {
		ui->rb_detectBase->setChecked(true);
	}
	ui->cb_whitePin->setChecked(m_param->IsWhitePin[index]);
	ui->cb_enableGrp->setChecked(m_param->grpEnable[index]);
	ui->cb_baseSameWithLead->setChecked(m_param->IsBaseSameWithLead[index]);
	ui->cb_inputRatio->setChecked(m_param->IsInputRatio[index]);

	if (m_param->whiteBlackDir[index] == 0) {
		ui->rb_blackToWhite->setChecked(true);
	}
	else if (m_param->whiteBlackDir[index] == 1) {
		ui->rb_whiteToBlack->setChecked(true);
	}
	if (m_param->searchDir[index] == 0)
		ui->rb_LfToRt->setChecked(true);
	else if (m_param->searchDir[index] == 1)
		ui->rb_RtToLf->setChecked(true);
	else if (m_param->searchDir[index] == 2)
		ui->rb_UpToDw->setChecked(true);
	else if (m_param->searchDir[index] == 3)
		ui->rb_DwToUp->setChecked(true);

	ui->dsp_factor->setValue(m_param->clipfactor[index]);
	ui->dsp_ratio->setValue(m_param->sizeRatio[index]);

	ui->sp_extendSearchLine->setValue(m_param->extendSearchLine[index]);
	ui->sp_searchLineGap->setValue(m_param->searchLineGap[index]);
	ui->sp_leadGrayDiff->setValue(m_param->leadGrayDiff[index]);
	ui->sp_baseGrayDiff->setValue(m_param->baseGrayDiff[index]);
	ui->sp_deleteEdge->setValue(m_param->deleteEdge[index]);
	ui->sp_noiseMax->setValue(m_param->noiseMax[index]);

	ui->sp_leadSearchLength->setValue(m_param->leadSearchLength[index]);
	ui->dsp_normalOffMin->setValue(m_param->normalOffMin[index]);
	ui->dsp_normalOffMax->setValue(m_param->normalOffMax[index]);
	ui->tabWidget->setCurrentIndex(index);
}

