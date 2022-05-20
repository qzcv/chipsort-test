#pragma once
#include <moduleClass.h>
#include "HalconCpp.h"

namespace Ui {class nccMatch_outNWdg;}
class nccMatch_outNParam;
class nccMatch_outNMod;
class ImageView;
using namespace qzcv;
using namespace Halcon;

class nccMatch_outNWdg : public ModSetWidget
{
	Q_OBJECT

public:
	explicit nccMatch_outNWdg(int i=0);
	~nccMatch_outNWdg();
	virtual void setModule(UnitModule* moudle);

protected:
	void showEvent(QShowEvent * event);
private slots:
	void on_bt_selectPattern_clicked();
	void on_bt_ensureSelect_clicked();
	void on_bt_cancelSelect_clicked();
	void on_bt_drawMinus_clicked();
	void on_bt_ensureMinus_clicked();
	void on_bt_cancelMinus_clicked();
	void comboBox_IndexChanged(int);
	void on_bt_selectOutPat_clicked();
	void on_bt_outPatOk_clicked();
	void on_bt_cancelOutPat_clicked();
	void bt_toggled(bool val);	

	void outAreaSpinBox_valueChanged(int val);
	void spinBox_valueChanged(int val);
	void CheckBox_toggled(bool val);
private:
	void connectSlots(bool link);
	void iniUi();
	void iniData();
	void setUiValue();
	void updateUi();
	void changeTemp(int index); //变更模板数
	void drawRects();
	void getDrawRects();
	void showRegionXld();

	void bt_threTest_toggled(bool val);
private:
	Ui::nccMatch_outNWdg *ui;
	bool m_hasConnect;
	nccMatch_outNParam* m_param;
	nccMatch_outNMod* m_module;
	int m_widgetType;
	int m_layerIdx;

	typedef enum { UnSel = 0, SelPattern, SelMinusRegion, SelOutPattern, threTest }NccOutNState; //ѡȡ״̬
	NccOutNState m_selState;

	ImageView* m_patternView;
	Hobject* m_image;
};

