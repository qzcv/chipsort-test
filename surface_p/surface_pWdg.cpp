#include "surface_pWdg.h"
#include "surface_pMod.h"
#include "surface_pParam.h"
#include "ui_surface_pWdg.h"
#include "surfacePAdvanceDialog.h"
#include "ui_surfacepadvancedialog.h"
#include "ImageView.h"
#include "HalconCpp.h"
#include "halOpenCV.h"
#include <QMessageBox>
#include "transformation.h"

surface_pWdg::surface_pWdg(int i) :
	ModSetWidget(),ui(new Ui::surface_pWdg),m_widgetType(i)
{
	m_hasConnect = false;
	ui->setupUi(this);
	iniData();
	iniUi();
	m_param = nullptr;
	m_module = nullptr;
}

surface_pWdg::~surface_pWdg()
{
	delete ui;
}

void surface_pWdg::setModule(UnitModule *module)
{
	m_module=dynamic_cast<surface_pMod*>(module);
	m_param = m_module->m_param;
	m_image = m_module->m_image;
	//currentView()->setLayer(m_layerIdx);
	//updateUi();
}

void surface_pWdg::showEvent(QShowEvent * event)
{
	updateUi();
}

void surface_pWdg::connectSlots(bool link)
{
	if(m_hasConnect==link)
		return;
	m_hasConnect=link;

	connectOrNot(link, m_advanceDialog->ui->bt_getPixSize, SIGNAL(clicked()), SLOT(bt_getPixSize_clicked()));
	connectOrNot(link, ui->cb_useRefer, SIGNAL(toggled(bool)), SLOT(check_box_toggled(bool)));
	connectOrNot(link, ui->cb_dispAfterDet, SIGNAL(toggled(bool)), SLOT(check_box_toggled(bool)));
	connectOrNot(link, m_advanceDialog->ui->cb_useDelete0, SIGNAL(toggled(bool)), SLOT(check_box_toggled(bool)));
	connectOrNot(link, m_advanceDialog->ui->cb_useDelete1, SIGNAL(toggled(bool)), SLOT(check_box_toggled(bool)));
	connectOrNot(link, m_advanceDialog->ui->cb_useDelete2, SIGNAL(toggled(bool)), SLOT(check_box_toggled(bool)));
	connectOrNot(link, m_advanceDialog->ui->cb_showDel, SIGNAL(toggled(bool)), SLOT(check_box_toggled(bool)));
	connectOrNot(link, ui->cb_ImgGauss, SIGNAL(toggled(bool)), SLOT(check_box_toggled(bool)));
	connectOrNot(link, ui->cb_rec, SIGNAL(toggled(bool)), SLOT(check_box_toggled(bool)));
	connectOrNot(link, ui->cb_fillOutRoi, SIGNAL(toggled(bool)), SLOT(check_box_toggled(bool)));

	connectOrNot(link, ui->cb_singleBadArea, SIGNAL(toggled(bool)), SLOT(check_box_toggled(bool)));
	connectOrNot(link, ui->cb_length, SIGNAL(toggled(bool)), SLOT(check_box_toggled(bool)));
	connectOrNot(link, ui->cb_totalBadArea, SIGNAL(toggled(bool)), SLOT(check_box_toggled(bool)));
	connectOrNot(link, ui->cb_width, SIGNAL(toggled(bool)), SLOT(check_box_toggled(bool)));
	connectOrNot(link, ui->cb_count, SIGNAL(toggled(bool)), SLOT(check_box_toggled(bool)));

	connectOrNot(link, m_advanceDialog->ui->cb_useReferPToDel, SIGNAL(toggled(bool)), SLOT(check_box_toggled(bool)));

	connectOrNot(link, m_advanceDialog->ui->rb_actual, SIGNAL(toggled(bool)), SLOT(radioButton_toggled(bool)));
	connectOrNot(link, m_advanceDialog->ui->rb_pixel, SIGNAL(toggled(bool)), SLOT(radioButton_toggled(bool)));

	connectOrNot(link, m_advanceDialog->ui->sp_up, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, m_advanceDialog->ui->sp_lf, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, m_advanceDialog->ui->sp_down, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, m_advanceDialog->ui->sp_rt, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_open, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_close, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, m_advanceDialog->ui->sp_deletePointZoom, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));

	connectOrNot(link, m_advanceDialog->ui->sp_deleteZoneZoom0, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, m_advanceDialog->ui->sp_deleteZoneZoom1, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, m_advanceDialog->ui->sp_deleteZoneZoom2, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));

	connectOrNot(link, m_advanceDialog->ui->dsp_pixSize, SIGNAL(valueChanged(double)), SLOT(doubleSpinBox_valueChanged(double)));
	connectOrNot(link, ui->dsp_badLengthMin, SIGNAL(valueChanged(double)), SLOT(doubleSpinBox_valueChanged(double)));
	connectOrNot(link, ui->dsp_badLengthMax, SIGNAL(valueChanged(double)), SLOT(doubleSpinBox_valueChanged(double)));
	connectOrNot(link, ui->dsp_badWidthMin, SIGNAL(valueChanged(double)), SLOT(doubleSpinBox_valueChanged(double)));
	connectOrNot(link, ui->dsp_badWidthMax, SIGNAL(valueChanged(double)), SLOT(doubleSpinBox_valueChanged(double)));
	connectOrNot(link, ui->sp_fixThreMax, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_fixThreMin, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_fixThreMax_2, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_fixThreMin_2, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_fixThreMax_3, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_fixThreMin_3, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));

	connectOrNot(link, ui->sp_meanSize, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_holeAreaMin, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_holeAreaMax, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_holeNumMin, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_holeNumMax, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_gaussSize, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_rag, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_loss, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_minRagLossArea, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));

	connectOrNot(link, m_advanceDialog->ui->sp_inputAreaZoom, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, m_advanceDialog->ui->sp_roundVal, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));

	connectOrNot(link, m_advanceDialog->ui->sp_plusDeleteRegion, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->dsp_badAreaMin, SIGNAL(valueChanged(double)), SLOT(doubleSpinBox_valueChanged(double)));
	connectOrNot(link, ui->dsp_badAreaMax, SIGNAL(valueChanged(double)), SLOT(doubleSpinBox_valueChanged(double)));
	connectOrNot(link, ui->sp_blackBad, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_whiteBad, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_blackBad_2, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_whiteBad_2, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->dsp_singleBadAreaMin, SIGNAL(valueChanged(double)), SLOT(doubleSpinBox_valueChanged(double)));
	connectOrNot(link, ui->dsp_singleBadAreaMax, SIGNAL(valueChanged(double)), SLOT(doubleSpinBox_valueChanged(double)));
	connectOrNot(link, ui->dsp_lengthMin, SIGNAL(valueChanged(double)), SLOT(doubleSpinBox_valueChanged(double)));
	connectOrNot(link, ui->dsp_lengthMax, SIGNAL(valueChanged(double)), SLOT(doubleSpinBox_valueChanged(double)));
	connectOrNot(link, ui->dsp_totalBadAreaMin, SIGNAL(valueChanged(double)), SLOT(doubleSpinBox_valueChanged(double)));
	connectOrNot(link, ui->dsp_totalBadAreaMax, SIGNAL(valueChanged(double)), SLOT(doubleSpinBox_valueChanged(double)));
	connectOrNot(link, ui->dsp_widthMin, SIGNAL(valueChanged(double)), SLOT(doubleSpinBox_valueChanged(double)));
	connectOrNot(link, ui->dsp_widthMax, SIGNAL(valueChanged(double)), SLOT(doubleSpinBox_valueChanged(double)));
	connectOrNot(link, ui->sp_countMin, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_countMax, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));

	connectOrNot(link, ui->sp_minArea, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_maxArea, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_threMin, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->hs_threMin, SIGNAL(valueChanged(int)), SLOT(slider_valueChanged(int)));
	connectOrNot(link, ui->sp_threMax, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->hs_threMax, SIGNAL(valueChanged(int)), SLOT(slider_valueChanged(int)));
	connectOrNot(link, ui->cob_roiType, SIGNAL(currentIndexChanged(int)), SLOT(comboBox_Index_Changed(int)));
	connectOrNot(link, ui->cob_type, SIGNAL(currentIndexChanged(int)), SLOT(comboBox_Index_Changed(int)));
}

