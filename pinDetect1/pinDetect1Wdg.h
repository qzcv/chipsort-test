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

	void tableWidget_valueChanged(int checkIdx, int grpIdx, int leadIdx, bool minOrMax, double val); //tabwidget数据变化
	void tableWidget_checkToggled(int index, bool check);
	void tableWidget_CopyItems(bool IsAll, int startCheck = 0, int CheckRange = 1, int starLead = 0, int leadRange = 1);

	void autoThre_bt_toggled(bool); //分割的弹窗
	void on_bt_autoThreshold_clicked();
	void on_bt_advance_clicked();
private:
	void connectSlots(bool link);
	void iniUi();
	void iniData();
	void setUiValue();
	void updateUi();
	void autoThreRegion();//自动分割区域
	void changeLeadOrient(bool updown);//改变管脚朝向
	void setCurrentGrp(int index);    //设置当前组别的ui
	void refreshAutoDiaUi(int index);
	void refreshTableWidget();
	void areaPaint();   //测试的时候实时响应数据的变化
private:
	Ui::pinDetect1Wdg *ui;
	bool m_hasConnect;
	pinDetect1Param* m_param;
	pinDetect1Mod* m_module;
	int m_widgetType;
	int m_layerIdx;
	typedef enum { UnSel = 0, SelRegion, SelAutoReg, SelAutoLead, SelSearch }SelState; //选取状态
	SelState m_selState;

	autoThreDialog* m_autoThreDialog;
	AdvancedDialog* m_advDialog;
	Hobject *m_image;

	bool m_testing;   //是否正在教导

	QDialog* m_dialog;
	QDialog* m_corrDialog;
	ParamTables *m_paramTables;  //存储
	ParamTables *m_corrTable;    //补偿值
};

