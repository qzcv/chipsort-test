#include "colorPickerWdg.h"
#include "colorPickerMod.h"
#include "colorPickerParam.h"
#include "ui_colorPickerWdg.h"
#include "ImageView.h"
#include "halOpenCV.h"

ColorPickerWdg::ColorPickerWdg(int i) :
	ModSetWidget(),ui(new Ui::ColorPickerWdg),m_widgetType(i)
{
	m_hasConnect = false;
	ui->setupUi(this);
	iniData();
	iniUi();
	m_param = nullptr;
	m_module = nullptr;
}

ColorPickerWdg::~ColorPickerWdg()
{
	delete ui;
}

void ColorPickerWdg::setModule(UnitModule *module)
{
	m_module=dynamic_cast<ColorPickerMod*>(module);
	m_param = m_module->m_param;
	m_image = m_module->m_image;
	//currentView()->setLayer(m_layerIdx);
	//updateUi();
}

void ColorPickerWdg::showEvent(QShowEvent * event)
{
	updateUi();
}

void ColorPickerWdg::connectSlots(bool link)
{
	if(m_hasConnect==link)
		return;
	m_hasConnect=link;

	connectOrNot(link, ui->sp_RoiNum, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_MinusNum, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_areaMin, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_areaMax, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_saturationMin, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_saturationMax, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_hueMin, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_hueMax, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_errAllAreaMax, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_errAllAreaMin, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_errSingleAreaMax, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_errSingleAreaMin, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_lightAreaThreshold, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_lightArea, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_WidthMin, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_WidthMax, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_heightMax, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_heightMin, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_AreaZoom, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_open, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_close, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));

	connectOrNot(link, ui->cb_useRefer, SIGNAL(toggled(bool)), SLOT(checkBox_toggled(bool)));
	connectOrNot(link, ui->cb_drawAllRegion, SIGNAL(toggled(bool)), SLOT(checkBox_toggled(bool)));
	connectOrNot(link, ui->cb_height, SIGNAL(toggled(bool)), SLOT(checkBox_toggled(bool)));
	connectOrNot(link, ui->cb_Width, SIGNAL(toggled(bool)), SLOT(checkBox_toggled(bool)));
	connectOrNot(link, ui->cb_notLess, SIGNAL(toggled(bool)), SLOT(checkBox_toggled(bool)));
	connectOrNot(link, ui->cb_errSingleArea, SIGNAL(toggled(bool)), SLOT(checkBox_toggled(bool)));
	connectOrNot(link, ui->cb_errAllArea, SIGNAL(toggled(bool)), SLOT(checkBox_toggled(bool)));
	connectOrNot(link, ui->cb_showRegionAfter, SIGNAL(toggled(bool)), SLOT(checkBox_toggled(bool)));
	connectOrNot(link, ui->cb_deleteLightArea, SIGNAL(toggled(bool)), SLOT(checkBox_toggled(bool)));
	connectOrNot(link, ui->cb_fillHole, SIGNAL(toggled(bool)), SLOT(checkBox_toggled(bool)));
	connectOrNot(link, ui->cb_open, SIGNAL(toggled(bool)), SLOT(checkBox_toggled(bool)));
	connectOrNot(link, ui->cb_close, SIGNAL(toggled(bool)), SLOT(checkBox_toggled(bool)));

	connectOrNot(link, ui->rb_cir, SIGNAL(toggled(bool)), SLOT(radioButton_toggled(bool)));
	connectOrNot(link, ui->rb_rec, SIGNAL(toggled(bool)), SLOT(radioButton_toggled(bool)));
	connectOrNot(link, ui->rb_useSelfReg, SIGNAL(toggled(bool)), SLOT(radioButton_toggled(bool)));
	connectOrNot(link, ui->rb_useAreaRefer, SIGNAL(toggled(bool)), SLOT(radioButton_toggled(bool)));
	connectOrNot(link, ui->rb_usePointRefer, SIGNAL(toggled(bool)), SLOT(radioButton_toggled(bool)));

	connectOrNot(link, ui->cob_currentMinusIndex, SIGNAL(currentIndexChanged(int)), SLOT(comboBox_IndexChanged(int)));
	connectOrNot(link, ui->cob_currentRoiIndex, SIGNAL(currentIndexChanged(int)), SLOT(comboBox_IndexChanged(int)));
}

void ColorPickerWdg::iniData()
{
	m_testing = false;
	m_image = nullptr;
	m_layerIdx = 1;
}

void ColorPickerWdg::iniUi()
{

}

