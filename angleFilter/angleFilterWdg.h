#pragma once
#include <moduleClass.h>

namespace Ui {class angleFilterWdg;}
class angleFilterParam;
class angleFilterMod;
class cusViewRotRectList;
class QSpinBox;
using namespace qzcv;

class angleFilterWdg : public ModSetWidget
{
	Q_OBJECT

public:
	explicit angleFilterWdg(int i=0);
	~angleFilterWdg();
	virtual void setModule(UnitModule* moudle);
protected:
	void showEvent(QShowEvent * event);
private slots:
	void bt_clicked();
	void bt_toggled(bool);
	void bt_lineSelect_clicked();
	void cb_toggled(bool);
	void sp_valueChanged(int);
	void cb_currentIndexChanged(int);
private:
	void connectSlots(bool link);
	void iniUi();
	void setUiValue();
	void updateUi();
	void setParamVisible(bool isVisible);
	void updateLineParam(int idx);
	void drawRects2(int num, double *row, double *col, double *angle,
		double *len1, double *len2, const QStringList& titles, int arrowSize = 5);
private:
	Ui::angleFilterWdg *ui;
	bool m_hasConnect;
	angleFilterParam* m_param;
	angleFilterMod* m_module;
	int m_widgetType;
	cusViewRotRectList *m_rectList;
	int m_layerIdx;

	QSpinBox *sp_minAng[2];
	QSpinBox *sp_maxAng[2];
};

