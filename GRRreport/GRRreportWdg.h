#pragma once
#include <moduleClass.h>
#include "GRRreportParam.h"

namespace Ui {class GRRreportWdg;}
class GRRreportMod;
class QDoubleSpinBox;
class QTreeWidgetItem;
using namespace qzcv;

class GRRreportWdg : public ModSetWidget
{
	Q_OBJECT

public:
	explicit GRRreportWdg(int i=0);
	~GRRreportWdg();
	virtual void setModule(UnitModule* moudle);
protected:
	void showEvent(QShowEvent * event);
	bool eventFilter(QObject *watched, QEvent *event);
private slots:
void le_textChanged(const QString &);
void sp_valueChanged(int);
void dsp_valueChanged(double);
void bt_clicked();
void bt_toggled(bool);
void cb_toggled(bool);
void tree_itemChanged(QTreeWidgetItem *, int);
void tree_itemDoubleClicked(QTreeWidgetItem *, int);
private:
	void connectSlots(bool link);
	void iniUi();
	void setUiValue();
	void setTopItem();
	void setSubItem();
	bool needUpdateTreeItem(int idx);
private:
	Ui::GRRreportWdg *ui;
	bool m_hasConnect;
	GRRreportParam* m_param;
	GRRreportMod* m_module;
	int m_widgetType;

	QList<QTreeWidgetItem *> m_topItem;
	QList<QList<QTreeWidgetItem *>> m_subItem;
	QList<QList<QDoubleSpinBox *>> sp_USL;
	QList<QList<QDoubleSpinBox *>> sp_LSL;
};

