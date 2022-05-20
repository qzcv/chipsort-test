#ifndef ADVANCEDDIALOG_H
#define ADVANCEDDIALOG_H

#include <QDialog>
#include "ui_AdvanceDialog.h"

class AdvancedDialog : public QDialog
{
	Q_OBJECT

public:
	AdvancedDialog(QWidget *parent = 0);
	~AdvancedDialog();
	Ui::ADVDialogClass ui;

public slots:
	virtual void accept();


private:
	
};

#endif // AUTOTHREDIALOG_H
