#pragma once
#include <moduleClass.h>
#include "HalconCpp.h"

namespace Ui {class surfaceMultWdg;}
class surfaceMultParam;
class surfaceMultMod;
class surMultAdvanceDialog;
using namespace qzcv;
using namespace Halcon;

class surfaceMultWdg : public ModSetWidget
{
	Q_OBJECT

public:
	explicit surfaceMultWdg(int i=0);
	~surfaceMultWdg();
	virtual void setModule(UnitModule* moudle);
protected:
	void showEvent(QShowEvent * event);
private slots:
	void check_box_toggled(bool check);
	void spinBox_valueChanged(int val);
	void doubleSpinBox_valueChanged(double val);
	void comboBox_Index_Changed(int index);
	void on_bt_selectRecRoi_toggled(bool check);
	void on_bt_selectCirRoi_toggled(bool check);
	void on_bt_advance_clicked();
	void on_bt_selectRotate_toggled(bool check);
	void on_bt_setRotateMult_clicked();
	void radioButton_toggled(bool);
	void on_bt_test_toggled(bool);
	void bt_clicked();

private:
	void connectSlots(bool link);
	void iniUi();
	void iniData();
	void setUiValue();
	void updateUi();
	void changeModel(bool isTarget);    //切换检测模式时候切换各控件名字
	void changeSpinBox(bool isPixOrAct);   //改变像素尺寸还是实际尺寸
	void setCurrentGrp(int index);
	void testParam();
private:
	Ui::surfaceMultWdg *ui;
	bool m_hasConnect;
	surfaceMultParam* m_param;
	surfaceMultMod* m_module;
	int m_widgetType;
	int m_layerIdx;
	surMultAdvanceDialog* m_advanceDialog;

	bool m_testing;   //是否正在测试
};