void ColorPickerWdg::setUiValue()
{
	if(!m_param)
		return;

	ui->sp_RoiNum->setValue(m_param->roiCount);
	ui->sp_MinusNum->setValue(m_param->MinusCount);
	ui->sp_areaMin->setValue(m_param->areaMin);
	ui->sp_areaMax->setValue(m_param->areaMax);
	ui->sp_heightMin->setValue(m_param->heightMin);
	ui->sp_heightMax->setValue(m_param->heightMax);
	ui->sp_WidthMin->setValue(m_param->widthMin);
	ui->sp_WidthMax->setValue(m_param->widthMax);
	ui->sp_saturationMax->setValue(m_param->SaturationMax);
	ui->sp_saturationMin->setValue(m_param->SaturationMin);
	ui->sp_hueMin->setValue(m_param->HueMin);
	ui->sp_hueMax->setValue(m_param->HueMax);
	ui->sp_errAllAreaMin->setValue(m_param->errAllMin);
	ui->sp_errAllAreaMax->setValue(m_param->errAllMax);
	ui->sp_errSingleAreaMin->setValue(m_param->errSingleMin);
	ui->sp_errSingleAreaMax->setValue(m_param->errSingleMax);
	ui->sp_lightAreaThreshold->setValue(m_param->lightAreaThreshold);
	ui->sp_lightArea->setValue(m_param->lightArea);
	ui->sp_AreaZoom->setValue(m_param->AreaZoom);
	ui->sp_open->setValue(m_param->openVal);
	ui->sp_close->setValue(m_param->closeVal);

	ui->cb_open->setChecked(m_param->IsOpening);
	ui->cb_close->setChecked(m_param->IsClosing);

	ui->cb_useRefer->setChecked(m_param->IsReferSection);
	ui->cb_height->setChecked(m_param->IsHeight);
	ui->cb_Width->setChecked(m_param->IsWidth);
	ui->cb_drawAllRegion->setChecked(m_param->IsShowAllRegion);
	ui->cb_notLess->setChecked(m_param->IsNotLess);
	ui->cb_errAllArea->setChecked(m_param->IsErrAllArea);
	ui->cb_errSingleArea->setChecked(m_param->IsErrSingleArea);
	ui->cb_showRegionAfter->setChecked(m_param->IsShowRegionAfterDetect);
	ui->cb_fillHole->setChecked(m_param->IsFillUpHole);
	ui->cb_deleteLightArea->setChecked(m_param->IsDeleteLightArea);

	if (m_param->nowIsRecOrCir == 0) {
		ui->rb_rec->setChecked(true);
	}
	else {
		ui->rb_cir->setChecked(true);
	}
	if (m_param->referType == 0) {
		ui->rb_useSelfReg->setChecked(true);
	}
	else if (m_param->referType == 1) {
		ui->rb_useAreaRefer->setChecked(true);
	}
	else if (m_param->referType == 2) {
		ui->rb_usePointRefer->setChecked(true);
	}

	for (int i = 0;i < m_param->roiCount;i++) {
		ui->cob_currentRoiIndex->addItem(tr("Region%1").arg(i));
	}
	for (int i = 0;i < m_param->MinusCount;i++) {
		ui->cob_currentMinusIndex->addItem(tr("Region%1").arg(i));
		//ui.cob_currentMinusIndex->addItem(QString::fromLocal8Bit("ÇøÓò%1").arg(i));
	}
	if (m_param->roiCount > 0) {
		ui->cob_currentRoiIndex->setCurrentIndex(m_param->currentRoiIndex);
	}
	else {
		//ui.cob_currentRoiIndex->setEnabled(false);
		//ui.bt_selectROI->setEnabled(false);
	}
	if (m_param->MinusCount > 0) {
		ui->cob_currentMinusIndex->setCurrentIndex(m_param->currentMinusIndex);
	}
	else {
		//ui.cob_currentMinusIndex->setEnabled(false);
		//ui.bt_selectMinusROI->setEnabled(false);
	}
}

void ColorPickerWdg::updateUi()
{
	connectSlots(false);
	setUiValue();
	connectSlots(true);
}

