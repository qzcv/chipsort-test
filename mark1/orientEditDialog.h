#ifndef ORIENTDIALOG_H
#define ORIENTDIALOG_H

#include <QDialog>
#include "ui_orientEdit.h"
#include "mark1Param.h"
#include "mark1Wdg.h"
class mark1Param;

namespace Ui{
	class OrientEditDialog;
}

class OrientEditDialog : public QDialog
{
	Q_OBJECT

public:
	OrientEditDialog(QWidget *parent = 0);
	~OrientEditDialog();
	void setParam(mark1Param *p);
	void refreshUI(); //ˢ�½���
	void getParam();  //��ȡ�����еĲ���

	int exec();
	void virtual accept();
	Ui::OrientEditClass ui;
	
private:	
	mark1Param* m_param;
};

#endif // SHOWWIDGET_H