void surface_pWdg::iniUi()
{

}

void surface_pWdg::iniData()
{
	m_advanceDialog = new surfacePAdvanceDialog(this);
	m_testing = false;
// 	m_areaRegion = NULL;
// 	m_areaRegion = new Hobject;
	m_Roiing = false;
// 	for (int i = 0;i < 3;i++) {
// 		m_delRegion[i] = NULL;
// 	}
	m_layerIdx = 0;
}

void surface_pWdg::setUiValue()
{
	if(!m_param)
		return;

	ui->cob_type->setCurrentIndex((int)m_param->testType);
	ui->stack_type->setCurrentIndex((int)m_param->testType);
	ui->tabWidget->setCurrentIndex(0);
	if (m_param->IsPixOrAct == 0) {
		m_advanceDialog->ui->rb_pixel->setChecked(true);
		changeSpinBox(0);
	}
	else if (m_param->IsPixOrAct == 1) {
		m_advanceDialog->ui->rb_actual->setChecked(true);
		changeSpinBox(1);
	}
	ui->sp_minArea->setValue(m_param->roiMinArea);
	ui->sp_maxArea->setValue(m_param->roiMaxArea);

	ui->sp_threMax->setValue(m_param->roiThreMax);
	ui->hs_threMax->setValue(m_param->roiThreMax);
	ui->sp_threMin->setValue(m_param->roiThreMin);
	ui->hs_threMin->setValue(m_param->roiThreMin);

	ui->cob_roiType->setCurrentIndex(m_param->roiType);
	int index = m_param->roiType;
	if (index == 0 || index == 1 || index == 4) {
		ui->cb_useRefer->setVisible(true);
	}
	else {
		ui->cb_useRefer->setVisible(false);
	}

	ui->cb_rec->setChecked(m_param->IsFitRec);
	ui->cb_fillOutRoi->setChecked(m_param->IsFillOut);
	ui->cb_useRefer->setChecked(m_param->referSection == 1);
	m_advanceDialog->ui->cb_useDelete0->setChecked(m_param->deleteArea_section[0] == 1);
	m_advanceDialog->ui->cb_useDelete1->setChecked(m_param->deleteArea_section[1] == 1);
	m_advanceDialog->ui->cb_useDelete2->setChecked(m_param->deleteArea_section[2] == 1);
	m_advanceDialog->ui->cb_showDel->setChecked(m_param->isShowDel);
	ui->cb_ImgGauss->setChecked(m_param->IsGauss);
	m_advanceDialog->ui->cb_useReferPToDel->setChecked(m_param->IsReferPtoDel);
	ui->sp_gaussSize->setEnabled(m_param->IsGauss);
	ui->sp_gaussSize->setValue(m_param->GaussSize);
	m_advanceDialog->ui->sp_deletePointZoom->setValue(m_param->delPointZoom);

	m_advanceDialog->ui->sp_up->setValue(m_param->zoom_up);
	m_advanceDialog->ui->sp_down->setValue(m_param->zoom_dw);
	m_advanceDialog->ui->sp_lf->setValue(m_param->zoom_lf);
	m_advanceDialog->ui->sp_rt->setValue(m_param->zoom_rt);
	m_advanceDialog->ui->sp_roundVal->setValue(m_param->roundVal);

	m_advanceDialog->ui->sp_inputAreaZoom->setValue(m_param->inputAreaZoom);

	ui->sp_fixThreMin->setValue(m_param->fixThreMin);
	ui->sp_fixThreMax->setValue(m_param->fixThreMax);
	ui->sp_fixThreMin_2->setValue(m_param->fixThreMin);
	ui->sp_fixThreMax_2->setValue(m_param->fixThreMax);
	ui->sp_fixThreMin_3->setValue(m_param->fixThreMin);
	ui->sp_fixThreMax_3->setValue(m_param->fixThreMax);

	ui->sp_meanSize->setValue(m_param->meanSize);
	m_advanceDialog->ui->sp_deleteZoneZoom0->setValue(m_param->deleteArea_zoom[0]);
	m_advanceDialog->ui->sp_deleteZoneZoom1->setValue(m_param->deleteArea_zoom[1]);
	m_advanceDialog->ui->sp_deleteZoneZoom2->setValue(m_param->deleteArea_zoom[2]);
	ui->sp_open->setValue(m_param->openVal);
	ui->sp_close->setValue(m_param->closeVal);

	m_advanceDialog->ui->sp_plusDeleteRegion->setValue(m_param->addDeleteArea);
	//以及一些相对应的显1示和AOI上的切换,有多少的区域就添加多1少个框子
	ui->sp_blackBad->setValue(m_param->black_bad_detect);
	ui->sp_whiteBad->setValue(m_param->white_bad_detect);
	ui->sp_blackBad_2->setValue(m_param->black_bad_detect);
	ui->sp_whiteBad_2->setValue(m_param->white_bad_detect);
	ui->sp_holeAreaMin->setValue(m_param->holeAreaMin);
	ui->sp_holeAreaMax->setValue(m_param->holeAreaMax);
	ui->sp_holeNumMin->setValue(m_param->holeNumMin);
	ui->sp_holeNumMax->setValue(m_param->holeNumMax);

	ui->sp_rag->setValue(m_param->minRag);
	ui->sp_loss->setValue(m_param->minLoss);
	ui->sp_minRagLossArea->setValue(m_param->minRagLossArea);

	m_advanceDialog->ui->dsp_pixSize->setValue(m_param->pixSize);
	ui->cb_singleBadArea->setChecked(m_param->single_badArea_valid);
	//ui.dsp_singleBadAreaMin->setEnabled(m_param->single_badArea_valid);
	//ui.dsp_singleBadAreaMax->setEnabled(m_param->single_badArea_valid);

	ui->cb_totalBadArea->setChecked(m_param->total_badArea_valid);
	//ui.dsp_totalBadAreaMin->setEnabled(m_param->total_badArea_valid);
	//ui.dsp_totalBadAreaMax->setEnabled(m_param->total_badArea_valid);

	ui->cb_length->setChecked(m_param->area_length_valid);
	//ui.dsp_lengthMin->setEnabled(m_param->area_length_valid);
	//ui.dsp_lengthMax->setEnabled(m_param->area_length_valid);


	ui->cb_width->setChecked(m_param->area_width_valid);
	//ui.dsp_widthMin->setEnabled(m_param->area_width_valid);
	//ui.dsp_widthMax->setEnabled(m_param->area_width_valid);

	ui->cb_count->setChecked(m_param->area_count_valid);
	//ui.sp_countMin->setEnabled(m_param->area_count_valid);
	//ui.sp_countMax->setEnabled(m_param->area_count_valid);
	ui->sp_countMin->setValue(m_param->area_count_min);
	ui->sp_countMax->setValue(m_param->area_count_max);

	ui->stackedWidget->setCurrentIndex(0);

	ui->cb_dispAfterDet->setVisible(ui->cob_roiType->currentIndex() < 2);
	ui->cb_dispAfterDet->setChecked(m_param->IsDispROI);
}

