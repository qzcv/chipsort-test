#ifndef SHOWWIDGET_H
#define SHOWWIDGET_H

#include <QDialog>
#include "ui_showWidget.h"
#include "mark1Param.h"
#include "mark1Wdg.h"
#include "ImageView.h"
class mark1Param;

namespace Ui{
	class ShowWidget;
}

class ShowWidget : public QDialog
{
	Q_OBJECT

public:
	ShowWidget(QWidget *parent = 0);
	~ShowWidget();
	void setParam(mark1Param *p);
	void showTemp();
	int exec();

signals:
	void closeShowWidget();

private:
	void closeEvent(QCloseEvent *event);
	Ui::showWidgetClass ui;
	ImageView* m_tempView;
	mark1Param* m_param;
};

#endif // SHOWWIDGET_H
