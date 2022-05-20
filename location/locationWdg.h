#pragma once
#include <moduleClass.h>
#include "HalconCpp.h"

namespace Ui {class locationWdg;}
class locationParam;
class locationMod;
class ParamTables;
using namespace qzcv;
using namespace Halcon;

class locationWdg : public ModSetWidget
{
	Q_OBJECT

public:
	explicit locationWdg(int i=0);
	~locationWdg();
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
	void setCurrentGroup(int index);  //刷新当前的属性页面
	void refreshTableWidget();

private:
	Ui::locationWdg *ui;
	bool m_hasConnect;
	locationParam* m_param;
	locationMod* m_module;
	int m_widgetType;
	int m_layerIdx;
	Hobject *m_image;
	QDialog* m_dialog;
	QDialog* m_corrDialog;
	ParamTables *m_paramTables;  //存储
	ParamTables *m_corrTable;    //补偿值
	ParamTables *m_offTable;     //补偿值偏置
};