void ColorPickerWdg::dispRoi(int roiORmin)
{
	//currentView()->clear();
	currentView()->clearLayer(m_layerIdx);
	currentView()->setDrawColor(0, 255, 0);
	int inparam = roiORmin;
	double recrow, reccol, recang, reclen1, reclen2, cirrow, circol, cirr;
	int roitype;
	QString title;
	if (inparam == 0) {
		roitype = m_param->RoiVec[m_param->currentRoiIndex].regionType;
		recrow = m_param->RoiVec[m_param->currentRoiIndex].RecRow;
		reccol = m_param->RoiVec[m_param->currentRoiIndex].RecCol;
		recang = m_param->RoiVec[m_param->currentRoiIndex].RecAngle;
		reclen1 = m_param->RoiVec[m_param->currentRoiIndex].RecLength1;
		reclen2 = m_param->RoiVec[m_param->currentRoiIndex].RecLength2;
		cirrow = m_param->RoiVec[m_param->currentRoiIndex].CirRow;
		circol = m_param->RoiVec[m_param->currentRoiIndex].CirCol;
		cirr = m_param->RoiVec[m_param->currentRoiIndex].CirR;
		//sprintf(buf, "roi%d", m_param->currentRoiIndex);
		title = QString("roi%1").arg(m_param->currentRoiIndex);
	}
	else if (inparam == 1) {
		roitype = m_param->MinusVec[m_param->currentMinusIndex].regionType;
		recrow = m_param->MinusVec[m_param->currentMinusIndex].RecRow;
		reccol = m_param->MinusVec[m_param->currentMinusIndex].RecCol;
		recang = m_param->MinusVec[m_param->currentMinusIndex].RecAngle;
		reclen1 = m_param->MinusVec[m_param->currentMinusIndex].RecLength1;
		reclen2 = m_param->MinusVec[m_param->currentMinusIndex].RecLength2;
		cirrow = m_param->MinusVec[m_param->currentMinusIndex].CirRow;
		circol = m_param->MinusVec[m_param->currentMinusIndex].CirCol;
		cirr = m_param->MinusVec[m_param->currentMinusIndex].CirR;
		//sprintf(buf, "min%d", m_param->currentMinusIndex);
		title = QString("minus%1").arg(m_param->currentMinusIndex);
	}
	if (roitype == 0) {
		currentView()->drawRect2(recrow, reccol, recang, reclen1, reclen2, title);
	}
	else {
		currentView()->drawCircle(cirrow, circol, cirr, title);
	}
	if (m_param->IsShowAllRegion) {
		currentView()->setColor(0, 255, 255);
		char roibuf[10][64];
		for (int i = 0;i < m_param->roiCount;i++) {
			if (i == m_param->currentRoiIndex&&inparam == 0)
				continue;
			sprintf_s(roibuf[i], "roi%d", i);
			if (m_param->RoiVec[i].regionType == 0) {
				currentView()->dispRect2(
					m_param->RoiVec[i].RecRow, m_param->RoiVec[i].RecCol, 
					m_param->RoiVec[i].RecAngle, m_param->RoiVec[i].RecLength1,
					m_param->RoiVec[i].RecLength2, roibuf[i]);
			}
			else {
				currentView()->dispCircle(m_param->RoiVec[i].CirRow, m_param->RoiVec[i].CirCol, m_param->RoiVec[i].CirR, roibuf[i]);
			}
		}
		char minbuf[10][64];
		currentView()->setColor(0, 0, 255);
		for (int i = 0;i < m_param->MinusCount;i++) {
			if (i == m_param->currentMinusIndex&&inparam == 1)
				continue;
			sprintf_s(minbuf[i], "min%d", i);
			if (m_param->MinusVec[i].regionType == 0) {
				currentView()->dispRect2(m_param->MinusVec[i].RecRow, m_param->MinusVec[i].RecCol, m_param->MinusVec[i].RecAngle, m_param->MinusVec[i].RecLength1,
					m_param->MinusVec[i].RecLength2, minbuf[i]);
			}
			else {
				currentView()->dispCircle(m_param->MinusVec[i].CirRow, m_param->MinusVec[i].CirCol, m_param->MinusVec[i].CirR, minbuf[i]);
			}
		}
	}
	currentView()->update();
}

void ColorPickerWdg::getRoi(int roiORmin)
{
	currentView()->finishDraw();
	if (roiORmin == 0) {
		if (m_param->RoiVec.size() == 0)
			return;
		if (m_param->RoiVec[m_param->currentRoiIndex].regionType == 0) {
			currentView()->getDrawRect2(m_param->RoiVec[m_param->currentRoiIndex].RecRow, m_param->RoiVec[m_param->currentRoiIndex].RecCol,
				m_param->RoiVec[m_param->currentRoiIndex].RecAngle, m_param->RoiVec[m_param->currentRoiIndex].RecLength1, m_param->RoiVec[m_param->currentRoiIndex].RecLength2);
		}
		else {
			currentView()->getDrawCircle(m_param->RoiVec[m_param->currentRoiIndex].CirRow, m_param->RoiVec[m_param->currentRoiIndex].CirCol,
				m_param->RoiVec[m_param->currentRoiIndex].CirR);
		}
	}
	else {
		if (m_param->MinusVec.size() == 0)
			return;
		if (m_param->MinusVec[m_param->currentMinusIndex].regionType == 0) {
			currentView()->getDrawRect2(m_param->MinusVec[m_param->currentMinusIndex].RecRow, m_param->MinusVec[m_param->currentMinusIndex].RecCol,
				m_param->MinusVec[m_param->currentMinusIndex].RecAngle, m_param->MinusVec[m_param->currentMinusIndex].RecLength1, m_param->MinusVec[m_param->currentMinusIndex].RecLength2);
		}
		else {
			currentView()->getDrawCircle(m_param->MinusVec[m_param->currentMinusIndex].CirRow, m_param->MinusVec[m_param->currentMinusIndex].CirCol,
				m_param->MinusVec[m_param->currentMinusIndex].CirR);
		}
	}
	//currentView()->clear();
	currentView()->clearLayer(m_layerIdx);
	currentView()->update();
}