void surface_pWdg::updateUi()
{
	connectSlots(false);
	setUiValue();
	connectSlots(true);
}

void surface_pWdg::enableUI(bool val)
{
}

void surface_pWdg::changeSpinBox(bool isPixOrAct)
{
	if (isPixOrAct == 0) {
		ui->dsp_badAreaMin->setDecimals(0);
		ui->dsp_badAreaMax->setDecimals(0);
		ui->dsp_badLengthMin->setDecimals(0);
		ui->dsp_badLengthMax->setDecimals(0);
		ui->dsp_badWidthMin->setDecimals(0);
		ui->dsp_badWidthMax->setDecimals(0);
		ui->dsp_singleBadAreaMin->setDecimals(0);
		ui->dsp_singleBadAreaMax->setDecimals(0);
		ui->dsp_totalBadAreaMin->setDecimals(0);
		ui->dsp_totalBadAreaMax->setDecimals(0);
		ui->dsp_lengthMin->setDecimals(0);
		ui->dsp_lengthMax->setDecimals(0);
		ui->dsp_widthMin->setDecimals(0);
		ui->dsp_widthMax->setDecimals(0);

		ui->dsp_badAreaMin->setValue(m_param->badArea_min / (m_param->pixSize*m_param->pixSize));
		ui->dsp_badAreaMax->setValue(m_param->badArea_max / (m_param->pixSize*m_param->pixSize));
		ui->dsp_badLengthMin->setValue(m_param->badLengthMin / m_param->pixSize);
		ui->dsp_badLengthMax->setValue(m_param->badLengthMax / m_param->pixSize);
		ui->dsp_badWidthMin->setValue(m_param->badWidthMin / m_param->pixSize);
		ui->dsp_badWidthMax->setValue(m_param->badWidthMax / m_param->pixSize);
		ui->dsp_singleBadAreaMin->setValue(m_param->single_badArea_min / (m_param->pixSize*m_param->pixSize));
		ui->dsp_singleBadAreaMax->setValue(m_param->single_badArea_max / (m_param->pixSize*m_param->pixSize));
		ui->dsp_totalBadAreaMin->setValue(m_param->total_badArea_min / (m_param->pixSize*m_param->pixSize));
		ui->dsp_totalBadAreaMax->setValue(m_param->total_badArea_max / (m_param->pixSize*m_param->pixSize));
		ui->dsp_lengthMin->setValue(m_param->area_length_min / m_param->pixSize);
		ui->dsp_lengthMax->setValue(m_param->area_length_max / m_param->pixSize);
		ui->dsp_widthMin->setValue(m_param->area_width_min / m_param->pixSize);
		ui->dsp_widthMax->setValue(m_param->area_width_max / m_param->pixSize);
	}
	else {
		ui->dsp_badAreaMin->setDecimals(6);
		ui->dsp_badAreaMax->setDecimals(6);
		ui->dsp_badLengthMin->setDecimals(3);
		ui->dsp_badLengthMax->setDecimals(3);
		ui->dsp_badWidthMin->setDecimals(3);
		ui->dsp_badWidthMax->setDecimals(3);
		ui->dsp_singleBadAreaMin->setDecimals(6);
		ui->dsp_singleBadAreaMax->setDecimals(6);
		ui->dsp_totalBadAreaMin->setDecimals(6);
		ui->dsp_totalBadAreaMax->setDecimals(6);
		ui->dsp_lengthMin->setDecimals(3);
		ui->dsp_lengthMax->setDecimals(3);
		ui->dsp_widthMin->setDecimals(3);
		ui->dsp_widthMax->setDecimals(3);

		ui->dsp_badAreaMin->setValue(m_param->badArea_min);
		ui->dsp_badAreaMax->setValue(m_param->badArea_max);
		ui->dsp_badLengthMin->setValue(m_param->badLengthMin);
		ui->dsp_badLengthMax->setValue(m_param->badLengthMax);
		ui->dsp_badWidthMin->setValue(m_param->badWidthMin);
		ui->dsp_badWidthMax->setValue(m_param->badWidthMax);
		ui->dsp_singleBadAreaMin->setValue(m_param->single_badArea_min);
		ui->dsp_singleBadAreaMax->setValue(m_param->single_badArea_max);
		ui->dsp_totalBadAreaMin->setValue(m_param->total_badArea_min);
		ui->dsp_totalBadAreaMax->setValue(m_param->total_badArea_max);
		ui->dsp_lengthMin->setValue(m_param->area_length_min);
		ui->dsp_lengthMax->setValue(m_param->area_length_max);
		ui->dsp_widthMin->setValue(m_param->area_width_min);
		ui->dsp_widthMax->setValue(m_param->area_width_max);
	}
}

