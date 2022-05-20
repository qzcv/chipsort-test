#include "edgeWarpWdg.h"
#include "edgeWarpMod.h"
#include "edgeWarpParam.h"
#include "ui_edgeWarpWdg.h"
#include "ImageView.h"
#include "customView.h"

edgeWarpWdg::edgeWarpWdg(int i) :
	ModSetWidget(),ui(new Ui::edgeWarpWdg),m_widgetType(i)
{
	m_hasConnect = false;
	ui->setupUi(this);
	iniUi();
	m_layerIdx = 0;
	m_param = nullptr;
	m_module = nullptr;
	m_rectList = nullptr;
}

edgeWarpWdg::~edgeWarpWdg()
{
	if(m_rectList)
		delete m_rectList;
	delete ui;
}

void edgeWarpWdg::setModule(UnitModule *module)
{
	m_module=dynamic_cast<edgeWarpMod*>(module);
	m_param = m_module->m_param;
	//currentView()->setLayer(m_layerIdx);
	//connectSlots(false);
	//setUiValue();
	//connectSlots(true);
}

void edgeWarpWdg::showEvent(QShowEvent * event)
{
	connectSlots(false);
	setUiValue();
	connectSlots(true);
}

void edgeWarpWdg::connectSlots(bool link)
{
	if(m_hasConnect==link)
		return;
	m_hasConnect=link;

	connectOrNot(link, ui->cb_showRegionAfterDet, SIGNAL(toggled(bool)), SLOT(checkBox_toggled(bool)));
	connectOrNot(link, ui->cb_errWarp, SIGNAL(toggled(bool)), SLOT(checkBox_toggled(bool)));
	connectOrNot(link, ui->cb_errWarpDiff, SIGNAL(toggled(bool)), SLOT(checkBox_toggled(bool)));
	connectOrNot(link, ui->cb_refer, SIGNAL(toggled(bool)), SLOT(checkBox_toggled(bool)));

	connectOrNot(link, ui->rb_UpToDw, SIGNAL(toggled(bool)), SLOT(radioButton_toggled(bool)));
	connectOrNot(link, ui->rb_DwToUp, SIGNAL(toggled(bool)), SLOT(radioButton_toggled(bool)));
	connectOrNot(link, ui->rb_LfToRt, SIGNAL(toggled(bool)), SLOT(radioButton_toggled(bool)));
	connectOrNot(link, ui->rb_RtToLf, SIGNAL(toggled(bool)), SLOT(radioButton_toggled(bool)));
	connectOrNot(link, ui->rb_white, SIGNAL(toggled(bool)), SLOT(radioButton_toggled(bool)));
	connectOrNot(link, ui->rb_black, SIGNAL(toggled(bool)), SLOT(radioButton_toggled(bool)));

	connectOrNot(link, ui->dsp_pixelSize, SIGNAL(valueChanged(double)), SLOT(doubleSpinBox_valueChanged(double)));
	connectOrNot(link, ui->dsp_errWarpMin, SIGNAL(valueChanged(double)), SLOT(doubleSpinBox_valueChanged(double)));
	connectOrNot(link, ui->dsp_errWarpMax, SIGNAL(valueChanged(double)), SLOT(doubleSpinBox_valueChanged(double)));
	connectOrNot(link, ui->dsp_errWarpDiffMin, SIGNAL(valueChanged(double)), SLOT(doubleSpinBox_valueChanged(double)));
	connectOrNot(link, ui->dsp_errWarpDiffMax, SIGNAL(valueChanged(double)), SLOT(doubleSpinBox_valueChanged(double)));
	connectOrNot(link, ui->dsp_offset, SIGNAL(valueChanged(double)), SLOT(doubleSpinBox_valueChanged(double)));

	connectOrNot(link, ui->sp_gradient, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_step, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_degRange, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_rmDist, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_iteratNum, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_lineOffset, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_leadThre, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));

	connectOrNot(link, ui->cob_pol, SIGNAL(currentIndexChanged(int)), SLOT(comboBox_indexChanged(int)));
	connectOrNot(link, ui->cob_first, SIGNAL(currentIndexChanged(int)), SLOT(comboBox_indexChanged(int)));
	connectOrNot(link, ui->cob_edgeIndex, SIGNAL(currentIndexChanged(int)), SLOT(comboBox_indexChanged(int)));
	connectOrNot(link, ui->cob_inputPixel, SIGNAL(currentIndexChanged(int)), SLOT(comboBox_indexChanged(int)));
}

