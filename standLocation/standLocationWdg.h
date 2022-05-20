#pragma once
#include <moduleClass.h>
#include "HalconCpp.h"

namespace Ui {class standLocationWdg;}
class standLocationParam;
class standLocationMod;
class ParamTables;
using namespace qzcv;
using namespace Halcon;

class standLocationWdg : public ModSetWidget
{
	Q_OBJECT

public:
	explicit standLocationWdg(int i=0);
	~standLocationWdg();
	virtual void setModule(UnitModule* moudle);
protected:
	virtual void showEvent(QShowEvent *event);
private slots:
void radioButton_toggled(bool check);
void checkBox_toggled(bool check);
void doubleSpinBox_valueChanged(double val);
void spinBox_valueChanged(int val);
void on_bt_selectRoi_toggled(bool check);
void comboBox_currentIndexChanged(int index);
void tabWidget_indexChange(int index);
//void on_bt_setRefer_clicked();
void on_bt_table_clicked();
void on_bt_corrTable_clicked();
void on_bt_advance_toggled(bool val);

void tableWidget_valueChanged(int checkIdx, int grpIdx, int leadIdx, bool minOrMax, double val); //tabwidget数据变化
void tableWidget_checkToggled(int index, bool check);

void tableWidget_CopyItems(bool IsAll, int startCheck = 0, int CheckRange = 1, int starLead = 0, int leadRange = 1);
private:
	void connectSlots(bool link);
	void iniUi();
	void iniData();
	void setUiValue();
	void setCurrentGroup(int i);
	void refreshTableWidget();
private:
	Ui::standLocationWdg *ui;
	bool m_hasConnect;
	standLocationParam* m_param;
	standLocationMod* m_module;
	int m_widgetType;
	int m_layIdx;

	QDialog* m_dialog;
	QDialog* m_corrDialog;
	ParamTables *m_paramTables;  //存储
	ParamTables *m_corrTable;    //补偿值
	ParamTables *m_offTable;     //补偿值偏置

	Hobject *m_image;
};

