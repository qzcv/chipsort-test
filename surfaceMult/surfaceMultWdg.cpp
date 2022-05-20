#include "surfaceMultWdg.h"
#include "surfaceMultMod.h"
#include "surfaceMultParam.h"
#include "ui_surfaceMultWdg.h"
#include "surmultadvancedialog.h"
#include "ui_surmultadvancedialog.h"
#include "ImageView.h"
#include <QMessageBox>
#include "halOpenCV.h"

surfaceMultWdg::surfaceMultWdg(int i) :
	ModSetWidget(),ui(new Ui::surfaceMultWdg),m_widgetType(i)
{
	m_hasConnect = false;
	ui->setupUi(this);
	iniData();
	iniUi();
	m_param = nullptr;
	m_module = nullptr;
}

surfaceMultWdg::~surfaceMultWdg()
{
	delete ui;
}

void surfaceMultWdg::setModule(UnitModule *module)
{
	m_module=dynamic_cast<surfaceMultMod*>(module);
	m_param = m_module->m_param;
	//currentView()->setLayer(m_layerIdx);
	//updateUi();
}

void surfaceMultWdg::showEvent(QShowEvent * event)
{
	updateUi();
}

void surfaceMultWdg::connectSlots(bool link)
{
	if(m_hasConnect==link)
		return;
	m_hasConnect=link;

	connectOrNot(link, ui->cb_dispAfterDet, SIGNAL(toggled(bool)), SLOT(check_box_toggled(bool)));
	connectOrNot(link, ui->cb_useRefer, SIGNAL(toggled(bool)), SLOT(check_box_toggled(bool)));
	connectOrNot(link, ui->cb_usePointRegionRefer, SIGNAL(toggled(bool)), SLOT(check_box_toggled(bool)));
	connectOrNot(link, ui->cb_useAreaRegionRefer, SIGNAL(toggled(bool)), SLOT(check_box_toggled(bool)));
	connectOrNot(link, m_advanceDialog->ui->cb_deleteRefer, SIGNAL(toggled(bool)), SLOT(check_box_toggled(bool)));
	connectOrNot(link, ui->cb_useSameParam, SIGNAL(toggled(bool)), SLOT(check_box_toggled(bool)));
	connectOrNot(link, ui->cb_dirRoi, SIGNAL(toggled(bool)), SLOT(check_box_toggled(bool)));

	connectOrNot(link, m_advanceDialog->ui->cb_useDelete0, SIGNAL(toggled(bool)), SLOT(check_box_toggled(bool)));
	connectOrNot(link, m_advanceDialog->ui->cb_useDelete1, SIGNAL(toggled(bool)), SLOT(check_box_toggled(bool)));
	connectOrNot(link, m_advanceDialog->ui->cb_useDelete2, SIGNAL(toggled(bool)), SLOT(check_box_toggled(bool)));
	connectOrNot(link, m_advanceDialog->ui->cb_showDel, SIGNAL(toggled(bool)), SLOT(check_box_toggled(bool)));
	connectOrNot(link, m_advanceDialog->ui->cb_useReferPToDel, SIGNAL(toggled(bool)), SLOT(check_box_toggled(bool)));
	connectOrNot(link, ui->cb_ImgGauss, SIGNAL(toggled(bool)), SLOT(check_box_toggled(bool)));

	connectOrNot(link, ui->cb_singleBadArea, SIGNAL(toggled(bool)), SLOT(check_box_toggled(bool)));
	connectOrNot(link, ui->cb_length, SIGNAL(toggled(bool)), SLOT(check_box_toggled(bool)));
	connectOrNot(link, ui->cb_totalBadArea, SIGNAL(toggled(bool)), SLOT(check_box_toggled(bool)));
	connectOrNot(link, ui->cb_width, SIGNAL(toggled(bool)), SLOT(check_box_toggled(bool)));
	connectOrNot(link, ui->cb_count, SIGNAL(toggled(bool)), SLOT(check_box_toggled(bool)));

	connectOrNot(link, ui->cb_fillOutRoi, SIGNAL(toggled(bool)), SLOT(check_box_toggled(bool)));

	connectOrNot(link, m_advanceDialog->ui->rb_allDelete, SIGNAL(toggled(bool)), SLOT(radioButton_toggled(bool)));
	connectOrNot(link, m_advanceDialog->ui->rb_dirDelete, SIGNAL(toggled(bool)), SLOT(radioButton_toggled(bool)));
	connectOrNot(link, ui->rb_defect, SIGNAL(toggled(bool)), SLOT(radioButton_toggled(bool)));
	connectOrNot(link, ui->rb_target, SIGNAL(toggled(bool)), SLOT(radioButton_toggled(bool)));

	connectOrNot(link, ui->rb_90, SIGNAL(toggled(bool)), SLOT(radioButton_toggled(bool)));
	connectOrNot(link, ui->rb_180, SIGNAL(toggled(bool)), SLOT(radioButton_toggled(bool)));
	connectOrNot(link, ui->rb_270, SIGNAL(toggled(bool)), SLOT(radioButton_toggled(bool)));
	connectOrNot(link, m_advanceDialog->ui->rb_pixel, SIGNAL(toggled(bool)), SLOT(radioButton_toggled(bool)));
	connectOrNot(link, m_advanceDialog->ui->rb_actual, SIGNAL(toggled(bool)), SLOT(radioButton_toggled(bool)));

	connectOrNot(link, m_advanceDialog->ui->sp_up, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, m_advanceDialog->ui->sp_lf, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, m_advanceDialog->ui->sp_down, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, m_advanceDialog->ui->sp_rt, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_RecRegionNum, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_CirRegionNum, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));

	connectOrNot(link, m_advanceDialog->ui->sp_deleteZoneZoom0, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, m_advanceDialog->ui->sp_deleteZoneZoom1, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, m_advanceDialog->ui->sp_deleteZoneZoom2, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, m_advanceDialog->ui->sp_DeleteRecRegion, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, m_advanceDialog->ui->sp_DeleteCirRegion, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, m_advanceDialog->ui->sp_deletePointZoom, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));

	connectOrNot(link, m_advanceDialog->ui->dsp_pixSize, SIGNAL(valueChanged(double)), SLOT(doubleSpinBox_valueChanged(double)));
	connectOrNot(link, ui->dsp_badAreaMin, SIGNAL(valueChanged(double)), SLOT(doubleSpinBox_valueChanged(double)));
	connectOrNot(link, ui->dsp_badAreaMax, SIGNAL(valueChanged(double)), SLOT(doubleSpinBox_valueChanged(double)));
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
	connectOrNot(link, ui->sp_open, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_close, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));

	connectOrNot(link, ui->sp_holeAreaMin, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_holeAreaMax, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_holeNumMin, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_holeNumMax, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_gaussSize, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_rag, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_loss, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_minRagLossArea, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));

	connectOrNot(link, m_advanceDialog->ui->sp_AreaZoom, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));

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

	connectOrNot(link, ui->cob_type, SIGNAL(currentIndexChanged(int)), SLOT(comboBox_Index_Changed(int)));
	connectOrNot(link, ui->cob_GroupIndex, SIGNAL(currentIndexChanged(int)), SLOT(comboBox_Index_Changed(int)));

	connectOrNot(link, m_advanceDialog->ui->bt_getPixelSize, SIGNAL(clicked()), SLOT(bt_clicked()));
}

void surfaceMultWdg::iniUi()
{

}

void surfaceMultWdg::iniData()
{
	m_advanceDialog = new surMultAdvanceDialog(this);
	m_testing = 0;
	m_layerIdx = 0;
}