void edgeWarpWdg::iniUi()
{

}

void edgeWarpWdg::setUiValue()
{
	if(!m_param)
		return;

	changeEdgeIndex(m_param->currentEdge);
	if (m_param->leadDir == 0) {
		ui->rb_UpToDw->setChecked(1);
	}
	else if (m_param->leadDir == 1) {
		ui->rb_DwToUp->setChecked(1);
	}
	else if (m_param->leadDir == 2) {
		ui->rb_LfToRt->setChecked(1);
	}
	else if (m_param->leadDir == 3) {
		ui->rb_RtToLf->setChecked(1);
	}
	if (m_param->IsWhite == 0) {
		ui->rb_black->setChecked(1);
	}
	else {
		ui->rb_white->setChecked(1);
	}
	ui->cb_errWarp->setChecked(m_param->IsErrWarp);
	ui->dsp_errWarpMin->setValue(m_param->errWarpMin);
	ui->dsp_errWarpMax->setValue(m_param->errWarpMax);
	ui->dsp_errWarpMin->setEnabled(m_param->IsErrWarp);
	ui->dsp_errWarpMax->setEnabled(m_param->IsErrWarp);

	ui->cb_errWarpDiff->setChecked(m_param->IsErrWarpDiff);
	ui->dsp_errWarpDiffMin->setValue(m_param->errWarpDiffMin);
	ui->dsp_errWarpDiffMax->setValue(m_param->errWarpDiffMax);
	ui->dsp_errWarpDiffMin->setEnabled(m_param->IsErrWarpDiff);
	ui->dsp_errWarpDiffMax->setEnabled(m_param->IsErrWarpDiff);

	ui->dsp_pixelSize->setValue(m_param->pixSize);
	ui->dsp_offset->setValue(m_param->warpOffset);

	ui->cb_refer->setChecked(m_param->IsRefer);
	ui->cb_showRegionAfterDet->setChecked(m_param->IsShowRegionAfterDet);
	ui->cob_edgeIndex->setCurrentIndex(m_param->currentEdge);
	ui->cob_inputPixel->setCurrentIndex(m_param->isInputPix);
	if (ui->bt_advance->isChecked()) {
		ui->bt_advance->setChecked(false);
	}
	ui->grp_advance->setVisible(false);
}

void edgeWarpWdg::drawRects2(int num, double * row, double * col, double * angle, double * len1, double * len2, const QStringList & titles, int arrowSize)
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

void edgeWarpWdg::changeEdgeIndex(int idx)
{
	ui->sp_gradient->setValue(m_param->gradient[idx]);
	ui->sp_step->setValue(m_param->stepVal[idx]);
	ui->sp_degRange->setValue(m_param->degRange[idx]);
	ui->sp_lineOffset->setValue(m_param->lineOffset[idx]);
	ui->sp_leadThre->setValue(m_param->leadThre[idx]);
	ui->sp_rmDist->setValue(m_param->rmDist[idx]);
	ui->sp_iteratNum->setValue(m_param->iterateNum[idx]);
	ui->cob_first->setCurrentIndex(m_param->first[idx]);
	ui->cob_pol->setCurrentIndex(m_param->pol[idx]);
}

void edgeWarpWdg::checkBox_toggled(bool check)
{
	QCheckBox* cb = qobject_cast<QCheckBox*>(sender());
	QString str0 = cb->text();
	QString str1 = QString("");
	if (check)
		str1 = tr(" Check");
	else
		str1 = tr(" Uncheck");
	if (cb == ui->cb_errWarp) {
		m_param->IsErrWarp = check;
		ui->dsp_errWarpMin->setEnabled(check);
		ui->dsp_errWarpMax->setEnabled(check);
	}
	else if (cb == ui->cb_errWarpDiff) {
		m_param->IsErrWarpDiff = check;
		ui->dsp_errWarpDiffMin->setEnabled(check);
		ui->dsp_errWarpDiffMax->setEnabled(check);
	}
	else if (cb == ui->cb_refer) {
		m_param->IsRefer = check;
	}
	else if (cb == ui->cb_showRegionAfterDet) {
		m_param->IsShowRegionAfterDet = check;
	}
	m_module->setParamChanged(ProductLevel);
	//log(LogL1, str0 + str1);
}

