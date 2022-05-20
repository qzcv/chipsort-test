#include "pinDistanceWdg.h"
#include "pinDistanceMod.h"
#include "pinDistanceParam.h"
#include "ui_pinDistanceWdg.h"
#include "ImageView.h"

pinDistanceWdg::pinDistanceWdg(int i) :
	ModSetWidget(),ui(new Ui::pinDistanceWdg),m_widgetType(i)
{
	m_hasConnect = false;
	ui->setupUi(this);
	iniUi();
	m_layerIdx = 0;
	m_param = nullptr;
	m_module = nullptr;
}

pinDistanceWdg::~pinDistanceWdg()
{
	delete ui;
}

void pinDistanceWdg::setModule(UnitModule *module)
{
	m_module=dynamic_cast<pinDistanceMod*>(module);
	m_param = m_module->m_param;
	//currentView()->setLayer(m_layerIdx);
	//connectSlots(false);
	//setUiValue();
	//connectSlots(true);
}

void pinDistanceWdg::connectSlots(bool link)
{
	if(m_hasConnect==link)
		return;
	m_hasConnect=link;

	connectOrNot(link, ui->cob_currentGrp, SIGNAL(currentIndexChanged(int)), SLOT(comboBox_IndexChanged(int)));
	connectOrNot(link, ui->cb_enableGrp, SIGNAL(toggled(bool)), SLOT(checkBox_toggled(bool)));
	connectOrNot(link, ui->cb_useRefer, SIGNAL(toggled(bool)), SLOT(checkBox_toggled(bool)));
	connectOrNot(link, ui->cb_distance, SIGNAL(toggled(bool)), SLOT(checkBox_toggled(bool)));
	connectOrNot(link, ui->sp_pinNum, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_threshold, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_edgeOffSet, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->dsp_ratio, SIGNAL(valueChanged(double)), SLOT(doubleSpinBox_valueChanged(double)));
	connectOrNot(link, ui->dsp_distanceMax, SIGNAL(valueChanged(double)), SLOT(doubleSpinBox_valueChanged(double)));
	connectOrNot(link, ui->dsp_distanceMin, SIGNAL(valueChanged(double)), SLOT(doubleSpinBox_valueChanged(double)));
}

void pinDistanceWdg::iniUi()
{

}

void pinDistanceWdg::setUiValue()
{
	if(!m_param)
		return;

	setCurrentGrp(m_param->currentGrp);
	ui->cb_distance->setChecked(m_param->errDistanceValid);
	ui->cb_useRefer->setChecked(m_param->IsRefer);
}

void pinDistanceWdg::setCurrentGrp(int index)
{
	ui->cob_currentGrp->setCurrentIndex(index);
	ui->cb_enableGrp->setChecked(m_param->grpEnable[index]);

	ui->dsp_ratio->setValue(m_param->sizeRatio[index]);
	ui->sp_pinNum->setValue(m_param->grpPinNum[index]);
	ui->sp_edgeOffSet->setValue(m_param->edgeOffset[index]);
	ui->sp_threshold->setValue(m_param->edgeThreshold[index]);
	ui->dsp_distanceMin->setValue(m_param->errDistanceMin[index]);
	ui->dsp_distanceMax->setValue(m_param->errDistanceMax[index]);
	ui->dsp_distanceMin->setEnabled(m_param->errDistanceValid);
	ui->dsp_distanceMax->setEnabled(m_param->errDistanceValid);
}

void pinDistanceWdg::checkBox_toggled(bool check)
{
	QCheckBox* cb = qobject_cast<QCheckBox*>(sender());
	QString str0 = cb->text();
	QString str1 = QString("");
	if (check)
		str1 = tr(" Check");
	else
		str1 = tr(" Uncheck");
	if (cb == ui->cb_enableGrp) {
		str0 = ui->cob_currentGrp->currentText() + QString(" ") + cb->text();
		m_param->grpEnable[m_param->currentGrp] = check;
	}
	else if (cb == ui->cb_useRefer) {
		m_param->IsRefer = check;
	}
	else if (cb == ui->cb_distance) {
		m_param->errDistanceValid = check;
		ui->dsp_distanceMin->setEnabled(check);
		ui->dsp_distanceMax->setEnabled(check);
	}
	m_module->setParamChanged(ProductLevel);
	//log(LogL1, str0 + str1);
}

void pinDistanceWdg::doubleSpinBox_valueChanged(double val)
{
	QDoubleSpinBox* dsp = qobject_cast<QDoubleSpinBox*>(sender());
	QString str0 = QString("");
	QString str1 = QString("");
	if (dsp == ui->dsp_ratio) {
		str0 = ui->cob_currentGrp->currentText() + QString(" ") + ui->label_23->text();
		str1 = tr(" change from %1 to %2")
			.arg(QString::number(m_param->sizeRatio[m_param->currentGrp], 'f', 3))
			.arg(QString::number(val, 'f', 3));
		m_param->sizeRatio[m_param->currentGrp] = val;
	}
	else if (dsp == ui->dsp_distanceMin) {
		str0 = ui->cb_distance->text() + tr(" min val");
		str1 = tr(" change from %1 to %2")
			.arg(QString::number(m_param->errDistanceMin[m_param->currentGrp], 'f', 3))
			.arg(QString::number(val, 'f', 3));
		m_param->errDistanceMin[m_param->currentGrp] = val;
	}
	else if (dsp == ui->dsp_distanceMax) {
		str0 = ui->cb_distance->text() + tr(" max val");
		str1 = tr(" change from %1 to %2")
			.arg(QString::number(m_param->errDistanceMax[m_param->currentGrp], 'f', 3))
			.arg(QString::number(val, 'f', 3));
		m_param->errDistanceMax[m_param->currentGrp] = val;
	}
	m_module->setParamChanged(ProductLevel);
	//log(LogL1, str0 + str1);
}