void surfaceMultWdg::setUiValue()
{
	if(!m_param)
		return;
	
	ui->cob_type->setCurrentIndex((int)m_param->testType[m_param->currentRoi]);
	ui->stack_type->setCurrentIndex((int)m_param->testType[m_param->currentRoi]);
	ui->tabWidget->setCurrentIndex(0);
	m_advanceDialog->ui->cb_deleteRefer->setChecked(m_param->deleteRefer);
	m_advanceDialog->ui->cb_deleteRefer->setEnabled((m_param->pointRegionRefer || m_param->areaRegionRefer));
	ui->cb_useRefer->setChecked(m_param->referSection);
	ui->cb_usePointRegionRefer->setChecked(m_param->pointRegionRefer);
	ui->cb_useAreaRegionRefer->setChecked(m_param->areaRegionRefer);
	ui->cb_useSameParam->setChecked(m_param->IsSameParam);
	m_advanceDialog->ui->cb_useDelete0->setChecked(m_param->deleteArea_section[0] == 1);
	m_advanceDialog->ui->cb_useDelete1->setChecked(m_param->deleteArea_section[1] == 1);
	m_advanceDialog->ui->cb_useDelete2->setChecked(m_param->deleteArea_section[2] == 1);
	m_advanceDialog->ui->cb_showDel->setChecked(m_param->isShowDel);
	ui->cb_dirRoi->setChecked(m_param->IsDirPattern);
	if (m_param->IsDirPattern) {
		ui->grp_dir->show();
	}
	else {
		ui->grp_dir->hide();
	}
	m_advanceDialog->ui->cb_useReferPToDel->setChecked(m_param->IsReferPtoDel);
	m_advanceDialog->ui->sp_deletePointZoom->setEnabled(m_param->IsReferPtoDel);
	m_advanceDialog->ui->sp_deletePointZoom->setValue(m_param->delPointZoom);
	if (m_param->IsDirDelete == 0) {
		m_advanceDialog->ui->rb_dirDelete->setChecked(true);
	}
	else {
		m_advanceDialog->ui->rb_allDelete->setChecked(true);
	}
	if (m_param->DetectModel == 0) {
		ui->rb_defect->setChecked(true);
		changeModel(0);
	}
	else {
		ui->rb_target->setChecked(true);
		changeModel(1);
	}
	if (m_param->IsPixOrAct == 0) {
		m_advanceDialog->ui->rb_pixel->setChecked(true);
		changeSpinBox(0);
	}
	else if (m_param->IsPixOrAct == 1) {
		m_advanceDialog->ui->rb_actual->setChecked(true);
		changeSpinBox(1);
	}
	m_advanceDialog->ui->dsp_pixSize->setValue(m_param->pixSize);

	if (m_param->multAngle == 0) {
		ui->rb_90->setChecked(1);
	}
	else if (m_param->multAngle == 1) {
		ui->rb_180->setChecked(1);
	}
	else if (m_param->multAngle == 2) {
		ui->rb_270->setChecked(1);
	}
	int idx = m_param->currentRoi;
	ui->cb_fillOutRoi->setChecked(m_param->IsFillOut[idx]);

	ui->cb_ImgGauss->setChecked(m_param->IsGauss[idx]);

	ui->sp_rag->setValue(m_param->minRag[idx]);
	ui->sp_loss->setValue(m_param->minLoss[idx]);
	ui->sp_minRagLossArea->setValue(m_param->minRagLossArea[idx]);

	ui->sp_RecRegionNum->setValue(m_param->RecRoiNum);
	ui->sp_CirRegionNum->setValue(m_param->CirRoiNum);
	m_advanceDialog->ui->sp_DeleteRecRegion->setValue(m_param->RecDeleteNum);
	m_advanceDialog->ui->sp_DeleteCirRegion->setValue(m_param->CirDeleteNum);

	ui->sp_gaussSize->setEnabled(m_param->IsGauss[idx]);
	ui->sp_gaussSize->setValue(m_param->GaussSize[idx]);

	m_advanceDialog->ui->sp_up->setValue(m_param->zoom_up);
	m_advanceDialog->ui->sp_down->setValue(m_param->zoom_down);
	m_advanceDialog->ui->sp_lf->setValue(m_param->zoom_lf);
	m_advanceDialog->ui->sp_rt->setValue(m_param->zoom_rt);

	m_advanceDialog->ui->sp_AreaZoom->setValue(m_param->areaZoom);

	ui->dsp_badLengthMin->setValue(m_param->badLengthMin[idx]);
	ui->dsp_badLengthMax->setValue(m_param->badLengthMax[idx]);
	ui->dsp_badWidthMax->setValue(m_param->badWidthMax[idx]);
	ui->dsp_badWidthMin->setValue(m_param->badWidthMin[idx]);
	ui->sp_fixThreMin->setValue(m_param->fixThreMin[idx]);
	ui->sp_fixThreMax->setValue(m_param->fixThreMax[idx]);
	ui->sp_fixThreMin_2->setValue(m_param->fixThreMin[idx]);
	ui->sp_fixThreMax_2->setValue(m_param->fixThreMax[idx]);
	ui->sp_fixThreMin_3->setValue(m_param->fixThreMin[idx]);
	ui->sp_fixThreMax_3->setValue(m_param->fixThreMax[idx]);

	ui->sp_meanSize->setValue(m_param->meanSize[idx]);
	m_advanceDialog->ui->sp_deleteZoneZoom0->setValue(m_param->deleteArea_zoom[0]);
	m_advanceDialog->ui->sp_deleteZoneZoom1->setValue(m_param->deleteArea_zoom[1]);
	m_advanceDialog->ui->sp_deleteZoneZoom2->setValue(m_param->deleteArea_zoom[2]);
	m_advanceDialog->ui->sp_DeleteRecRegion->setValue(m_param->RecDeleteNum);
	m_advanceDialog->ui->sp_DeleteCirRegion->setValue(m_param->CirDeleteNum);

	//以及一些相对应的显1示和AOI上的切换,有多少的区域就添加多1少个框子
	ui->dsp_badAreaMin->setValue(m_param->badArea_min[idx]);
	ui->dsp_badAreaMax->setValue(m_param->badArea_max[idx]);
	ui->sp_blackBad->setValue(m_param->black_bad_detect[idx]);
	ui->sp_whiteBad->setValue(m_param->white_bad_detect[idx]);
	ui->sp_blackBad_2->setValue(m_param->black_bad_detect[idx]);
	ui->sp_whiteBad_2->setValue(m_param->white_bad_detect[idx]);

	ui->sp_holeAreaMin->setValue(m_param->holeAreaMin[idx]);
	ui->sp_holeAreaMax->setValue(m_param->holeAreaMax[idx]);
	ui->sp_holeNumMin->setValue(m_param->holeNumMin[idx]);
	ui->sp_holeNumMax->setValue(m_param->holeNumMax[idx]);

	ui->sp_open->setValue(m_param->openVal[idx]);
	ui->sp_close->setValue(m_param->closeVal[idx]);

	ui->cb_singleBadArea->setChecked(m_param->single_badArea_valid[idx]);
	ui->dsp_singleBadAreaMin->setEnabled(m_param->single_badArea_valid[idx]);
	ui->dsp_singleBadAreaMax->setEnabled(m_param->single_badArea_valid[idx]);
	ui->dsp_singleBadAreaMin->setValue(m_param->single_badArea_min[idx]);
	ui->dsp_singleBadAreaMax->setValue(m_param->single_badArea_max[idx]);

	ui->cb_totalBadArea->setChecked(m_param->total_badArea_valid[idx]);
	ui->dsp_totalBadAreaMin->setEnabled(m_param->total_badArea_valid[idx]);
	ui->dsp_totalBadAreaMax->setEnabled(m_param->total_badArea_valid[idx]);
	ui->dsp_totalBadAreaMin->setValue(m_param->total_badArea_min[idx]);
	ui->dsp_totalBadAreaMax->setValue(m_param->total_badArea_max[idx]);

	ui->cb_length->setChecked(m_param->area_length_valid[idx]);
	ui->dsp_lengthMin->setEnabled(m_param->area_length_valid[idx]);
	ui->dsp_lengthMax->setEnabled(m_param->area_length_valid[idx]);
	ui->dsp_lengthMin->setValue(m_param->area_length_min[idx]);
	ui->dsp_lengthMax->setValue(m_param->area_length_max[idx]);

	ui->cb_width->setChecked(m_param->area_width_valid[idx]);
	ui->dsp_widthMin->setEnabled(m_param->area_width_valid[idx]);
	ui->dsp_widthMax->setEnabled(m_param->area_width_valid[idx]);
	ui->dsp_widthMin->setValue(m_param->area_width_min[idx]);
	ui->dsp_widthMax->setValue(m_param->area_width_max[idx]);

	ui->cb_count->setChecked(m_param->area_count_valid[idx]);
	ui->sp_countMin->setEnabled(m_param->area_count_valid[idx]);
	ui->sp_countMax->setEnabled(m_param->area_count_valid[idx]);
	ui->sp_countMin->setValue(m_param->area_count_min[idx]);
	ui->sp_countMax->setValue(m_param->area_count_max[idx]);
	ui->cob_GroupIndex->clear();
	for (int i = 0;i < m_param->RecRoiNum + m_param->CirRoiNum;i++) {
		if (i < m_param->RecRoiNum) {
			ui->cob_GroupIndex->addItem(tr("Rect ROI%1").arg(i + 1));
		}
		else if (i >= m_param->RecRoiNum) {
			ui->cob_GroupIndex->addItem(tr("Cir ROI%1").arg(i - m_param->RecRoiNum + 1));
		}
	}
	ui->cob_GroupIndex->setCurrentIndex(m_param->currentRoi);
	setCurrentGrp(m_param->currentRoi);
	ui->tabWidget->setCurrentIndex(0);

	ui->cb_dispAfterDet->setVisible(ui->cb_useRefer->isChecked());
	ui->cb_dispAfterDet->setChecked(m_param->IsDispROI);
}

void surfaceMultWdg::updateUi()
{
	connectSlots(false);
	setUiValue();
	connectSlots(true);
}

void surfaceMultWdg::changeModel(bool isTarget)
{
	if (!isTarget) {  //缺陷模式
		ui->tabWidget->setTabText(0, tr("Defect definition"));
		ui->lb_blackBad->setText(tr("Black defect"));
		ui->lb_whiteBad->setText(tr("White defect"));
		ui->lb_blackBad_2->setText(tr("Black defect"));
		ui->lb_whiteBad_2->setText(tr("White defect"));
		ui->cb_singleBadArea->setText(tr("single defect area"));
		ui->cb_totalBadArea->setText(tr("total defect area"));
		ui->groupBox_5->setTitle(tr("defect range"));
	}
	else {            //目标模式
		ui->tabWidget->setTabText(0, tr("Target definition"));
		ui->lb_blackBad->setText(tr("Black target"));
		ui->lb_whiteBad->setText(tr("White target"));
		ui->lb_blackBad_2->setText(tr("Black target"));
		ui->lb_whiteBad_2->setText(tr("White target"));
		ui->cb_singleBadArea->setText(tr("single target area"));
		ui->cb_totalBadArea->setText(tr("total target area"));
		ui->groupBox_5->setTitle(tr("target range"));
	}
}