void edgeWarpWdg::doubleSpinBox_valueChanged(double val)
{
	QDoubleSpinBox* dsp = qobject_cast<QDoubleSpinBox*>(sender());
	QString str0 = QString("");
	QString str1 = QString("");
	if (dsp == ui->dsp_pixelSize) {
		str0 = ui->label->text();
		str1 = tr(" change from %1 to %2").arg(QString::number(m_param->pixSize, 'f', 3)).arg(QString::number(val, 'f', 3));
		m_param->pixSize = val;
	}
	else if (dsp == ui->dsp_errWarpMin) {
		str0 = ui->cb_errWarp->text() + tr(" min val");
		str1 = tr(" change from %1 to %2").arg(QString::number(m_param->errWarpMin, 'f', 3)).arg(QString::number(val, 'f', 3));
		m_param->errWarpMin = val;
	}
	else if (dsp == ui->dsp_errWarpMax) {
		str0 = ui->cb_errWarp->text() + tr(" max val");
		str1 = tr(" change from %1 to %2").arg(QString::number(m_param->errWarpMax, 'f', 3)).arg(QString::number(val, 'f', 3));
		m_param->errWarpMax = val;
	}
	else if (dsp == ui->dsp_errWarpDiffMin) {
		str0 = ui->cb_errWarpDiff->text() + tr(" min val");
		str1 = tr(" change from %1 to %2").arg(QString::number(m_param->errWarpDiffMin, 'f', 3)).arg(QString::number(val, 'f', 3));
		m_param->errWarpDiffMin = val;
	}
	else if (dsp == ui->dsp_errWarpDiffMax) {
		str0 = ui->cb_errWarp->text() + tr(" max val");
		str1 = tr(" change from %1 to %2").arg(QString::number(m_param->errWarpDiffMax, 'f', 3)).arg(QString::number(val, 'f', 3));
		m_param->errWarpDiffMax = val;
	}
	else if (dsp == ui->dsp_offset) {
		str0 = ui->label_2->text();
		str1 = tr(" change from %1 to %2").arg(QString::number(m_param->warpOffset, 'f', 3)).arg(QString::number(val, 'f', 3));
		m_param->warpOffset = val;
	}
	m_module->setParamChanged(ProductLevel);
	//log(LogL1, str0 + str1);
}

void edgeWarpWdg::spinBox_valueChanged(int val)
{
	QSpinBox* sp = qobject_cast<QSpinBox*>(sender());
	QString str0 = QString("");
	QString str1 = QString("");
	int i = m_param->currentEdge;
	if (sp == ui->sp_gradient)
	{
		str0 = ui->cob_edgeIndex->currentText() + QString(" ") + ui->label_24->text();
		str1 = tr(" change from %1 to %2").arg(m_param->gradient[i]).arg(val);
		m_param->gradient[i] = val;
	}
	else if (sp == ui->sp_step)
	{
		str0 = ui->cob_edgeIndex->currentText() + QString(" ") + ui->label_33->text();
		str1 = tr(" change from %1 to %2").arg(m_param->stepVal[i]).arg(val);
		m_param->stepVal[i] = val;
	}
	else if (sp == ui->sp_degRange)
	{
		str0 = ui->cob_edgeIndex->currentText() + QString(" ") + ui->label_35->text();
		str1 = tr(" change from %1 to %2").arg(m_param->degRange[i]).arg(val);
		m_param->degRange[i] = val;
	}
	else if (sp == ui->sp_rmDist)
	{
		str0 = ui->cob_edgeIndex->currentText() + QString(" ") + ui->label_37->text();
		str1 = tr(" change from %1 to %2").arg(m_param->rmDist[i]).arg(val);
		m_param->rmDist[i] = val;
	}
	else if (sp == ui->sp_iteratNum)
	{
		str0 = ui->cob_edgeIndex->currentText() + QString(" ") + ui->label_38->text();
		str1 = tr(" change from %1 to %2").arg(m_param->iterateNum[i]).arg(val);
		m_param->iterateNum[i] = val;
	}
	else if (sp == ui->sp_lineOffset)
	{
		str0 = ui->cob_edgeIndex->currentText() + QString(" ") + ui->label_39->text();
		str1 = tr(" change from %1 to %2").arg(m_param->lineOffset[i]).arg(val);
		m_param->lineOffset[i] = val;
	}
	else if (sp == ui->sp_leadThre)
	{
		str0 = ui->cob_edgeIndex->currentText() + QString(" ") + ui->label_40->text();
		str1 = tr(" change from %1 to %2").arg(m_param->leadThre[i]).arg(val);
		m_param->leadThre[i] = val;
	}
	m_module->setParamChanged(ProductLevel);
	//log(LogL1, str0 + str1);
}

