#include "edgeInterWdg.h"
#include "edgeInterMod.h"
#include "edgeInterParam.h"
#include "ui_edgeInterWdg.h"
#include "ImageView.h"
#include "customView.h"
#include "imageViewItem.h"
#include <QMessageBox>
 
edgeInterWdg::edgeInterWdg(int i) :
	ModSetWidget(),ui(new Ui::edgeInterWdg),m_widgetType(i)
{
	m_hasConnect = false;
	ui->setupUi(this);
	iniData();
	iniUi();
	m_param = nullptr;
	m_module = nullptr;
}

edgeInterWdg::~edgeInterWdg()
{
	delete ui;
}

void edgeInterWdg::setModule(UnitModule *module)
{
	m_module=dynamic_cast<edgeInterMod*>(module);
	m_param = m_module->m_param;
	//currentView()->setLayer(m_layerIdx);
	//updateUi();
}

void edgeInterWdg::showEvent(QShowEvent * event)
{
	updateUi();
}

void edgeInterWdg::connectSlots(bool link)
{
	if(m_hasConnect==link)
		return;
	m_hasConnect=link;

	connectOrNot(link, ui->cb_showRegionAfterDet, SIGNAL(toggled(bool)), SLOT(checkBox_toggled(bool)));
	connectOrNot(link, ui->cb_errInter, SIGNAL(toggled(bool)), SLOT(checkBox_toggled(bool)));
	connectOrNot(link, ui->cb_refer, SIGNAL(toggled(bool)), SLOT(checkBox_toggled(bool)));
	connectOrNot(link, ui->cb_errX, SIGNAL(toggled(bool)), SLOT(checkBox_toggled(bool)));
	connectOrNot(link, ui->cb_errY, SIGNAL(toggled(bool)), SLOT(checkBox_toggled(bool)));

	connectOrNot(link, ui->dsp_pixelSize, SIGNAL(valueChanged(double)), SLOT(doubleSpinBox_valueChanged(double)));
	connectOrNot(link, ui->dsp_errInterMin, SIGNAL(valueChanged(double)), SLOT(doubleSpinBox_valueChanged(double)));
	connectOrNot(link, ui->dsp_errInterMax, SIGNAL(valueChanged(double)), SLOT(doubleSpinBox_valueChanged(double)));
	connectOrNot(link, ui->dsp_errXMin, SIGNAL(valueChanged(double)), SLOT(doubleSpinBox_valueChanged(double)));
	connectOrNot(link, ui->dsp_errXMax, SIGNAL(valueChanged(double)), SLOT(doubleSpinBox_valueChanged(double)));
	connectOrNot(link, ui->dsp_errYMin, SIGNAL(valueChanged(double)), SLOT(doubleSpinBox_valueChanged(double)));
	connectOrNot(link, ui->dsp_errYMax, SIGNAL(valueChanged(double)), SLOT(doubleSpinBox_valueChanged(double)));
	connectOrNot(link, ui->dsp_X, SIGNAL(valueChanged(double)), SLOT(doubleSpinBox_valueChanged(double)));
	connectOrNot(link, ui->dsp_Y, SIGNAL(valueChanged(double)), SLOT(doubleSpinBox_valueChanged(double)));
	connectOrNot(link, ui->dsp_offset, SIGNAL(valueChanged(double)), SLOT(doubleSpinBox_valueChanged(double)));

	connectOrNot(link, ui->sp_gradient, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_step, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_degRange, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_lineOffset, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_rmDist, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_iteratNum, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));

	connectOrNot(link, ui->cob_pol, SIGNAL(currentIndexChanged(int)), SLOT(comboBox_indexChanged(int)));
	connectOrNot(link, ui->cob_first, SIGNAL(currentIndexChanged(int)), SLOT(comboBox_indexChanged(int)));
	connectOrNot(link, ui->cob_edgeIndex, SIGNAL(currentIndexChanged(int)), SLOT(comboBox_indexChanged(int)));
}

void edgeInterWdg::iniUi()
{

}

void edgeInterWdg::iniData()
{
	m_rectList = nullptr;
	m_layerIdx = 1;
}

