#pragma once
#include <moduleClass.h>

namespace Ui {class smooth_preWdg;}
class smooth_preParam;
class smooth_preMod;
using namespace qzcv;

class smooth_preWdg : public ModSetWidget
{
	Q_OBJECT

public:
	explicit smooth_preWdg(int i=0);
	~smooth_preWdg();
	virtual void setModule(UnitModule* moudle);

protected:
	void showEvent(QShowEvent * event);

private slots:
	void typeChanged(int val);
	void intParamChanged(int val);
	void doubleParamChanged(double val);

private:
	void connectSlots(bool link);
	void iniUi();
	void setUiValue();
	void setObjVisible();
private:
	Ui::smooth_preWdg *ui;
	bool m_hasConnect;
	smooth_preParam* m_param;
	smooth_preMod* m_module;
	int m_widgetType;
};

