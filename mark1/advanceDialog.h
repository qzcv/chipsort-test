#ifndef ADVANCEDIALOG_H
#define ADVANCEDIALOG_H

#include <QDialog>
#include "ui_advanceDialog.h"
#include "mark1Param.h"
#include "mark1Wdg.h"
class mark1Param;

namespace Ui{
	class AdvanceSetDialog;
}

class AdvanceSetDialog : public QDialog
{
	Q_OBJECT

public:
	AdvanceSetDialog(QWidget *parent = 0);
	~AdvanceSetDialog();
	void setParam(mark1Param *p);
	void refreshUI(); //ˢ�½���
	void getParam();  //��ȡ�����еĲ���

	int exec();
	void virtual accept();
	Ui::AdvanceDialogClass ui;
	
private:
	mark1Param* m_param;
};

#endif // SHOWWIDGET_H
