#pragma once
#include <moduleClass.h>

namespace Ui {class edgeInterWdg;}
class edgeInterParam;
class edgeInterMod;
class cusViewRotRectList;
using namespace qzcv;

class edgeInterWdg : public ModSetWidget
{
	Q_OBJECT

public:
	explicit edgeInterWdg(int i=0);
	~edgeInterWdg();
	virtual void setModule(UnitModule* moudle);
protected:
	void showEvent(QShowEvent * event);
private slots:
	void checkBox_toggled(bool check);
	void doubleSpinBox_valueChanged(double val);
	void spinBox_valueChanged(int val);
	void comboBox_indexChanged(int val);
	void on_bt_selectRoi_clicked();
	void on_bt_ok_clicked();
	void on_bt_cancel_clicked();
	void on_bt_setOrigin_clicked();
	void on_bt_advance_toggled(bool);
private:
	void connectSlots(bool link);
	void iniUi();
	void iniData();
	void setUiValue();
	void updateUi();
	void changeEdgeIndex(int index);   //切换边界指数时候的UI变化
	void drawRects2(int num, double *row, double *col, double *angle,
		double *len1, double *len2, const QStringList& titles, int arrowSize = 5);
private:
	Ui::edgeInterWdg *ui;
	bool m_hasConnect;
	edgeInterParam* m_param;
	edgeInterMod* m_module;
	int m_widgetType;
	int m_layerIdx;
	cusViewRotRectList *m_rectList;
};