void surfaceMultWdg::changeSpinBox(bool isPixOrAct)
{
	int idx = m_param->currentRoi;
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

		ui->dsp_badAreaMin->setValue(m_param->badArea_min[idx] / (m_param->pixSize*m_param->pixSize));
		ui->dsp_badAreaMax->setValue(m_param->badArea_max[idx] / (m_param->pixSize*m_param->pixSize));
		ui->dsp_badLengthMin->setValue(m_param->badLengthMin[idx] / m_param->pixSize);
		ui->dsp_badLengthMax->setValue(m_param->badLengthMax[idx] / m_param->pixSize);
		ui->dsp_badWidthMin->setValue(m_param->badWidthMin[idx] / m_param->pixSize);
		ui->dsp_badWidthMax->setValue(m_param->badWidthMax[idx] / m_param->pixSize);
		ui->dsp_singleBadAreaMin->setValue(m_param->single_badArea_min[idx] / (m_param->pixSize*m_param->pixSize));
		ui->dsp_singleBadAreaMax->setValue(m_param->single_badArea_max[idx] / (m_param->pixSize*m_param->pixSize));
		ui->dsp_totalBadAreaMin->setValue(m_param->total_badArea_min[idx] / (m_param->pixSize*m_param->pixSize));
		ui->dsp_totalBadAreaMax->setValue(m_param->total_badArea_max[idx] / (m_param->pixSize*m_param->pixSize));
		ui->dsp_lengthMin->setValue(m_param->area_length_min[idx] / m_param->pixSize);
		ui->dsp_lengthMax->setValue(m_param->area_length_max[idx] / m_param->pixSize);
		ui->dsp_widthMin->setValue(m_param->area_width_min[idx] / m_param->pixSize);
		ui->dsp_widthMax->setValue(m_param->area_width_max[idx] / m_param->pixSize);
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

		ui->dsp_badAreaMin->setValue(m_param->badArea_min[idx]);
		ui->dsp_badAreaMax->setValue(m_param->badArea_max[idx]);
		ui->dsp_badLengthMin->setValue(m_param->badLengthMin[idx]);
		ui->dsp_badLengthMax->setValue(m_param->badLengthMax[idx]);
		ui->dsp_badWidthMin->setValue(m_param->badWidthMin[idx]);
		ui->dsp_badWidthMax->setValue(m_param->badWidthMax[idx]);
		ui->dsp_singleBadAreaMin->setValue(m_param->single_badArea_min[idx]);
		ui->dsp_singleBadAreaMax->setValue(m_param->single_badArea_max[idx]);
		ui->dsp_totalBadAreaMin->setValue(m_param->total_badArea_min[idx]);
		ui->dsp_totalBadAreaMax->setValue(m_param->total_badArea_max[idx]);
		ui->dsp_lengthMin->setValue(m_param->area_length_min[idx]);
		ui->dsp_lengthMax->setValue(m_param->area_length_max[idx]);
		ui->dsp_widthMin->setValue(m_param->area_width_min[idx]);
		ui->dsp_widthMax->setValue(m_param->area_width_max[idx]);
	}
}

void surfaceMultWdg::setCurrentGrp(int idx)
{
	connectSlots(false);
	ui->cob_type->setCurrentIndex((int)m_param->testType[idx]);
	ui->stack_type->setCurrentIndex((int)m_param->testType[idx]);
	ui->cb_fillOutRoi->setChecked(m_param->IsFillOut[idx]);
	ui->cb_ImgGauss->setChecked(m_param->IsGauss[idx]);

	ui->sp_gaussSize->setEnabled(m_param->IsGauss[idx]);
	ui->sp_gaussSize->setValue(m_param->GaussSize[idx]);
	//ui.dsp_badLengthMin->setValue(m_param->badLengthMin[idx]);
	//ui.dsp_badLengthMax->setValue(m_param->badLengthMax[idx]);
	//ui.dsp_badWidthMax->setValue(m_param->badWidthMax[idx]);
	//ui.dsp_badWidthMin->setValue(m_param->badWidthMin[idx]);
	ui->sp_fixThreMin->setValue(m_param->fixThreMin[idx]);
	ui->sp_fixThreMax->setValue(m_param->fixThreMax[idx]);
	ui->sp_meanSize->setValue(m_param->meanSize[idx]);

	//以及一些相对应的显1示和AOI上的切换,有多少的区域就添加多1少个框子
	//ui.dsp_badAreaMin->setValue(m_param->badArea_min[idx]);
	//ui.dsp_badAreaMax->setValue(m_param->badArea_max[idx]);
	ui->sp_blackBad->setValue(m_param->black_bad_detect[idx]);
	ui->sp_whiteBad->setValue(m_param->white_bad_detect[idx]);

	ui->sp_blackBad_2->setValue(m_param->black_bad_detect[idx]);
	ui->sp_whiteBad_2->setValue(m_param->white_bad_detect[idx]);

	ui->sp_holeAreaMin->setValue(m_param->holeAreaMin[idx]);
	ui->sp_holeAreaMax->setValue(m_param->holeAreaMax[idx]);
	ui->sp_holeNumMin->setValue(m_param->holeNumMin[idx]);
	ui->sp_holeNumMax->setValue(m_param->holeNumMax[idx]);

	ui->cb_singleBadArea->setChecked(m_param->single_badArea_valid[idx]);
	ui->dsp_singleBadAreaMin->setEnabled(m_param->single_badArea_valid[idx]);
	ui->dsp_singleBadAreaMax->setEnabled(m_param->single_badArea_valid[idx]);
	//ui.dsp_singleBadAreaMin->setValue(m_param->single_badArea_min[idx]);
	//ui.dsp_singleBadAreaMax->setValue(m_param->single_badArea_max[idx]);

	ui->cb_totalBadArea->setChecked(m_param->total_badArea_valid[idx]);
	ui->dsp_totalBadAreaMin->setEnabled(m_param->total_badArea_valid[idx]);
	ui->dsp_totalBadAreaMax->setEnabled(m_param->total_badArea_valid[idx]);
	//ui.dsp_totalBadAreaMin->setValue(m_param->total_badArea_min[idx]);
	//ui.dsp_totalBadAreaMax->setValue(m_param->total_badArea_max[idx]);

	ui->cb_length->setChecked(m_param->area_length_valid[idx]);
	ui->dsp_lengthMin->setEnabled(m_param->area_length_valid[idx]);
	ui->dsp_lengthMax->setEnabled(m_param->area_length_valid[idx]);
	//ui.dsp_lengthMin->setValue(m_param->area_length_min[idx]);
	//ui.dsp_lengthMax->setValue(m_param->area_length_max[idx]);

	ui->cb_width->setChecked(m_param->area_width_valid[idx]);
	ui->dsp_widthMin->setEnabled(m_param->area_width_valid[idx]);
	ui->dsp_widthMax->setEnabled(m_param->area_width_valid[idx]);
	//ui.dsp_widthMin->setValue(m_param->area_width_min[idx]);
	//ui.dsp_widthMax->setValue(m_param->area_width_max[idx]);

	ui->cb_count->setChecked(m_param->area_count_valid[idx]);
	ui->sp_countMin->setEnabled(m_param->area_count_valid[idx]);
	ui->sp_countMax->setEnabled(m_param->area_count_valid[idx]);
	ui->sp_countMin->setValue(m_param->area_count_min[idx]);
	ui->sp_countMax->setValue(m_param->area_count_max[idx]);
	ui->sp_open->setValue(m_param->openVal[idx]);
	ui->sp_close->setValue(m_param->closeVal[idx]);
	changeSpinBox(m_param->IsPixOrAct);
	connectSlots(true);
}

void surfaceMultWdg::testParam()
{
	Hobject dispReg;
	m_module->calculate(&dispReg);
	RlRegion rlReg;
	HalOpenCV::hal2cvRegion(dispReg, &rlReg);

	//currentView()->clear();
	currentView()->clearLayer(m_layerIdx);
	currentView()->setColor(Qt::green);
	currentView()->setFilled(true);
	currentView()->dispRegion(&rlReg);
	currentView()->update();
	currentView()->setFilled(false);
}

