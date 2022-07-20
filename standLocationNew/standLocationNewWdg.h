#pragma once
#include <moduleClass.h>
#include "HalconCpp.h"

namespace Ui {class standLocationNewWdg;}
class standLocationNewParam;
class standLocationNewMod;
class ParamTables;
using namespace qzcv;
using namespace Halcon;

class standLocationNewWdg : public ModSetWidget
{
	Q_OBJECT

public:
	explicit standLocationNewWdg(int i=0);
	~standLocationNewWdg();
	virtual void setModule(UnitModule* moudle);
protected:
	void showEvent(QShowEvent * event);
private slots:
void radioButton_toggled(bool check);
void checkBox_toggled(bool check);
void doubleSpinBox_valueChanged(double val);
void spinBox_valueChanged(int val);
void on_bt_selectRoi_toggled(bool check);
void comboBox_currentIndexChanged(int index);
void tabWidget_indexChange(int index);
void on_bt_table_clicked();
void on_bt_corrTable_clicked();
void on_bt_advance_toggled(bool val);

void tableWidget_valueChanged(int checkIdx, int grpIdx, int leadIdx, bool minOrMax, double val); //tabwidget数据变化
void tableWidget_checkToggled(int index, bool check);
void tableWidget_CopyItems(bool IsAll, int startCheck = 0, int CheckRange = 1, int starLead = 0, int leadRange = 1);
private:
	void connectSlots(bool link);
	void iniUi();
	void setUiValue();
	void refreshTableWidget();
	void setCurrentGroup(int index);  //刷新当前的属性页面
private:
	Ui::standLocationNewWdg *ui;
	bool m_hasConnect;
	standLocationNewParam* m_param;
	standLocationNewMod* m_module;
	int m_widgetType;
	int m_layerIdx;

	QDialog* m_dialog;
	QDialog* m_corrDialog;
	ParamTables *m_paramTables;  //存储
	ParamTables *m_corrTable;    //补偿值
	ParamTables *m_offTable;     //补偿值偏置

	int m_grpIndex;  //误差范围的组数 0左上,1右下
	int m_leadIndex; //当前的管脚组数

	Hobject *m_image;
};