void ColorPickerWdg::testParam()
{
	if (m_param->roiCount == 0) {
		return;
	}
	if (m_module->p_im->empty()) 
		return; 
	HalOpenCV::cv2halImg(*m_module->p_im, m_image, false);
	Hobject roiregion;
	if (m_param->RoiVec[0].regionType == 0) {
		gen_rectangle2(&roiregion, m_param->RoiVec[0].RecRow, m_param->RoiVec[0].RecCol, m_param->RoiVec[0].RecAngle,
			m_param->RoiVec[0].RecLength1, m_param->RoiVec[0].RecLength2);
	}
	else {
		gen_circle(&roiregion, m_param->RoiVec[0].CirRow, m_param->RoiVec[0].CirCol, m_param->RoiVec[0].CirR);
	}
	Hobject dstImg;
	reduce_domain(*m_image, roiregion, &dstImg);
	Hobject grayArea, lightarea;
	if (m_param->IsDeleteLightArea) {
		rgb1_to_gray(dstImg, &grayArea);
		threshold(grayArea, &lightarea, m_param->lightAreaThreshold, 255);
		connection(lightarea, &lightarea);
		m_module->mselect_shape(lightarea, &lightarea, "area", "and", m_param->lightArea, 99999);
		union1(lightarea, &lightarea);
		Hobject diffRegion;
		difference(dstImg, lightarea, &diffRegion);
		reduce_domain(dstImg, diffRegion, &dstImg);
	}
	HTuple dstArea, dstRow, dstCol;
	area_center(dstImg, &dstArea, &dstRow, &dstCol);
	Hobject redImg, greenImg, blueImg;
	Hobject HueImg, saturImg, IntyImg;
	Hobject saturatedImg, hueSaturated, currentColor, selectColor;
	decompose3(dstImg, &redImg, &greenImg, &blueImg);
	trans_from_rgb(redImg, greenImg, blueImg, &HueImg, &saturImg, &IntyImg, "hsv");
	threshold(saturImg, &saturatedImg, m_param->SaturationMin, m_param->SaturationMax < m_param->SaturationMin ? m_param->SaturationMin : m_param->SaturationMax);
	reduce_domain(HueImg, saturatedImg, &hueSaturated);
	threshold(hueSaturated, &currentColor, m_param->HueMin, m_param->HueMax < m_param->HueMin ? m_param->HueMin : m_param->HueMax);
	connection(currentColor, &currentColor);
	m_module->mselect_shape(currentColor, &selectColor, "area", "and", m_param->areaMin, m_param->areaMax);
	if (m_param->IsHeight) {
		m_module->mselect_shape(selectColor, &selectColor, "height", "and", m_param->heightMin, m_param->heightMax);
	}
	if (m_param->IsWidth) {
		m_module->mselect_shape(selectColor, &selectColor, "width", "and", m_param->widthMin, m_param->widthMax);
	}
	if (m_param->IsFillUpHole) {
		fill_up(selectColor, &selectColor);
	}
	HTuple colorArea, colorRow, colorCol;
	area_center(selectColor, &colorArea, &colorRow, &colorCol);
	int areaCounts = colorArea.Num();
	union1(selectColor, &selectColor);
	//currentView()->clear();
	currentView()->clearLayer(m_layerIdx);
	currentView()->setFilled(false);
	currentView()->setColor(0, 255, 255);
	RlRegion r1, r2;
	HalOpenCV::hal2cvRegion(roiregion, &r1);
	currentView()->dispRegion(&r1, "Roi");
	currentView()->setColor(255, 0, 255);
	currentView()->setFilled(true);
	HalOpenCV::hal2cvRegion(selectColor, &r2);
	currentView()->dispRegion(&r2);
	currentView()->update();
}