void pinDistanceWdg::spinBox_valueChanged(int val)
{
	QSpinBox* sp = qobject_cast<QSpinBox*>(sender());
	QString str0 = QString("");
	QString str1 = QString("");
	if (sp == ui->sp_pinNum) {
		pinDistanceParam::RoiRegion seg;
		seg.row1 = seg.col1 = 100;
		seg.row2 = seg.col2 = 150;  //给新添加的roi固定的开始位1置
		if (val >= m_param->grpPinNum[m_param->currentGrp]) {
			for (int i = m_param->grpPinNum[m_param->currentGrp];i < val;i++) {
				seg.row1 = seg.col1 += 10;
				seg.row2 = seg.col2 += 10;
				m_param->RoiVec[m_param->currentGrp].push_back(seg);
			}
		}
		else if (val < m_param->grpPinNum[m_param->currentGrp]) {
			vector<pinDistanceParam::RoiRegion>::iterator itbegin = m_param->RoiVec[m_param->currentGrp].begin() + val + 1;
			vector<pinDistanceParam::RoiRegion>::iterator itEnd = m_param->RoiVec[m_param->currentGrp].end();
			m_param->RoiVec[m_param->currentGrp].erase(itbegin, itEnd);
		}
		str0 = ui->cob_currentGrp->currentText() + QString(" ") + ui->label_4->text();
		str1 = tr(" change from %1 to %2").arg(m_param->grpPinNum[m_param->currentGrp]).arg(val);
		m_param->grpPinNum[m_param->currentGrp] = val;
	}
	else if (sp == ui->sp_threshold) {
		str0 = ui->cob_currentGrp->currentText() + QString(" ") + ui->label->text();
		str1 = tr(" change from %1 to %2").arg(m_param->edgeThreshold[m_param->currentGrp]).arg(val);
		m_param->edgeThreshold[m_param->currentGrp] = val;
	}
	else if (sp == ui->sp_edgeOffSet) {
		str0 = ui->cob_currentGrp->currentText() + QString(" ") + ui->label_15->text();
		str1 = tr(" change from %1 to %2").arg(m_param->edgeOffset[m_param->currentGrp]).arg(val);
		m_param->edgeOffset[m_param->currentGrp] = val;
	}
	m_module->setParamChanged(ProductLevel);
	//log(LogL1, str0 + str1);
}

void pinDistanceWdg::on_bt_selectRoi_toggled(bool check)
{
	if (check) {
		ui->cob_currentGrp->setEnabled(false);
		ui->sp_pinNum->setEnabled(false);
		//currentView()->clear();
		currentView()->clearLayer(m_layerIdx);
		double row1[10], col1[10], row2[10], col2[10];
		QStringList title;
		for (int i = 0;i < m_param->grpPinNum[m_param->currentGrp];i++) {
			row1[i] = m_param->RoiVec[m_param->currentGrp][i].row1;
			col1[i] = m_param->RoiVec[m_param->currentGrp][i].col1;
			row2[i] = m_param->RoiVec[m_param->currentGrp][i].row2;
			col2[i] = m_param->RoiVec[m_param->currentGrp][i].col2;
			title << tr("lead%1").arg(i + 1);
		}
		currentView()->setColor(255, 0, 128);
		currentView()->drawRects1(m_param->grpPinNum[m_param->currentGrp], row1, col1, row2, col2, title);
		currentView()->update();
	}
	else {
		//log(LogL1, ui->bt_selectRoi->text());
		ui->cob_currentGrp->setEnabled(true);
		ui->sp_pinNum->setEnabled(true);
		currentView()->finishDraw();
		double row1[10], col1[10], row2[10], col2[10];
		currentView()->getDrawRects1(m_param->grpPinNum[m_param->currentGrp], row1, col1, row2, col2);
		for (int i = 0;i < m_param->grpPinNum[m_param->currentGrp];i++) {
			m_param->RoiVec[m_param->currentGrp][i].row1 = row1[i];
			m_param->RoiVec[m_param->currentGrp][i].col1 = col1[i];
			m_param->RoiVec[m_param->currentGrp][i].row2 = row2[i];
			m_param->RoiVec[m_param->currentGrp][i].col2 = col2[i];
		}
		//currentView()->clear();
		currentView()->clearLayer(m_layerIdx);
		currentView()->update();

		m_module->setParamChanged(ProductLevel);
	}
}

void pinDistanceWdg::comboBox_IndexChanged(int index)
{
	QComboBox* cob = qobject_cast<QComboBox*>(sender());
	QString str0 = QString("");
	if (cob == ui->cob_currentGrp) {
		str0 = tr(" change from %1 to %2").arg(cob->itemText(m_param->currentGrp)).arg(cob->itemText(index));
		m_param->currentGrp = index;
		connectSlots(false);
		setCurrentGrp(index);
		connectSlots(true);
	}
	m_module->setParamChanged(ProductLevel);
	//log(LogL1, str0);
}
