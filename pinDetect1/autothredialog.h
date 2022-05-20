#ifndef AUTOTHREDIALOG_H
#define AUTOTHREDIALOG_H

#include <QDialog>
#include "ui_autothredialog.h"

class autoThreDialog : public QDialog
{
	Q_OBJECT

public:
	autoThreDialog(QWidget *parent = 0);
	~autoThreDialog();
	Ui::autoThreDialog ui;

public slots:
	virtual void accept();


private:
	
};

#endif // AUTOTHREDIALOG_H