void ColorPickerWdg::on_bt_selectROI_toggled(bool check)
{
	if (check) {
		dispRoi(0);
		ui->bt_selectROI->setText(tr("select ROI OK"));
		ui->bt_selectMinusROI->setEnabled(false);
		ui->bt_testing->setEnabled(false);
	}
	else {
		//log(LogL1, ui.bt_selectROI->text());
		getRoi(0);
		ui->bt_selectROI->setText(tr("select ROI"));
		if (m_param->MinusCount != 0) {
			ui->bt_selectMinusROI->setEnabled(true);
		}
		ui->bt_testing->setEnabled(true);

		m_module->setParamChanged(ProductLevel);
	}
}

void ColorPickerWdg::on_bt_selectMinusROI_toggled(bool check)
{
	if (check) {
		dispRoi(1);
		ui->bt_selectMinusROI->setText(tr("Exclusion Ok"));
		ui->bt_selectROI->setEnabled(false);
		ui->bt_testing->setEnabled(false);
	}
	else {
		getRoi(1);
		//log(LogL1, ui.bt_selectMinusROI->text());
		ui->bt_selectMinusROI->setText(tr("Select Exclusion"));
		ui->bt_selectROI->setEnabled(true);
		ui->bt_testing->setEnabled(true);

		m_module->setParamChanged(ProductLevel);
	}
}

void ColorPickerWdg::on_bt_testing_toggled(bool check)
{
	ui->bt_selectMinusROI->setEnabled(!check);
	ui->bt_selectROI->setEnabled(!check);
	m_testing = check;
	if (check) {
		testParam();
		ui->bt_testing->setText(tr("Exit test"));
	}
	else {
		//currentView()->clear();
		currentView()->clearLayer(m_layerIdx);
		currentView()->update();
		ui->bt_testing->setText(tr("Test mode"));
	}
}

