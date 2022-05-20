#pragma once
#include <moduleClass.h>

namespace Ui {class pinDistanceWdg;}
class pinDistanceParam;
class pinDistanceMod;
using namespace qzcv;

class pinDistanceWdg : public ModSetWidget
{
	Q_OBJECT

public:
	explicit pinDistanceWdg(int i=0);
	~pinDistanceWdg();
	virtual void setModule(UnitModule* moudle);
private slots:
	void checkBox_toggled(bool check);
	void doubleSpinBox_valueChanged(double val);
	void spinBox_valueChanged(int val);
	void on_bt_selectRoi_toggled(bool check);
	void comboBox_IndexChanged(int index);
private:
	void connectSlots(bool link);
	void iniUi();
	void setUiValue();
	void setCurrentGrp(int index);    //设置当前组别的ui
private:
	Ui::pinDistanceWdg *ui;
	bool m_hasConnect;
	pinDistanceParam* m_param;
	pinDistanceMod* m_module;
	int m_widgetType;
	int m_layerIdx;
};

