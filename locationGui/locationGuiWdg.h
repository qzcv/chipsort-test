#pragma once
#include <moduleClass.h>
#include "HalconCpp.h"

namespace Ui {class locationGuiWdg;}
class locationGuiParam;
class locationGuiMod;
class QTableWidgetItem;
class QRadioButton;
class QSpinBox;
class QDoubleSpinBox;
class QCheckBox;
class QTableWidget;
using namespace qzcv;
using namespace Halcon;

#define  CheckBoxNum 4
#define  ChildTableMaxNum 10

class locationGuiWdg : public ModSetWidget
{
	Q_OBJECT

public:
	explicit locationGuiWdg(int i=0);
	~locationGuiWdg();
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
	void on_bt_LTuseLead1_clicked();
	void on_bt_RBuseLead1_clicked();

	void Cb_Toggled(bool val);
	void Child_Changed(QTableWidgetItem * item);

private:
	void connectSlots(bool link);
	void iniUi();
	void setUiValue();
	void updateUi();

	void IniTableUI();
	void IniTableSlots();
	void IniTableData();
	void refrenshTable();
private:
	Ui::locationGuiWdg *ui;
	bool m_hasConnect;
	locationGuiParam* m_param;
	locationGuiMod* m_module;
	int m_widgetType;
	int m_layerIdx;
	Hobject *m_image;
	int m_grpIndex;  //误差范围的组数 0左上,1右下
	int m_leadIndex; //当前的管脚组数

	QRadioButton* rb_blackToWhite[4];
	QRadioButton* rb_whiteToBlack[4];
	QRadioButton* rb_LfToRt[4];
	QRadioButton* rb_RtToLf[4];
	QRadioButton* rb_UpToDw[4];
	QRadioButton* rb_DwToUp[4];
	QSpinBox* sp_extendSearchLine[4];
	QSpinBox* sp_minGrayDiff[4];
	QSpinBox* sp_searchLineGap[4];
	QSpinBox* sp_lineOffset[4];
	QSpinBox* sp_offsetPlus[4];
	QSpinBox* sp_minArea[4];
	QSpinBox* sp_standoffOffset[4];
	QSpinBox* sp_grayval[4];
	QSpinBox* sp_edgeSearchWidth[4];
	QSpinBox* sp_pinSearchWidth[4];
	QDoubleSpinBox* dsp_factor[4];
	QCheckBox* cb_UseStandOff[4];

	QCheckBox *m_cbBox[CheckBoxNum];
	QTableWidget *m_childTable[ChildTableMaxNum];
};

