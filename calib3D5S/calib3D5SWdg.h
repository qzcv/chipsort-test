#pragma once
#include <moduleClass.h>

namespace Ui {class calib3D5SWdg;}
class calib3D5SParam;
class calib3D5SMod;
class QDoubleSpinBox;
class QLabel;

using namespace qzcv;

class calib3D5SWdg : public ModSetWidget
{
	Q_OBJECT

public:
	explicit calib3D5SWdg(int i=0);
	~calib3D5SWdg();
	virtual void setModule(UnitModule* moudle);
protected:
	void showEvent(QShowEvent * event);
private slots:
	void actualSizeChanged(double val);
	void on_bt_Select_clicked();
	void on_bt_OK_clicked();
	void on_bt_Cancel_clicked();
	void on_bt_calib_clicked();
	void radioButton_toggled(bool val);

private:
	void connectSlots(bool link);
	void iniData();
	void iniUi();
	void setUiValue();
	void updateUi();

	void selectSize();
	void getSelectSize();
	void selectLine();
	void getSelectLine();
	void selectPrj();
	void getSelectPrj();

private:
	Ui::calib3D5SWdg *ui;
	bool m_hasConnect;
	calib3D5SParam* m_param;
	calib3D5SMod* m_module;
	int m_widgetType;

	QDoubleSpinBox* sp_actual[5];
	QDoubleSpinBox* sp_depth[4];
	QLabel* lb_imageSize[5];
	QLabel* lb_pixelSize[5];
	QLabel* lb_angle[4];
	QLabel* lb_offset[4];
	QLabel* lb_aOffset[4];
	QLabel* lb_prjRatio[4];

	enum { Idle, SelectSize, SelectLine, SelectPrj }m_selectState;
};

