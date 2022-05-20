#ifndef ADVANCESET_H
#define ADVANCESET_H

#include <QDialog>
#include "ui_AdvanceSet.h"

class AdvanceSet : public QDialog
{
	Q_OBJECT

public:
	AdvanceSet(QWidget *parent = 0);
	~AdvanceSet();
	Ui::advanceset ui;

private:
	
};

#endif // LEADCONTACT_H
