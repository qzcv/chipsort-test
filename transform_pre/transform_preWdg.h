#pragma once
#include <moduleClass.h>

namespace Ui {class transform_preWdg;}
class transform_preParam;
class transform_preMod;
using namespace qzcv;

class transform_preWdg : public ModSetWidget
{
	Q_OBJECT

public:
	explicit transform_preWdg(int i=0);
	~transform_preWdg();
	virtual void setModule(UnitModule* moudle);

protected:
	void showEvent(QShowEvent * event);
private slots:
	void on_cb_type_currentIndexChanged(int val);
private:
	void connectSlots(bool link);
	void iniUi();
	void setUiValue();
private:
	Ui::transform_preWdg *ui;
	bool m_hasConnect;
	transform_preParam* m_param;
	transform_preMod* m_module;
	int m_widgetType;
};

