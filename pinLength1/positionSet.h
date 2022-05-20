#ifndef POSITIONSET_H
#define POSITIONSET_H

#include <QDialog>
#include "ui_positionSet.h"

class PositionSet : public QDialog
{
	Q_OBJECT

public:
	PositionSet(QWidget *parent = 0);
	~PositionSet();
	Ui::PositionSetClass ui;

private:
	
};

#endif // LEADCONTACT_H
