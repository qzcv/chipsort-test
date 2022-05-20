#include "AdvanceDialog.h"

AdvancedDialog::AdvancedDialog(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
}

AdvancedDialog::~AdvancedDialog()
{

}

void AdvancedDialog::accept()
{
	QDialog::accept();
}