void edgeInterWdg::setUiValue()
{
	if(!m_param)
		return;

	ui->cob_edgeIndex->clear();
	for (int i = 0;i < MAX_EDGE_LINE;i++) {
		ui->cob_edgeIndex->addItem(tr("Edge %1").arg(i + 1));
	}
	changeEdgeIndex(m_param->currentEdge);
	ui->cb_errInter->setChecked(m_param->IsErrInter == 1);
	ui->dsp_errInterMin->setValue(m_param->errInterMin);
	ui->dsp_errInterMax->setValue(m_param->errInterMax);

	ui->cb_errX->setChecked(m_param->IsErrX);
	ui->dsp_errXMin->setValue(m_param->errXMin);
	ui->dsp_errXMax->setValue(m_param->errXMax);

	ui->cb_errY->setChecked(m_param->IsErrY);
	ui->dsp_errYMin->setValue(m_param->errYMin);
	ui->dsp_errYMax->setValue(m_param->errYMax);

	ui->dsp_X->setValue(m_param->referX);
	ui->dsp_Y->setValue(m_param->referY);

	ui->dsp_pixelSize->setValue(m_param->pixSize);
	ui->dsp_offset->setValue(m_param->interOffset);

	ui->cb_refer->setChecked(m_param->IsRefer);
	ui->cb_showRegionAfterDet->setChecked(m_param->IsShowRegionAfterDet);
	ui->cob_edgeIndex->setCurrentIndex(m_param->currentEdge);
	if (ui->bt_advance->isChecked())
		ui->bt_advance->setChecked(false);
	ui->grp_advance->setVisible(false);
}

void edgeInterWdg::updateUi()
{
	connectSlots(false);
	setUiValue();
	connectSlots(true);
}

void edgeInterWdg::changeEdgeIndex(int idx)
{
	ui->sp_gradient->setValue(m_param->gradient[idx]);
	ui->sp_step->setValue(m_param->stepVal[idx]);
	ui->sp_degRange->setValue(m_param->degRange[idx]);
	ui->sp_lineOffset->setValue(m_param->lineOffset[idx]);
	ui->sp_rmDist->setValue(m_param->rmDist[idx]);
	ui->sp_iteratNum->setValue(m_param->iterateNum[idx]);
	ui->cob_first->setCurrentIndex(m_param->first[idx]);
	ui->cob_pol->setCurrentIndex(m_param->pol[idx]);
}

void edgeInterWdg::drawRects2(int num, double * row, double * col, double * angle, double * len1, double * len2, const QStringList & titles, int arrowSize)
{
	QList<ImageViewRotRectList::RotRectListItem> list;
	for (auto i = 0;i < num;++i)
	{
		ImageViewRotRectList::RotRectListItem item;
		item.center = QPointF(col[i], row[i]);
		item.angle = angle[i];
		item.len[0] = len1[i];
		item.len[1] = len2[i];
		item.text = titles[i];

		list.append(item);
	}

	if (m_rectList)
		delete m_rectList;
	m_rectList = new cusViewRotRectList(list, arrowSize);
	currentView()->drawItem(m_rectList, true);
}

void edgeInterWdg::checkBox_toggled(bool check)
{
	QCheckBox* cb = qobject_cast<QCheckBox*>(sender());
	QString str0 = QString("");
	if (check)
		str0 = tr(" Check");
	else
		str0 = tr(" Uncheck");
	if (cb == ui->cb_errInter) {
		m_param->IsErrInter = check;
	}
	else if (cb == ui->cb_errX) {
		m_param->IsErrX = check;
	}
	else if (cb == ui->cb_errY) {
		m_param->IsErrY = check;
	}
	else if (cb == ui->cb_refer) {
		m_param->IsRefer = check;
	}
	else if (cb == ui->cb_showRegionAfterDet) {
		m_param->IsShowRegionAfterDet = check;
	}
	//log(LogL1, cb->text() + str0);

	m_module->setParamChanged(ProductLevel);
}

