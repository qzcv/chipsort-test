#pragma once
#include <moduleClass.h>
#include "HalconCpp.h"

namespace Ui {class ColorPickerWdg;}
class ColorPickerParam;
class ColorPickerMod;
using namespace qzcv;
using namespace Halcon;

class ColorPickerWdg : public ModSetWidget
{
	Q_OBJECT

public:
	explicit ColorPickerWdg(int i=0);
	~ColorPickerWdg();
	virtual void setModule(UnitModule* moudle);
protected:
	void showEvent(QShowEvent * event);
private slots:
	void on_bt_selectROI_toggled(bool check);
	void on_bt_selectMinusROI_toggled(bool check);
	void on_bt_testing_toggled(bool check);
	void spinBox_valueChanged(int val);
	void checkBox_toggled(bool check);
	void radioButton_toggled(bool check);
	void comboBox_IndexChanged(int index);
private:
	void connectSlots(bool link);
	void iniData();
	void iniUi();
	void setUiValue();
	void updateUi();
	void dispRoi(int roiORmin);   //0roi,1minus
	void getRoi(int roiORmin);
	void testParam();             //进入测试模式，开始动态测试参数   
private:
	Ui::ColorPickerWdg *ui;
	bool m_hasConnect;
	ColorPickerParam* m_param;
	ColorPickerMod* m_module;
	int m_widgetType;
	int m_layerIdx;

	Hobject *m_image;
	bool m_testing;          //是否在测试模式中
};

