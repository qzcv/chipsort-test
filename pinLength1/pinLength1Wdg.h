#pragma once
#include <moduleClass.h>
#include "HalconCpp.h"

namespace Ui {class pinLength1Wdg;}
class pinLength1Param;
class pinLength1Mod;
class AdvanceSet;
class PositionSet;
class ParamTables;
using namespace qzcv;
using namespace Halcon;

class pinLength1Wdg : public ModSetWidget
{
	Q_OBJECT

public:
	explicit pinLength1Wdg(int i=0);
	~pinLength1Wdg();
	virtual void setModule(UnitModule* moudle);
protected:
	void showEvent(QShowEvent * event);

private slots:
	void bt_toggled(bool checked);
	void comboBox_Index_changed(int Index);
	void spinBox_valueChanged(int val);
	void douSpinBox_valueChanged(double val);
	void checkBox_valueChanged(bool check);
	void slider_valueChanged(int val);
	void radioButton_toggled(bool check);
	void on_bt_highLevel_toggled(bool val);
	void on_bt_locate_clicked();
	void on_bt_advanceLead_clicked();

	void on_bt_table_clicked();
	void on_bt_corrTable_clicked();

	void tableWidget_valueChanged(int checkIdx, int grpIdx, int leadIdx, bool minOrMax, double val); //tabwidget数据变化
	void tableWidget_checkToggled(int index, bool check);
	void tableWidget_CopyItems(bool IsAll, int startCheck = 0, int CheckRange = 1, int starLead = 0, int leadRange = 1);

private:
	void connectSlots(bool link);
	void iniUi();
	void iniData();
	void setUiValue();
	void updateUi();
	void drawTestPin();
	void changeErrGroup();
	void changeLeadDir(bool isLRDW);
	void refreshTableWidget();

	void bt_selectRoi_toggled(bool check);
	void bt_selectPin_toggled(bool check);
	void bt_testPin_toggled(bool check);
	void bt_selectRotate_toggled(bool val);
	void bt_selectSearch_toggled(bool val);

private:
	Ui::pinLength1Wdg *ui;
	bool m_hasConnect;
	pinLength1Param* m_param;
	pinLength1Mod* m_module;
	int m_widgetType;
	int m_layerIdx;
	AdvanceSet *m_advanceSet;
	PositionSet *m_positionSet;
	Hobject *m_image;

private:
	typedef enum { UnSel = 0, SelSegRegion, SelLead, SelSearch, SelRotate }SelState; //选取状态
	SelState m_selState;

	int m_grpIndex;  //误差范围的组数 0左上,1右下

	QDialog* m_dialog;
	QDialog* m_corrDialog;
	ParamTables *m_paramTables;  //存储
	ParamTables *m_corrTable;    //补偿值
};