void surfaceMultWdg::check_box_toggled(bool check)
{
	QCheckBox* cb = qobject_cast<QCheckBox*>(sender());
	QString str0 = QString("");
	QString str1 = cb->text();
	QString str2;
	if (check)
		str2 = tr(" Check");
	else
		str2 = tr(" unCheck");

	int idx = m_param->currentRoi;
	if (cb == ui->cb_singleBadArea) {
		str0 = tr("ROI%1 ").arg(QString::number(idx));
		ui->dsp_singleBadAreaMin->setEnabled(check);
		ui->dsp_singleBadAreaMax->setEnabled(check);
		m_param->single_badArea_valid[idx] = check;
	}
	else if (cb == ui->cb_totalBadArea) {
		str0 = tr("ROI%1 ").arg(QString::number(idx));
		ui->dsp_totalBadAreaMin->setEnabled(check);
		ui->dsp_totalBadAreaMax->setEnabled(check);
		m_param->total_badArea_valid[idx] = check;
	}
	else if (cb == ui->cb_fillOutRoi) {
		str0 = tr("ROI%1 ").arg(QString::number(idx));
		m_param->IsFillOut[idx] = check;
	}
	else if (cb == ui->cb_length) {
		str0 = tr("ROI%1 ").arg(QString::number(idx));
		ui->dsp_lengthMin->setEnabled(check);
		ui->dsp_lengthMax->setEnabled(check);
		m_param->area_length_valid[idx] = check;
	}
	else if (cb == ui->cb_width) {
		str0 = tr("ROI%1 ").arg(QString::number(idx));
		ui->dsp_widthMin->setEnabled(check);
		ui->dsp_widthMax->setEnabled(check);
		m_param->area_width_valid[idx] = check;
	}
	else if (cb == ui->cb_count) {
		str0 = tr("ROI%1 ").arg(QString::number(idx));
		ui->sp_countMin->setEnabled(check);
		ui->sp_countMax->setEnabled(check);
		m_param->area_count_valid[idx] = check;
	}
	else if (cb == ui->cb_useRefer) {
		m_param->referSection = check;
		if (ui->cb_usePointRegionRefer->isChecked() && check) {
			ui->cb_usePointRegionRefer->setChecked(false);
		}
		if (ui->cb_useAreaRegionRefer->isChecked() && check) {
			ui->cb_useAreaRegionRefer->setChecked(false);
		}
		if (check) {
			m_advanceDialog->ui->cb_deleteRefer->setEnabled(!check);
			m_advanceDialog->ui->cb_deleteRefer->setChecked(!check);
		}
		ui->cb_dispAfterDet->setVisible(check);
	}
	else if (cb == ui->cb_usePointRegionRefer) {
		m_param->pointRegionRefer = check;
		if (ui->cb_useRefer->isChecked() && check) {
			ui->cb_useRefer->setChecked(false);
		}
		if (ui->cb_useAreaRegionRefer->isChecked() && check) {
			ui->cb_useAreaRegionRefer->setChecked(false);
		}
		if (check) {
			m_advanceDialog->ui->cb_deleteRefer->setEnabled(check);
		}
	}
	else if (cb == ui->cb_useAreaRegionRefer) {
		m_param->areaRegionRefer = check;
		if (ui->cb_useRefer->isChecked() && check) {
			ui->cb_useRefer->setChecked(false);
		}
		if (ui->cb_usePointRegionRefer->isChecked() && check) {
			ui->cb_usePointRegionRefer->setChecked(false);
		}
		if (check) {
			m_advanceDialog->ui->cb_deleteRefer->setEnabled(check);
		}
	}
	else if (cb == m_advanceDialog->ui->cb_deleteRefer) {
		m_param->deleteRefer = check;
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
		str0 = tr("ROI%1 ").arg(QString::number(idx));
		m_param->IsGauss[idx] = check;
		ui->sp_gaussSize->setEnabled(check);
	}
	else if (cb == ui->cb_useSameParam) {
		m_param->IsSameParam = check;
	}
	else if (cb == ui->cb_dirRoi) {
		m_param->IsDirPattern = check;
		if (check) {
			ui->grp_dir->show();
		}
		else {
			ui->grp_dir->hide();
		}
	}
	else if (cb == m_advanceDialog->ui->cb_useReferPToDel) {
		m_param->IsReferPtoDel = check;
		m_advanceDialog->ui->sp_deletePointZoom->setEnabled(check);
	}
	else if (cb == ui->cb_dispAfterDet)
	{
		m_param->IsDispROI = check;
	}
	m_module->setParamChanged(QvsParamLevel::ProductLevel);
	//log(LogL1, str0 + str1 + str2);
}

