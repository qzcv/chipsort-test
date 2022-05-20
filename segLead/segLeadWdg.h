#pragma once
#include <moduleClass.h>
#include "HalconCpp.h"

namespace Ui {class segLeadWdg;}
class segLeadParam;
class segLeadMod;
using namespace qzcv;
using namespace Halcon;

class segLeadWdg : public ModSetWidget
{
	Q_OBJECT

public:
	explicit segLeadWdg(int i=0);
	~segLeadWdg();
	virtual void setModule(UnitModule* moudle);
protected:
	void showEvent(QShowEvent * event);
private slots:
	void check_box_toggled(bool check);
	void spinBox_valueChanged(int val);
	void doubleSpinBox_valueChanged(double val);
	void comboBox_Index_Changed(int index);
	void on_bt_selectRoi_toggled(bool check);
	void radioButton_toggled(bool check);
	void on_bt_useLead1_clicked();
	void on_bt_test_toggled(bool val);
	void on_bt_autoLead_toggled(bool val);
	void slider_valueChanged(int val);
private:
	void connectSlots(bool link);
	void iniUi();
	void setUiValue();
	void updateUi();
	void drawRect();
	void changeErrLead();
	void autoThreshold(); //执行自动分割的操作
	void testVal();
private:
	Ui::segLeadWdg *ui;
	bool m_hasConnect;
	segLeadParam* m_param;
	segLeadMod* m_module;
	Hobject *m_image;
	int m_widgetType;
	int m_layerIdx;
	int m_leadIndex; //当前的管脚索引
	bool m_testing;

};

