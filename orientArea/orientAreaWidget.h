#pragma once
#include <moduleClass.h>

namespace Ui { class orientAreaWidget; }
class orientAreaParam;
class orientAreaModule;
using namespace qzcv;

class orientAreaWidget : public ModSetWidget
{
	Q_OBJECT

public:
	explicit orientAreaWidget(int i=0);
	~orientAreaWidget();
	virtual void setModule(UnitModule* moudle);

protected:
	void showEvent(QShowEvent * event);

private slots:
void bt_toggled(bool);
void cb_toggled(bool);
void sp_valueChanged(int);
void cb_currentIndexChanged(int);

private:
	void connectSlots(bool link);
	void iniUi();
	void setUiValue();
	void testParam();
	void updateUi();

private:
	Ui::orientAreaWidget *ui;
	bool m_hasConnect;
	orientAreaParam* m_param;
	orientAreaModule* m_module;
	int m_widgetType;
	int m_layerIdx;
};

