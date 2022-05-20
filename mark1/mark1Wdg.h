#pragma once
#include <moduleClass.h>
#include "HalconCpp.h"
#include <vector>

namespace Ui {class mark1Wdg;}
class mark1Param;
class mark1Mod;
class ShowWidget;
class thresholdWidget;
class OrientEditDialog;
class AdvanceSetDialog;
using namespace std;
using namespace qzcv;
using namespace Halcon;
 
class mark1Wdg : public ModSetWidget
{
	Q_OBJECT

public:
	explicit mark1Wdg(int i=0);
	~mark1Wdg();
	virtual void setModule(UnitModule* moudle);
	typedef struct {
		double srow;
		double scol;
	}sPoint;  //鼠标事件选中的点
protected:
	virtual void showEvent(QShowEvent * event);
private slots:
	void on_bt_selectRoi_clicked();
	void on_bt_selectPattern_clicked();
	void on_bt_selectOk_clicked();
	void on_bt_selectCancel_clicked();

	void on_bt_studyTmp_clicked();
	void bt_clearTmp_clicked();
	void on_bt_setCharCounts_clicked();
	void bt_testCharThreshold_toggled(bool check);
	void bt_saveImageDir_clicked();
	void on_bt_high_clicked(bool check);

	void on_bt_dir_clicked();
	void on_bt_viewTemp_clicked(bool val);

	void check_box_toggled(bool check);
	void doubleSpinBox_valueChanged(double val);
	void spinBox_valueChanged(int val);
	void comboBox_Index_Changed(int index);

	void radioButton_toggled(bool check);
	void thresholdWidget_thresholdAndArea_changed(int thre, int minarea, int tol, int horVer, int charGap, int minHole);
	void thresholdWidgetAccept();
	void thresholdWidgetReject();
	void viewMousePressed(int button, int wRow, int wCol, double iRow, double iCol);  //触发鼠标事件的槽函数
private:
	void connectSlots(bool link);
	void iniData();
	void iniUi();
	void setUiValue();
	void updateUi();
	void showCharRegion();
	bool checkPointInside(
		double recrow1, double reccol1, double recrow2, double reccol2, 
		double pointrow, double pointcol);  //检查点击的点是否在区域内
private:
	Ui::mark1Wdg *ui;
	bool m_hasConnect;
	mark1Param* m_param;
	mark1Mod* m_module;
	int m_widgetType;

	int m_layerIdx;
	Hobject *m_image;
	int m_showPatIndex;  //当前显示的图片index
	bool m_connectView;  //是否有connect view的press信号和slot

	typedef enum { UnSel = 0, SelPattern, SelRoiRegion, StudyTmp }selectState; //选取状态
	selectState m_selectState;

	thresholdWidget* m_thresholdWidget;
	OrientEditDialog* m_orientDialog;
	AdvanceSetDialog* m_advanceDialog;
	ShowWidget* m_showWidget;

	vector<sPoint> m_sPoint; //鼠标事件选中的点

	int m_tmpThreshold;   //用于在自动模板时的临时阈1值
	int m_tmpMinArea;     //用于在自动模板时的临时最小面1积
	int m_tmpTol;
	int m_tmpHorOrVer;
	int m_tmpCharGap;
	int m_tmpMinHole;
	vector<int> m_tmpHoles;  //用于在自动模板时的临时孔洞数
	vector<int> m_tmpConns;  //用于在自动模板时的临时连通数
	HTuple m_smallRecRow1, m_smallRecRow2;
	HTuple m_smallRecCol1, m_smallRecCol2;
};