void surfaceMultWdg::spinBox_valueChanged(int val)
{
	QSpinBox* sp = qobject_cast<QSpinBox*>(sender());
	QString str0 = QString("");
	QString str1 = QString("");
	QString str2 = QString("");
	int idx = m_param->currentRoi;
	if (sp == m_advanceDialog->ui->sp_up) {
		str1 = m_advanceDialog->ui->label_12->text() + m_advanceDialog->ui->label_2->text();
		str2 = tr(" change from %1 to %2").arg(QString::number(m_param->zoom_up)).arg(QString::number(val));
		m_param->zoom_up = val;
	}
	else if (sp == m_advanceDialog->ui->sp_lf) {
		str1 = m_advanceDialog->ui->label_12->text() + m_advanceDialog->ui->label_7->text();
		str2 = tr(" change from %1 to %2").arg(QString::number(m_param->zoom_lf)).arg(QString::number(val));
		m_param->zoom_lf = val;
	}
	else if (sp == m_advanceDialog->ui->sp_down) {
		str1 = m_advanceDialog->ui->label_12->text() + m_advanceDialog->ui->label_5->text();
		str2 = tr(" change from %1 to %2").arg(QString::number(m_param->zoom_down)).arg(QString::number(val));
		m_param->zoom_down = val;
	}
	else if (sp == m_advanceDialog->ui->sp_rt) {
		str1 = m_advanceDialog->ui->label_12->text() + m_advanceDialog->ui->label_8->text();
		str2 = tr(" change from %1 to %2").arg(QString::number(m_param->zoom_rt)).arg(QString::number(val));
		m_param->zoom_rt = val;
	}
	else if (sp == ui->sp_fixThreMin) {
		str0 = tr("ROI%1 ").arg(QString::number(idx));
		str1 = ui->label_9->text() + tr(" min val ");
		str2 = tr(" change from %1 to %2").arg(QString::number(m_param->fixThreMin[idx])).arg(QString::number(val));
		m_param->fixThreMin[idx] = val;
		//m_loadingProject = true;
		connectSlots(false);
		ui->sp_fixThreMin_2->setValue(val);
		ui->sp_fixThreMin_3->setValue(val);
		connectSlots(true);
		//m_loadingProject = false;
		if (m_testing) {
			testParam();
		}
	}
	else if (sp == ui->sp_fixThreMax) {
		str0 = tr("ROI%1 ").arg(QString::number(idx));
		str1 = ui->label_9->text() + tr(" max val ");
		str2 = tr(" change from %1 to %2").arg(QString::number(m_param->fixThreMax[idx])).arg(QString::number(val));
		m_param->fixThreMax[idx] = val;
		//m_loadingProject = true;
		connectSlots(false);
		ui->sp_fixThreMax_2->setValue(val);
		ui->sp_fixThreMax_3->setValue(val);
		connectSlots(true);
		//m_loadingProject = false;
		if (m_testing) {
			testParam();
		}
	}
	else if (sp == ui->sp_fixThreMin_2) {
		str0 = tr("ROI%1 ").arg(QString::number(idx));
		str1 = ui->label_11->text() + tr(" min val ");
		str2 = tr(" change from %1 to %2").arg(QString::number(m_param->fixThreMin[idx])).arg(QString::number(val));
		m_param->fixThreMin[idx] = val;
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
		str0 = tr("ROI%1 ").arg(QString::number(idx));
		str1 = ui->label_11->text() + tr(" max val ");
		str2 = tr(" change from %1 to %2").arg(QString::number(m_param->fixThreMax[idx])).arg(QString::number(val));
		m_param->fixThreMax[idx] = val;
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
		str0 = tr("ROI%1 ").arg(QString::number(idx));
		str1 = ui->label_13->text() + tr(" min val ");
		str2 = tr(" change from %1 to %2").arg(QString::number(m_param->fixThreMin[idx])).arg(QString::number(val));
		m_param->fixThreMin[idx] = val;
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
		str0 = tr("ROI%1 ").arg(QString::number(idx));
		str1 = ui->label_13->text() + tr(" max val ");
		str2 = tr(" change from %1 to %2").arg(QString::number(m_param->fixThreMax[idx])).arg(QString::number(val));
		m_param->fixThreMax[idx] = val;
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
		str0 = tr("ROI%1 ").arg(QString::number(idx));
		str1 = ui->label_10->text();
		str2 = tr(" change from %1 to %2").arg(QString::number(m_param->meanSize[idx])).arg(QString::number(val));
		m_param->meanSize[idx] = val;
		if (m_testing) {
			testParam();
		}
	}
	else if (sp == ui->sp_gaussSize) {
		str0 = tr("ROI%1 ").arg(QString::number(idx));
		str1 = ui->cb_ImgGauss->text();
		str2 = tr(" change from %1 to %2").arg(QString::number(m_param->GaussSize[idx])).arg(QString::number(val));
		m_param->GaussSize[idx] = val;
		if (m_testing) {
			testParam();
		}
	}
	else if (sp == ui->sp_open) {
		str0 = tr("ROI%1 ").arg(QString::number(idx));
		str1 = ui->label_4->text();
		str2 = tr(" change from %1 to %2").arg(QString::number(m_param->openVal[idx])).arg(QString::number(val));
		m_param->openVal[idx] = val;
	}
	else if (sp == ui->sp_close) {
		str0 = tr("ROI%1 ").arg(QString::number(idx));
		str1 = ui->label_6->text();
		str2 = tr(" change from %1 to %2").arg(QString::number(m_param->closeVal[idx])).arg(QString::number(val));
		m_param->closeVal[idx] = val;
		if (m_testing) {
			testParam();
		}
	}
	else if (sp == ui->sp_blackBad) {
		str0 = tr("ROI%1 ").arg(QString::number(idx));
		str1 = ui->lb_blackBad->text();
		str2 = tr(" change from %1 to %2").arg(QString::number(m_param->black_bad_detect[idx])).arg(QString::number(val));
		m_param->black_bad_detect[idx] = val;
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
		str0 = tr("ROI%1 ").arg(QString::number(idx));
		str1 = ui->lb_whiteBad->text();
		str2 = tr(" change from %1 to %2").arg(QString::number(m_param->white_bad_detect[idx])).arg(QString::number(val));
		m_param->white_bad_detect[idx] = val;
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
		str0 = tr("ROI%1 ").arg(QString::number(idx));
		str1 = ui->lb_blackBad_2->text();
		str2 = tr(" change from %1 to %2").arg(QString::number(m_param->black_bad_detect[idx])).arg(QString::number(val));
		m_param->black_bad_detect[idx] = val;
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
		str0 = tr("ROI%1 ").arg(QString::number(idx));
		str1 = ui->lb_whiteBad_2->text();
		str2 = tr(" change from %1 to %2").arg(QString::number(m_param->white_bad_detect[idx])).arg(QString::number(val));
		m_param->white_bad_detect[idx] = val;
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
		str0 = tr("ROI%1 ").arg(QString::number(idx));
		str1 = ui->groupBox_5->title() + ui->cb_count->text() + tr(" min val ");
		str2 = tr(" change from %1 to %2").arg(QString::number(m_param->area_count_min[idx])).arg(QString::number(val));
		m_param->area_count_min[idx] = val;
	}
	else if (sp == ui->sp_countMax) {
		str0 = tr("ROI%1 ").arg(QString::number(idx));
		str1 = ui->groupBox_5->title() + ui->cb_count->text() + tr(" max val ");
		str2 = tr(" change from %1 to %2").arg(QString::number(m_param->area_count_max[idx])).arg(QString::number(val));
		m_param->area_count_max[idx] = val;
	}
	else if (sp == m_advanceDialog->ui->sp_deleteZoneZoom0) {
		str1 = m_advanceDialog->ui->label_14->text() + m_advanceDialog->ui->cb_useDelete0->text() + m_advanceDialog->ui->label_15->text();
		str2 = tr(" change from %1 to %2").arg(QString::number(m_param->deleteArea_zoom[0])).arg(QString::number(val));
		m_param->deleteArea_zoom[0] = val;
	}
	else if (sp == m_advanceDialog->ui->sp_deleteZoneZoom1) {
		str1 = m_advanceDialog->ui->label_14->text() + m_advanceDialog->ui->cb_useDelete1->text() + m_advanceDialog->ui->label_15->text();
		str2 = tr(" change from %1 to %2").arg(QString::number(m_param->deleteArea_zoom[1])).arg(QString::number(val));
		m_param->deleteArea_zoom[1] = val;
	}
	else if (sp == m_advanceDialog->ui->sp_deleteZoneZoom2) {
		str1 = m_advanceDialog->ui->label_14->text() + m_advanceDialog->ui->cb_useDelete2->text() + m_advanceDialog->ui->label_15->text();
		str2 = tr(" change from %1 to %2").arg(QString::number(m_param->deleteArea_zoom[2])).arg(QString::number(val));
		m_param->deleteArea_zoom[2] = val;
	}
	else if (sp == m_advanceDialog->ui->sp_DeleteRecRegion) {
		str1 = m_advanceDialog->ui->label_16->text();
		str2 = tr(" change from %1 to %2").arg(QString::number(m_param->RecDeleteNum)).arg(QString::number(val));
		if (val > MAX_Delete_Rec_Num) {
			m_advanceDialog->ui->sp_DeleteRecRegion->setValue(MAX_Delete_Rec_Num);
			return;
		}
		m_param->RecDeleteNum = val;

	}
	else if (sp == m_advanceDialog->ui->sp_DeleteCirRegion) {
		str1 = m_advanceDialog->ui->label_25->text();
		str2 = tr(" change from %1 to %2").arg(QString::number(m_param->CirDeleteNum)).arg(QString::number(val));
		if (val > MAX_Delete_Cir_Num) {
			m_advanceDialog->ui->sp_DeleteCirRegion->setValue(MAX_Delete_Cir_Num);
		}
		m_param->CirDeleteNum = val;
	}
	else if (sp == m_advanceDialog->ui->sp_AreaZoom) {
		str1 = m_advanceDialog->ui->label_3->text();
		str2 = tr(" change from %1 to %2").arg(QString::number(m_param->areaZoom)).arg(QString::number(val));
		m_param->areaZoom = val;
	}
	else  if (sp == ui->sp_holeAreaMax) {
		str0 = tr("ROI%1 ").arg(QString::number(idx));
		str1 = ui->label_24->text() + tr(" max val ");
		str2 = tr(" change from %1 to %2").arg(QString::number(m_param->holeAreaMax[idx])).arg(QString::number(val));
		m_param->holeAreaMax[idx] = val;
		if (m_testing) {
			testParam();
		}
	}
	else  if (sp == ui->sp_holeAreaMin) {
		str0 = tr("ROI%1 ").arg(QString::number(idx));
		str1 = ui->label_24->text() + tr(" min val ");
		str2 = tr(" change from %1 to %2").arg(QString::number(m_param->holeAreaMin[idx])).arg(QString::number(val));
		m_param->holeAreaMin[idx] = val;
		if (m_testing) {
			testParam();
		}
	}
	else  if (sp == ui->sp_holeNumMax) {
		str0 = tr("ROI%1 ").arg(QString::number(idx));
		str1 = ui->label_31->text() + tr(" max val ");
		str2 = tr(" change from %1 to %2").arg(QString::number(m_param->holeNumMax[idx])).arg(QString::number(val));
		m_param->holeNumMax[idx] = val;
		if (m_testing) {
			testParam();
		}
	}
	else  if (sp == ui->sp_holeNumMin) {
		str0 = tr("ROI%1 ").arg(QString::number(idx));
		str1 = ui->label_31->text() + tr(" min val ");
		str2 = tr(" change from %1 to %2").arg(QString::number(m_param->holeNumMin[idx])).arg(QString::number(val));
		m_param->holeNumMin[idx] = val;
		if (m_testing) {
			testParam();
		}
	}
	else if (sp == ui->sp_rag) {
		str0 = tr("ROI%1 ").arg(QString::number(idx));
		str1 = ui->label_32->text();
		str2 = tr(" change from %1 to %2").arg(QString::number(m_param->minRag[idx])).arg(QString::number(val));
		m_param->minRag[idx] = val;
		if (m_testing) {
			testParam();
		}
	}
	else if (sp == ui->sp_loss) {
		str0 = tr("ROI%1 ").arg(QString::number(idx));
		str1 = ui->label_33->text();
		str2 = tr(" change from %1 to %2").arg(QString::number(m_param->minLoss[idx])).arg(QString::number(val));
		m_param->minLoss[idx] = val;
		if (m_testing) {
			testParam();
		}
	}
	else if (sp == ui->sp_minRagLossArea) {
		str0 = tr("ROI%1 ").arg(QString::number(idx));
		str1 = ui->label_34->text();
		str2 = tr(" change from %1 to %2").arg(QString::number(m_param->minRagLossArea[idx])).arg(QString::number(val));
		m_param->minRagLossArea[idx] = val;
		if (m_testing) {
			testParam();
		}
	}
	else if (sp == m_advanceDialog->ui->sp_deletePointZoom) {
		str1 = m_advanceDialog->ui->cb_useReferPToDel->text() + m_advanceDialog->ui->label_18->text();
		str2 = tr(" change from %1 to %2").arg(QString::number(m_param->delPointZoom)).arg(QString::number(val));
		m_param->delPointZoom = val;
	}
	else  if (sp == ui->sp_RecRegionNum) {
		str1 = ui->label_26->text();
		str2 = tr(" change from %1 to %2").arg(QString::number(m_param->RecRoiNum)).arg(QString::number(val));
		if (m_param->CirRoiNum == 0 && val == 0) {
			//disconnect(ui->sp_RecRegionNum, SIGNAL(valueChanged(int)), this, SLOT(spinBox_valueChanged(int)));
			connectSlots(false);
			ui->sp_RecRegionNum->setValue(m_param->RecRoiNum);
			connectSlots(true);
			//connect(ui.sp_RecRegionNum, SIGNAL(valueChanged(int)), this, SLOT(spinBox_valueChanged(int)));
			return;
		}
		if (val > MAX_Rec_Num) {
			ui->sp_RecRegionNum->setValue(MAX_Rec_Num);
			return;
		}
		m_param->RecRoiNum = val;
		//m_loadingProject = true;
		connectSlots(false);
		ui->cob_GroupIndex->clear();
		for (int i = 0;i < m_param->RecRoiNum + m_param->CirRoiNum;i++) {
			if (i < m_param->RecRoiNum) {
				ui->cob_GroupIndex->addItem(tr("Rect ROI%1").arg(i + 1));
			}
			else if (i >= m_param->RecRoiNum) {
				ui->cob_GroupIndex->addItem(tr("Cir ROI%1").arg(i - m_param->RecRoiNum + 1));
			}
		}
		connectSlots(true);
		//m_loadingProject = false;
		ui->cob_GroupIndex->setCurrentIndex(0);
	}
	else  if (sp == ui->sp_CirRegionNum) {
		str1 = ui->label_27->text();
		str2 = tr(" change from %1 to %2").arg(QString::number(m_param->CirRoiNum)).arg(QString::number(val));
		if (m_param->RecRoiNum == 0 && val == 0) {
			//disconnect(ui.sp_CirRegionNum, SIGNAL(valueChanged(int)), this, SLOT(spinBox_valueChanged(int)));
			connectSlots(false);
			ui->sp_CirRegionNum->setValue(m_param->CirRoiNum);
			connectSlots(true);
			//connect(ui.sp_CirRegionNum, SIGNAL(valueChanged(int)), this, SLOT(spinBox_valueChanged(int)));
			return;
		}
		if (val > MAX_Cir_Num) {
			ui->sp_CirRegionNum->setValue(MAX_Cir_Num);
			return;
		}
		m_param->CirRoiNum = val;
		//m_loadingProject = true;
		connectSlots(false);
		ui->cob_GroupIndex->clear();
		for (int i = 0;i < m_param->RecRoiNum + m_param->CirRoiNum;i++) {
			if (i < m_param->RecRoiNum) {
				ui->cob_GroupIndex->addItem(tr("Rect ROI%1").arg(i + 1));
			}
			else if (i >= m_param->RecRoiNum) {
				ui->cob_GroupIndex->addItem(tr("Cir ROI%1").arg(i - m_param->RecRoiNum + 1));
			}
		}
		ui->cob_GroupIndex->setCurrentIndex(0);
		connectSlots(true);
		//m_loadingProject = false;
	}
	m_module->setParamChanged(QvsParamLevel::ProductLevel);
	//log(LogL1, str0 + str1 + str2);
}