void surface_pWdg::spinBox_valueChanged(int val)
{
	QSpinBox* sp = qobject_cast<QSpinBox*>(sender());

	QString str0 = QString("");
	QString str1 = QString("");
	if (sp == m_advanceDialog->ui->sp_up) {
		str0 = m_advanceDialog->ui->label_26->text() + m_advanceDialog->ui->label_13->text();
		str1 = tr(" change from %1 to %2").arg(QString::number(m_param->zoom_up)).arg(QString::number(val));
		m_param->zoom_up = val;
	}
	else if (sp == m_advanceDialog->ui->sp_lf) {
		str0 = m_advanceDialog->ui->label_26->text() + m_advanceDialog->ui->label_27->text();
		str1 = tr(" change from %1 to %2").arg(QString::number(m_param->zoom_lf)).arg(QString::number(val));
		m_param->zoom_lf = val;
	}
	else if (sp == m_advanceDialog->ui->sp_down) {
		str0 = m_advanceDialog->ui->label_26->text() + m_advanceDialog->ui->label_11->text();
		str1 = tr(" change from %1 to %2").arg(QString::number(m_param->zoom_dw)).arg(QString::number(val));
		m_param->zoom_dw = val;
	}
	else if (sp == m_advanceDialog->ui->sp_rt) {
		str0 = m_advanceDialog->ui->label_26->text() + m_advanceDialog->ui->label_19->text();
		str1 = tr(" change from %1 to %2").arg(QString::number(m_param->zoom_rt)).arg(QString::number(val));
		m_param->zoom_rt = val;
	}
	else if (sp == m_advanceDialog->ui->sp_roundVal) {
		str0 = m_advanceDialog->ui->lb_round->text();
		str1 = tr(" change from %1 to %2").arg(QString::number(m_param->roundVal)).arg(QString::number(val));
		m_param->roundVal = val;
	}
	else if (sp == ui->sp_fixThreMin) {
		str0 = ui->label_2->text() + tr(" min val ");
		str1 = tr(" change from %1 to %2").arg(QString::number(m_param->fixThreMin)).arg(QString::number(val));

		m_param->fixThreMin = val;
//		m_loadingProject = true;
		connectSlots(false);
		ui->sp_fixThreMin_2->setValue(val);
		ui->sp_fixThreMin_3->setValue(val);
		connectSlots(true);
//		m_loadingProject = false;
		if (m_testing) {
			testParam();
		}
	}
	else if (sp == ui->sp_fixThreMax) {
		str0 = ui->label_2->text() + tr(" max val ");
		str1 = tr(" change from %1 to %2").arg(QString::number(m_param->fixThreMax)).arg(QString::number(val));

		m_param->fixThreMax = val;
//		m_loadingProject = true;
		connectSlots(false);
		ui->sp_fixThreMax_2->setValue(val);
		ui->sp_fixThreMax_3->setValue(val);
		connectSlots(true);
//		m_loadingProject = false;
		if (m_testing) {
			testParam();
		}
	}
	else if (sp == ui->sp_fixThreMin_2) {
		str0 = ui->label_4->text() + tr(" min val ");
		str1 = tr(" change from %1 to %2").arg(QString::number(m_param->fixThreMin)).arg(QString::number(val));

		m_param->fixThreMin = val;
		//m_loadingProject = true;
		connectSlots(false);
		ui->sp_fixThreMin->setValue(val);
		ui->sp_fixThreMin_3->setValue(val);
		connectSlots(true);
		//m_loadingProject = false;
		if (m_testing) {
			testParam();
		}
	}
	else if (sp == ui->sp_fixThreMax_2) {
		str0 = ui->label_4->text() + tr(" max val ");
		str1 = tr(" change from %1 to %2").arg(QString::number(m_param->fixThreMax)).arg(QString::number(val));

		m_param->fixThreMax = val;
		//m_loadingProject = true;
		connectSlots(false);
		ui->sp_fixThreMax_3->setValue(val);
		ui->sp_fixThreMax->setValue(val);
		connectSlots(true);
		//m_loadingProject = false;
		if (m_testing) {
			testParam();
		}
	}
	else if (sp == ui->sp_fixThreMin_3) {
		str0 = ui->label_5->text() + tr(" min val ");
		str1 = tr(" change from %1 to %2").arg(QString::number(m_param->fixThreMin)).arg(QString::number(val));

		m_param->fixThreMin = val;
		//m_loadingProject = true;
		connectSlots(false);
		ui->sp_fixThreMin_2->setValue(val);
		ui->sp_fixThreMin->setValue(val);
		connectSlots(true);
		//m_loadingProject = false;
		if (m_testing) {
			testParam();
		}
	}
	else if (sp == ui->sp_fixThreMax_3) {
		str0 = ui->label_5->text() + tr(" max val ");
		str1 = tr(" change from %1 to %2").arg(QString::number(m_param->fixThreMax)).arg(QString::number(val));

		m_param->fixThreMax = val;
		//m_loadingProject = true;
		connectSlots(false);
		ui->sp_fixThreMax_2->setValue(val);
		ui->sp_fixThreMax->setValue(val);
		connectSlots(true);
		//m_loadingProject = false;
		if (m_testing) {
			testParam();
		}
	}
	else if (sp == ui->sp_meanSize) {
		str0 = ui->label_3->text();
		str1 = tr(" change from %1 to %2").arg(QString::number(m_param->meanSize)).arg(QString::number(val));

		m_param->meanSize = val;
		if (m_testing) {
			testParam();
		}
	}
	else if (sp == ui->sp_gaussSize) {
		str0 = ui->cb_ImgGauss->text();
		str1 = tr(" change from %1 to %2").arg(QString::number(m_param->GaussSize)).arg(QString::number(val));

		m_param->GaussSize = val;
		if (m_testing) {
			testParam();
		}
	}
	else if (sp == m_advanceDialog->ui->sp_plusDeleteRegion) {
		str0 = m_advanceDialog->ui->label_16->text();
		str1 = tr(" change from %1 to %2").arg(QString::number(m_param->addDeleteArea)).arg(QString::number(val));

		surface_pParam::deleteRegion seg;
		seg.row = seg.col = 125;
		seg.length1 = seg.length2 = 25;  //给新添加的roi固定的开始位1置
		seg.angle = 0;
		if (val >= m_param->addDeleteArea) {
			for (int i = m_param->addDeleteArea;i < val;i++) {
				seg.row = seg.col += 10;
				m_param->deleteRegionVector.push_back(seg);
			}
		}
		else if (val < m_param->addDeleteArea) {
			vector<surface_pParam::deleteRegion>::iterator itbegin = m_param->deleteRegionVector.begin() + val + 1;
			vector<surface_pParam::deleteRegion>::iterator itEnd = m_param->deleteRegionVector.end();
			m_param->deleteRegionVector.erase(itbegin, itEnd);
		}
		m_param->addDeleteArea = val;
		if (ui->bt_selectRoi->isChecked()) {
			currentView()->finishDraw();
			//currentView()->clear();
			currentView()->clearLayer(m_layerIdx);
			currentView()->setDrawColor(0, 255, 0);
			double row[10], col[10], length1[10], length2[10], angle[10];
			//const char* title[10] = { "搜寻区域" };
			//char buf[10][32];
			QStringList title;
			title << QString::fromLocal8Bit("搜寻区域");			
			for (int i = 0;i < 1 + m_param->addDeleteArea;i++) {
				row[i] = m_param->deleteRegionVector[i].row;
				col[i] = m_param->deleteRegionVector[i].col;
				length1[i] = m_param->deleteRegionVector[i].length1;
				length2[i] = m_param->deleteRegionVector[i].length2;
				angle[i] = m_param->deleteRegionVector[i].angle;
				if (i > 0) {
// 					sprintf(buf[i], "delete%d", i);
// 					title[i] = buf[i];
					title << QString("delete%1").arg(i);
				}
			}
			currentView()->drawRects2(1 + m_param->addDeleteArea, row, col, angle, length1, length2, title);
			currentView()->update();
		}
	}
	else if (sp == ui->sp_blackBad) {
		str0 = ui->label_18->text();
		str1 = tr(" change from %1 to %2").arg(QString::number(m_param->black_bad_detect)).arg(QString::number(val));

		m_param->black_bad_detect = val;
		//m_loadingProject = true;
		connectSlots(false);
		ui->sp_blackBad_2->setValue(val);    //两个控件共用一个参数
		connectSlots(true);
		//m_loadingProject = false;
		if (m_testing) {
			testParam();
		}
	}
	else if (sp == ui->sp_whiteBad) {
		str0 = ui->label_22->text();
		str1 = tr(" change from %1 to %2").arg(QString::number(m_param->white_bad_detect)).arg(QString::number(val));

		m_param->white_bad_detect = val;
		//m_loadingProject = true;
		connectSlots(false);
		ui->sp_whiteBad_2->setValue(val);    //两个控件共用一个参数
		connectSlots(true);
		//m_loadingProject = false;
		if (m_testing) {
			testParam();
		}
	}
	else if (sp == ui->sp_blackBad_2) {
		str0 = ui->label_23->text();
		str1 = tr(" change from %1 to %2").arg(QString::number(m_param->black_bad_detect)).arg(QString::number(val));

		m_param->black_bad_detect = val;
		//m_loadingProject = true;
		connectSlots(false);
		ui->sp_blackBad->setValue(val);
		connectSlots(true);
		//m_loadingProject = false;
		if (m_testing) {
			testParam();
		}
	}
	else if (sp == ui->sp_whiteBad_2) {
		str0 = ui->label_30->text();
		str1 = tr(" change from %1 to %2").arg(QString::number(m_param->white_bad_detect)).arg(QString::number(val));

		m_param->white_bad_detect = val;
		//m_loadingProject = true;
		connectSlots(false);
		ui->sp_whiteBad->setValue(val);
		connectSlots(true);
		//m_loadingProject = false;
		if (m_testing) {
			testParam();
		}
	}
	else if (sp == ui->sp_countMin) {
		str0 = ui->cb_count->text() + tr(" min val ");
		str1 = tr(" change from %1 to %2").arg(QString::number(m_param->area_count_min)).arg(QString::number(val));

		m_param->area_count_min = val;
	}
	else if (sp == ui->sp_countMax) {
		str0 = ui->cb_count->text() + tr(" max val ");
		str1 = tr(" change from %1 to %2").arg(QString::number(m_param->area_count_max)).arg(QString::number(val));

		m_param->area_count_max = val;
	}
	else if (sp == m_advanceDialog->ui->sp_deleteZoneZoom0) {
		str0 = m_advanceDialog->ui->cb_useDelete0->text() + m_advanceDialog->ui->label_15->text();
		str1 = tr(" change from %1 to %2").arg(QString::number(m_param->deleteArea_zoom[0])).arg(QString::number(val));

		m_param->deleteArea_zoom[0] = val;
	}
	else if (sp == m_advanceDialog->ui->sp_deleteZoneZoom1) {
		str0 = m_advanceDialog->ui->cb_useDelete1->text() + m_advanceDialog->ui->label_15->text();
		str1 = tr(" change from %1 to %2").arg(QString::number(m_param->deleteArea_zoom[1])).arg(QString::number(val));

		m_param->deleteArea_zoom[1] = val;
	}
	else if (sp == m_advanceDialog->ui->sp_deleteZoneZoom2) {
		str0 = m_advanceDialog->ui->cb_useDelete2->text() + m_advanceDialog->ui->label_15->text();
		str1 = tr(" change from %1 to %2").arg(QString::number(m_param->deleteArea_zoom[2])).arg(QString::number(val));

		m_param->deleteArea_zoom[2] = val;
	}
	else if (sp == m_advanceDialog->ui->sp_inputAreaZoom) {
		str0 = m_advanceDialog->ui->label_8->text();
		str1 = tr(" change from %1 to %2").arg(QString::number(m_param->inputAreaZoom)).arg(QString::number(val));

		m_param->inputAreaZoom = val;
	}
	else  if (sp == ui->sp_holeAreaMax) {
		str0 = ui->label_24->text() + tr(" max val ");
		str1 = tr(" change from %1 to %2").arg(QString::number(m_param->holeAreaMax)).arg(QString::number(val));

		m_param->holeAreaMax = val;
		if (m_testing) {
			testParam();
		}
	}
	else  if (sp == ui->sp_holeAreaMin) {
		str0 = ui->label_24->text() + tr(" min val ");
		str1 = tr(" change from %1 to %2").arg(QString::number(m_param->holeAreaMin)).arg(QString::number(val));

		m_param->holeAreaMin = val;
		if (m_testing) {
			testParam();
		}
	}
	else  if (sp == ui->sp_holeNumMax) {
		str0 = ui->label_31->text() + tr(" max val ");
		str1 = tr(" change from %1 to %2").arg(QString::number(m_param->holeNumMax)).arg(QString::number(val));

		m_param->holeNumMax = val;
		if (m_testing) {
			testParam();
		}
	}
	else  if (sp == ui->sp_holeNumMin) {
		str0 = ui->label_31->text() + tr(" min val ");
		str1 = tr(" change from %1 to %2").arg(QString::number(m_param->holeNumMin)).arg(QString::number(val));

		m_param->holeNumMin = val;
		if (m_testing) {
			testParam();
		}
	}
	else if (sp == ui->sp_rag) {
		str0 = ui->label_32->text();
		str1 = tr(" change from %1 to %2").arg(QString::number(m_param->minRag)).arg(QString::number(val));

		m_param->minRag = val;
		if (m_testing) {
			testParam();
		}
	}
	else if (sp == ui->sp_loss) {
		str0 = ui->label_33->text();
		str1 = tr(" change from %1 to %2").arg(QString::number(m_param->minLoss)).arg(QString::number(val));

		m_param->minLoss = val;
		if (m_testing) {
			testParam();
		}
	}
	else if (sp == ui->sp_minRagLossArea) {
		str0 = ui->label_34->text();
		str1 = tr(" change from %1 to %2").arg(QString::number(m_param->minRagLossArea)).arg(QString::number(val));

		m_param->minRagLossArea = val;
		if (m_testing) {
			testParam();
		}
	}
	else  if (sp == ui->sp_open) {
		str0 = ui->label_9->text();
		str1 = tr(" change from %1 to %2").arg(QString::number(m_param->openVal)).arg(QString::number(val));

		m_param->openVal = val;
		if (m_testing) {
			testParam();
		}
	}
	else  if (sp == ui->sp_close) {
		str0 = ui->label_10->text();
		str1 = tr(" change from %1 to %2").arg(QString::number(m_param->closeVal)).arg(QString::number(val));

		m_param->closeVal = val;
		if (m_testing) {
			testParam();
		}
	}
	else  if (sp == m_advanceDialog->ui->sp_deletePointZoom) {
		str0 = m_advanceDialog->ui->cb_useReferPToDel->text() + m_advanceDialog->ui->label_25->text();
		str1 = tr(" change from %1 to %2").arg(QString::number(m_param->delPointZoom)).arg(QString::number(val));

		m_param->delPointZoom = val;
	}
	else if (sp == ui->sp_minArea) {
		str0 = ui->label->text() + tr(" min val ");
		str1 = tr(" change from %1 to %2").arg(QString::number(m_param->roiMinArea)).arg(QString::number(val));

		if (val > m_param->roiMaxArea) {
			ui->sp_minArea->setValue(m_param->roiMaxArea);
			return;
		}
		m_param->roiMinArea = val;
		if (m_Roiing) {
			//currentView()->clear();
			currentView()->clearLayer(m_layerIdx);
			drawRoi(false);
			currentView()->update();
		}
	}
	else if (sp == ui->sp_maxArea) {
		str0 = ui->label->text() + tr(" max val ");
		str1 = tr(" change from %1 to %2").arg(QString::number(m_param->roiMaxArea)).arg(QString::number(val));

		if (val < m_param->roiMinArea) {
			ui->sp_maxArea->setValue(m_param->roiMinArea);
			return;
		}
		m_param->roiMaxArea = val;
		if (m_Roiing) {
			//currentView()->clear();
			currentView()->clearLayer(m_layerIdx);
			drawRoi(false);
			currentView()->update();
		}
	}
	else if (sp == ui->sp_threMin) {
		str0 = ui->groupBox_2->title() + tr(" min val ");
		str1 = tr(" change from %1 to %2").arg(QString::number(m_param->roiThreMin)).arg(QString::number(val));

		if (val > m_param->roiThreMax) {
			ui->sp_threMin->setValue(m_param->roiThreMax);
			return;
		}
		m_param->roiThreMin = val;
		//m_loadingProject = true;
		connectSlots(false);
		ui->hs_threMin->setValue(val);
		connectSlots(true);
		//m_loadingProject = false;
		if (m_Roiing) {
			//currentView()->clear();
			currentView()->clearLayer(m_layerIdx);
			drawRoi(false);
			currentView()->update();
		}
	}
	else if (sp == ui->sp_threMax) {
		str0 = ui->groupBox_2->title() + tr(" max val ");
		str1 = tr(" change from %1 to %2").arg(QString::number(m_param->roiThreMax)).arg(QString::number(val));

		if (val < m_param->roiThreMin) {
			ui->sp_threMax->setValue(m_param->roiThreMin);
			return;
		}
		m_param->roiThreMax = val;
		//m_loadingProject = true;
		connectSlots(false);
		ui->hs_threMax->setValue(val);
		connectSlots(true);
		//m_loadingProject = false;
		if (m_Roiing) {
			//currentView()->clear();
			currentView()->clearLayer(m_layerIdx);
			drawRoi(false);
			currentView()->update();
		}
	}
	m_module->setParamChanged(QvsParamLevel::ProductLevel);
//	log(LogL1, str0 + str1);
}

