#pragma once
#include <moduleClass.h>

namespace Ui {class dimensionMeasureWdg;}
class dimensionMeasureParam;
class dimensionMeasureMod;
class cusViewRotRectList;
using namespace qzcv;

class dimensionMeasureWdg : public ModSetWidget
{
	Q_OBJECT

public:
	explicit dimensionMeasureWdg(int i=0);
	~dimensionMeasureWdg();
	virtual void setModule(UnitModule* moudle);
protected:
	void showEvent(QShowEvent * event);
private slots:
void on_bt_selectROI_toggled(bool check);
void spinBox_valueChanged(int val);
void checkBox_toggled(bool check);
void comboBox_IndexChanged(int index);
void doubleSpinBox_valueChanged(double val);
void radioButton_toggled(bool val);
private:
	void connectSlots(bool link);
	void iniUi();
	void setUiValue();
	void updateUi();
	void drawRects2(int num, double *row, double *col, double *angle,
		double *len1, double *len2, const QStringList& titles, int arrowSize = 5);
	void dispRoi();
	void getRoi();
private:
	Ui::dimensionMeasureWdg *ui;
	bool m_hasConnect;
	dimensionMeasureParam* m_param;
	dimensionMeasureMod* m_module;
	int m_widgetType;
	int m_layerIdx;
	cusViewRotRectList *m_rectList;
};

