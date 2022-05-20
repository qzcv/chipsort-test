#pragma once
#include <moduleClass.h>

namespace Ui {class edgeLineWdg;}
class edgeLineParam;
class edgeLineMod;
class QSpinBox;
class QComboBox;
class QCheckBox;
class cusViewRotRectList;
using namespace qzcv;

class edgeLineWdg : public ModSetWidget
{
	Q_OBJECT

public:
	explicit edgeLineWdg(int i=0);
	~edgeLineWdg();
	virtual void setModule(UnitModule* moudle);
protected:
	void showEvent(QShowEvent * event);
private slots:
	void on_bt_setOrigin_clicked();
	void checkBox_toggled(bool check);
	void spinBox_valueChanged(int val);
	void comboBox_indexChanged(int val);
	void on_bt_selectRoi_clicked();
	void on_bt_ok_clicked();
	void on_bt_cancel_clicked();
private:
	void connectSlots(bool link);
	void iniUi();
	void setUiValue();
	void updateUi();
	void drawRects2(int num, double *row, double *col, double *angle,
		double *len1, double *len2, const QStringList& titles, int arrowSize = 5);
private:
	Ui::edgeLineWdg *ui;
	bool m_hasConnect;
	edgeLineParam* m_param;
	edgeLineMod* m_module;
	int m_widgetType;
	int m_layerIdx;
	cusViewRotRectList *m_rectList;

private:
	QSpinBox* sp_gradient[4];
	QSpinBox* sp_step[4];
	QSpinBox* sp_degRange[4];
	QSpinBox* sp_rmDist[4];
	QSpinBox* sp_iterateNum[4];
	QComboBox* cob_pol[4];
	QComboBox* cob_first[4];

	QSpinBox* sp_lineOffset[4];
	QCheckBox* cb_fixWidth[4];
	QSpinBox* sp_fixWidth[4];
};

