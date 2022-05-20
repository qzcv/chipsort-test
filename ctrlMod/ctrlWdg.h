#pragma once
#include <moduleClass.h>

namespace Ui { class CtrlWdg; }
class CtrlParam;
class CtrlMod;
class QLabel;
class QLineEdit;
class QGridLayout;
class QTreeWidgetItem;
using namespace qzcv;

class CtrlWdg : public ModSetWidget
{
	Q_OBJECT

public:
	explicit CtrlWdg(int i = 0);
	~CtrlWdg();
	virtual void setModule(UnitModule* moudle);
protected:
	virtual void showEvent(QShowEvent * event);
	bool eventFilter(QObject *watched, QEvent *event);
private slots:
	void sp_valueChanged(int val);
	void bt_clicked();
	void cb_toggled(bool);
	void le_textEdited(const QString & str);
private:
	void connectSlots(bool link);
	void iniUi();
	void setUiValue();

	//calibReport
	void setTextEditNum();
	void genCalibReport_html(int st, int ispix);
	void genCalibReport_xlsx(int st, int ispix);

private:
	Ui::CtrlWdg *ui;
	bool m_hasConnect;
	CtrlParam* m_param;
	CtrlMod* m_module;
	int m_widgetType;
private:
	QList<QLabel *> lb_stIdx;
	QList<QLineEdit *> le_stName;
	QGridLayout *m_layout;
	QTreeWidgetItem *treeWdg_item[2];
};

