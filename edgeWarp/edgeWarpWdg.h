#pragma once
#include <moduleClass.h>

namespace Ui {class edgeWarpWdg;}
class edgeWarpParam;
class edgeWarpMod;
class cusViewRotRectList;
using namespace qzcv;

class edgeWarpWdg : public ModSetWidget
{
	Q_OBJECT

public:
	explicit edgeWarpWdg(int i=0);
	~edgeWarpWdg();
	virtual void setModule(UnitModule* moudle);
protected:
	virtual void showEvent(QShowEvent * event);
private slots:
	void checkBox_toggled(bool check);
	void doubleSpinBox_valueChanged(double val);
	void spinBox_valueChanged(int val);
	void comboBox_indexChanged(int val);
	void on_bt_selectRoi_clicked();
	void on_bt_ok_clicked();
	void on_bt_cancel_clicked();
	void radioButton_toggled(bool);
	void on_bt_advance_toggled(bool);

private:
	void connectSlots(bool link);
	void iniUi();
	void setUiValue();
	void drawRects2(int num, double *row, double *col, double *angle,
		double *len1, double *len2, const QStringList& titles, int arrowSize = 5);
	void changeEdgeIndex(int index);   //切换边界指数时候的UI变化
private:
	Ui::edgeWarpWdg *ui;
	bool m_hasConnect;
	edgeWarpParam* m_param;
	edgeWarpMod* m_module;
	int m_widgetType;
	int m_layerIdx;
	cusViewRotRectList *m_rectList;
};

