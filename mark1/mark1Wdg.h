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
	}sPoint;  //����¼�ѡ�еĵ�
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
	void viewMousePressed(int button, int wRow, int wCol, double iRow, double iCol);  //��������¼��Ĳۺ���
private:
	void connectSlots(bool link);
	void iniData();
	void iniUi();
	void setUiValue();
	void updateUi();
	void showCharRegion();
	bool checkPointInside(
		double recrow1, double reccol1, double recrow2, double reccol2, 
		double pointrow, double pointcol);  //������ĵ��Ƿ���������
private:
	Ui::mark1Wdg *ui;
	bool m_hasConnect;
	mark1Param* m_param;
	mark1Mod* m_module;
	int m_widgetType;

	int m_layerIdx;
	Hobject *m_image;
	int m_showPatIndex;  //��ǰ��ʾ��ͼƬindex
	bool m_connectView;  //�Ƿ���connect view��press�źź�slot

	typedef enum { UnSel = 0, SelPattern, SelRoiRegion, StudyTmp }selectState; //ѡȡ״̬
	selectState m_selectState;

	thresholdWidget* m_thresholdWidget;
	OrientEditDialog* m_orientDialog;
	AdvanceSetDialog* m_advanceDialog;
	ShowWidget* m_showWidget;

	vector<sPoint> m_sPoint; //����¼�ѡ�еĵ�

	int m_tmpThreshold;   //�������Զ�ģ��ʱ����ʱ��1ֵ
	int m_tmpMinArea;     //�������Զ�ģ��ʱ����ʱ��С��1��
	int m_tmpTol;
	int m_tmpHorOrVer;
	int m_tmpCharGap;
	int m_tmpMinHole;
	vector<int> m_tmpHoles;  //�������Զ�ģ��ʱ����ʱ�׶���
	vector<int> m_tmpConns;  //�������Զ�ģ��ʱ����ʱ��ͨ��
	HTuple m_smallRecRow1, m_smallRecRow2;
	HTuple m_smallRecCol1, m_smallRecCol2;
};

