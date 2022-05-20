#include "edgeLineWdg.h"
#include "edgeLineMod.h"
#include "edgeLineParam.h"
#include "ui_edgeLineWdg.h"
#include <QMessageBox>
#include "ImageView.h"
#include "imageViewItem.h"
#include "customView.h"

edgeLineWdg::edgeLineWdg(int i) :
	ModSetWidget(),ui(new Ui::edgeLineWdg),m_widgetType(i)
{
	m_hasConnect = false;
	ui->setupUi(this);
	iniUi();
	m_layerIdx = 0;
	m_param = nullptr;
	m_module = nullptr;
	m_rectList = nullptr;
}

edgeLineWdg::~edgeLineWdg()
{
	delete ui;
}

void edgeLineWdg::setModule(UnitModule *module)
{
	m_module=dynamic_cast<edgeLineMod*>(module);
	m_param = m_module->m_param;
	//currentView()->setLayer(m_layerIdx);
	//updateUi();
}

void edgeLineWdg::showEvent(QShowEvent * event)
{
	updateUi();
}

void edgeLineWdg::connectSlots(bool link)
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

	connectOrNot(link, ui->sp_errWidthMin, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_errWidthMax, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_errAngle, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_errHeightMin, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_errHeightMax, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_errCenterOffset, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));

	for (int i = 0;i < 4;i++) {
		connectOrNot(link, sp_gradient[i], SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
		connectOrNot(link, sp_step[i], SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
		connectOrNot(link, sp_degRange[i], SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
		connectOrNot(link, sp_lineOffset[i], SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
		connectOrNot(link, sp_rmDist[i], SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
		connectOrNot(link, sp_iterateNum[i], SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
		connectOrNot(link, sp_fixWidth[i], SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
		connectOrNot(link, sp_lineOffset[i], SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));

		connectOrNot(link, cb_fixWidth[i], SIGNAL(toggled(bool)), SLOT(checkBox_toggled(bool)));

		connectOrNot(link, cob_pol[i], SIGNAL(currentIndexChanged(int)), SLOT(comboBox_indexChanged(int)));
		connectOrNot(link, cob_first[i], SIGNAL(currentIndexChanged(int)), SLOT(comboBox_indexChanged(int)));
	}
}

void edgeLineWdg::iniUi()
{
	for (int i = 0;i < 4;i++) {
		sp_gradient[i] = findChild<QSpinBox*>(QString("sp_gradient_%1").arg(i));
		sp_step[i] = findChild<QSpinBox*>(QString("sp_step_%1").arg(i));
		sp_degRange[i] = findChild<QSpinBox*>(QString("sp_degRange_%1").arg(i));
		sp_lineOffset[i] = findChild<QSpinBox*>(QString("sp_lineOffset_%1").arg(i));
		sp_rmDist[i] = findChild<QSpinBox*>(QString("sp_rmDist_%1").arg(i));
		sp_iterateNum[i] = findChild<QSpinBox*>(QString("sp_iteratNum_%1").arg(i));
		sp_fixWidth[i] = findChild<QSpinBox*>(QString("sp_fixWidth_%1").arg(i));
		sp_lineOffset[i] = findChild<QSpinBox*>(QString("sp_lineOffset_%1").arg(i));

		cb_fixWidth[i] = findChild<QCheckBox*>(QString("cb_fixWidth_%1").arg(i));

		cob_pol[i] = findChild<QComboBox*>(QString("cb_pol_%1").arg(i));
		cob_first[i] = findChild<QComboBox*>(QString("cb_first_%1").arg(i));
	}
}

void edgeLineWdg::setUiValue()
{
	if(!m_param)
		return;

	for (int i = 0;i < 4;i++) {
		sp_gradient[i]->setValue(m_param->gradient[i]);
		sp_step[i]->setValue(m_param->stepVal[i]);
		sp_degRange[i]->setValue(m_param->degRange[i]);
		sp_lineOffset[i]->setValue(m_param->lineOffset[i]);
		sp_rmDist[i]->setValue(m_param->rmDist[i]);
		sp_iterateNum[i]->setValue(m_param->iterateNum[i]);
		cob_first[i]->setCurrentIndex(m_param->first[i]);
		cob_pol[i]->setCurrentIndex(m_param->pol[i]);

		cb_fixWidth[i]->setChecked(m_param->IsFixWidth[i]);
		sp_fixWidth[i]->setEnabled(m_param->IsFixWidth[i]);
		sp_fixWidth[i]->setValue(m_param->fixWidth[i]);
	}
	ui->cb_errWidth->setChecked(m_param->IsErrWidth == 1);
	ui->sp_errWidthMin->setValue(m_param->errWidthMin);
	ui->sp_errWidthMax->setValue(m_param->errWidthMax);
	ui->sp_errWidthMin->setEnabled(m_param->IsErrWidth==1);
	ui->sp_errWidthMax->setEnabled(m_param->IsErrWidth==1);

	ui->cb_errHeight->setChecked(m_param->IsErrHeight == 1);
	ui->sp_errHeightMin->setValue(m_param->errHeightMin);
	ui->sp_errHeightMax->setValue(m_param->errHeightmax);
	ui->sp_errHeightMin->setEnabled(m_param->IsErrHeight==1);
	ui->sp_errHeightMax->setEnabled(m_param->IsErrHeight==1);

	ui->cb_errAngle->setChecked(m_param->IsErrAngle == 1);
	ui->sp_errAngle->setValue(m_param->errAngle);
	ui->sp_errAngle->setEnabled(m_param->IsErrAngle==1);

	ui->cb_errCenterOffset->setChecked(m_param->IsErrCenterOffset == 1);
	ui->sp_errCenterOffset->setValue(m_param->errCenterOffset);
	ui->sp_errCenterOffset->setEnabled(m_param->IsErrCenterOffset==1);

	ui->cb_refer->setChecked(m_param->IsRefer);
	ui->cb_showRegionAfterDet->setChecked(m_param->IsShowRegionAfterDet);
}

void edgeLineWdg::updateUi()
{
	connectSlots(false);
	setUiValue();
	connectSlots(true);
}

void edgeLineWdg::drawRects2(int num, double * row, double * col, double * angle, double * len1, double * len2, const QStringList & titles, int arrowSize)
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

void edgeLineWdg::on_bt_setOrigin_clicked()
{
	if (QMessageBox::information(this, tr("Hint"),
		tr("Are you sure to change the current center point:(%1,%2) to the template center?")
		.arg(m_module->m_centerCol, 0, 'f', 3).arg(m_module->m_centerRow, 0, 'f', 3),
		QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
		//log(LogL1, ui.bt_setOrigin->text());
		m_param->TmpCenterRow = m_module->m_centerRow;
		m_param->TmpCenterCol = m_module->m_centerCol;
		
		m_module->setParamChanged(ProductLevel);
	}
}

void edgeLineWdg::checkBox_toggled(bool check)
{
	QCheckBox* cb = qobject_cast<QCheckBox*>(sender());
	QString str0 = cb->text();
	QString str1 = QString("");
	if (check)
		str1 = tr(" Check");
	else
		str1 = tr(" Uncheck");

	if (cb == ui->cb_errWidth) {
		m_param->IsErrWidth = check;
		ui->sp_errWidthMin->setEnabled(check);
		ui->sp_errWidthMax->setEnabled(check);
	}
	else if (cb == ui->cb_errHeight) {
		m_param->IsErrHeight = check;
		ui->sp_errHeightMin->setEnabled(check);
		ui->sp_errHeightMax->setEnabled(check);
	}
	else if (cb == ui->cb_errAngle) {
		m_param->IsErrAngle = check;
		ui->sp_errAngle->setEnabled(check);
	}
	else if (cb == ui->cb_errCenterOffset) {
		m_param->IsErrCenterOffset = check;
		ui->sp_errCenterOffset->setEnabled(check);
	}
	else if (cb == ui->cb_refer) {
		m_param->IsRefer = check;
	}
	else if (cb == ui->cb_showRegionAfterDet) {
		m_param->IsShowRegionAfterDet = check;
	}
	else {
		for (int i = 0;i < 4;i++) {
			if (cb == cb_fixWidth[i]) {
				str0 = ui->tabWidget->tabText(i) + QString(" ") + cb->text();
				m_param->IsFixWidth[i] = check;
				sp_fixWidth[i]->setEnabled(check);
				break;
			}
		}
	}
	m_module->setParamChanged(ProductLevel);
	//log(LogL1, str0 + str1);
}

void edgeLineWdg::spinBox_valueChanged(int val)
{
	QSpinBox* sp = qobject_cast<QSpinBox*>(sender());
	QString str0 = QString("");
	QString str1 = QString("");
	if (sp == ui->sp_errWidthMin) {
		str0 = ui->cb_errWidth->text() + tr(" min val");
		str1 = tr(" change from %1 to %2").arg(m_param->errWidthMin).arg(val);
		m_param->errWidthMin = val;
	}
	else if (sp == ui->sp_errWidthMax) {
		str0 = ui->cb_errWidth->text() + tr(" max val");
		str1 = tr(" change from %1 to %2").arg(m_param->errWidthMax).arg(val);
		m_param->errWidthMax = val;
	}
	else if (sp == ui->sp_errHeightMin) {
		str0 = ui->cb_errHeight->text() + tr(" min val");
		str1 = tr(" change from %1 to %2").arg(m_param->errHeightMin).arg(val);
		m_param->errHeightMin = val;
	}
	else if (sp == ui->sp_errHeightMax) {
		str0 = ui->cb_errHeight->text() + tr(" max val");
		str1 = tr(" change from %1 to %2").arg(m_param->errHeightmax).arg(val);
		m_param->errHeightmax = val;
	}
	else if (sp == ui->sp_errAngle) {
		str0 = ui->cb_errAngle->text();
		str1 = tr(" change from %1 to %2").arg(m_param->errAngle).arg(val);
		m_param->errAngle = val;
	}
	else if (sp == ui->sp_errCenterOffset) {
		str0 = ui->cb_errCenterOffset->text();
		str1 = tr(" change from %1 to %2").arg(m_param->errCenterOffset).arg(val);
		m_param->errCenterOffset = val;
	}
	else {
		for (int i = 0;i < 4;i++)
		{
			if (sp == sp_gradient[i])
			{
				str0 = ui->tabWidget->tabText(i) + QString(" ") + ui->label_24->text();
				str1 = tr(" change from %1 to %2").arg(m_param->gradient[i]).arg(val);
				m_param->gradient[i] = val;
			}
			else if (sp == sp_step[i])
			{
				str0 = ui->tabWidget->tabText(i) + QString(" ") + ui->label_33->text();
				str1 = tr(" change from %1 to %2").arg(m_param->stepVal[i]).arg(val);
				m_param->stepVal[i] = val;
			}
			else if (sp == sp_degRange[i])
			{
				str0 = ui->tabWidget->tabText(i) + QString(" ") + ui->label_35->text();
				str1 = tr(" change from %1 to %2").arg(m_param->degRange[i]).arg(val);
				m_param->degRange[i] = val;
			}
			else if (sp == sp_rmDist[i])
			{
				str0 = ui->tabWidget->tabText(i) + QString(" ") + ui->label_37->text();
				str1 = tr(" change from %1 to %2").arg(m_param->rmDist[i]).arg(val);
				m_param->rmDist[i] = val;
			}
			else if (sp == sp_iterateNum[i])
			{
				str0 = ui->tabWidget->tabText(i) + QString(" ") + ui->label_38->text();
				str1 = tr(" change from %1 to %2").arg(m_param->iterateNum[i]).arg(val);
				m_param->iterateNum[i] = val;
			}
			else if (sp == sp_lineOffset[i])
			{
				str0 = ui->tabWidget->tabText(i) + QString(" ") + ui->label_39->text();
				str1 = tr(" change from %1 to %2").arg(m_param->lineOffset[i]).arg(val);
				m_param->lineOffset[i] = val;
			}
			else if (sp == sp_fixWidth[i])
			{
				str0 = ui->tabWidget->tabText(i) + QString(" ") + ui->cb_fixWidth_0->text();
				str1 = tr(" change from %1 to %2").arg(m_param->fixWidth[i]).arg(val);
				m_param->fixWidth[i] = val;
			}
		}
	}
	m_module->setParamChanged(ProductLevel);
	//log(LogL1, str0 + str1);
}

void edgeLineWdg::comboBox_indexChanged(int val)
{
	QComboBox* cob = qobject_cast<QComboBox*>(sender());
	for (int i = 0;i < 4;i++) {
		if (cob == cob_pol[i]) {
			QString str0 = ui->tabWidget->tabText(i) + QString(" ") + ui->label_34->text();
			QString str1 = tr(" change from %1 to %2").arg(cob->itemText(m_param->pol[i])).arg(cob->itemText(val));
			m_param->pol[i] = val;
			break;
		}
		else if (cob == cob_first[i]) {
			QString str0 = ui->tabWidget->tabText(i) + QString(" ") + ui->label_34->text();
			QString str1 = tr(" change from %1 to %2").arg(cob->itemText(m_param->first[i])).arg(cob->itemText(val));
			m_param->first[i] = val;
			break;
		}
	}
	m_module->setParamChanged(ProductLevel);
	//log(LogL1, str0 + str1);
}

void edgeLineWdg::on_bt_selectRoi_clicked()
{
	ui->bt_ok->setEnabled(true);
	ui->bt_cancel->setEnabled(true);
	ui->bt_selectRoi->setEnabled(false);
	double row[4], col[4], angle[4], len1[4], len2[4];
	QStringList title;
	title << tr("up") << tr("right") << tr("down") << tr("left");
	for (int i = 0;i < 4;i++) {
		row[i] = m_param->roiReg[i].row;
		col[i] = m_param->roiReg[i].col;
		angle[i] = m_param->roiReg[i].angle;
		len1[i] = m_param->roiReg[i].length1;
		len2[i] = m_param->roiReg[i].length2;
	}
	//currentView()->clear();
	currentView()->clearLayer(m_layerIdx);
	currentView()->setColor(255, 0, 128);
	drawRects2(4, row, col, angle, len1, len2, title, 10);
	currentView()->update();
}

void edgeLineWdg::on_bt_ok_clicked()
{
	ui->bt_ok->setEnabled(0);
	ui->bt_cancel->setEnabled(0);
	ui->bt_selectRoi->setEnabled(1);

	currentView()->finishDraw();
	double row[4], col[4], angle[4], len1[4], len2[4];
	currentView()->getDrawRects2(4, row, col, angle, len1, len2);
	for (int i = 0;i < 4;i++) {
		m_param->roiReg[i].row = row[i];
		m_param->roiReg[i].col = col[i];
		m_param->roiReg[i].angle = angle[i];
		m_param->roiReg[i].length1 = len1[i];
		m_param->roiReg[i].length2 = len2[i];
	}
	//currentView()->clear();
	currentView()->clearLayer(m_layerIdx);
	currentView()->update();

	//log(LogL1, ui.bt_ok->text());
	m_module->setParamChanged(ProductLevel);
}

void edgeLineWdg::on_bt_cancel_clicked()
{
	ui->bt_ok->setEnabled(0);
	ui->bt_cancel->setEnabled(0);
	ui->bt_selectRoi->setEnabled(1);
	currentView()->finishDraw();
	//currentView()->clear();
	currentView()->clearLayer(m_layerIdx);
	currentView()->update();
}
