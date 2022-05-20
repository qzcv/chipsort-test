#include "imageSegment_preWdg.h"
#include "imageSegment_preMod.h"
#include "imageSegment_preParam.h"
#include "ui_imageSegment_preWdg.h"
#include "ImageView.h"
#include "halOpenCV.h"

imageSegment_preWdg::imageSegment_preWdg(int i) :
	ModSetWidget(),ui(new Ui::imageSegment_preWdg),m_widgetType(i)
{
	m_hasConnect = false;
	ui->setupUi(this);
	m_IsTesting = false;
	m_layerIdx = 0;
	iniUi();
	m_param = nullptr;
	m_module = nullptr;
}

imageSegment_preWdg::~imageSegment_preWdg()
{
	delete ui;
}

void imageSegment_preWdg::setModule(UnitModule *module)
{
	m_module=dynamic_cast<imageSegment_preMod*>(module);
	m_param = m_module->m_param;
	//currentView()->setLayer(m_layerIdx);
	//updateUi();
}

void imageSegment_preWdg::showEvent(QShowEvent * event)
{
	updateUi();
}

void imageSegment_preWdg::connectSlots(bool link)
{
	if(m_hasConnect==link)
		return;
	m_hasConnect=link;

	connectOrNot(link, ui->cob_algorithmType, SIGNAL(currentIndexChanged(int)), SLOT(comboBox_indexChanged(int)));
	connectOrNot(link, ui->cob_roi, SIGNAL(currentIndexChanged(int)), SLOT(comboBox_indexChanged(int)));

	connectOrNot(link, ui->cb_moduleValid, SIGNAL(toggled(bool)), SLOT(checkBox_toggled(bool)));
	connectOrNot(link, ui->cb_allPic, SIGNAL(toggled(bool)), SLOT(checkBox_toggled(bool)));
	connectOrNot(link, ui->cb_meanGrow, SIGNAL(toggled(bool)), SLOT(checkBox_toggled(bool)));
	connectOrNot(link, ui->cb_noCenter, SIGNAL(toggled(bool)), SLOT(checkBox_toggled(bool)));

	connectOrNot(link, ui->sp_RoiCounts, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_threMin, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_threMax, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_areaMin, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_areaMax, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));

	connectOrNot(link, ui->sp_centerX, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_centerY, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_rowWid, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_colWid, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_tol, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_minGrowArea, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));

	connectOrNot(link, ui->sp_param1, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_param2, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_param3, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->hs_threMin, SIGNAL(valueChanged(int)), SLOT(hslider_valueChanged(int)));
	connectOrNot(link, ui->hs_threMax, SIGNAL(valueChanged(int)), SLOT(hslider_valueChanged(int)));
	connectOrNot(link, ui->rb_threshold, SIGNAL(toggled(bool)), SLOT(radioButton_toggled(bool)));
	connectOrNot(link, ui->rb_regiongrowing, SIGNAL(toggled(bool)), SLOT(radioButton_toggled(bool)));
	connectOrNot(link, ui->rb_roiCenter, SIGNAL(toggled(bool)), SLOT(radioButton_toggled(bool)));
	connectOrNot(link, ui->rb_setCenter, SIGNAL(toggled(bool)), SLOT(radioButton_toggled(bool)));

}

void imageSegment_preWdg::iniUi()
{

}

void imageSegment_preWdg::setUiValue()
{
	if(!m_param)
		return;

	ui->bt_test->setVisible(0);

	if (m_param->thresholdType == 0) {
		ui->rb_threshold->setChecked(1);
		ui->stackedWidget->setCurrentIndex(0);
	}
	else if (m_param->thresholdType == 1) {
		ui->rb_regiongrowing->setChecked(1);
		ui->stackedWidget->setCurrentIndex(1);
	}

	if (m_param->centerType == 0)
		ui->rb_roiCenter->setChecked(1);
	else if (m_param->centerType == 1)
		ui->rb_setCenter->setChecked(1);

	ui->cb_meanGrow->setChecked(m_param->IsMeanGrow);
	ui->cb_noCenter->setChecked(m_param->IsNoCenter);
	ui->cb_moduleValid->setChecked(m_param->moduleValid);
	ui->cb_allPic->setChecked(m_param->allImgValid);
	ui->bt_selectRoi->setEnabled(!m_param->allImgValid);
	ui->sp_RoiCounts->setValue(m_param->RoiCounts);
	changeRoi(0);

	ui->cob_roi->clear();
	for (int i = 0;i < m_param->RoiCounts;i++) {
		ui->cob_roi->addItem(tr("ROI-%1").arg(i + 1));
	}
	ui->cob_roi->setCurrentIndex(0);
	ui->sp_centerX->setValue(m_param->centerX[0]);
	ui->sp_centerY->setValue(m_param->centerY[0]);
	ui->sp_rowWid->setValue(m_param->rowWid);
	ui->sp_colWid->setValue(m_param->colWid);
	ui->sp_tol->setValue(m_param->thresholdTol);
	ui->sp_minGrowArea->setValue(m_param->minGrowArea);

	ui->cob_algorithmType->setCurrentIndex(m_param->algorithmType);
	ui->sp_param1->setValue(m_param->param1[m_param->algorithmType]);
	ui->sp_param2->setValue(m_param->param2[m_param->algorithmType]);
	ui->sp_param3->setValue(m_param->param3[m_param->algorithmType]);
}