void surface_pWdg::doubleSpinBox_valueChanged(double val)
{
	QDoubleSpinBox* dsp = qobject_cast<QDoubleSpinBox*>(sender());

	QString str0 = QString("");
	QString str1 = QString("");
	if (dsp == ui->dsp_badLengthMin) {
		if (m_param->IsPixOrAct == 0) {
			val *= m_param->pixSize;
		}
		str0 = ui->tabWidget->tabText(0) + ui->label_20->text() + tr(" min val ");
		str1 = tr(" change from %1 to %2").arg(QString::number(m_param->badLengthMin, 'f', 3)).arg(QString::number(val, 'f', 3));
		m_param->badLengthMin = val;
		if (m_testing) {
			testParam();
		}
	}
	else if (dsp == ui->dsp_badLengthMax) {
		if (m_param->IsPixOrAct == 0) {
			val *= m_param->pixSize;
		}
		str0 = ui->tabWidget->tabText(0) + ui->label_20->text() + tr(" max val ");
		str1 = tr(" change from %1 to %2").arg(QString::number(m_param->badLengthMax, 'f', 3)).arg(QString::number(val, 'f', 3));
		m_param->badLengthMax = val;
		if (m_testing) {
			testParam();
		}
	}
	else if (dsp == ui->dsp_badWidthMin) {
		if (m_param->IsPixOrAct == 0) {
			val *= m_param->pixSize;
		}
		str0 = ui->tabWidget->tabText(0) + ui->label_21->text() + tr(" min val ");
		str1 = tr(" change from %1 to %2").arg(QString::number(m_param->badWidthMin, 'f', 3)).arg(QString::number(val, 'f', 3));
		m_param->badWidthMin = val;
		if (m_testing) {
			testParam();
		}
	}
	else if (dsp == ui->dsp_badWidthMax) {
		if (m_param->IsPixOrAct == 0) {
			val *= m_param->pixSize;
		}
		str0 = ui->tabWidget->tabText(0) + ui->label_21->text() + tr(" max val ");
		str1 = tr(" change from %1 to %2").arg(QString::number(m_param->badWidthMax, 'f', 3)).arg(QString::number(val, 'f', 3));
		m_param->badWidthMax = val;
		if (m_testing) {
			testParam();
		}
	}
	else if (dsp == ui->dsp_badAreaMin) {
		if (m_param->IsPixOrAct == 0) {
			val *= m_param->pixSize*m_param->pixSize;
		}
		str0 = ui->tabWidget->tabText(0) + ui->label_17->text() + tr(" min val ");
		str1 = tr(" change from %1 to %2").arg(QString::number(m_param->badArea_min, 'f', 6)).arg(QString::number(val, 'f', 6));
		m_param->badArea_min = val;
		if (m_testing) {
			testParam();
		}
	}
	else if (dsp == ui->dsp_badAreaMax) {
		if (m_param->IsPixOrAct == 0) {
			val *= m_param->pixSize*m_param->pixSize;
		}
		str0 = ui->tabWidget->tabText(0) + ui->label_17->text() + tr(" max val ");
		str1 = tr(" change from %1 to %2").arg(QString::number(m_param->badArea_max, 'f', 6)).arg(QString::number(val, 'f', 6));
		m_param->badArea_max = val;
		if (m_testing) {
			testParam();
		}
	}
	else if (dsp == ui->dsp_singleBadAreaMin) {
		if (m_param->IsPixOrAct == 0) {
			val *= m_param->pixSize*m_param->pixSize;
		}
		str0 = ui->groupBox_5->title() + ui->cb_singleBadArea->text() + tr(" min val ");
		str1 = tr(" change from %1 to %2").arg(QString::number(m_param->single_badArea_min, 'f', 6)).arg(QString::number(val, 'f', 6));
		m_param->single_badArea_min = val;
	}
	else if (dsp == ui->dsp_singleBadAreaMax) {
		if (m_param->IsPixOrAct == 0) {
			val *= m_param->pixSize*m_param->pixSize;
		}
		str0 = ui->groupBox_5->title() + ui->cb_singleBadArea->text() + tr(" max val ");
		str1 = tr(" change from %1 to %2").arg(QString::number(m_param->single_badArea_max, 'f', 6)).arg(QString::number(val, 'f', 6));
		m_param->single_badArea_max = val;
	}
	else if (dsp == ui->dsp_totalBadAreaMin) {
		if (m_param->IsPixOrAct == 0) {
			val *= m_param->pixSize*m_param->pixSize;
		}
		str0 = ui->groupBox_5->title() + ui->cb_totalBadArea->text() + tr(" min val ");
		str1 = tr(" change from %1 to %2").arg(QString::number(m_param->total_badArea_min, 'f', 6)).arg(QString::number(val, 'f', 6));
		m_param->total_badArea_min = val;
	}
	else if (dsp == ui->dsp_totalBadAreaMax) {
		if (m_param->IsPixOrAct == 0) {
			val *= m_param->pixSize*m_param->pixSize;
		}
		str0 = ui->groupBox_5->title() + ui->cb_totalBadArea->text() + tr(" max val ");
		str1 = tr(" change from %1 to %2").arg(QString::number(m_param->total_badArea_max, 'f', 6)).arg(QString::number(val, 'f', 6));
		m_param->total_badArea_max = val;
	}
	else if (dsp == ui->dsp_lengthMin) {
		if (m_param->IsPixOrAct == 0) {
			val *= m_param->pixSize;
		}
		str0 = ui->groupBox_5->title() + ui->cb_length->text() + tr(" min val ");
		str1 = tr(" change from %1 to %2").arg(QString::number(m_param->area_length_min, 'f', 3)).arg(QString::number(val, 'f', 3));
		m_param->area_length_min = val;
	}
	else if (dsp == ui->dsp_lengthMax) {
		if (m_param->IsPixOrAct == 0) {
			val *= m_param->pixSize;
		}
		str0 = ui->groupBox_5->title() + ui->cb_length->text() + tr(" max val ");
		str1 = tr(" change from %1 to %2").arg(QString::number(m_param->area_length_max, 'f', 3)).arg(QString::number(val, 'f', 3));
		m_param->area_length_max = val;
	}
	else if (dsp == ui->dsp_widthMin) {
		if (m_param->IsPixOrAct == 0) {
			val *= m_param->pixSize;
		}
		str0 = ui->groupBox_5->title() + ui->cb_width->text() + tr(" min val ");
		str1 = tr(" change from %1 to %2").arg(QString::number(m_param->area_width_min, 'f', 3)).arg(QString::number(val, 'f', 3));
		m_param->area_width_min = val;
	}
	else if (dsp == ui->dsp_widthMax) {
		if (m_param->IsPixOrAct == 0) {
			val *= m_param->pixSize;
		}
		str0 = ui->groupBox_5->title() + ui->cb_width->text() + tr(" max val ");
		str1 = tr(" change from %1 to %2").arg(QString::number(m_param->area_width_max, 'f', 3)).arg(QString::number(val, 'f', 3));
		m_param->area_width_max = val;
	}
	else if (dsp == m_advanceDialog->ui->dsp_pixSize) {
		str0 = m_advanceDialog->ui->label_28->text();
		str1 = tr(" change from %1 to %2").arg(QString::number(m_param->pixSize, 'f', 3)).arg(QString::number(val, 'f', 3));

		m_param->pixSize = val;
		//m_loadingProject = true;
		if (m_param->IsPixOrAct == 0) {
			connectSlots(false);
			changeSpinBox(0);
			connectSlots(true);
		}
		//m_loadingProject = false;
	}
	m_module->setParamChanged(QvsParamLevel::ProductLevel);
//	log(LogL1, str0 + str1);
}

