#pragma once
#include <moduleClass.h>
#include "composeND_preParam.h"

namespace Ui {class composeND_preWidget;}
class composeND_preModule;
class QPushButton;
class QComboBox;
class QSpinBox;
class QGroupBox;
using namespace qzcv;

class composeND_preWidget : public ModSetWidget
{
	Q_OBJECT

public:
	explicit composeND_preWidget(int i=0);
	~composeND_preWidget();
	virtual void setModule(UnitModule* moudle);
protected:
	virtual void showEvent(QShowEvent * event);
private slots:
void bt_clicked();
void cb_currentIndexChanged(int);
void sp_valueChanged(int);
private:
	void connectSlots(bool link);
	void iniUi();
	void setUiValue();
	void iniQtObject();
	void checkROI(int num, double *row0, double *col0, double *row1, double *col1);
private:
	Ui::composeND_preWidget *ui;
	bool m_hasConnect;
	composeND_preParam* m_param;
	composeND_preModule* m_module;
	int m_widgetType;
	int m_layerIdx;

	QComboBox* cb_imgIdx[MAX_REGION];
	QPushButton* bt_Sel[MAX_REGION];
	QPushButton* bt_Ok[MAX_REGION];
	QPushButton* bt_Cancle[MAX_REGION];
	QSpinBox* sp_roiNum[MAX_REGION];
	QGroupBox* gb_block[MAX_REGION];
};