void edgeWarpWdg::comboBox_indexChanged(int val)
{
	QComboBox* cob = qobject_cast<QComboBox*>(sender());
	QString str0 = QString("");
	QString str1 = QString("");
	if (cob == ui->cob_pol) {
		int i = m_param->currentEdge;
		str0 = ui->cob_edgeIndex->currentText() + QString(" ") + ui->label_34->text();
		str1 = tr(" change from %1 to %2").arg(cob->itemText(m_param->pol[i])).arg(cob->itemText(val));
		m_param->pol[i] = val;
	}
	else if (cob == ui->cob_first) {
		int i = m_param->currentEdge;
		str0 = ui->cob_edgeIndex->currentText() + QString(" ") + ui->label_34->text();
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
	else if (cob == ui->cob_inputPixel) {
		str0 = ui->label->text();
		str1 = tr(" change from %1 to %2").arg(cob->itemText(m_param->isInputPix)).arg(cob->itemText(val));
		m_param->isInputPix = val;
	}
	m_module->setParamChanged(ProductLevel);
}

void edgeWarpWdg::on_bt_selectRoi_clicked()
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

		title << ui->cob_edgeIndex->itemText(i);
	}
	//currentView()->clear();
	currentView()->clearLayer(m_layerIdx);
	currentView()->setColor(255, 0, 128);
	drawRects2(MAX_EDGE_LINE, row, col, angle, len1, len2, title, 10);
	currentView()->update();
}

void edgeWarpWdg::on_bt_ok_clicked()
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

void edgeWarpWdg::on_bt_cancel_clicked()
{
	ui->bt_ok->setEnabled(0);
	ui->bt_cancel->setEnabled(0);
	ui->bt_selectRoi->setEnabled(1);

	currentView()->finishDraw();
	//currentView()->clear();
	currentView()->clearLayer(m_layerIdx);
	currentView()->update();
}

void edgeWarpWdg::radioButton_toggled(bool val)
{
	QRadioButton* rb = qobject_cast<QRadioButton*>(sender());
	QString str0 = QString("");
	QString str1 = QString("");
	if (val) {
		QRadioButton* RB = NULL;
		if (m_param->leadDir == 1)
			RB = ui->rb_DwToUp;
		else if (m_param->leadDir == 2)
			RB = ui->rb_LfToRt;
		else if (m_param->leadDir == 3)
			RB = ui->rb_RtToLf;
		else if (m_param->leadDir == 0)
			RB = ui->rb_UpToDw;

		if (rb == ui->rb_UpToDw) {
			str0 = ui->groupBox_2->title();
			str1 = tr(" change from %1 to %2").arg(RB->text()).arg(rb->text());
			m_param->leadDir = 0;
		}
		else if (rb == ui->rb_DwToUp) {
			str0 = ui->groupBox_2->title();
			str1 = tr(" change from %1 to %2").arg(RB->text()).arg(rb->text());
			m_param->leadDir = 1;
		}
		else if (rb == ui->rb_LfToRt) {
			str0 = ui->groupBox_2->title();
			str1 = tr(" change from %1 to %2").arg(RB->text()).arg(rb->text());
			m_param->leadDir = 2;
		}
		else if (rb == ui->rb_RtToLf) {
			str0 = ui->groupBox_2->title();
			str1 = tr(" change from %1 to %2").arg(RB->text()).arg(rb->text());
			m_param->leadDir = 3;
		}
		else if (rb == ui->rb_black) {
			str0 = ui->groupBox_4->title();
			str1 = tr(" change from %1 to %2").arg(ui->rb_white->text()).arg(ui->rb_black->text());
			m_param->IsWhite = 0;
		}
		else if (rb == ui->rb_white) {
			str0 = ui->groupBox_4->title();
			str1 = tr(" change from %1 to %2").arg(ui->rb_black->text()).arg(ui->rb_white->text());
			m_param->IsWhite = 1;
		}
		m_module->setParamChanged(ProductLevel);
		//log(LogL1, str0 + str1);
	}
}

void edgeWarpWdg::on_bt_advance_toggled(bool val)
{
	ui->grp_advance->setVisible(val);
	if (val) {
		ui->bt_advance->setText(tr("exit advance"));
	}
	else {
		ui->bt_advance->setText(tr("advance"));
	}
}