void surfaceMultWdg::doubleSpinBox_valueChanged(double val)
{
	QDoubleSpinBox* dsp = qobject_cast<QDoubleSpinBox*>(sender());

	QString str0 = QString("");
	QString str1 = QString("");
	QString str2 = QString("");
	int idx = m_param->currentRoi;
	if (dsp == ui->dsp_badLengthMin) {
		if (m_param->IsPixOrAct == 0) {
			val *= m_param->pixSize;
		}
		str0 = tr("ROI%1 ").arg(QString::number(idx));
		str1 = ui->tabWidget->tabText(0) + ui->label_20->text() + tr(" min val ");
		str2 = tr(" change from %1 to %2").arg(QString::number(m_param->badLengthMin[idx], 'f', 3)).arg(QString::number(val, 'f', 3));
		m_param->badLengthMin[idx] = val;
		if (m_testing) {
			testParam();
		}
	}
	else if (dsp == ui->dsp_badLengthMax) {
		if (m_param->IsPixOrAct == 0) {
			val *= m_param->pixSize;
		}
		str0 = tr("ROI%1 ").arg(QString::number(idx));
		str1 = ui->tabWidget->tabText(0) + ui->label_20->text() + tr(" max val ");
		str2 = tr(" change from %1 to %2").arg(QString::number(m_param->badLengthMax[idx], 'f', 3)).arg(QString::number(val, 'f', 3));
		m_param->badLengthMax[idx] = val;
		if (m_testing) {
			testParam();
		}
	}
	else if (dsp == ui->dsp_badWidthMin) {
		if (m_param->IsPixOrAct == 0) {
			val *= m_param->pixSize;
		}
		str0 = tr("ROI%1 ").arg(QString::number(idx));
		str1 = ui->tabWidget->tabText(0) + ui->label_21->text() + tr(" min val ");
		str2 = tr(" change from %1 to %2").arg(QString::number(m_param->badWidthMin[idx], 'f', 3)).arg(QString::number(val, 'f', 3));
		m_param->badWidthMin[idx] = val;
		if (m_testing) {
			testParam();
		}
	}
	else if (dsp == ui->dsp_badWidthMax) {
		if (m_param->IsPixOrAct == 0) {
			val *= m_param->pixSize;
		}
		str0 = tr("ROI%1 ").arg(QString::number(idx));
		str1 = ui->label_21->text() + tr(" max val ");
		str2 = ui->tabWidget->tabText(0) + tr(" change from %1 to %2").arg(QString::number(m_param->badWidthMax[idx], 'f', 3)).arg(QString::number(val, 'f', 3));
		m_param->badWidthMax[idx] = val;
		if (m_testing) {
			testParam();
		}
	}
	else if (dsp == ui->dsp_badAreaMin) {
		if (m_param->IsPixOrAct == 0) {
			val *= m_param->pixSize*m_param->pixSize;
		}
		str0 = tr("ROI%1 ").arg(QString::number(idx));
		str1 = ui->tabWidget->tabText(0) + ui->label_17->text() + tr(" min val ");
		str2 = tr(" change from %1 to %2").arg(QString::number(m_param->badArea_min[idx], 'f', 6)).arg(QString::number(val, 'f', 6));
		m_param->badArea_min[idx] = val;
		if (m_testing) {
			testParam();
		}
	}
	else if (dsp == ui->dsp_badAreaMax) {
		if (m_param->IsPixOrAct == 0) {
			val *= m_param->pixSize*m_param->pixSize;
		}
		str0 = tr("ROI%1 ").arg(QString::number(idx));
		str1 = ui->tabWidget->tabText(0) + ui->label_17->text() + tr(" max val ");
		str2 = tr(" change from %1 to %2").arg(QString::number(m_param->badArea_max[idx], 'f', 6)).arg(QString::number(val, 'f', 6));
		m_param->badArea_max[idx] = val;
		if (m_testing) {
			testParam();
		}
	}
	else if (dsp == ui->dsp_singleBadAreaMin) {
		if (m_param->IsPixOrAct == 0) {
			val *= m_param->pixSize*m_param->pixSize;
		}
		str0 = tr("ROI%1 ").arg(QString::number(idx));
		str1 = ui->groupBox_5->title() + ui->cb_singleBadArea->text() + tr(" min val ");
		str2 = tr(" change from %1 to %2").arg(QString::number(m_param->single_badArea_min[idx], 'f', 6)).arg(QString::number(val, 'f', 6));
		m_param->single_badArea_min[idx] = val;
	}
	else if (dsp == ui->dsp_singleBadAreaMax) {
		if (m_param->IsPixOrAct == 0) {
			val *= m_param->pixSize*m_param->pixSize;
		}
		str0 = tr("ROI%1 ").arg(QString::number(idx));
		str1 = ui->groupBox_5->title() + ui->cb_singleBadArea->text() + tr(" max val ");
		str2 = tr(" change from %1 to %2").arg(QString::number(m_param->single_badArea_max[idx], 'f', 6)).arg(QString::number(val, 'f', 6));
		m_param->single_badArea_max[idx] = val;
	}
	else if (dsp == ui->dsp_totalBadAreaMin) {
		if (m_param->IsPixOrAct == 0) {
			val *= m_param->pixSize*m_param->pixSize;
		}
		str0 = tr("ROI%1 ").arg(QString::number(idx));
		str1 = ui->groupBox_5->title() + ui->cb_totalBadArea->text() + tr(" min val ");
		str2 = tr(" change from %1 to %2").arg(QString::number(m_param->total_badArea_min[idx], 'f', 6)).arg(QString::number(val, 'f', 6));
		m_param->total_badArea_min[idx] = val;
	}
	else if (dsp == ui->dsp_totalBadAreaMax) {
		if (m_param->IsPixOrAct == 0) {
			val *= m_param->pixSize*m_param->pixSize;
		}
		str0 = tr("ROI%1 ").arg(QString::number(idx));
		str1 = ui->groupBox_5->title() + ui->cb_totalBadArea->text() + tr(" max val ");
		str2 = tr(" change from %1 to %2").arg(QString::number(m_param->total_badArea_max[idx], 'f', 6)).arg(QString::number(val, 'f', 6));
		m_param->total_badArea_max[idx] = val;
	}
	else if (dsp == ui->dsp_lengthMin) {
		if (m_param->IsPixOrAct == 0) {
			val *= m_param->pixSize;
		}
		str0 = tr("ROI%1 ").arg(QString::number(idx));
		str1 = ui->groupBox_5->title() + ui->cb_length->text() + tr(" min val ");
		str2 = tr(" change from %1 to %2").arg(QString::number(m_param->area_length_min[idx], 'f', 3)).arg(QString::number(val, 'f', 3));
		m_param->area_length_min[idx] = val;
	}
	else if (dsp == ui->dsp_lengthMax) {
		if (m_param->IsPixOrAct == 0) {
			val *= m_param->pixSize;
		}
		str0 = tr("ROI%1 ").arg(QString::number(idx));
		str1 = ui->groupBox_5->title() + ui->cb_length->text() + tr(" max val ");
		str2 = tr(" change from %1 to %2").arg(QString::number(m_param->area_length_max[idx], 'f', 3)).arg(QString::number(val, 'f', 3));
		m_param->area_length_max[idx] = val;
	}
	else if (dsp == ui->dsp_widthMin) {
		if (m_param->IsPixOrAct == 0) {
			val *= m_param->pixSize;
		}
		str0 = tr("ROI%1 ").arg(QString::number(idx));
		str1 = ui->groupBox_5->title() + ui->cb_width->text() + tr(" min val ");
		str2 = tr(" change from %1 to %2").arg(QString::number(m_param->area_width_min[idx], 'f', 3)).arg(QString::number(val, 'f', 3));
		m_param->area_width_min[idx] = val;
	}
	else if (dsp == ui->dsp_widthMax) {
		if (m_param->IsPixOrAct == 0) {
			val *= m_param->pixSize;
		}
		str0 = tr("ROI%1 ").arg(QString::number(idx));
		str1 = ui->groupBox_5->title() + ui->cb_width->text() + tr(" max val ");
		str2 = tr(" change from %1 to %2").arg(QString::number(m_param->area_width_max[idx], 'f', 3)).arg(QString::number(val, 'f', 3));
		m_param->area_width_max[idx] = val;
	}
	else if (dsp == m_advanceDialog->ui->dsp_pixSize) {
		str0 = tr("ROI%1 ").arg(QString::number(idx));
		str1 = m_advanceDialog->ui->label_28->text();
		str2 = tr(" change from %1 to %2").arg(QString::number(m_param->pixSize, 'f', 3)).arg(QString::number(val, 'f', 3));
		m_param->pixSize = val;
		if (m_param->IsPixOrAct == 0) {
			connectSlots(false);
			changeSpinBox(0);
			connectSlots(true);
		}
		//m_loadingProject = false;
	}
	m_module->setParamChanged(QvsParamLevel::ProductLevel);
	//log(LogL1, str0 + str1 + str2);
}

