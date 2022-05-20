#pragma once
#include <moduleClass.h>

namespace Ui {class arithmetic_preWidget;}
class arithmetic_preParam;
class arithmetic_preModule;
using namespace qzcv;

class arithmetic_preWidget : public ModSetWidget
{
	Q_OBJECT

public:
	explicit arithmetic_preWidget(int i=0);
	~arithmetic_preWidget();
	virtual void setModule(UnitModule* moudle);
protected:
	virtual void showEvent(QShowEvent * event);
	private slots:
	void bt_toggled(bool);
	void cb_toggled(bool);
	void cb_currentIndexChanged(int);
	void dsp_valueChanged(double);
	void sp_valueChanged(int);
private:
	void connectSlots(bool link);
	void iniUi();
	void setUiValue();
	void visible(int idx);
private:
	Ui::arithmetic_preWidget *ui;
	bool m_hasConnect;
	arithmetic_preParam* m_param;
	arithmetic_preModule* m_module;
	int m_widgetType;
	int m_layerIdx;
};