void ColorPickerWdg::spinBox_valueChanged(int val)
{
	QSpinBox* sp = qobject_cast<QSpinBox*>(sender());
	QString str0 = QString("");
	QString str1 = QString("");
	if (sp == ui->sp_RoiNum) {
		if (ui->bt_selectROI->isChecked() || (ui->bt_selectMinusROI->isChecked() && m_param->IsShowAllRegion)) {
			getRoi(0);
		}
		ColorPickerParam::segRegion seg;
		seg.regionType = m_param->nowIsRecOrCir;
		seg.RecRow = seg.RecCol = 100;
		seg.RecLength1 = seg.RecLength2 = 40;
		seg.RecAngle = 0;
		seg.CirCol = seg.CirRow = 100;
		seg.CirR = 40;
		if (val >= m_param->roiCount) {
			for (int i = m_param->roiCount;i < val;i++) {
				seg.RecRow += val * 20;
				seg.RecCol += val * 20;
				seg.CirRow += val * 20;
				seg.CirCol += val * 20;
				m_param->RoiVec.push_back(seg);
			}
		}
		else if (val < m_param->roiCount) {
			vector<ColorPickerParam::segRegion>::iterator itbegin = m_param->RoiVec.begin() + val;
			vector<ColorPickerParam::segRegion>::iterator itEnd = m_param->RoiVec.end();
			m_param->RoiVec.erase(itbegin, itEnd);
		}
		str0 = ui->label->text();
		str1 = tr(" change from %1 to %2").arg(m_param->roiCount).arg(val);
		m_param->roiCount = val;
		if (m_param->currentRoiIndex > val - 1) {
			m_param->currentRoiIndex = val - 1;
		}
		if (ui->bt_selectROI->isChecked() || (ui->bt_selectMinusROI&&m_param->IsShowAllRegion)) {
			if (m_param->roiCount > 0) {
				dispRoi(0);
			}
		}
		connectSlots(false);
		ui->cob_currentRoiIndex->clear();
		for (int i = 0;i < m_param->roiCount;i++) {
			ui->cob_currentRoiIndex->addItem(tr("region%1").arg(i));
		}
		ui->cob_currentRoiIndex->setCurrentIndex(m_param->currentRoiIndex);
		connectSlots(true);

		if (val <= 0) {
			if (ui->bt_selectROI->isChecked()) {
				ui->bt_selectROI->setChecked(false);
			}
			ui->cob_currentRoiIndex->setEnabled(false);
			ui->bt_selectROI->setEnabled(false);
		}
		else if (val > 0) {
			ui->cob_currentRoiIndex->setEnabled(true);
			ui->bt_selectROI->setEnabled(true);
		}
	}
	else if (sp == ui->sp_MinusNum) {
		if (ui->bt_selectMinusROI->isChecked() || (ui->bt_selectROI->isChecked() && m_param->IsShowAllRegion)) {
			getRoi(1);
		}
		ColorPickerParam::segRegion seg;
		seg.regionType = m_param->nowIsRecOrCir;
		seg.RecRow = seg.RecCol = 100;
		seg.RecLength1 = seg.RecLength2 = 40;
		seg.RecAngle = 0;
		seg.CirCol = seg.CirRow = 100;
		seg.CirR = 40;
		if (val >= m_param->MinusCount) {
			for (int i = m_param->MinusCount;i < val;i++) {
				seg.RecRow += val * 20;
				seg.RecCol += val * 20;
				seg.CirRow += val * 20;
				seg.CirCol += val * 20;
				m_param->MinusVec.push_back(seg);
			}
		}
		else if (val < m_param->MinusCount) {
			vector<ColorPickerParam::segRegion>::iterator itbegin = m_param->MinusVec.begin() + val;
			vector<ColorPickerParam::segRegion>::iterator itEnd = m_param->MinusVec.end();
			m_param->MinusVec.erase(itbegin, itEnd);
		}
		str0 = ui->label_2->text();
		str1 = tr(" change from %1 to %2").arg(m_param->MinusCount).arg(val);
		m_param->MinusCount = val;
		if (val > 0 && (m_param->currentMinusIndex > val - 1)) {
			m_param->currentMinusIndex = val - 1;
		}
		if (ui->bt_selectMinusROI->isChecked() || (ui->bt_selectROI&&m_param->IsShowAllRegion)) {
			if (m_param->MinusCount > 0) {
				dispRoi(1);
			}
		}
		connectSlots(false);
		ui->cob_currentMinusIndex->clear();
		for (int i = 0;i < m_param->MinusCount;i++) {
			ui->cob_currentMinusIndex->addItem(tr("region%1").arg(i));
		}
		ui->cob_currentMinusIndex->setCurrentIndex(m_param->currentMinusIndex);
		connectSlots(true);
		if (val <= 0) {
			if (ui->bt_selectMinusROI->isChecked()) {
				ui->bt_selectMinusROI->setChecked(false);
			}
			ui->cob_currentMinusIndex->setEnabled(false);
			ui->bt_selectMinusROI->setEnabled(false);
		}
		else if (val > 0) {
			ui->cob_currentMinusIndex->setEnabled(true);
			ui->bt_selectMinusROI->setEnabled(true);
		}
	}
	else if (sp == ui->sp_areaMin) {
		if (val > m_param->areaMax) {
			ui->sp_areaMin->setValue(m_param->areaMax);
			return;
		}
		str0 = ui->label_8->text() + tr(" min val");
		str1 = tr(" change from %1 to %2").arg(m_param->areaMin).arg(val);
		m_param->areaMin = val;
		if (m_testing) {
			testParam();
		}
	}
	else if (sp == ui->sp_areaMax) {
		if (val < m_param->areaMin) {
			ui->sp_areaMax->setValue(m_param->areaMin);
			return;
		}
		str0 = ui->label_8->text() + tr(" max val");
		str1 = tr(" change from %1 to %2").arg(m_param->areaMax).arg(val);
		m_param->areaMax = val;
		if (m_testing) {
			testParam();
		}
	}
	else if (sp == ui->sp_heightMin) {
		if (val > m_param->heightMax) {
			ui->sp_heightMin->setValue(m_param->heightMax);
			return;
		}
		str0 = ui->cb_height->text() + tr(" min val");
		str1 = tr(" change from %1 to %2").arg(m_param->heightMin).arg(val);
		m_param->heightMin = val;
		if (m_testing) {
			testParam();
		}
	}
	else if (sp == ui->sp_heightMax) {
		if (val < m_param->heightMin) {
			ui->sp_heightMax->setValue(m_param->heightMin);
			return;
		}
		str0 = ui->cb_height->text() + tr(" max val");
		str1 = tr(" change from %1 to %2").arg(m_param->heightMax).arg(val);
		m_param->heightMax = val;
		if (m_testing) {
			testParam();
		}
	}
	else if (sp == ui->sp_WidthMin) {
		if (val > m_param->widthMax) {
			ui->sp_WidthMin->setValue(m_param->widthMax);
			return;
		}
		str0 = ui->cb_Width->text() + tr(" min val");
		str1 = tr(" change from %1 to %2").arg(m_param->widthMin).arg(val);
		m_param->widthMin = val;
		if (m_testing) {
			testParam();
		}
	}
	else if (sp == ui->sp_WidthMax) {
		if (val < m_param->widthMin) {
			ui->sp_WidthMax->setValue(m_param->widthMin);
			return;
		}
		str0 = ui->cb_Width->text() + tr(" max val");
		str1 = tr(" change from %1 to %2").arg(m_param->widthMax).arg(val);
		m_param->widthMax = val;
		if (m_testing) {
			testParam();
		}
	}
	else if (sp == ui->sp_saturationMax) {
		if (val < m_param->SaturationMin) {
			ui->sp_saturationMax->setValue(m_param->SaturationMin);
			return;
		}
		str0 = ui->label_6->text() + tr(" max val");
		str1 = tr(" change from %1 to %2").arg(m_param->SaturationMax).arg(val);
		m_param->SaturationMax = val;
		if (m_testing) {
			testParam();
		}
	}
	else if (sp == ui->sp_saturationMin) {
		if (val > m_param->SaturationMax) {
			ui->sp_saturationMin->setValue(m_param->SaturationMax);
			return;
		}
		str0 = ui->label_6->text() + tr(" min val");
		str1 = tr(" change from %1 to %2").arg(m_param->SaturationMin).arg(val);
		m_param->SaturationMin = val;
		if (m_testing) {
			testParam();
		}
	}
	else if (sp == ui->sp_hueMin) {
		if (val > m_param->HueMax) {
			ui->sp_hueMin->setValue(m_param->HueMax);
			return;
		}
		str0 = ui->label_7->text() + tr(" min val");
		str1 = tr(" change from %1 to %2").arg(m_param->HueMin).arg(val);
		m_param->HueMin = val;
		if (m_testing) {
			testParam();
		}
	}
	else if (sp == ui->sp_hueMax) {
		if (val < m_param->HueMin) {
			ui->sp_hueMax->setValue(m_param->HueMin);
			return;
		}
		str0 = ui->label_7->text() + tr(" max val");
		str1 = tr(" change from %1 to %2").arg(m_param->HueMax).arg(val);
		m_param->HueMax = val;
		if (m_testing) {
			testParam();
		}
	}
	else if (sp == ui->sp_open) {
		str0 = ui->cb_open->text();
		str1 = tr(" change from %1 to %2").arg(m_param->openVal).arg(val);
		m_param->openVal = val;
		if (m_testing) {
			testParam();
		}
	}
	else if (sp == ui->sp_close) {
		str0 = ui->cb_close->text();
		str1 = tr(" change from %1 to %2").arg(m_param->closeVal).arg(val);
		m_param->closeVal = val;
		if (m_testing) {
			testParam();
		}
	}
	else if (sp == ui->sp_errAllAreaMin) {
		str0 = ui->cb_errAllArea->text() + tr(" min val");
		str1 = tr(" change from %1 to %2").arg(m_param->errAllMin).arg(val);
		m_param->errAllMin = val;
	}
	else if (sp == ui->sp_errAllAreaMax) {
		str0 = ui->cb_errAllArea->text() + tr(" max val");
		str1 = tr(" change from %1 to %2").arg(m_param->errAllMax).arg(val);
		m_param->errAllMax = val;
	}
	else if (sp == ui->sp_errSingleAreaMin) {
		str0 = ui->cb_errSingleArea->text() + tr(" min val");
		str1 = tr(" change from %1 to %2").arg(m_param->errSingleMin).arg(val);
		m_param->errSingleMin = val;
	}
	else if (sp == ui->sp_errSingleAreaMax) {
		str0 = ui->cb_errSingleArea->text() + tr(" max val");
		str1 = tr(" change from %1 to %2").arg(m_param->errSingleMax).arg(val);
		m_param->errSingleMax = val;
	}
	else if (sp == ui->sp_lightAreaThreshold) {
		str0 = ui->label_3->text();
		str1 = tr(" change from %1 to %2").arg(m_param->lightAreaThreshold).arg(val);
		m_param->lightAreaThreshold = val;
	}
	else if (sp == ui->sp_lightArea) {
		str0 = ui->label_4->text();
		str1 = tr(" change from %1 to %2").arg(m_param->lightArea).arg(val);
		m_param->lightArea = val;
	}
	else if (sp == ui->sp_AreaZoom) {
		str0 = ui->label_19->text();
		str1 = tr(" change from %1 to %2").arg(m_param->AreaZoom).arg(val);
		m_param->AreaZoom = val;
	}
	m_module->setParamChanged(ProductLevel);
	//log(LogL1, str0 + str1);
}