void imageSegment_preWdg::updateUi()
{
	connectSlots(false);
	setUiValue();
	connectSlots(true);
}

void imageSegment_preWdg::dispRoi()
{
	if (!m_param->allImgValid)
	{
		currentView()->setColor(0, 255, 0);
		currentView()->setFilled(false);
		for (int i = 0;i < m_param->RoiCounts;i++)
		{
			currentView()->dispRect1(
				m_param->segRegionVector[i].row1, m_param->segRegionVector[i].col1, 
				m_param->segRegionVector[i].row2, m_param->segRegionVector[i].col2, QString("roi%1").arg(i + 1));
		}
	}
}

void imageSegment_preWdg::changeRoi(int index)
{
	ui->sp_threMin->setValue(m_param->minGray[index]);
	ui->sp_threMax->setValue(m_param->maxGray[index]);
	ui->sp_areaMin->setValue(m_param->minArea[index]);
	ui->sp_areaMax->setValue(m_param->maxArea[index]);
	ui->hs_threMin->setValue(m_param->minGray[index]);
	ui->hs_threMax->setValue(m_param->maxGray[index]);
}

void imageSegment_preWdg::thresholdChanged()
{
	if (m_module->p_iIm->empty())
		return;
	if (!m_param->moduleValid)
		return;

	Hobject segImg;
	m_module->calReg(&segImg);

	//currentView()->clear();
	currentView()->clearLayer(m_layerIdx);
	currentView()->setColor(Qt::black);
	currentView()->setFilled(true);
	for (int i = 0;i < m_param->RoiCounts;i++)
	{
		currentView()->dispRect1(
			m_param->segRegionVector[i].row1, m_param->segRegionVector[i].col1,
			m_param->segRegionVector[i].row2, m_param->segRegionVector[i].col2);
	}
	union1(segImg, &segImg);
	Hlong num;
	count_obj(segImg, &num);
	currentView()->setColor(Qt::white);
	if (num > 0) {
		currentView()->setFilled(true);
		RlRegion reg;
		HalOpenCV::hal2cvRegion(segImg, &reg);
		currentView()->dispRegion(&reg);
	}
	currentView()->update();
}

void imageSegment_preWdg::checkBox_toggled(bool check)
{
	QCheckBox* cb = qobject_cast<QCheckBox*>(sender());
	if (cb == ui->cb_moduleValid) {
		m_param->moduleValid = check;
	}
	else if (cb == ui->cb_allPic) {
		m_param->allImgValid = check;
		ui->bt_selectRoi->setEnabled(!check);
	}
	else if (cb == ui->cb_meanGrow) {
		m_param->IsMeanGrow = check;
	}
	else if (cb == ui->cb_noCenter) {
		m_param->IsNoCenter = check;
	}
	m_module->setParamChanged(ProductLevel);
}

