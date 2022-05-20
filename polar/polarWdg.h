#pragma once
#include <moduleClass.h>

namespace Ui {class polarWdg;}
class polarParam;
class polarMod;
class cusViewRotRectList;
using namespace qzcv;

class polarWdg : public ModSetWidget
{
	Q_OBJECT

public:
	explicit polarWdg(int i=0);
	~polarWdg();
	virtual void setModule(UnitModule* moudle);
protected:
	void showEvent(QShowEvent * event);
private slots:
	void bt_toggled(bool checked);
	void bt_clicked();
	void bt_lineSelect_clicked();
	void bt_assistRectSelect_clicked();
	void sp_valueChanged(int value);
	void cb_currentIndexChanged(int index);
	void cb_toggled(bool checked);
	void rb_clicked();
private:
	void connectSlots(bool link);
	void iniUi();
	void setUiValue();
	void updateUi();
	void drawRects2(int num, double *row, double *col, double *angle,
		double *len1, double *len2, const QStringList& titles, int arrowSize = 5);
	void setParamVisible(bool isVisible);
	void updateLineParam(int index);
private:
	Ui::polarWdg *ui;
	bool m_hasConnect;
	polarParam* m_param;
	polarMod* m_module;
	int m_widgetType;
	int m_layerIdx;
	cusViewRotRectList *m_rectList;
	QStringList m_title = { tr("left 1"),tr("left 2"),tr("right 1"),tr("right 2") };
};