void ColorPickerWdg::checkBox_toggled(bool check)
{
	QCheckBox* cb = qobject_cast<QCheckBox*>(sender());
	QString str0 = QString("");
	if (check)
		str0 = tr(" Check");
	else
		str0 = tr(" Uncheck");
	if (cb == ui->cb_useRefer) {
		m_param->IsReferSection = check;
	}
	else if (cb == ui->cb_drawAllRegion) {
		if (ui->bt_selectMinusROI->isChecked() ||
			ui->bt_selectROI->isChecked()) {
			getRoi(ui->bt_selectMinusROI->isChecked());
			m_param->IsShowAllRegion = check;
			dispRoi(ui->bt_selectMinusROI->isChecked());
		}
		m_param->IsShowAllRegion = check;
	}
	else if (cb == ui->cb_notLess) {
		m_param->IsNotLess = check;
		if (check) {
			ui->cb_errSingleArea->setChecked(false);
		}
	}
	else if (cb == ui->cb_height) {
		m_param->IsHeight = check;
	}
	else if (cb == ui->cb_Width) {
		m_param->IsWidth = check;
	}
	else if (cb == ui->cb_errAllArea) {
		m_param->IsErrAllArea = check;
		ui->sp_errAllAreaMin->setEnabled(check);
		ui->sp_errAllAreaMax->setEnabled(check);
	}
	else if (cb == ui->cb_errSingleArea) {
		m_param->IsErrSingleArea = check;
		ui->sp_errSingleAreaMin->setEnabled(check);
		ui->sp_errSingleAreaMax->setEnabled(check);
		if (check) {
			ui->cb_notLess->setChecked(false);
		}
	}
	else if (cb == ui->cb_showRegionAfter) {
		m_param->IsShowRegionAfterDetect = check;
	}
	else if (cb == ui->cb_deleteLightArea) {
		m_param->IsDeleteLightArea = check;
		ui->sp_lightAreaThreshold->setEnabled(check);
		ui->sp_lightArea->setEnabled(check);
	}
	else if (cb == ui->cb_fillHole) {
		m_param->IsFillUpHole = check;
	}
	else if (cb == ui->cb_open) {
		m_param->IsOpening = check;
	}
	else if (cb == ui->cb_close) {
		m_param->IsClosing = check;
	}
	//log(LogL1, cb->text() + str0);

	m_module->setParamChanged(ProductLevel);
}

