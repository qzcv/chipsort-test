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
	void changeSpinBox(bool isPixOrAct);   //改变像素尺寸还是实际尺寸
	void drawRoi(bool isGetRoi);   //实时绘制ROI,isgetroi代表是否获取结果
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
	bool m_testing;   //是否正在教导

//	Hobject* m_delRegion[3];
//	Hobject* m_inAreaRegion; //传进来的区域指针
	bool m_IsRoiOrDel;       //在选择圆形ROI时，是选择roi,0还是deleteROI,1
	bool m_Roiing;           //是否正在进行绘制区域的选择
//	Hobject* m_areaRegion;  //传出去的面积区域指针;
	HTuple m_homeMat2D;  //转换矩阵
	Hobject *m_image;
};