void surface_pWdg::comboBox_Index_Changed(int index)
{
	QComboBox* cob = qobject_cast<QComboBox*>(sender());
	QString str0 = QString("");

	if (cob == ui->cob_roiType) {
		str0 = tr(" change from %1 to %2").arg(cob->itemText(m_param->roiType)).arg(cob->itemText(index));

		m_param->roiType = index;
		if (index == 0 || index == 1 || index == 4) {
			ui->cb_useRefer->setVisible(true);
		}
		else {
			ui->cb_useRefer->setVisible(false);
		}

		ui->cb_dispAfterDet->setVisible(ui->cob_roiType->currentIndex() < 2);
	}
	else if (cob == ui->cob_type) {
		str0 = tr(" change from %1 to %2").arg(cob->itemText(m_param->testType)).arg(cob->itemText(index));

		m_param->testType = (surface_pParam::TestType)index;
		ui->stack_type->setCurrentIndex(index);
		if (m_testing) {
			testParam();
		}
	}
	m_module->setParamChanged(QvsParamLevel::ProductLevel);
//	log(LogL1, str0);
}

void surface_pWdg::on_bt_selectRoi_toggled(bool check)
{
	QString str0 = ui->bt_selectRoi->text();

	if (check) {
		//currentView()->clear();
		currentView()->clearLayer(m_layerIdx);
		currentView()->setDrawColor(0, 255, 0);
		ui->bt_selectRoi->setText(tr("ROI OK"));
		//ui.sp_plusDeleteRegion->setEnabled(false);
		double row[10], col[10], angle[10], length1[10], length2[10];
		QStringList title;
		for (auto i = 0;i < 10;i++)
			title << "";
		int a = 1;   //辅助用于圆形ROI时候的检测
		int b = 0;

		m_IsRoiOrDel = 0;
		if (m_param->roiType == 4) {  //绘制区域
			m_Roiing = true;
			ui->stackedWidget->setCurrentIndex(1);
			drawRoi(false);
		}
		if (m_param->roiType == 1) {
			QMessageBox::StandardButton button;
			button = QMessageBox::question(this, tr("select type"),
				tr("select circular ROI?"), QMessageBox::Yes | QMessageBox::No);
			if (button == QMessageBox::Yes) {
				m_IsRoiOrDel = 0;
				currentView()->drawCircle(m_param->cirRow, m_param->cirCol, m_param->cirR, "Roi");
				currentView()->update();
				return;
			}
			else {
				m_IsRoiOrDel = 1;
			}
		}
		if (m_param->roiType == 1 && m_IsRoiOrDel == 1) {
			a = 0;
			b = 1;
		}
		for (int i = 0;i < a + m_param->addDeleteArea;i++) {
			row[i] = m_param->deleteRegionVector[i + b].row;
			col[i] = m_param->deleteRegionVector[i + b].col;
			angle[i] = m_param->deleteRegionVector[i + b].angle;
			length1[i] = m_param->deleteRegionVector[i + b].length1;
			length2[i] = m_param->deleteRegionVector[i + b].length2;
			if (i == 0 && !(m_param->roiType == 1 && m_IsRoiOrDel == 1)) {
				title[0] = "Roi";
			}
			else {
				title[i] = QString("delete%1").arg(i);
			}
		}
		if (a + m_param->addDeleteArea <= 0) {
			return;
		}
		currentView()->drawRects2(a + m_param->addDeleteArea, row, col, angle, length1, length2, title);
		currentView()->update();

	}
	else {
		currentView()->finishDraw();
		currentView()->update();
		ui->bt_selectRoi->setText(tr("Select ROI"));
		if (m_param->roiType == 1 && m_IsRoiOrDel == 0) {
			currentView()->getDrawCircle(m_param->cirRow, m_param->cirCol, m_param->cirR);
		}
		else {

			int a = 1;
			int b = 0;
			if (m_param->roiType == 1 && m_IsRoiOrDel == 1) {
				a = 0;
				b = 1;
			}
			if (a + m_param->addDeleteArea <= 0) {
				return;
			}
			double row[10], col[10], angle[10], length1[10], length2[10];
			currentView()->getDrawRects2(a + m_param->addDeleteArea, row, col, angle, length1, length2);
			for (int i = 0;i < a + m_param->addDeleteArea;i++) {
				m_param->deleteRegionVector[i + b].row = row[i];
				m_param->deleteRegionVector[i + b].col = col[i];
				m_param->deleteRegionVector[i + b].length1 = length1[i];
				m_param->deleteRegionVector[i + b].length2 = length2[i];
				m_param->deleteRegionVector[i + b].angle = angle[i];
			}
		}

		if (m_param->roiType == 4) {  //绘制区域
			m_Roiing = false;
			ui->stackedWidget->setCurrentIndex(0);
			drawRoi(true);
		}

		m_module->setParamChanged(QvsParamLevel::ProductLevel);
	}

//	log(LogL1, str0);
}