void imageSegment_preWdg::spinBox_valueChanged(int val)
{
	QSpinBox* sp = qobject_cast<QSpinBox*>(sender());
	if (sp == ui->sp_RoiCounts) {
		if (m_param->RoiCounts == val || m_param->allImgValid)
			return;
		if (ui->bt_selectRoi->isChecked())
		{
			currentView()->finishDraw();
			double row1[30], col1[30], row2[30], col2[30];
			currentView()->getDrawRects1(m_param->RoiCounts, row1, col1, row2, col2);
			for (int i = 0;i < m_param->RoiCounts;i++) {
				m_param->segRegionVector[i].row1 = row1[i];
				m_param->segRegionVector[i].col1 = col1[i];
				m_param->segRegionVector[i].row2 = row2[i];
				m_param->segRegionVector[i].col2 = col2[i];
			}
		}
		if (val > m_param->RoiCounts) {
			imageSegment_preParam::segRegion seg;
			for (int i = m_param->RoiCounts;i < val;i++)
			{
				seg.row1 = seg.col1 = 50 + 10 * i;
				seg.row2 = seg.col2 = 100 + 10 * i;
				m_param->segRegionVector.push_back(seg);
			}
		}
		else if (val < m_param->RoiCounts) {
			vector<imageSegment_preParam::segRegion>::iterator itBegin = m_param->segRegionVector.begin() + val;
			vector<imageSegment_preParam::segRegion>::iterator itEnd = m_param->segRegionVector.end();
			m_param->segRegionVector.erase(itBegin, itEnd);
		}
		m_param->RoiCounts = val;
		if (ui->bt_selectRoi->isChecked()) {
			currentView()->finishDraw();
			//currentView()->clear();
			currentView()->clearLayer(m_layerIdx);
			currentView()->update();
			double row1[30], col1[30], row2[30], col2[30];
			QStringList title;
			for (int i = 0;i < m_param->RoiCounts;i++) {
				row1[i] = m_param->segRegionVector[i].row1;
				col1[i] = m_param->segRegionVector[i].col1;
				row2[i] = m_param->segRegionVector[i].row2;
				col2[i] = m_param->segRegionVector[i].col2;
				title << QString("roi%1").arg(i + 1);
			}
			currentView()->setColor(255, 0, 128);
			currentView()->drawRects1(m_param->RoiCounts, row1, col1, row2, col2, title);
			currentView()->update();
		}
		else {
			currentView()->finishDraw();
			//currentView()->clear();
			currentView()->clearLayer(m_layerIdx);
			dispRoi();
			currentView()->update();
		}
		connectSlots(false);
		ui->cob_roi->clear();
		for (int i = 0;i < m_param->RoiCounts;i++) {
			ui->cob_roi->addItem(tr("ROI-%1").arg(i + 1));
		}
		connectSlots(true);
		ui->cob_roi->setCurrentIndex(0);
	}
	else if (sp == ui->sp_threMin) {
		int mindex = ui->cob_roi->currentIndex();
		if (val >= m_param->maxGray[mindex]) {
			connectSlots(false);
			ui->sp_threMin->setValue(m_param->minGray[mindex]);
			connectSlots(true);
			return;
		}
		m_param->minGray[mindex] = val;
		connectSlots(false);
		ui->hs_threMin->setValue(val);
		connectSlots(true);
		thresholdChanged();
	}
	else if (sp == ui->sp_threMax) {
		int mindex = ui->cob_roi->currentIndex();
		if (val <= m_param->minGray[mindex]) {
			connectSlots(false);
			ui->sp_threMax->setValue(m_param->maxGray[mindex]);
			connectSlots(true);
			return;
		}
		m_param->maxGray[mindex] = val;
		connectSlots(false);
		ui->hs_threMax->setValue(val);
		connectSlots(true);
		thresholdChanged();
	}
	else if (sp == ui->sp_areaMin) {
		m_param->minArea[ui->cob_roi->currentIndex()] = val;
		thresholdChanged();
	}
	else if (sp == ui->sp_areaMax) {
		m_param->maxArea[ui->cob_roi->currentIndex()] = val;
		thresholdChanged();
	}
	else if (sp == ui->sp_centerX) {
		m_param->centerX[ui->cob_roi->currentIndex()] = val;
		thresholdChanged();
	}
	else if (sp == ui->sp_centerY) {
		m_param->centerY[ui->cob_roi->currentIndex()] = val;
		thresholdChanged();
	}
	else if (sp == ui->sp_rowWid) {
		m_param->rowWid = val;
		thresholdChanged();
	}
	else if (sp == ui->sp_colWid) {
		m_param->colWid = val;
		thresholdChanged();
	}
	else if (sp == ui->sp_tol) {
		m_param->thresholdTol = val;
		thresholdChanged();
	}
	else if (sp == ui->sp_minGrowArea) {
		m_param->minGrowArea = val;
		thresholdChanged();
	}
	else if (sp == ui->sp_param1) {
		m_param->param1[m_param->algorithmType] = val;
	}
	else if (sp == ui->sp_param2) {
		m_param->param2[m_param->algorithmType] = val;
	}
	else if (sp == ui->sp_param3) {
		m_param->param3[m_param->algorithmType] = val;
	}
	m_module->setParamChanged(ProductLevel);
}