void surfaceMultWdg::comboBox_Index_Changed(int index)
{
	QComboBox* cob = qobject_cast<QComboBox*>(sender());
	QString str0 = QString("");
	QString str1 = QString("");
	if (cob == ui->cob_GroupIndex) {
		str1 = tr(" change from %1 to %2").arg(cob->itemText(m_param->currentRoi)).arg(cob->itemText(index));
		m_param->currentRoi = index;
		//m_loadingProject = true;
		setCurrentGrp(index);
		//m_loadingProject = false;
	}
	else if (cob == ui->cob_type) {
		str0 = tr("ROI%1 ").arg(QString::number(m_param->currentRoi));
		str1 = tr(" change from %1 to %2").arg(cob->itemText(m_param->testType[m_param->currentRoi])).arg(cob->itemText(index));
		m_param->testType[m_param->currentRoi] = (surfaceMultParam::TestType)index;
		ui->stack_type->setCurrentIndex(index);
		if (m_testing) {
			testParam();
		}
	}
	m_module->setParamChanged(QvsParamLevel::ProductLevel);
}

void surfaceMultWdg::on_bt_selectRecRoi_toggled(bool check)
{
	QString str0 = ui->bt_selectRecRoi->text();
	if (check) {
		//currentView()->clear();
		currentView()->clearLayer(m_layerIdx);
		currentView()->setDrawColor(0, 255, 0);
		ui->bt_selectRecRoi->setText(tr("Rect-ROI OK"));
		ui->bt_selectCirRoi->setEnabled(false);
		ui->sp_RecRegionNum->setEnabled(false);
		m_advanceDialog->ui->sp_DeleteRecRegion->setEnabled(false);
		if (m_param->RecRoiNum + m_param->RecDeleteNum == 0) {
			return;
		}

		double row[MAX_Rec_Num + MAX_Delete_Rec_Num], col[MAX_Rec_Num + MAX_Delete_Rec_Num], 
			angle[MAX_Rec_Num + MAX_Delete_Rec_Num], length1[MAX_Rec_Num + MAX_Delete_Rec_Num], length2[MAX_Rec_Num + MAX_Delete_Rec_Num];
		//const char* title[MAX_Rec_Num + MAX_Delete_Rec_Num] = { "" };
		//char buf[MAX_Rec_Num + MAX_Delete_Rec_Num][32];
		QStringList title;
		for (auto i = 0;i < MAX_Rec_Num + MAX_Delete_Rec_Num;++i)
			title << "";		
		for (int i = 0;i < m_param->RecRoiNum + m_param->RecDeleteNum;i++) {
			if (i < m_param->RecRoiNum) {
				row[i] = m_param->RecRoi[i].row;
				col[i] = m_param->RecRoi[i].col;
				angle[i] = m_param->RecRoi[i].angle;
				length1[i] = m_param->RecRoi[i].length1;
				length2[i] = m_param->RecRoi[i].length2;
				//sprintf(buf[i], "Roi%d", i + 1);
				//title[i] = buf[i];
				title[i] = QString("Roi%1").arg(i + 1);
			}
			else {
				row[i] = m_param->DeleteRec[i - m_param->RecRoiNum].drow;
				col[i] = m_param->DeleteRec[i - m_param->RecRoiNum].dcol;
				angle[i] = m_param->DeleteRec[i - m_param->RecRoiNum].dangle;
				length1[i] = m_param->DeleteRec[i - m_param->RecRoiNum].dlength1;
				length2[i] = m_param->DeleteRec[i - m_param->RecRoiNum].dlength2;
				//sprintf(buf[i], "del%d", i + 1 - m_param->RecRoiNum);
				//title[i] = buf[i];
				title[i] = QString("del%1").arg(i + 1 - m_param->RecRoiNum);
			}
		}
		currentView()->drawRects2(m_param->RecRoiNum + m_param->RecDeleteNum, row, col, angle, length1, length2, title);
		currentView()->update();
	}
	else {
		currentView()->finishDraw();
		currentView()->update();
		ui->bt_selectRecRoi->setText(tr("Select Rect ROI"));
		ui->bt_selectCirRoi->setEnabled(true);
		ui->sp_RecRegionNum->setEnabled(true);
		m_advanceDialog->ui->sp_DeleteRecRegion->setEnabled(true);
		if (m_param->RecRoiNum + m_param->RecDeleteNum == 0) {
			return;
		}
		double row[MAX_Rec_Num + MAX_Delete_Rec_Num], col[MAX_Rec_Num + MAX_Delete_Rec_Num],
			angle[MAX_Rec_Num + MAX_Delete_Rec_Num],
			length1[MAX_Rec_Num + MAX_Delete_Rec_Num], length2[MAX_Rec_Num + MAX_Delete_Rec_Num];
		currentView()->getDrawRects2(m_param->RecRoiNum + m_param->RecDeleteNum, row, col, angle, length1, length2);
		for (int i = 0;i < m_param->RecRoiNum + m_param->RecDeleteNum;i++) {
			if (i < m_param->RecRoiNum) {
				m_param->RecRoi[i].row = row[i];
				m_param->RecRoi[i].col = col[i];
				m_param->RecRoi[i].angle = angle[i];
				m_param->RecRoi[i].length1 = length1[i];
				m_param->RecRoi[i].length2 = length2[i];
			}
			else {
				m_param->DeleteRec[i - m_param->RecRoiNum].drow = row[i];
				m_param->DeleteRec[i - m_param->RecRoiNum].dcol = col[i];
				m_param->DeleteRec[i - m_param->RecRoiNum].dangle = angle[i];
				m_param->DeleteRec[i - m_param->RecRoiNum].dlength1 = length1[i];
				m_param->DeleteRec[i - m_param->RecRoiNum].dlength2 = length2[i];
			}
		}
		m_module->setParamChanged(QvsParamLevel::ProductLevel);
	}
	//log(LogL1, str0);
}

void surfaceMultWdg::on_bt_selectCirRoi_toggled(bool check)
{
	QString str0 = ui->bt_selectCirRoi->text();
	if (check) {
		//currentView()->clear();
		currentView()->clearLayer(m_layerIdx);
		currentView()->setDrawColor(0, 255, 0);
		ui->bt_selectCirRoi->setText(tr("Cir-ROI OK"));
		ui->bt_selectRecRoi->setEnabled(false);
		ui->sp_CirRegionNum->setEnabled(false);
		m_advanceDialog->ui->sp_DeleteCirRegion->setEnabled(false);
		if (m_param->CirRoiNum + m_param->CirDeleteNum == 0) {
			return;
		}

		double row[MAX_Cir_Num + MAX_Delete_Cir_Num], col[MAX_Cir_Num + MAX_Delete_Cir_Num], r[MAX_Cir_Num + MAX_Delete_Cir_Num];
		//const char* title[MAX_Cir_Num + MAX_Delete_Cir_Num] = { "" };
		//char buf[MAX_Cir_Num + MAX_Delete_Cir_Num][32];
		QStringList title;
		for (auto i = 0;i < MAX_Cir_Num + MAX_Delete_Cir_Num;++i)
			title << "";
		for (int i = 0;i < m_param->CirRoiNum + m_param->CirDeleteNum;i++) {
			if (i < m_param->CirRoiNum) {
				row[i] = m_param->CirRoi[i].rrow;
				col[i] = m_param->CirRoi[i].rcol;
				r[i] = m_param->CirRoi[i].r;
				//sprintf(buf[i], "Roi%d", i + 1);
				//title[i] = buf[i];
				title[i] = QString("Roi%1").arg(i + 1);
			}
			else {
				row[i] = m_param->DeleteCir[i - m_param->CirRoiNum].drrow;
				col[i] = m_param->DeleteCir[i - m_param->CirRoiNum].drcol;
				r[i] = m_param->DeleteCir[i - m_param->CirRoiNum].dr;
				//sprintf(buf[i], "del%d", i + 1 - m_param->CirRoiNum);
				//title[i] = buf[i];
				title[i] = QString("del%1").arg(i + 1 - m_param->CirRoiNum);
			}
		}
		currentView()->drawCircles(m_param->CirRoiNum + m_param->CirDeleteNum, row, col, r, title);
		currentView()->update();
	}
	else {
		currentView()->finishDraw();
		currentView()->update();
		ui->bt_selectCirRoi->setText(tr("Select Cir-ROI"));
		ui->bt_selectRecRoi->setEnabled(true);
		ui->sp_CirRegionNum->setEnabled(true);
		m_advanceDialog->ui->sp_DeleteCirRegion->setEnabled(true);
		if (m_param->CirRoiNum + m_param->CirDeleteNum == 0) {
			return;
		}
		double row[MAX_Cir_Num + MAX_Delete_Cir_Num], col[MAX_Cir_Num + MAX_Delete_Cir_Num], r[MAX_Cir_Num + MAX_Delete_Cir_Num];
		currentView()->getDrawCircles(m_param->CirRoiNum + m_param->CirDeleteNum, row, col, r);
		for (int i = 0;i < m_param->CirRoiNum + m_param->CirDeleteNum;i++) {
			if (i < m_param->CirRoiNum) {
				m_param->CirRoi[i].rrow = row[i];
				m_param->CirRoi[i].rcol = col[i];
				m_param->CirRoi[i].r = r[i];
			}
			else {
				m_param->DeleteCir[i - m_param->CirRoiNum].drrow = row[i];
				m_param->DeleteCir[i - m_param->CirRoiNum].drcol = col[i];
				m_param->DeleteCir[i - m_param->CirRoiNum].dr = r[i];
			}
		}
		m_module->setParamChanged(QvsParamLevel::ProductLevel);
	}
	//log(LogL1, str0);
}