void edgeInterWdg::doubleSpinBox_valueChanged(double val)
{
	QDoubleSpinBox* dsp = qobject_cast<QDoubleSpinBox*>(sender());
	QString str0 = QString("");
	QString str1 = QString("");
	if (dsp == ui->dsp_pixelSize) {
		str0 = ui->label->text();
		str1 = tr(" change from %1 to %2")
			.arg(QString::number(m_param->pixSize, 'f', 3))
			.arg(QString::number(val, 'f', 3));
		m_param->pixSize = val;
	}
	else if (dsp == ui->dsp_errInterMin) {
		str0 = ui->cb_errInter->text() + tr(" min val");
		str1 = tr(" change from %1 to %2")
			.arg(QString::number(m_param->errInterMin, 'f', 3))
			.arg(QString::number(val, 'f', 3));
		m_param->errInterMin = val;
	}
	else if (dsp == ui->dsp_errInterMax) {
		str0 = ui->cb_errInter->text() + tr(" max val");
		str1 = tr(" change from %1 to %2")
			.arg(QString::number(m_param->errInterMax, 'f', 3))
			.arg(QString::number(val, 'f', 3));
		m_param->errInterMax = val;
	}
	else if (dsp == ui->dsp_errXMin) {
		str0 = ui->cb_errX->text() + tr(" min val");
		str1 = tr(" change from %1 to %2")
			.arg(QString::number(m_param->errXMin, 'f', 3))
			.arg(QString::number(val, 'f', 3));
		m_param->errXMin = val;
	}
	else if (dsp == ui->dsp_errXMax) {
		str0 = ui->cb_errX->text() + tr(" max val");
		str1 = tr(" change from %1 to %2")
			.arg(QString::number(m_param->errXMax, 'f', 3))
			.arg(QString::number(val, 'f', 3));
		m_param->errXMax = val;
	}
	else if (dsp == ui->dsp_errYMin) {
		str0 = ui->cb_errY->text() + tr(" min val");
		str1 = tr(" change from %1 to %2")
			.arg(QString::number(m_param->errYMin, 'f', 3))
			.arg(QString::number(val, 'f', 3));
		m_param->errYMin = val;
	}
	else if (dsp == ui->dsp_errYMax) {
		str0 = ui->cb_errY->text() + tr(" max val");
		str1 = tr(" change from %1 to %2")
			.arg(QString::number(m_param->errYMax, 'f', 3))
			.arg(QString::number(val, 'f', 3));
		m_param->errYMax = val;
	}
	else if (dsp == ui->dsp_X) {
		str0 = ui->label_3->text() + tr(" X:");
		str1 = tr(" change from %1 to %2")
			.arg(QString::number(m_param->referX, 'f', 3))
			.arg(QString::number(val, 'f', 3));
		m_param->referX = val;
	}
	else if (dsp == ui->dsp_Y) {
		str0 = ui->label_3->text() + tr(" Y:");
		str1 = tr(" change from %1 to %2")
			.arg(QString::number(m_param->referY, 'f', 3))
			.arg(QString::number(val, 'f', 3));
		m_param->referY = val;
	}
	else if (dsp == ui->dsp_offset) {
		str0 = ui->label_2->text();
		str1 = tr(" change from %1 to %2")
			.arg(QString::number(m_param->interOffset, 'f', 3))
			.arg(QString::number(val, 'f', 3));
		m_param->interOffset = val;
	}
	//log(LogL1, str0 + str1);

	m_module->setParamChanged(ProductLevel);
}

void edgeInterWdg::spinBox_valueChanged(int val)
{
	QSpinBox* sp = qobject_cast<QSpinBox*>(sender());
	QString str0 = ui->cob_edgeIndex->currentText();
	QString str1 = QString("");
	int i = m_param->currentEdge;
	if (sp == ui->sp_gradient)
	{
		str0 += QString(" ") + ui->label_24->text();
		str1 = tr(" change from %1 to %2").arg(m_param->gradient[i]).arg(val);
		m_param->gradient[i] = val;
	}
	else if (sp == ui->sp_step)
	{
		str0 += QString(" ") + ui->label_33->text();
		str1 = tr(" change from %1 to %2").arg(m_param->stepVal[i]).arg(val);
		m_param->stepVal[i] = val;
	}
	else if (sp == ui->sp_degRange)
	{
		str0 += QString(" ") + ui->label_35->text();
		str1 = tr(" change from %1 to %2").arg(m_param->degRange[i]).arg(val);
		m_param->degRange[i] = val;
	}
	else if (sp == ui->sp_rmDist)
	{
		str0 += QString(" ") + ui->label_37->text();
		str1 = tr(" change from %1 to %2").arg(m_param->rmDist[i]).arg(val);
		m_param->rmDist[i] = val;
	}
	else if (sp == ui->sp_iteratNum)
	{
		str0 += QString(" ") + ui->label_38->text();
		str1 = tr(" change from %1 to %2").arg(m_param->iterateNum[i]).arg(val);
		m_param->iterateNum[i] = val;
	}
	else if (sp == ui->sp_lineOffset)
	{
		str0 += QString(" ") + ui->label_39->text();
		str1 = tr(" change from %1 to %2").arg(m_param->lineOffset[i]).arg(val);
		m_param->lineOffset[i] = val;
	}
	//log(LogL1, str0 + str1);

	m_module->setParamChanged(ProductLevel);
}