void surface_pWdg::bt_getPixSize_clicked()
{	
	if (m_module->p_pixelSize->size() == 0)
		return;
	double pixSize = m_module->p_pixelSize->at(0);
	if (pixSize < 0.00001)
		return;
	m_advanceDialog->ui->dsp_pixSize->setValue(pixSize);
}

void surface_pWdg::radioButton_toggled(bool check)
{
	QRadioButton* rb = qobject_cast<QRadioButton*>(sender());
	QString str0 = QString("");

	if (check) {
		if (rb == m_advanceDialog->ui->rb_pixel) {
			str0 = tr(" change from %1 to %2").arg(m_advanceDialog->ui->rb_actual->text()).arg(m_advanceDialog->ui->rb_pixel->text());

			m_param->IsPixOrAct = 0;
			connectSlots(false);
			changeSpinBox(0);
			connectSlots(true);
		}
		else if (rb == m_advanceDialog->ui->rb_actual) {
			str0 = tr(" change from %1 to %2").arg(m_advanceDialog->ui->rb_pixel->text()).arg(m_advanceDialog->ui->rb_actual->text());

			m_param->IsPixOrAct = 1;
			connectSlots(false);
			changeSpinBox(1);
			connectSlots(true);
		}
		m_module->setParamChanged(QvsParamLevel::ProductLevel);
		//log(LogL1, str0);
	}
}

void surface_pWdg::slider_valueChanged(int val)
{
	QSlider* sl = qobject_cast<QSlider*>(sender());
	if (sl == ui->hs_threMin) {
		if (val > m_param->roiThreMax) {
			ui->hs_threMin->setValue(m_param->roiThreMax);
			return;
		}
		m_param->roiThreMin = val;
		ui->sp_threMin->setValue(val);
	}
	else if (sl == ui->hs_threMax) {
		if (val < m_param->roiThreMin) {
			ui->hs_threMax->setValue(m_param->roiThreMin);
			return;
		}
		m_param->roiThreMax = val;
		ui->sp_threMax->setValue(val);
	}
}

