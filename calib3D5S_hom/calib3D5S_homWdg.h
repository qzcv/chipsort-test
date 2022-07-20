#pragma once
#include <moduleClass.h>
#include "calib3D5S_homParam.h"
#include "HalconCpp.h"

namespace Ui {class calib3D5S_homWdg;}
class calib3D5S_homMod;
class QDoubleSpinBox;
using namespace qzcv;
using namespace Halcon;

class calib3D5S_homWdg : public ModSetWidget
{
	Q_OBJECT

public:
	explicit calib3D5S_homWdg(int i=0);
	~calib3D5S_homWdg();
	virtual void setModule(UnitModule* moudle);
private slots:
	void machinePosChanged(double val);
	void on_bt_Select_clicked();
	void on_bt_OK_clicked();
	void on_bt_Cancel_clicked();
	void checkBox_toggled(bool);
	void radioButton_toggled(bool val);
	void on_bt_selectVerify_toggled(bool val);
	void on_bt_verify_clicked();
	void on_bt_calib_clicked();

private:
	void connectSlots(bool link);
	void iniUi();
	void setUiValue();
	void setRoi();
	void getRoi();
private:
	Ui::calib3D5S_homWdg *ui;
	bool m_hasConnect;
	calib3D5S_homParam* m_param;
	calib3D5S_homMod* m_module;
	int m_widgetType;
	int m_layerIdx;
	Hobject *m_image;
private:
	QDoubleSpinBox *m_btX[BtPointsNum], *m_btY[BtPointsNum];
	QDoubleSpinBox *m_abcdX[PointsNum], *m_abcdY[PointsNum], *m_abcdZ[PointsNum];
	QDoubleSpinBox *m_sideX[2][PointsNum], *m_sideY[2][PointsNum];
};