void ColorPickerWdg::radioButton_toggled(bool check)
{
	QRadioButton* rb = qobject_cast<QRadioButton*>(sender());
	QString str0 = QString("");
	if (rb == ui->rb_rec) {
		if (check)
			str0 = tr(" change from %1 to %2").arg(ui->rb_cir->text()).arg(ui->rb_rec->text());
		else
			str0 = tr(" change from %1 to %2").arg(ui->rb_rec->text()).arg(ui->rb_cir->text());
		//log(LogL1, str0);
		m_param->nowIsRecOrCir = !check;
	}
	else if (rb == ui->rb_cir) {
		m_param->nowIsRecOrCir = check;
	}
	if (check) {
		QRadioButton* RB = NULL;
		if (m_param->referType == 0)
			RB = ui->rb_useSelfReg;
		else if (m_param->referType == 1)
			RB = ui->rb_useAreaRefer;
		else if (m_param->referType == 2)
			RB = ui->rb_usePointRefer;

		if (rb == ui->rb_useSelfReg) {
			str0 = tr(" change from %1 to %2").arg(RB->text()).arg(rb->text());
			m_param->referType = 0;
		}
		else if (rb == ui->rb_useAreaRefer) {
			str0 = tr(" change from %1 to %2").arg(RB->text()).arg(rb->text());
			m_param->referType = 1;
		}
		else if (rb == ui->rb_usePointRefer) {
			str0 = tr(" change from %1 to %2").arg(RB->text()).arg(rb->text());
			m_param->referType = 2;
		}
		//log(LogL1, str0);
	}

	m_module->setParamChanged(ProductLevel);
}

void ColorPickerWdg::comboBox_IndexChanged(int index)
{
	QComboBox* cob = qobject_cast<QComboBox*>(sender());
	QString str0 = QString("");
	if (cob == ui->cob_currentRoiIndex) {
		if (ui->bt_selectROI->isChecked()) {
			getRoi(0);
		}
		str0 = tr(" ROI index") + tr(" change from %1 to %2").arg(cob->itemText(m_param->currentRoiIndex)).arg(cob->itemText(index));
		m_param->currentRoiIndex = index;
		if (ui->bt_selectROI->isChecked()) {
			dispRoi(0);
		}
	}
	else if (cob == ui->cob_currentMinusIndex) {
		if (ui->bt_selectMinusROI->isChecked()) {
			getRoi(1);
		}
		str0 = tr(" Minus index") + tr(" change from %1 to %2").arg(cob->itemText(m_param->currentMinusIndex)).arg(cob->itemText(index));
		m_param->currentMinusIndex = index;
		if (ui->bt_selectMinusROI->isChecked()) {
			dispRoi(1);
		}
	}
	//log(LogL1, str0);

	m_module->setParamChanged(ProductLevel);
}