void imageSegment_preWdg::hslider_valueChanged(int val)
{
	QSlider* hs = qobject_cast<QSlider*>(sender());
	if (hs == ui->hs_threMin) {
		int mindex = ui->cob_roi->currentIndex();
		if (val >= m_param->maxGray[mindex]) {
			connectSlots(false);
			ui->hs_threMin->setValue(m_param->minGray[mindex]);
			connectSlots(true);
			return;
		}
		m_param->minGray[mindex] = val;
		connectSlots(false);
		ui->sp_threMin->setValue(val);
		connectSlots(true);
		thresholdChanged();
	}
	else if (hs == ui->hs_threMax) {
		int mindex = ui->cob_roi->currentIndex();
		if (val <= m_param->minGray[mindex]) {
			connectSlots(false);
			ui->hs_threMax->setValue(m_param->maxGray[mindex]);
			connectSlots(true);
			return;
		}
		m_param->maxGray[mindex] = val;
		connectSlots(false);
		ui->sp_threMax->setValue(val);
		connectSlots(true);
		thresholdChanged();
	}
	m_module->setParamChanged(ProductLevel);
}

void imageSegment_preWdg::on_bt_selectRoi_toggled(bool check)
{
	if (check) {
		if (m_param->RoiCounts == 0) {
			return;
		}
		//currentView()->clear();
		currentView()->clearLayer(m_layerIdx);
		double row1[30], col1[30], row2[30], col2[30];
		QStringList title;
		for (int i = 0;i < m_param->RoiCounts;i++) {
			row1[i] = m_param->segRegionVector[i].row1;
			col1[i] = m_param->segRegionVector[i].col1;
			row2[i] = m_param->segRegionVector[i].row2;
			col2[i] = m_param->segRegionVector[i].col2;
			title << QString("roi%1").arg(i + 1);
		}
		currentView()->setColor(255, 0, 128);
		currentView()->drawRects1(m_param->RoiCounts, row1, col1, row2, col2, title);
		currentView()->update();
	}
	else {
		if (m_param->RoiCounts == 0) {
			return;
		}
		currentView()->finishDraw();
		double row1[30], col1[30], row2[30], col2[30];
		currentView()->getDrawRects1(m_param->RoiCounts, row1, col1, row2, col2);
		for (int i = 0;i < m_param->RoiCounts;i++) {
			m_param->segRegionVector[i].row1 = row1[i];
			m_param->segRegionVector[i].col1 = col1[i];
			m_param->segRegionVector[i].row2 = row2[i];
			m_param->segRegionVector[i].col2 = col2[i];
		}
		//currentView()->clear();
		currentView()->clearLayer(m_layerIdx);
		dispRoi();
		currentView()->update();

		m_module->setParamChanged(ProductLevel);
	}
}

void imageSegment_preWdg::radioButton_toggled(bool val)
{
	QRadioButton* rb = qobject_cast<QRadioButton*>(sender());
	if (val) {
		if (rb == ui->rb_threshold) {
			m_param->thresholdType = 0;
			ui->stackedWidget->setCurrentIndex(0);
		}
		else if (rb == ui->rb_regiongrowing) {
			m_param->thresholdType = 1;
			ui->stackedWidget->setCurrentIndex(1);
		}
		else if (rb == ui->rb_roiCenter) {
			m_param->centerType = 0;
		}
		else if (rb == ui->rb_setCenter) {
			m_param->centerType = 1;
		}
		m_module->setParamChanged(ProductLevel);
	}
}

void imageSegment_preWdg::comboBox_indexChanged(int index)
{
	QComboBox* cob = qobject_cast<QComboBox*>(sender());
	if (cob == ui->cob_roi) {
		connectSlots(false);
		ui->sp_centerX->setValue(m_param->centerX[index]);
		ui->sp_centerY->setValue(m_param->centerY[index]);
		changeRoi(index);
		connectSlots(true);
	}
	else if (cob == ui->cob_algorithmType)
	{
		m_param->algorithmType = index;
		connectSlots(false);
		ui->sp_param1->setValue(m_param->param1[m_param->algorithmType]);
		ui->sp_param2->setValue(m_param->param2[m_param->algorithmType]);
		ui->sp_param3->setValue(m_param->param3[m_param->algorithmType]);
		connectSlots(true);
	}
	m_module->setParamChanged(ProductLevel);
}

void imageSegment_preWdg::on_bt_test_toggled(bool val)
{
	m_IsTesting = val;
	if (val) {
		ui->bt_test->setText(tr("exit test"));
	}
	else {
		ui->bt_test->setText(tr("test mode"));
	}
}
