#pragma once
#include <moduleClass.h>

namespace Ui {class calib3D5S_newWdg;}
class calib3D5S_newParam;
class calib3D5S_newMod;
class QDoubleSpinBox;
class QLabel;
using namespace qzcv;

class calib3D5S_newWdg : public ModSetWidget
{
	Q_OBJECT

public:
	explicit calib3D5S_newWdg(int i=0);
	~calib3D5S_newWdg();
	virtual void setModule(UnitModule* moudle);
protected:
	virtual void showEvent(QShowEvent * event);
private slots:
	void actualSizeChanged(double val);
	void on_bt_Select_clicked();
	void on_bt_OK_clicked();
	void on_bt_Cancel_clicked();
	void radioButton_toggled(bool val);

	void on_bt_calib_clicked();
	void on_bt_compose_clicked();
private:
	void connectSlots(bool link);
	void iniUi();
	void setUiValue();

	void selectSize();
	void getSelectSize();
	void selectLine();
	void getSelectLine();
	void selectPrj();
	void getSelectPrj();
	void selectStand();
	void getSelectStand();
private:
	Ui::calib3D5S_newWdg *ui;
	bool m_hasConnect;
	calib3D5S_newParam* m_param;
	calib3D5S_newMod* m_module;
	int m_widgetType;

	enum { Idle, SelectSize, SelectLine, SelectPrj, SelectStand }m_selectState;

private:
	QDoubleSpinBox* sp_actual[5];
	QDoubleSpinBox* sp_depth[4];
	QDoubleSpinBox* sp_stand[4]; //实际站高
	QLabel* lb_imageSize[5];
	QLabel* lb_pixelSize[5];
	QLabel* lb_angle[4];
	QLabel* lb_offset[4];
	QLabel* lb_aOffset[4];
	QLabel* lb_prjRatio[4];
	QLabel* lb_RS[4]; //识别站高
	QLabel* lb_ES[4]; //识别差值
};

