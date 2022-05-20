#include "autothredialog.h"

autoThreDialog::autoThreDialog(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
}

autoThreDialog::~autoThreDialog()
{

}

void autoThreDialog::accept()
{
	if (ui.bt_selSearchRoi->isChecked()){
		ui.bt_selSearchRoi->setChecked(false);
	}
	if (ui.bt_selAutoThre->isChecked()){
		ui.bt_selAutoThre->setChecked(false);
	}
	if (ui.bt_selectPin->isChecked()){
		ui.bt_selectPin->setChecked(false);
	}
	QDialog::accept();
}