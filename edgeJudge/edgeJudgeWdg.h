#pragma once
#include <moduleClass.h>
#include "HalconCpp.h"

namespace Ui {class edgeJudgeWdg;}
class edgeJudgeParam;
class edgeJudgeMod;
using namespace qzcv;
using namespace Halcon;

class edgeJudgeWdg : public ModSetWidget
{
	Q_OBJECT

public:
	explicit edgeJudgeWdg(int i=0);
	~edgeJudgeWdg();
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
void on_bt_advance_toggled(bool val);
void tabWidget_currentIndexChanged(int index);
private:
	void connectSlots(bool link);
	void iniUi();
	void setUiValue();
	void changeGroupIndex(int idx);
private:
	Ui::edgeJudgeWdg *ui;
	bool m_hasConnect;
	edgeJudgeParam* m_param;
	edgeJudgeMod* m_module;
	int m_widgetType;
	int m_layerIdx;
	bool m_IsSelecting;   //是否正在教导
	int m_nowGrpIndex;    //当前所处的组别index
};