void edgeInterWdg::comboBox_indexChanged(int val)
{
	QComboBox* cob = qobject_cast<QComboBox*>(sender());
	QString str0 = QString("");
	QString str1 = QString("");
	if (cob == ui->cob_pol) {
		int i = m_param->currentEdge;
		str0 = ui->cob_edgeIndex->currentIndex() + QString(" ") + ui->label_34->text();
		str1 = tr(" change from %1 to %2").arg(cob->itemText(m_param->pol[i])).arg(cob->itemText(val));
		m_param->pol[i] = val;
	}
	else if (cob == ui->cob_first) {
		int i = m_param->currentEdge;
		str0 = ui->cob_edgeIndex->currentIndex() + QString(" ") + ui->label_34->text();
		str1 = tr(" change from %1 to %2").arg(cob->itemText(m_param->first[i])).arg(cob->itemText(val));
		m_param->first[i] = val;
	}
	else if (cob == ui->cob_edgeIndex) {
		str0 = ui->label_25->text();
		str1 = tr(" change from %1 to %2").arg(cob->itemText(m_param->currentEdge)).arg(cob->itemText(val));
		m_param->currentEdge = val;
		connectSlots(false);
		changeEdgeIndex(val);
		connectSlots(true);
	}
	//log(LogL1, str0 + str1);

	m_module->setParamChanged(ProductLevel);
}

void edgeInterWdg::on_bt_selectRoi_clicked()
{
	ui->bt_ok->setEnabled(true);
	ui->bt_cancel->setEnabled(true);
	ui->bt_selectRoi->setEnabled(false);
	double row[MAX_EDGE_LINE], col[MAX_EDGE_LINE], angle[MAX_EDGE_LINE], len1[MAX_EDGE_LINE], len2[MAX_EDGE_LINE];
	QStringList title;
	for (int i = 0;i < MAX_EDGE_LINE;i++) {
		row[i] = m_param->roiReg[i].row;
		col[i] = m_param->roiReg[i].col;
		angle[i] = m_param->roiReg[i].angle;
		len1[i] = m_param->roiReg[i].length1;
		len2[i] = m_param->roiReg[i].length2;

		title << QString("edge%1").arg(i + 1);
	}
	//currentView()->clear();
	currentView()->clearLayer(m_layerIdx);
	currentView()->setColor(255, 0, 128);
//	currentView()->drawRects2(MAX_EDGE_LINE, row, col, angle, len1, len2, title, 10);
	drawRects2(MAX_EDGE_LINE, row, col, angle, len1, len2, title, 10);
	currentView()->update();
}

void edgeInterWdg::on_bt_ok_clicked()
{
	ui->bt_ok->setEnabled(0);
	ui->bt_cancel->setEnabled(0);
	ui->bt_selectRoi->setEnabled(1);

	currentView()->finishDraw();
	double row[MAX_EDGE_LINE], col[MAX_EDGE_LINE], angle[MAX_EDGE_LINE], len1[MAX_EDGE_LINE], len2[MAX_EDGE_LINE];
	currentView()->getDrawRects2(MAX_EDGE_LINE, row, col, angle, len1, len2);
	for (int i = 0;i < MAX_EDGE_LINE;i++) {
		m_param->roiReg[i].row = row[i];
		m_param->roiReg[i].col = col[i];
		m_param->roiReg[i].angle = angle[i];
		m_param->roiReg[i].length1 = len1[i];
		m_param->roiReg[i].length2 = len2[i];
	}
	//currentView()->clear();
	currentView()->clearLayer(m_layerIdx);
	currentView()->update();

	m_module->setParamChanged(ProductLevel);
	//log(LogL1, ui.bt_ok->text());
}

void edgeInterWdg::on_bt_cancel_clicked()
{
	ui->bt_ok->setEnabled(0);
	ui->bt_cancel->setEnabled(0);
	ui->bt_selectRoi->setEnabled(1);
	currentView()->finishDraw();
	//currentView()->clear();
	currentView()->clearLayer(m_layerIdx);
	currentView()->update();
}

void edgeInterWdg::on_bt_setOrigin_clicked()
{
	if (QMessageBox::information(this, tr("Hint"), 
		tr("Are you sure to set the currently computed center point as the reference center point?"),
		QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
		//log(LogL1, ui.bt_setOrigin->text());
		ui->dsp_X->setValue(m_module->m_centerX);
		ui->dsp_Y->setValue(m_module->m_centerY);

		m_module->setParamChanged(ProductLevel);
	}
}

void edgeInterWdg::on_bt_advance_toggled(bool val)
{
	ui->grp_advance->setVisible(val);
	if (val)
		ui->bt_advance->setText(tr("exit advance"));
	else
		ui->bt_advance->setText(tr("advance"));
}
