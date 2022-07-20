#pragma once
#include <moduleClass.h>
#include "HalconCpp.h"

namespace Ui {class standEdgeWdg;}
class standEdgeParam;
class standEdgeMod;
using namespace qzcv;
using namespace Halcon;

class standEdgeWdg : public ModSetWidget
{
	Q_OBJECT

public:
	explicit standEdgeWdg(int i=0);
	~standEdgeWdg();
	virtual void setModule(UnitModule* moudle);
protected:
	virtual void showEvent(QShowEvent * event);
private slots:
	void on_bt_setOrigin_clicked();
	void radioButton_toggled(bool check);
	void checkBox_toggled(bool check);
	void doubleSpinBox_valueChanged(double val);
	void spinBox_valueChanged(int val);
	void on_bt_selectRoi_toggled(bool check);
	void comboBox_currentIndexChanged(int index);
	void tabWidget_indexChange(int index);
	void on_bt_highLevel_toggled(bool val);
	void on_bt_helpRect_toggled(bool val);
private:
	void connectSlots(bool link);
	void iniUi();
	void setUiValue();
	void setCurrentGroup(int index);  //刷新当前的属性页面
private:
	Ui::standEdgeWdg *ui;
	bool m_hasConnect;
	standEdgeParam* m_param;
	standEdgeMod* m_module;
	int m_widgetType;
	int m_layerIdx;

	Hobject *m_image;
};

