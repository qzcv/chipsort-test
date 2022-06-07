#pragma once
#include <moduleClass.h>
#include "HalconCpp.h"
#include "fitEdge.h"

namespace Ui {class edgeLocationWdg;}
class edgeLocationParam;
class edgeLocationMod;
class AdvancedDialog;
using namespace qzcv;
using namespace danke;
using namespace Halcon;

class edgeLocationWdg : public ModSetWidget
{
	Q_OBJECT

public:
	explicit edgeLocationWdg(int i=0);
	~edgeLocationWdg();
	virtual void setModule(UnitModule* moudle);
protected:
	void showEvent(QShowEvent * event);
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
	void on_bt_AdvDialog_clicked();
	void tabWidget_currentIndexChanged(int index);

private:
	void connectSlots(bool link);
	void iniUi();
	void iniData();
	void setUiValue();
	void changeGroupIndex(int index);
	void changeFixWidthValid();
private:
	Ui::edgeLocationWdg *ui;
	bool m_hasConnect;
	edgeLocationParam* m_param;
	edgeLocationMod* m_module;
	int m_widgetType;
	bool m_IsSelecting;
	int m_nowGrpIndex;
	int m_layerIdx;
	AdvancedDialog* m_advDialog;
	Hobject *m_image;

	fitEdgeLine* m_edgeLine[4];
};

