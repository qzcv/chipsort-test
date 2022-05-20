#pragma once
#include <moduleClass.h>
#include "pinWarpParam.h"

namespace Ui {class pinWarpWdg;}
class pinWarpMod;
class QDoubleSpinBox;
class QCheckBox;
using namespace qzcv;

class pinWarpWdg : public ModSetWidget
{
	Q_OBJECT

public:
	explicit pinWarpWdg(int i = 0);
	~pinWarpWdg();
	virtual void setModule(UnitModule* moudle);
protected:
	virtual void showEvent(QShowEvent * event);
private slots:
	void radioButton_toggled(bool check);
	void checkBox_toggled(bool check);
	void doubleSpinBox_valueChanged(double val);
	void spinBox_valueChanged(int val);
	void on_bt_selectRoi_toggled(bool check);
	void comboBox_IndexChanged(int index);
private:
	void connectSlots(bool link);
	void iniUi();
	void setUiValue();
	void updateUi();
	void setCurrentGrp(int index);    //设置当前组别的ui
private:
	Ui::pinWarpWdg *ui;
	bool m_hasConnect;
	pinWarpParam* m_param;
	pinWarpMod* m_module;
	int m_widgetType;
	int m_layerIdx;
private:
	QCheckBox* cb_totalWidth[GRP_NUM];
	QCheckBox* cb_errWarp[GRP_NUM];
	QDoubleSpinBox* dsp_TotalWidthMin[GRP_NUM];
	QDoubleSpinBox* dsp_TotalWidthMax[GRP_NUM];
	QDoubleSpinBox* dsp_ErrWarp[GRP_NUM];
};

