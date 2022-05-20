#include "advanceDialog.h"

AdvanceSetDialog::AdvanceSetDialog(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
}

AdvanceSetDialog::~AdvanceSetDialog()
{
}

void AdvanceSetDialog::setParam(mark1Param * p)
{
	m_param = p;
}

void AdvanceSetDialog::refreshUI()
{

}

void AdvanceSetDialog::getParam()
{

}

int AdvanceSetDialog::exec()
{
	refreshUI();
	return QDialog::exec();
}

void AdvanceSetDialog::accept()
{
	getParam();
	QDialog::accept();
}