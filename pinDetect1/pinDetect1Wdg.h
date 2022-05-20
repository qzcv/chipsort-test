#pragma once
#include <moduleClass.h>
#include "HalconCpp.h"

namespace Ui {class pinDetect1Wdg;}
class pinDetect1Param;
class pinDetect1Mod;
class autoThreDialog;
class AdvancedDialog;
class ParamTables;
using namespace qzcv;
using namespace Halcon;

class pinDetect1Wdg : public ModSetWidget
{
	Q_OBJECT

public:
	explicit pinDetect1Wdg(int i=0);
	~pinDetect1Wdg();
	virtual void setModule(UnitModule* moudle);
protected:
	void showEvent(QShowEvent * event);
private slots:
	void radioButton_toggled(bool check);
	void checkBox_toggled(bool check);
	void doubleSpinBox_valueChanged(double val);
	void spinBox_valueChanged(int val);
	void bt_selectRoi_toggled(bool check);
	void comboBox_IndexChanged(int index);
	void tabWidget_indexChange(int index);
	void on_bt_highLevel_toggled(bool val);
	void on_bt_test_toggled(bool val);
	void slider_valueChanged(int val);
	void on_bt_table_clicked();
	void bt_corrTable_clicked();

	void tableWidget_valueChanged(int checkIdx, int grpIdx, int leadIdx, bool minOrMax, double val); //tabwidget���ݱ仯
	void tableWidget_checkToggled(int index, bool check);
	void tableWidget_CopyItems(bool IsAll, int startCheck = 0, int CheckRange = 1, int starLead = 0, int leadRange = 1);

	void autoThre_bt_toggled(bool); //�ָ�ĵ���
	void on_bt_autoThreshold_clicked();
	void on_bt_advance_clicked();
private:
	void connectSlots(bool link);
	void iniUi();
	void iniData();
	void setUiValue();
	void updateUi();
	void autoThreRegion();//�Զ��ָ�����
	void changeLeadOrient(bool updown);//�ı�ܽų���
	void setCurrentGrp(int index);    //���õ�ǰ����ui
	void refreshAutoDiaUi(int index);
	void refreshTableWidget();
	void areaPaint();   //���Ե�ʱ��ʵʱ��Ӧ���ݵı仯
private:
	Ui::pinDetect1Wdg *ui;
	bool m_hasConnect;
	pinDetect1Param* m_param;
	pinDetect1Mod* m_module;
	int m_widgetType;
	int m_layerIdx;
	typedef enum { UnSel = 0, SelRegion, SelAutoReg, SelAutoLead, SelSearch }SelState; //ѡȡ״̬
	SelState m_selState;

	autoThreDialog* m_autoThreDialog;
	AdvancedDialog* m_advDialog;
	Hobject *m_image;

	bool m_testing;   //�Ƿ����ڽ̵�

	QDialog* m_dialog;
	QDialog* m_corrDialog;
	ParamTables *m_paramTables;  //�洢
	ParamTables *m_corrTable;    //����ֵ
};

