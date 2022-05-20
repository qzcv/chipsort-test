#pragma once
#include <moduleClass.h>
#include "HalconCpp.h"
#include "fitEdge.h"

namespace Ui {class edgeLocationWdg;}
class edgeLocationParam;
class edgeLocationMod;
using namespace qzcv;
using namespace Halcon;
using namespace danke;

class edgeLocationWdg : public ModSetWidget
{
	Q_OBJECT

public:
	explicit edgeLocationWdg(int i=0);
	~edgeLocationWdg();
	virtual void setModule(UnitModule* moudle);
private slots:
	void on_bt_setOrigin_clicked();
	void radioButton_toggled(bool check);
	void checkBox_toggled(bool check);
	void doubleSpinBox_valueChanged(double val);
	void spinBox_valueChanged(int val);
	void comboBox_indexChanged(int index);
	void on_bt_selectRoi_toggled(bool check);
	void on_bt_HelpRect_toggled(bool check);
	void on_bt_advance_toggled(bool val);
	void tabWidget_currentIndexChanged(int index);

private:
	void connectSlots(bool link);
	void iniData();
	void iniUi();
	void setUiValue();
	void changeGroupIndex(int index);
private:
	Ui::edgeLocationWdg *ui;
	bool m_hasConnect;
	edgeLocationParam* m_param;
	edgeLocationMod* m_module;
	int m_widgetType;
	int m_layerIdx;
	int m_nowGrpIndex;
	bool m_IsSelecting;   //是否正在教导
	fitEdgeLine *m_edgeLine[4];
};

