#pragma once
#include <moduleClass.h>
#include "HalconCpp.h"

namespace Ui {class nccMatch_fourDirWdg;}
class nccMatch_fourDirParam;
class nccMatch_fourDirMod;
class ImageView;
class QSpinBox;
using namespace qzcv;
using namespace Halcon;

class nccMatch_fourDirWdg : public ModSetWidget
{
	Q_OBJECT

public:
	explicit nccMatch_fourDirWdg(int i=0);
	~nccMatch_fourDirWdg();
	virtual void setModule(UnitModule* moudle);

protected:
	void showEvent(QShowEvent * event);
private slots:
	void on_bt_selectPattern_clicked();
	void on_bt_ensureSelect_clicked();
	void on_bt_cancelSelect_clicked();
	void bt_toggled(bool checked);
	void comboBox_Index_Changed(int index);
	void radioButton_toggled(bool);
	void checkBox_toggled(bool val);
	void spinBox_ValueChanged(int val);
	void on_bt_selectOutPat_clicked();
	void on_bt_outPatOk_clicked();
	void on_bt_cancelOutPat_clicked();
	void outAreaSpinBox_valueChanged(int val);
private:
	void connectSlots(bool link);
	void iniUi();
	void iniData();
	void setUiValue();
	void drawRects();
	void getDrawRects();
	void showRegionXld();
	void autoSetMultPattern();
	void updateUi();

	void bt_editSignal_toggled(bool);
	void bt_threTest_toggled(bool val);

private:
	Ui::nccMatch_fourDirWdg *ui;
	bool m_hasConnect;
	nccMatch_fourDirParam* m_param;
	nccMatch_fourDirMod* m_module;
	int m_widgetType;
	int m_layerIdx;
	bool m_selectingOut;
	bool m_selectingPat;
	Hobject *m_image;
	ImageView *m_patternView;
	QSpinBox* sp_sig[2][4];
};

