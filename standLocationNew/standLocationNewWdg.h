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

void tableWidget_valueChanged(int checkIdx, int grpIdx, int leadIdx, bool minOrMax, double val); //tabwidget���ݱ仯
void tableWidget_checkToggled(int index, bool check);
void tableWidget_CopyItems(bool IsAll, int startCheck = 0, int CheckRange = 1, int starLead = 0, int leadRange = 1);
private:
	void connectSlots(bool link);
	void iniUi();
	void setUiValue();
	void refreshTableWidget();
	void setCurrentGroup(int index);  //ˢ�µ�ǰ������ҳ��
private:
	Ui::standLocationNewWdg *ui;
	bool m_hasConnect;
	standLocationNewParam* m_param;
	standLocationNewMod* m_module;
	int m_widgetType;
	int m_layerIdx;

	QDialog* m_dialog;
	QDialog* m_corrDialog;
	ParamTables *m_paramTables;  //�洢
	ParamTables *m_corrTable;    //����ֵ
	ParamTables *m_offTable;     //����ֵƫ��

	int m_grpIndex;  //��Χ������ 0����,1����
	int m_leadIndex; //��ǰ�Ĺܽ�����

	Hobject *m_image;
};

