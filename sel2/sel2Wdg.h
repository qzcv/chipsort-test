#pragma once
#include <moduleClass.h>

namespace Ui {class sel2Wdg;}
class sel2Param;
class sel2Mod;
using namespace qzcv;

class sel2Wdg : public ModSetWidget
{
	Q_OBJECT

public:
	explicit sel2Wdg(int i=0);
	~sel2Wdg();
	virtual void setModule(UnitModule* moudle);
private slots:
void cob_currentIndexChanged(int idx);
private:
	void connectSlots(bool link);
	void iniUi();
	void setUiValue();
private:
	Ui::sel2Wdg *ui;
	bool m_hasConnect;
	sel2Param* m_param;
	sel2Mod* m_module;
	int m_widgetType;
};