void surface_pWdg::on_bt_advance_clicked(bool val)
{
	if (m_advanceDialog->exec() == QDialog::Accepted) {
	}
}

void surface_pWdg::on_bt_test_toggled(bool val)
{
	if (val) {
		ui->bt_test->setText(tr("exit test"));
		m_testing = true;
		testParam();
// 		RlRegion reg;
// 		reg.genRect(0, 0, 30, 100);
// 		currentView()->setFilled(true);
// 		currentView()->setColor(Qt::green);
// 		currentView()->dispRegion(&reg);
// 
// 		cv::Mat m = vector_angle_to_rigid(cv::Point2d(0, 0), 0, cv::Point2d(0, 0), CV_PI / 10);
// 		HTuple hm;
// 		HalOpenCV::cv2HalHomMat2D(m, &hm);
// 
// 		RlRegion rlTransReg;
// 		rlTransReg.affineTrans(reg, m, false);
// 		currentView()->setColor(Qt::red);
// 		currentView()->dispRegion(&rlTransReg);
// 
// 		Hobject hReg, hTransReg;
// 		HalOpenCV::cv2halRegion(reg, &hReg);
// 		affine_trans_region(hReg, &hTransReg, hm, "false");
// 		currentView()->setColor(Qt::blue);
// 		RlRegion tmp;
// 		HalOpenCV::hal2cvRegion(hTransReg, &tmp);
// 		currentView()->dispRegion(&tmp);
// 		currentView()->update();
// 		currentView()->setFilled(false);
	}
	else {
		ui->bt_test->setText(tr("test"));
		m_testing = false;
		//currentView()->clear();
		currentView()->clearLayer(m_layerIdx);
		currentView()->update();
	}
}

void surface_pWdg::drawRoi(bool isGetRoi)
{
	Hobject threRect, tmpImage, tmpResult;
	if (m_module->p_im->empty()) {
		return;
	}

	HalOpenCV::cv2halImg(*m_module->p_im, m_image, false);

	Hlong channum;
	Hobject grayimg;
	count_channels(*m_image, &channum);
	if (channum == 3) {
		rgb1_to_gray(*m_image, &grayimg);
	}
	else {
		copy_image(*m_image, &grayimg);
	}
	gen_rectangle2(&threRect, m_param->deleteRegionVector[0].row, m_param->deleteRegionVector[0].col,
		m_param->deleteRegionVector[0].angle, m_param->deleteRegionVector[0].length1, m_param->deleteRegionVector[0].length2);
	reduce_domain(grayimg, threRect, &tmpImage);
	threshold(tmpImage, &tmpImage, m_param->roiThreMin, m_param->roiThreMax < m_param->roiThreMin ? m_param->roiThreMin : m_param->roiThreMax);
	connection(tmpImage, &tmpImage);
	m_module->mselect_shape(tmpImage, &tmpImage, "area", "and", m_param->roiMinArea, m_param->roiMaxArea < m_param->roiMinArea ? m_param->roiMinArea : m_param->roiMaxArea);
	if (!isGetRoi) {
		currentView()->setFilled(true);
		currentView()->setColor(0, 255, 255);
		RlRegion reg;
		HalOpenCV::hal2cvRegion(tmpImage, &reg);
		currentView()->dispRegion(&reg);
	}
	else {
		union1(tmpImage, &tmpImage);
		HTuple num;
		count_obj(tmpImage, &num);
		if (num[0].I() > 0) {
			copy_obj(tmpImage, &m_param->roiRegion, 1, 1);
		}
		else {
			gen_empty_obj(&m_param->roiRegion);
		}
	}
}

void surface_pWdg::testParam()
{
	if (m_module->p_im->empty())
		return;

	Hobject dstImg, showTmp;
	m_module->calTargetReg(&dstImg, &showTmp);
	union1(dstImg, &dstImg);
	union1(showTmp, &showTmp);

	//currentView()->clear();
	currentView()->clearLayer(m_layerIdx);
	if (m_param->testType == surface_pParam::ragAndLoss)
	{
		currentView()->setColor(255, 0, 0);
		currentView()->setFilled(true);
		RlRegion tmpReg;
		HalOpenCV::hal2cvRegion(showTmp, &tmpReg);
		currentView()->dispRegion(&tmpReg);
	}
	currentView()->setFilled(true);
	currentView()->setColor(0, 255, 0);
	RlRegion dstImgReg;
	HalOpenCV::hal2cvRegion(dstImg, &dstImgReg);
	currentView()->dispRegion(&dstImgReg);
	currentView()->update();
}

void surface_pWdg::check_box_toggled(bool check)
{
	QCheckBox* cb = qobject_cast<QCheckBox*>(sender());

	QString str0 = cb->text();
	QString str1;
	if (check)
		str1 = tr(" Check");
	else
		str1 = tr(" unCheck");

	if (cb == ui->cb_singleBadArea) {
		ui->dsp_singleBadAreaMin->setEnabled(check);
		ui->dsp_singleBadAreaMax->setEnabled(check);
		m_param->single_badArea_valid = check;
	}
	else if (cb == ui->cb_totalBadArea) {
		ui->dsp_totalBadAreaMin->setEnabled(check);
		ui->dsp_totalBadAreaMax->setEnabled(check);
		m_param->total_badArea_valid = check;
	}
	else if (cb == ui->cb_length) {
		ui->dsp_lengthMin->setEnabled(check);
		ui->dsp_lengthMax->setEnabled(check);
		m_param->area_length_valid = check;
	}
	else if (cb == ui->cb_width) {
		ui->dsp_widthMin->setEnabled(check);
		ui->dsp_widthMax->setEnabled(check);
		m_param->area_width_valid = check;
	}
	else if (cb == ui->cb_count) {
		ui->sp_countMin->setEnabled(check);
		ui->sp_countMax->setEnabled(check);
		m_param->area_count_valid = check;
	}
	else if (cb == ui->cb_useRefer) {
		m_param->referSection = check;
	}
	else if (cb == m_advanceDialog->ui->cb_useDelete0) {
		m_param->deleteArea_section[0] = check;
	}
	else if (cb == m_advanceDialog->ui->cb_useDelete1) {
		m_param->deleteArea_section[1] = check;
	}
	else if (cb == m_advanceDialog->ui->cb_useDelete2) {
		m_param->deleteArea_section[2] = check;
	}
	else if (cb == m_advanceDialog->ui->cb_showDel) {
		m_param->isShowDel = check;
	}
	else if (cb == ui->cb_ImgGauss) {
		m_param->IsGauss = check;
		ui->sp_gaussSize->setEnabled(check);
	}
	else if (cb == ui->cb_fillOutRoi) {
		m_param->IsFillOut = check;
	}
	else if (cb == ui->cb_rec) {
		m_param->IsFitRec = check;
	}
	else if (cb == m_advanceDialog->ui->cb_useReferPToDel) {
		m_param->IsReferPtoDel = check;
	}
	else if (cb == ui->cb_dispAfterDet)
	{
		m_param->IsDispROI = check;
	}
	m_module->setParamChanged(QvsParamLevel::ProductLevel);
//	log(LogL1, str0 + str1);
}