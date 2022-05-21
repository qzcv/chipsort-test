#pragma once
#include <moduleClass.h>

namespace Ui {class edgeLocationWdg;}
class edgeLocationParam;
class edgeLocationMod;
class AdvancedDialog;
using namespace qzcv;

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
	void setUiValue();
private:
	Ui::edgeLocationWdg *ui;
	bool m_hasConnect;
	edgeLocationParam* m_param;
	edgeLocationMod* m_module;
	int m_widgetType;

	int m_nowGrpIndex;

	AdvancedDialog* m_advDialog;

	fitEdgeLine* m_edgeLine[4];
};

