#pragma once
#include <moduleClass.h>

namespace Ui {class imageSegment_preWdg;}
class imageSegment_preParam;
class imageSegment_preMod;
using namespace qzcv;

class imageSegment_preWdg : public ModSetWidget
{
	Q_OBJECT

public:
	explicit imageSegment_preWdg(int i=0);
	~imageSegment_preWdg();
	virtual void setModule(UnitModule* moudle);
protected:
	void showEvent(QShowEvent * event);
private slots:
	void checkBox_toggled(bool check);
	void spinBox_valueChanged(int val);
	void hslider_valueChanged(int val);
	void on_bt_selectRoi_toggled(bool check);
	void radioButton_toggled(bool val);
	void comboBox_indexChanged(int index);
	void on_bt_test_toggled(bool val);
private:
	void connectSlots(bool link);
	void iniUi();
	void setUiValue();
	void updateUi();
	void dispRoi();
	void changeRoi(int index);
	void thresholdChanged();
private:
	Ui::imageSegment_preWdg *ui;
	bool m_hasConnect;
	imageSegment_preParam* m_param;
	imageSegment_preMod* m_module;
	int m_widgetType;
	int m_layerIdx;
	bool m_IsTesting;
};

