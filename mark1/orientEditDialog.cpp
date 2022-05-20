#include "orientEditDialog.h"

OrientEditDialog::OrientEditDialog(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
}

OrientEditDialog::~OrientEditDialog()
{
}

void OrientEditDialog::setParam(mark1Param * p)
{
	m_param = p;
}

void OrientEditDialog::refreshUI()
{
	//dir
	if (m_param->Is2Dor4D == 0) {
		ui.rb_2D->setChecked(true);
	}
	else if (m_param->Is2Dor4D == 1) {
		ui.rb_4D->setChecked(true);
	}
	ui.cb_inverse->setChecked(m_param->IsInverse);
	ui.cb_pin1->setCurrentIndex(m_param->pin1OutIdx + 1);
	ui.cb_pin2->setCurrentIndex(m_param->pin2OutIdx + 1);
	ui.cb_defaultSig->setChecked(m_param->IsDefaultSig);
	ui.cb_showAngle->setChecked(m_param->isShowAngle);
	ui.tab_2->setVisible(false);
}

void OrientEditDialog::getParam()
{
	if (ui.rb_2D->isChecked()) {
		m_param->Is2Dor4D = 0;
	}
	else if (ui.rb_4D->isChecked()) {
		m_param->Is2Dor4D = 1;
	}
	m_param->IsInverse = ui.cb_inverse->isChecked();
	m_param->pin1OutIdx = ui.cb_pin1->currentIndex() - 1;
	m_param->pin2OutIdx = ui.cb_pin2->currentIndex() - 1;
	m_param->IsDefaultSig = ui.cb_defaultSig->isChecked();
	m_param->isShowAngle = ui.cb_showAngle->isChecked();
}

int OrientEditDialog::exec()
{
	refreshUI();
	return QDialog::exec();
}

void OrientEditDialog::accept()
{
	getParam();
	QDialog::accept();
}