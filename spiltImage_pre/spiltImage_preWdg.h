#pragma once
#include <moduleClass.h>

namespace Ui {class spiltImage_preWdg;}
class spiltImage_preParam;
class spiltImage_preMod;
using namespace qzcv;

class spiltImage_preWdg : public ModSetWidget
{
	Q_OBJECT

public:
	explicit spiltImage_preWdg(int i=0);
	~spiltImage_preWdg();
	virtual void setModule(UnitModule* moudle);

protected:
	void showEvent(QShowEvent * event);

private slots:
void bt_toggled(bool checked);
void cb_toggled(bool checked);
void sp_valueChanged(int val);

private:
	void connectSlots(bool link);
	void iniUi();
	void setUiValue();
	void dispRoi();
	void updateUi();
private:
	Ui::spiltImage_preWdg *ui;
	bool m_hasConnect;
	spiltImage_preParam* m_param;
	spiltImage_preMod* m_module;
	int m_widgetType;
	int m_layerIdx;
};