void surfaceMultWdg::on_bt_advance_clicked()
{
	if (m_advanceDialog->exec() == QDialog::Accepted) {
	}
}

void surfaceMultWdg::on_bt_selectRotate_toggled(bool check)
{
	if (check) {
		//currentView()->clear();
		currentView()->clearLayer(m_layerIdx);
		currentView()->setDrawColor(0, 255, 0);
		ui->bt_selectRotate->setText(tr("Rotating Region OK"));
		//ui.bt_selectRotate->setText(tr("确认旋转区域"));
		ui->bt_selectRecRoi->setEnabled(false);
		ui->bt_selectCirRoi->setEnabled(false);
		ui->bt_setRotateMult->setEnabled(false);
		currentView()->drawRect1(m_param->rotateRow1, m_param->rotateCol1, m_param->rotateRow2, m_param->rotateCol2);
		currentView()->update();
	}
	else {
		currentView()->finishDraw();
		currentView()->update();
		ui->bt_selectRotate->setText(tr("Select Rotating Region"));
		ui->bt_selectRecRoi->setEnabled(true);
		ui->bt_selectCirRoi->setEnabled(true);
		ui->bt_setRotateMult->setEnabled(true);
		currentView()->getDrawRect1(m_param->rotateRow1, m_param->rotateCol1, m_param->rotateRow2, m_param->rotateCol2);
		m_module->setParamChanged(QvsParamLevel::ProductLevel);
	}
}

void surfaceMultWdg::on_bt_setRotateMult_clicked()
{
	if (QMessageBox::information(this, tr("Hint"), tr("Do you want to set direction pattern by rotating this Angle?")
		, QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
		int angleVal[3] = { 90,180,270 };
		double rRow, rCol;   //旋转区域的中心点
		rRow = (m_param->rotateRow1 + m_param->rotateRow2) / 2.0;
		rCol = (m_param->rotateCol1 + m_param->rotateCol2) / 2.0;
		double centerRow, centerCol;
		HTuple hommat;
		double rangle = PI*angleVal[m_param->multAngle] / 180.0;
		vector_angle_to_rigid(rRow, rCol, 0.0, rRow, rCol, rangle, &hommat);  //建立旋转关系矩阵
		if (m_param->RecRoiNum > 0) {
			centerRow = m_param->RecRoi[0].row;
			centerCol = m_param->RecRoi[0].col;
			affine_trans_pixel(hommat, centerRow, centerCol, &centerRow, &centerCol);  //旋转ROI
			if (m_param->RecRoiNum == 1) {
				//m_loadingProject = true;
				connectSlots(false);
				m_param->RecRoiNum = 2;
				ui->sp_RecRegionNum->setValue(2);
				connectSlots(true);
				//m_loadingProject = false;
			}
			m_param->RecRoi[1].row = centerRow;
			m_param->RecRoi[1].col = centerCol;
			m_param->RecRoi[1].length1 = m_param->RecRoi[0].length1;
			m_param->RecRoi[1].length2 = m_param->RecRoi[0].length2;
			m_param->RecRoi[1].angle = m_param->RecRoi[0].angle + rangle;
		}
		else if (m_param->CirRoiNum > 0) {
			centerRow = m_param->CirRoi[0].rrow;
			centerCol = m_param->CirRoi[0].rcol;
			affine_trans_pixel(hommat, centerRow, centerCol, &centerRow, &centerCol);  //旋转ROI
			if (m_param->CirRoiNum == 1) {
				//m_loadingProject = true;
				connectSlots(false);
				m_param->CirRoiNum = 2;
				ui->sp_CirRegionNum->setValue(2);
				connectSlots(true);
				//m_loadingProject = false;
			}
			m_param->CirRoi[1].rrow = centerRow;
			m_param->CirRoi[1].rcol = centerCol;
			m_param->CirRoi[1].r = m_param->CirRoi[0].r;
		}
		else {
			QMessageBox::warning(this, tr("Warning!"), tr("No Region For Direction Pattern!")
				, QMessageBox::Yes);
			return;
		}

		if (m_param->RecDeleteNum > 0) {
			centerRow = m_param->DeleteRec[0].drow;
			centerCol = m_param->DeleteRec[0].dcol;
			affine_trans_pixel(hommat, centerRow, centerCol, &centerRow, &centerCol);  //旋转ROI
			if (m_param->RecDeleteNum == 1) {
				//m_loadingProject = true;
				connectSlots(false);
				m_param->RecDeleteNum = 2;
				m_advanceDialog->ui->sp_DeleteRecRegion->setValue(2);
				connectSlots(true);
				//m_loadingProject = false;
			}
			m_param->DeleteRec[1].drow = centerRow;
			m_param->DeleteRec[1].dcol = centerCol;
			m_param->DeleteRec[1].dlength1 = m_param->DeleteRec[0].dlength1;
			m_param->DeleteRec[1].dlength2 = m_param->DeleteRec[0].dlength2;
			m_param->DeleteRec[1].dangle = m_param->DeleteRec[0].dangle + rangle;
		}
		else if (m_param->CirDeleteNum > 0) {
			centerRow = m_param->DeleteCir[0].drrow;
			centerCol = m_param->DeleteCir[0].drcol;
			affine_trans_pixel(hommat, centerRow, centerCol, &centerRow, &centerCol);  //旋转ROI
			if (m_param->CirDeleteNum == 1) {
				//m_loadingProject = true;
				connectSlots(false);
				m_param->CirDeleteNum = 2;
				m_advanceDialog->ui->sp_DeleteCirRegion->setValue(2);
				connectSlots(true);
				//m_loadingProject = false;
			}
			m_param->DeleteCir[1].drrow = centerRow;
			m_param->DeleteCir[1].drcol = centerCol;
			m_param->DeleteCir[1].dr = m_param->DeleteCir[0].dr;
		}
		m_module->setParamChanged(QvsParamLevel::ProductLevel);
	}
}

void surfaceMultWdg::radioButton_toggled(bool val)
{
	QRadioButton* rb = qobject_cast<QRadioButton*>(sender());
	QString str0 = QString("");
	QString str1 = QString("");
	int idx = m_param->currentRoi;
	if (val) {
		if (rb == m_advanceDialog->ui->rb_dirDelete) {
			str1 = tr(" change from %1 to %2")
				.arg(m_advanceDialog->ui->rb_allDelete->text())
				.arg(m_advanceDialog->ui->rb_dirDelete->text());
			m_param->IsDirDelete = 0;
		}
		else if (rb == m_advanceDialog->ui->rb_allDelete) {
			str1 = tr(" change from %1 to %2")
				.arg(m_advanceDialog->ui->rb_dirDelete->text())
				.arg(m_advanceDialog->ui->rb_allDelete->text());
			m_param->IsDirDelete = 1;
		}
		else if (rb == ui->rb_defect) {
			str1 = tr(" change from %1 to %2").arg(ui->rb_target->text()).arg(ui->rb_defect->text());
			m_param->DetectModel = 0;
			changeModel(0);
		}
		else if (rb == ui->rb_target) {
			str1 = tr(" change from %1 to %2").arg(ui->rb_defect->text()).arg(ui->rb_target->text());
			m_param->DetectModel = 1;
			changeModel(1);
		}
		else if (rb == ui->rb_90) {
			m_param->multAngle = 0;
		}
		else if (rb == ui->rb_180) {
			m_param->multAngle = 1;
		}
		else if (rb == ui->rb_270) {
			m_param->multAngle = 2;
		}
		else if (rb == m_advanceDialog->ui->rb_pixel) {
			str1 = tr(" change from %1 to %2")
				.arg(m_advanceDialog->ui->rb_actual->text())
				.arg(m_advanceDialog->ui->rb_pixel->text());
			m_param->IsPixOrAct = 0;
			connectSlots(false);
			changeSpinBox(0);
			connectSlots(true);
		}
		else if (rb == m_advanceDialog->ui->rb_actual) {
			str1 = tr(" change from %1 to %2")
				.arg(m_advanceDialog->ui->rb_pixel->text())
				.arg(m_advanceDialog->ui->rb_actual->text());
			m_param->IsPixOrAct = 1;
			connectSlots(false);
			changeSpinBox(1);
			connectSlots(true);
		}
		m_module->setParamChanged(QvsParamLevel::ProductLevel);
		//log(LogL1, str0 + str1);
	}
}

void surfaceMultWdg::on_bt_test_toggled(bool val)
{
	if (val) {
		ui->bt_test->setText(tr("exit test"));
		m_testing = true;
		testParam();
	}
	else {
		ui->bt_test->setText(tr("test"));
		m_testing = false;
		//currentView()->clear();
		currentView()->clearLayer(m_layerIdx);
		currentView()->update();
	}
}

void surfaceMultWdg::bt_clicked()
{
	QPushButton *bt = qobject_cast<QPushButton *>(sender());

	if (bt == m_advanceDialog->ui->bt_getPixelSize)
	{
		if (m_param->IsPixOrAct == 0)
			return;
		if (m_module->p_pixelSize->size() == 0)
			return;
		double pixSize = m_module->p_pixelSize->at(0);
		if (pixSize < 0.00001)
			return;
		m_advanceDialog->ui->dsp_pixSize->setValue(pixSize);
	}
}
