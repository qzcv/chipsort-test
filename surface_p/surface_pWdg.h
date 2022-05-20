#pragma once
#include <moduleClass.h>
#include <HalconCpp.h>

namespace Ui {class surface_pWdg;}
class surface_pParam;
class surface_pMod;
class surfacePAdvanceDialog;

using namespace qzcv;
using namespace Halcon;

class surface_pWdg : public ModSetWidget
{
	Q_OBJECT

public:
	explicit surface_pWdg(int i=0);
	~surface_pWdg();
	virtual void setModule(UnitModule* moudle);

protected:
	void showEvent(QShowEvent * event);
private slots:
	void check_box_toggled(bool check);
	void spinBox_valueChanged(int val);
	void doubleSpinBox_valueChanged(double val);
	void comboBox_Index_Changed(int index);
	void on_bt_selectRoi_toggled(bool check);
	void bt_getPixSize_clicked();
	void radioButton_toggled(bool check);
	void slider_valueChanged(int val);
	void on_bt_advance_clicked(bool val);
	void on_bt_test_toggled(bool val);
private:
	void connectSlots(bool link);
	void iniUi();
	void iniData();
	void setUiValue();
	void updateUi();
	void enableUI(bool val);
	void changeSpinBox(bool isPixOrAct);   //�ı����سߴ绹��ʵ�ʳߴ�
	void drawRoi(bool isGetRoi);   //ʵʱ����ROI,isgetroi�����Ƿ��ȡ���
	void testParam();

private:
	Ui::surface_pWdg *ui;
	bool m_hasConnect;
	surface_pParam* m_param;
	surface_pMod* m_module;
	int m_widgetType;
	int m_layerIdx;
	surfacePAdvanceDialog *m_advanceDialog;

	int current_width, current_height;
	bool m_testing;   //�Ƿ����ڽ̵�

//	Hobject* m_delRegion[3];
//	Hobject* m_inAreaRegion; //������������ָ��
	bool m_IsRoiOrDel;       //��ѡ��Բ��ROIʱ����ѡ��roi,0����deleteROI,1
	bool m_Roiing;           //�Ƿ����ڽ��л��������ѡ��
//	Hobject* m_areaRegion;  //����ȥ���������ָ��;
	HTuple m_homeMat2D;  //ת������
	Hobject *m_image;
};

