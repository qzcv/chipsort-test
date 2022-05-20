#include "pinLength1Wdg.h"
#include "pinLength1Mod.h"
#include "pinLength1Param.h"
#include "ui_pinLength1Wdg.h"
#include "ImageView.h"
#include "halOpenCV.h"
#include "advanceSet.h"
#include "paramTables.h"
#include "positionSet.h"

pinLength1Wdg::pinLength1Wdg(int i) :
	ModSetWidget(), ui(new Ui::pinLength1Wdg), m_widgetType(i)
{
	m_hasConnect = false;
	ui->setupUi(this);
	iniData();
	iniUi();
}

pinLength1Wdg::~pinLength1Wdg()
{
	delete m_advanceSet;
	delete ui;
}

void pinLength1Wdg::setModule(UnitModule *module)
{
	m_module = dynamic_cast<pinLength1Mod*>(module);
	m_param = m_module->m_param;
	m_image = m_module->m_image;
	//m_paramTables->setGroupLeadNumber(m_param->pinCont[0], m_param->pinCont[1]);
	//m_corrTable->setGroupLeadNumber(m_param->pinCont[0], m_param->pinCont[1]);
	//currentView()->setLayer(m_layerIdx);
	//updateUi();
}

void pinLength1Wdg::showEvent(QShowEvent * event)
{
	updateUi();
}

void pinLength1Wdg::connectSlots(bool link)
{
	if (m_hasConnect == link)
		return;
	m_hasConnect = link;

	connectOrNot(link, ui->bt_testPin, SIGNAL(toggled(bool)), SLOT(bt_toggled(bool)));
	connectOrNot(link, ui->bt_selectPin, SIGNAL(toggled(bool)), SLOT(bt_toggled(bool)));
	connectOrNot(link, ui->bt_selectRoi, SIGNAL(toggled(bool)), SLOT(bt_toggled(bool)));
	connectOrNot(link, ui->bt_selectRotate, SIGNAL(toggled(bool)), SLOT(bt_toggled(bool)));
	connectOrNot(link, ui->bt_selectSearch, SIGNAL(toggled(bool)), SLOT(bt_toggled(bool)));

	connectOrNot(link, ui->rb_grp0, SIGNAL(toggled(bool)), SLOT(radioButton_toggled(bool)));
	connectOrNot(link, ui->rb_grp1, SIGNAL(toggled(bool)), SLOT(radioButton_toggled(bool)));
	connectOrNot(link, ui->rb_2angle, SIGNAL(toggled(bool)), SLOT(radioButton_toggled(bool)));
	connectOrNot(link, ui->rb_4angle, SIGNAL(toggled(bool)), SLOT(radioButton_toggled(bool)));
	connectOrNot(link, m_advanceSet->ui.rb_LFRT, SIGNAL(toggled(bool)), SLOT(radioButton_toggled(bool)));
	connectOrNot(link, m_advanceSet->ui.rb_UPDW, SIGNAL(toggled(bool)), SLOT(radioButton_toggled(bool)));

	connectOrNot(link, m_advanceSet->ui.rb_SOP, SIGNAL(toggled(bool)), SLOT(radioButton_toggled(bool)));
	connectOrNot(link, m_advanceSet->ui.rb_QFN, SIGNAL(toggled(bool)), SLOT(radioButton_toggled(bool)));
	connectOrNot(link, m_positionSet->ui.rb_EdgeSOP, SIGNAL(toggled(bool)), SLOT(radioButton_toggled(bool)));
	connectOrNot(link, m_positionSet->ui.rb_EdgeQFN, SIGNAL(toggled(bool)), SLOT(radioButton_toggled(bool)));

	connectOrNot(link, m_advanceSet->ui.sp_openAndClosing, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, m_positionSet->ui.sp_up, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, m_positionSet->ui.sp_down, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, m_positionSet->ui.sp_left, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, m_positionSet->ui.sp_right, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));

	connectOrNot(link, m_advanceSet->ui.sp_roioffUp, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, m_advanceSet->ui.sp_roioffDown, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, m_advanceSet->ui.sp_roioffLeft, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, m_advanceSet->ui.sp_roioffRight, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, m_advanceSet->ui.sp_crossGray, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, m_advanceSet->ui.sp_crossStep, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));

	connectOrNot(link, ui->sp_pinContLfUp, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_pinContRtDw, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_dirtyMin, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_widthMin, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, m_advanceSet->ui.sp_pinSize, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_threshold, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_contrast, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, m_advanceSet->ui.sp_widthOff, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, m_advanceSet->ui.sp_leadLossIndex, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));

	connectOrNot(link, ui->dsp_ratio0, SIGNAL(valueChanged(double)), SLOT(douSpinBox_valueChanged(double)));
	connectOrNot(link, ui->dsp_ratio1, SIGNAL(valueChanged(double)), SLOT(douSpinBox_valueChanged(double)));

	connectOrNot(link, ui->hs_threshold, SIGNAL(valueChanged(int)), SLOT(slider_valueChanged(int)));

	connectOrNot(link, m_advanceSet->ui.cb_inputRatio, SIGNAL(toggled(bool)), SLOT(checkBox_valueChanged(bool)));
	connectOrNot(link, m_advanceSet->ui.cb_showAfterDet, SIGNAL(toggled(bool)), SLOT(checkBox_valueChanged(bool)));
	connectOrNot(link, m_advanceSet->ui.cb_NoDetectTop, SIGNAL(toggled(bool)), SLOT(checkBox_valueChanged(bool)));
	connectOrNot(link, m_advanceSet->ui.cb_useInputRegion, SIGNAL(toggled(bool)), SLOT(checkBox_valueChanged(bool)));

	connectOrNot(link, ui->cb_useDir, SIGNAL(toggled(bool)), SLOT(checkBox_valueChanged(bool)));
	connectOrNot(link, m_positionSet->ui.cb_useRefer, SIGNAL(toggled(bool)), SLOT(checkBox_valueChanged(bool)));
	connectOrNot(link, m_positionSet->ui.cb_referEdgeRev, SIGNAL(toggled(bool)), SLOT(checkBox_valueChanged(bool)));
	connectOrNot(link, m_positionSet->ui.cb_interUseEdgeAngle, SIGNAL(toggled(bool)), SLOT(checkBox_valueChanged(bool)));
	connectOrNot(link, m_positionSet->ui.cb_onlyReferAngle_0, SIGNAL(toggled(bool)), SLOT(checkBox_valueChanged(bool)));
	connectOrNot(link, m_positionSet->ui.cb_onlyReferAngle_1, SIGNAL(toggled(bool)), SLOT(checkBox_valueChanged(bool)));

	connectOrNot(link, ui->cb_delOffSearchROI, SIGNAL(toggled(bool)), SLOT(checkBox_valueChanged(bool)));

	connectOrNot(link, m_positionSet->ui.cb_usePinAreaRefer, SIGNAL(toggled(bool)), SLOT(checkBox_valueChanged(bool)));
	connectOrNot(link, m_advanceSet->ui.cb_IgnoreRoot, SIGNAL(toggled(bool)), SLOT(checkBox_valueChanged(bool)));
	connectOrNot(link, m_advanceSet->ui.cb_whitePin, SIGNAL(toggled(bool)), SLOT(checkBox_valueChanged(bool)));
	connectOrNot(link, m_advanceSet->ui.cb_groupSame, SIGNAL(toggled(bool)), SLOT(checkBox_valueChanged(bool)));
	connectOrNot(link, m_advanceSet->ui.cb_crossGray, SIGNAL(toggled(bool)), SLOT(checkBox_valueChanged(bool)));

	connectOrNot(link, ui->cb_threshold, SIGNAL(toggled(bool)), SLOT(checkBox_valueChanged(bool)));
	connectOrNot(link, ui->cb_errWidth, SIGNAL(toggled(bool)), SLOT(checkBox_valueChanged(bool)));
	connectOrNot(link, ui->cb_errLength, SIGNAL(toggled(bool)), SLOT(checkBox_valueChanged(bool)));
	connectOrNot(link, ui->cb_errInterval, SIGNAL(toggled(bool)), SLOT(checkBox_valueChanged(bool)));
	connectOrNot(link, ui->cb_errMaxDiff, SIGNAL(toggled(bool)), SLOT(checkBox_valueChanged(bool)));
	connectOrNot(link, ui->cb_errAllMaxDiff, SIGNAL(toggled(bool)), SLOT(checkBox_valueChanged(bool)));
	connectOrNot(link, ui->cb_errSpan, SIGNAL(toggled(bool)), SLOT(checkBox_valueChanged(bool)));

	connectOrNot(link, ui->cob_startAngle, SIGNAL(currentIndexChanged(int)), SLOT(comboBox_Index_changed(int)));

	connectOrNot(link, m_paramTables, SIGNAL(CellChanged(int, int, int, bool, double)), SLOT(tableWidget_valueChanged(int, int, int, bool, double)));
	connectOrNot(link, m_corrTable, SIGNAL(CellChanged(int, int, int, bool, double)), SLOT(tableWidget_valueChanged(int, int, int, bool, double)));

	connectOrNot(link, m_paramTables, SIGNAL(CopyItems(bool, int, int, int, int)), SLOT(tableWidget_CopyItems(bool, int, int, int, int)));
	connectOrNot(link, m_corrTable, SIGNAL(CopyItems(bool, int, int, int, int)), SLOT(tableWidget_CopyItems(bool, int, int, int, int)));
}

void pinLength1Wdg::iniUi()
{

}

void pinLength1Wdg::iniData()
{
	m_param = nullptr;
	m_module = nullptr;
	m_image = nullptr;
	m_paramTables = nullptr;
	m_layerIdx = 0;
	m_grpIndex = 0;
	m_selState = UnSel;
	m_advanceSet = new AdvanceSet(this);
	m_positionSet = new PositionSet(this);

	QStringList cbname;
	cbname << tr("length") << tr("width") << tr("interval") << tr("span") << tr("length diff") << tr("all length diff");

	QStringList corrname;
	corrname << tr("length offset") << tr("width offset") << tr("interval offset") << tr("span offset");

	m_dialog = new QDialog(this);
	m_corrDialog = new QDialog(this);

	m_dialog->setMinimumSize(910, 550);
	m_dialog->setWindowTitle(tr("range of param"));

	m_corrDialog->setMinimumSize(910, 350);
	m_corrDialog->setWindowTitle(tr("range of corr param"));


	QVBoxLayout* V1Lay = new QVBoxLayout;
	QVBoxLayout* V2Lay = new QVBoxLayout;
	QGroupBox *grpParam, *grpCorr;
	QVBoxLayout *grpV1, *grpV2;
	grpV1 = new QVBoxLayout;
	grpV2 = new QVBoxLayout;
	grpParam = new QGroupBox();
	grpCorr = new QGroupBox();
	grpParam->setMinimumWidth(900);
	grpCorr->setMinimumWidth(900);

	vector<ParamTables::ParamType> cbtype;
	for (int i = 0;i < cbname.size();i++) {
		ParamTables::ParamType tmptype = ParamTables::singleParam;
		cbtype.push_back(tmptype);
	}
	cbtype[4] = ParamTables::groupParam;
	cbtype[5] = ParamTables::sameParam;
	m_paramTables = new ParamTables(cbname.size(), cbtype, false, true, MAX_PINGRP_NUM, MAX_PINGRP_NUM, cbname, grpParam);
	m_paramTables->setHeadName(tr("param range"));
	m_paramTables->setTableHeight(500);

	vector<ParamTables::ParamType> corrtype;
	for (int i = 0;i < corrname.size();i++) {
		ParamTables::ParamType tmptype = ParamTables::singleParam;
		corrtype.push_back(tmptype);
	}
	m_corrTable = new ParamTables(corrname.size(), corrtype, false, false, MAX_PINGRP_NUM, MAX_PINGRP_NUM, corrname, grpCorr);
	m_corrTable->setValueRange(-999.999, 999.999, 3);
	m_corrTable->setHeadName(tr("compensation"));
	m_corrTable->setTableHeight(300);
	grpV1->addWidget(grpParam);
	grpV2->addWidget(grpCorr);
	V1Lay->addLayout(grpV1, 1);
	V2Lay->addLayout(grpV2, 1);
	m_dialog->setLayout(V1Lay);
	m_corrDialog->setLayout(V2Lay);
}

void pinLength1Wdg::setUiValue()
{
	if (!m_param)
		return;

	m_paramTables->setGroupLeadNumber(m_param->pinCont[0], m_param->pinCont[1]);
	m_corrTable->setGroupLeadNumber(m_param->pinCont[0], m_param->pinCont[1]);
	refreshTableWidget();

	ui->rb_grp0->setChecked(true);

	ui->rb_grp0->setVisible(!m_param->IsGroupEnableSame);
	ui->rb_grp1->setVisible(!m_param->IsGroupEnableSame);
	if (m_param->IsGroupEnableSame) {
		m_param->errWidthValid[1] = m_param->errWidthValid[0];
		m_param->errLengthValid[1] = m_param->errLengthValid[0];
		m_param->errInterValid[1] = m_param->errInterValid[0];
		m_param->errMaxDiffValid[1] = m_param->errMaxDiffValid[0];
		m_param->errSpanValid = m_param->errSpanValid;
	}
	m_advanceSet->ui.cb_groupSame->setChecked(m_param->IsGroupEnableSame);
	m_advanceSet->ui.cb_inputRatio->setChecked(m_param->IsInputSize);
	m_advanceSet->ui.cb_showAfterDet->setChecked(m_param->IsShowAfterDet);
	m_advanceSet->ui.cb_NoDetectTop->setChecked(m_param->IsNoDetectTop);
	m_advanceSet->ui.cb_useInputRegion->setChecked(m_param->IsUseInputRegion);
	m_advanceSet->ui.cb_crossGray->setChecked(m_param->CrossGrayValid);
	if (m_param->AngleType == 0) {
		ui->rb_2angle->setChecked(1);
	}
	else if (m_param->AngleType == 1) {
		ui->rb_4angle->setChecked(1);
	}

	if (m_param->LRorUD == 0) {
		m_advanceSet->ui.rb_LFRT->setChecked(1);
	}
	else if (m_param->LRorUD == 1) {
		m_advanceSet->ui.rb_UPDW->setChecked(1);
	}
	if (m_param->IsSOPType == 0) {
		m_advanceSet->ui.rb_QFN->setChecked(1);
	}
	else if (m_param->IsSOPType == 1) {
		m_advanceSet->ui.rb_SOP->setChecked(1);
	}

	if (m_param->IsEdgeSopType == 0) {
		m_positionSet->ui.rb_EdgeQFN->setChecked(1);
		m_positionSet->ui.grp_qfn->setVisible(1);
	}
	else if (m_param->IsEdgeSopType == 1) {
		m_positionSet->ui.rb_EdgeSOP->setChecked(1);
		m_positionSet->ui.grp_qfn->setVisible(0);
	}

	ui->cb_useDir->setChecked(m_param->IsUseDir);
	ui->cb_delOffSearchROI->setChecked(m_param->IsDelOffSearchRoi);
	m_positionSet->ui.cb_useRefer->setChecked(m_param->IsReferSection);
	m_positionSet->ui.cb_onlyReferAngle_0->setChecked(m_param->IsOnlyReferEdge[0]);
	m_positionSet->ui.cb_onlyReferAngle_1->setChecked(m_param->IsOnlyReferEdge[1]);

	m_positionSet->ui.cb_interUseEdgeAngle->setChecked(m_param->IsInterUseEdgeAngle);
	m_positionSet->ui.cb_referEdgeRev->setChecked(m_param->IsReferEdgeRev);
	m_positionSet->ui.cb_usePinAreaRefer->setChecked(m_param->PinAreaRefer);
	m_positionSet->ui.sp_up->setValue(m_param->offsetUp);
	m_positionSet->ui.sp_down->setValue(m_param->offsetDown);
	m_positionSet->ui.sp_left->setValue(m_param->offsetLeft);
	m_positionSet->ui.sp_right->setValue(m_param->offsetRight);
	ui->sp_pinContLfUp->setValue(m_param->pinCont[0]);
	ui->sp_pinContRtDw->setValue(m_param->pinCont[1]);
	ui->sp_dirtyMin->setValue(m_param->leadAreaMin);
	ui->sp_contrast->setValue(m_param->leadContrast);
	m_advanceSet->ui.sp_widthOff->setValue(m_param->widthOffset);
	m_advanceSet->ui.sp_leadLossIndex->setValue(m_param->leadLossIndex);
	m_advanceSet->ui.sp_crossStep->setValue(m_param->crossStep);
	m_advanceSet->ui.sp_crossGray->setValue(m_param->crossGray);

	m_advanceSet->ui.sp_openAndClosing->setValue(m_param->openAndClosing);

	ui->sp_widthMin->setValue(m_param->widthMin);

	ui->cb_width->setChecked(m_param->IsWidth);
	m_advanceSet->ui.sp_pinSize->setValue(m_param->pinSize);

	m_advanceSet->ui.sp_roioffUp->setValue(m_param->roiOffUp);
	m_advanceSet->ui.sp_roioffDown->setValue(m_param->roiOffDown);
	m_advanceSet->ui.sp_roioffLeft->setValue(m_param->roiOffLeft);
	m_advanceSet->ui.sp_roioffRight->setValue(m_param->roiOffRight);

	ui->sp_threshold->setValue(m_param->threValue);
	ui->hs_threshold->setValue(m_param->threValue);

	ui->dsp_ratio0->setValue(m_param->SizeRation[0]);
	ui->dsp_ratio1->setValue(m_param->SizeRation[1]);

	ui->tabWidget_2->setCurrentIndex(0);

	changeErrGroup();

	m_advanceSet->ui.cb_IgnoreRoot->setChecked(m_param->IsIgnoreRoot);
	m_advanceSet->ui.cb_whitePin->setChecked(m_param->IsWhitePin);
	ui->cb_threshold->setChecked(m_param->IsThreshold);
	//ui.hs_threshold->setEnabled(!m_param->IsThreshold);
	//ui.sp_threshold->setEnabled(!m_param->IsThreshold);
	ui->cb_errAllMaxDiff->setChecked(m_param->errAllMaxDiffValid);
	ui->grp_high->setVisible(false);
	ui->tabWidget->setCurrentIndex(0);
	if (ui->bt_advanceLead->isChecked()) {
		ui->bt_advanceLead->setChecked(false);
	}

	ui->cob_startAngle->setCurrentIndex(m_param->startAngle);
}

void pinLength1Wdg::updateUi()
{
	connectSlots(false);
	setUiValue();
	connectSlots(true);
}

void pinLength1Wdg::drawTestPin()
{
	currentView()->finishDraw();
	//currentView()->clear();
	currentView()->clearLayer(m_layerIdx);
	currentView()->setColor(255, 255, 0);
	currentView()->setFilled(false);

	HalOpenCV::cv2halImg(*m_module->p_im, m_image, false);

	int DrawCount = m_param->pinCont[0] + m_param->pinCont[1];
	double DrawRow1[40], DrawRow2[40], DrawCol1[40], DrawCol2[40];
	//	const char* title[40] = {};
	//	char roibuf[40][32];
	Hobject segImg;
	for (int i = 0;i < DrawCount;i++) {
		DrawRow1[i] = m_param->segRegionVector[i + 2].row1;
		DrawCol1[i] = m_param->segRegionVector[i + 2].col1;
		DrawRow2[i] = m_param->segRegionVector[i + 2].row2;
		DrawCol2[i] = m_param->segRegionVector[i + 2].col2;
		// 		if (i < m_param->pinCont[0]) {
		// 			if (!m_param->LRorUD) {
		// 				sprintf(roibuf[i], "左脚%d", i + 1);
		// 			}
		// 			else {
		// 				sprintf(roibuf[i], "上脚%d", i + 1);
		// 			}
		// 			title[i] = roibuf[i];
		// 		}
		// 		else {
		// 			if (!m_param->LRorUD) {
		// 				sprintf(roibuf[i], "右脚%d", i + 1 - m_param->pinCont[0]);
		// 			}
		// 			else {
		// 				sprintf(roibuf[i], "下脚%d", i + 1 - m_param->pinCont[0]);
		// 			}
		// 			title[i] = roibuf[i];
		// 		}
		Hobject threRec, tmpImg;
		gen_rectangle1(&threRec, DrawRow1[i], DrawCol1[i], DrawRow2[i], DrawCol2[i]);
		reduce_domain(*m_image, threRec, &tmpImg);
		if (m_param->IsWhitePin) {
			threshold(tmpImg, &tmpImg, m_param->threValue, 255);
		}
		else {
			threshold(tmpImg, &tmpImg, 0, m_param->threValue);
		}
		if (m_param->openAndClosing > 0) {
			int vall = m_param->openAndClosing;
			closing_rectangle1(tmpImg, &tmpImg, vall * 2, vall * 2);
			opening_rectangle1(tmpImg, &tmpImg, vall, vall);
		}
		connection(tmpImg, &tmpImg);
		select_shape(tmpImg, &tmpImg, "area", "and", m_param->leadAreaMin, 999999);
		if (m_param->IsWidth) {
			select_shape(tmpImg, &tmpImg, "width", "and", m_param->widthMin, 9999);
			select_shape(tmpImg, &tmpImg, "height", "and", m_param->widthMin, 9999);
		}
		union1(tmpImg, &tmpImg);
		union2(tmpImg, segImg, &segImg);
		currentView()->dispRect1(DrawRow1[i], DrawCol1[i], DrawRow2[i], DrawCol2[i]);
	}
	currentView()->setColor(0, 255, 0);
	currentView()->setFilled(true);
	RlRegion reg;
	HalOpenCV::hal2cvRegion(segImg, &reg);
	currentView()->dispRegion(&reg);
	currentView()->update();
}

void pinLength1Wdg::changeErrGroup()
{
	int tmpidx;
	tmpidx = m_param->IsGroupEnableSame == 1 ? 0 : m_grpIndex;
	ui->cb_errWidth->setChecked(m_param->errWidthValid[tmpidx]);
	ui->cb_errLength->setChecked(m_param->errLengthValid[tmpidx]);
	ui->cb_errInterval->setChecked(m_param->errInterValid[tmpidx]);
	ui->cb_errMaxDiff->setChecked(m_param->errMaxDiffValid[tmpidx]);
	ui->cb_errSpan->setChecked(m_param->errSpanValid);
}

void pinLength1Wdg::changeLeadDir(bool isLRDW)
{
	bool check = isLRDW;
	m_param->segRegionVector.clear();
	pinLength1Param::segRegion seg;
	if (currentView() && this->isVisible()) {
		if (ui->bt_selectRoi->isChecked()) {
			currentView()->finishDraw();
			//currentView()->clear();
			currentView()->clearLayer(m_layerIdx);
		}
	}
	if (check) {
		seg.row1 = seg.col1 = 50;
		seg.row2 = 100;
		seg.col2 = 150;
		m_param->segRegionVector.push_back(seg);
		seg.row1 = 120;
		seg.col1 = 50;
		seg.row2 = 170;
		seg.col2 = 150;
		m_param->segRegionVector.push_back(seg);
		seg.row1 = 60;
		seg.col1 = 50;
		seg.row2 = 90;
		seg.col2 = 80;
		for (int i = 0;i < m_param->pinCont[0];i++) {
			seg.col1 += 10;
			seg.col2 += 10;
			m_param->segRegionVector.push_back(seg);
		}
		seg.row1 = 130;
		seg.col1 = 50;
		seg.row2 = 160;
		seg.col2 = 80;
		for (int i = 0;i < m_param->pinCont[1];i++) {
			seg.col1 += 10;
			seg.col2 += 10;
			m_param->segRegionVector.push_back(seg);
		}
	}
	else {
		seg.row1 = seg.col1 = 50;
		seg.row2 = 150;
		seg.col2 = 100;
		m_param->segRegionVector.push_back(seg);
		seg.row1 = 50;
		seg.col1 = 120;
		seg.row2 = 150;
		seg.col2 = 170;
		m_param->segRegionVector.push_back(seg);
		seg.row1 = 50;
		seg.col1 = 60;
		seg.row2 = 80;
		seg.col2 = 90;
		for (int i = 0;i < m_param->pinCont[0];i++) {
			seg.row1 += 10;
			seg.row2 += 10;
			m_param->segRegionVector.push_back(seg);
		}
		seg.row1 = 50;
		seg.col1 = 130;
		seg.row2 = 80;
		seg.col2 = 160;
		for (int i = 0;i < m_param->pinCont[1];i++) {
			seg.row1 += 10;
			seg.row2 += 10;
			m_param->segRegionVector.push_back(seg);
		}
	}
	if (currentView() && this->isVisible() && ui->bt_selectRoi->isChecked()) {
		double DrawRow1[2], DrawRow2[2], DrawCol1[2], DrawCol2[2];
		QStringList title;
		for (int i = 0;i < 2;i++) {
			DrawRow1[i] = m_param->segRegionVector[i].row1;
			DrawCol1[i] = m_param->segRegionVector[i].col1;
			DrawRow2[i] = m_param->segRegionVector[i].row2;
			DrawCol2[i] = m_param->segRegionVector[i].col2;
			if (!m_param->LRorUD) {
				if (i == 0) {
					title << tr("Left Roi");
					//sprintf(buf[i], "左区域");
				}
				else if (i == 1) {
					title << tr("Right Roi");
					//sprintf(buf[i], "右区域");
				}
			}
			else {
				if (i == 0) {
					title << tr("Up Roi");
					//sprintf(buf[i], "上区域");
				}
				else if (i == 1) {
					title << tr("Down Roi");
					//sprintf(buf[i], "下区域");
				}
			}
		}
		currentView()->setFilled(false);
		currentView()->setColor(255, 60, 255);
		currentView()->drawRects1(2, DrawRow1, DrawCol1, DrawRow2, DrawCol2, title);
		currentView()->setColor(255, 255, 0);
		currentView()->update();
	}
}

void pinLength1Wdg::refreshTableWidget()
{
	for (int i = 0;i < 2;i++) {
		for (int j = 0;j < m_param->pinCont[i];j++) {
			m_paramTables->setValue(0, i, j, 0, m_param->errLengthMin[i][j]);
			m_paramTables->setValue(0, i, j, 1, m_param->errLengthMax[i][j]);
			m_paramTables->setValue(1, i, j, 0, m_param->errWidthMin[i][j]);
			m_paramTables->setValue(1, i, j, 1, m_param->errWidthMax[i][j]);
			m_paramTables->setValue(2, i, j, 0, m_param->errInterMin[i][j]);
			m_paramTables->setValue(2, i, j, 1, m_param->errInterMax[i][j]);
			m_paramTables->setValue(3, i, j, 0, m_param->errSpanMin[j]);
			m_paramTables->setValue(3, i, j, 1, m_param->errSpanMax[j]);

			m_corrTable->setValue(0, i, j, 0, m_param->LeadLengthCorrNew[i][j]);
			m_corrTable->setValue(1, i, j, 0, m_param->LeadWidthCorrNew[i][j]);
			m_corrTable->setValue(2, i, j, 0, m_param->LeadInterCorrNew[i][j]);
			m_corrTable->setValue(3, i, j, 0, m_param->LeadSpanCorr[j]);
		}
		m_paramTables->setValue(4, i, 0, 0, m_param->errMaxDiffMin[i]);
		m_paramTables->setValue(4, i, 0, 1, m_param->errMaxDiff[i]);
	}
	m_paramTables->setValue(5, 0, 0, 0, m_param->errAllMaxDiffMin);
	m_paramTables->setValue(5, 0, 0, 1, m_param->errAllMaxDiff);
}

void pinLength1Wdg::bt_selectRoi_toggled(bool check)
{
	//log(LogL1, ui->bt_selectRoi->text());
	QString str0 = ui->bt_selectRoi->text();
	if (check) {
		if (m_selState != UnSel) {
			disconnect(ui->bt_selectRoi, SIGNAL(toggled(false)), this, SLOT(bt_toggled(bool)));
			ui->bt_selectRoi->setChecked(false);
			connect(ui->bt_selectRoi, SIGNAL(toggled(false)), SLOT(bt_toggled(bool)));
			return;
		}
		m_selState = SelSegRegion;
		if (!m_param->IsThreshold) {
			ui->sp_pinContLfUp->setEnabled(!check);
			ui->sp_pinContRtDw->setEnabled(!check);
		}
		currentView()->clearLayer(m_layerIdx);
		ui->bt_selectRoi->setText(tr("ROI OK"));
		double DrawRow1[2], DrawRow2[2], DrawCol1[2], DrawCol2[2];
		QStringList title;
		if (!m_param->LRorUD)
			title << tr("Left") << tr("Right");
		else
			title << tr("Up") << tr("Down");
		for (int i = 0;i < 2;i++) {
			DrawRow1[i] = m_param->segRegionVector[i].row1;
			DrawCol1[i] = m_param->segRegionVector[i].col1;
			DrawRow2[i] = m_param->segRegionVector[i].row2;
			DrawCol2[i] = m_param->segRegionVector[i].col2;
		}
		currentView()->setFilled(false);
		currentView()->setColor(255, 60, 255);
		currentView()->drawRects1(2, DrawRow1, DrawCol1, DrawRow2, DrawCol2, title);
		currentView()->setColor(255, 255, 0);
		currentView()->update();
	}
	else if (!check) {
		m_selState = UnSel;
		ui->bt_selectRoi->setText(tr("Select ROI"));
		currentView()->clearLayer(m_layerIdx);
		currentView()->finishDraw();
		double DrawRow1[2], DrawRow2[2], DrawCol1[2], DrawCol2[2];
		currentView()->getDrawRects1(2, DrawRow1, DrawCol1, DrawRow2, DrawCol2);
		for (int i = 0;i < 2;i++) {
			m_param->segRegionVector[i].row1 = DrawRow1[i];
			m_param->segRegionVector[i].col1 = DrawCol1[i];
			m_param->segRegionVector[i].row2 = DrawRow2[i];
			m_param->segRegionVector[i].col2 = DrawCol2[i];
		}
		currentView()->setColor(255, 255, 0);
		currentView()->setFilled(false);
		QStringList title;
		if (m_param->LRorUD)
			title << tr("Up") << tr("Down");
		else
			title << tr("Left") << tr("Right");
		currentView()->dispRect1(
			m_param->segRegionVector[0].row1, m_param->segRegionVector[0].col1,
			m_param->segRegionVector[0].row2, m_param->segRegionVector[0].col2, title[0]);
		currentView()->dispRect1(
			m_param->segRegionVector[1].row1, m_param->segRegionVector[1].col1,
			m_param->segRegionVector[1].row2, m_param->segRegionVector[1].col2, title[1]);
		currentView()->update();
	}

	//log(LogL1, str0);
}

void pinLength1Wdg::bt_selectPin_toggled(bool check)
{
	QString str0 = ui->bt_selectPin->text();
	if (check) {
		if (m_selState != UnSel) {
			disconnect(ui->bt_selectPin, SIGNAL(toggled(bool)), this, SLOT(bt_toggled(bool)));
			ui->bt_selectPin->setChecked(false);
			connect(ui->bt_selectPin, SIGNAL(toggled(bool)), this, SLOT(bt_toggled(bool)));
			return;
		}
		m_selState = SelLead;
		ui->cb_threshold->setEnabled(false);
		ui->bt_testPin->setEnabled(false);
		ui->bt_selectPin->setText(tr("Lead OK"));
		currentView()->finishDraw();
		currentView()->clearLayer(m_layerIdx);
		currentView()->setColor(255, 255, 0);
		currentView()->setFilled(false);
		QStringList title;
		if (m_param->LRorUD)
			title << tr("Up") << tr("Down");
		else
			title << tr("Left") << tr("Right");
		currentView()->dispRect1(
			m_param->segRegionVector[0].row1, m_param->segRegionVector[0].col1,
			m_param->segRegionVector[0].row2, m_param->segRegionVector[0].col2, title[0]);
		currentView()->dispRect1(
			m_param->segRegionVector[1].row1, m_param->segRegionVector[1].col1,
			m_param->segRegionVector[1].row2, m_param->segRegionVector[1].col2, title[1]);
		int DrawCount = m_param->pinCont[0] + m_param->pinCont[1];
		double DrawRow1[40], DrawRow2[40], DrawCol1[40], DrawCol2[40];
		QStringList titles;
		for (int i = 0;i < DrawCount;i++) {
			DrawRow1[i] = m_param->segRegionVector[i + 2].row1;
			DrawCol1[i] = m_param->segRegionVector[i + 2].col1;
			DrawRow2[i] = m_param->segRegionVector[i + 2].row2;
			DrawCol2[i] = m_param->segRegionVector[i + 2].col2;
			if (i < m_param->pinCont[0]) {
				if (!m_param->LRorUD) {
					titles << tr("LeftLead%1").arg(i + 1);
					//sprintf(roibuf[i], "LeftLead%d", i + 1);
				}
				else {
					titles << tr("UpLead%1").arg(i + 1);
					//sprintf(roibuf[i], "UpLead%d", i + 1);
				}
			}
			else {
				if (!m_param->LRorUD) {
					titles << tr("RightLead%1").arg(i + 1 - m_param->pinCont[0]);
					//sprintf(roibuf[i], "RightLead%d", i + 1 - m_param->pinCont[0]);
				}
				else {
					titles << tr("DownLead%1").arg(i + 1 - m_param->pinCont[0]);
					//sprintf(roibuf[i], "DownLead%d", i + 1 - m_param->pinCont[0]);
				}
			}
		}
		currentView()->setFilled(false);
		currentView()->setColor(255, 60, 255);
		currentView()->drawRects1(DrawCount, DrawRow1, DrawCol1, DrawRow2, DrawCol2, titles);
		currentView()->setColor(255, 255, 0);
		currentView()->update();
	}
	else if (!check) {
		m_selState = UnSel;
		ui->bt_selectRoi->setEnabled(true);
		ui->cb_threshold->setEnabled(true);
		ui->bt_testPin->setEnabled(true);
		ui->bt_selectPin->setText(tr("Select Lead"));
		currentView()->clearLayer(m_layerIdx);
		currentView()->finishDraw();
		double DrawRow1[40], DrawRow2[40], DrawCol1[40], DrawCol2[40];
		int DrawCount = m_param->pinCont[0] + m_param->pinCont[1];
		currentView()->getDrawRects1(DrawCount, DrawRow1, DrawCol1, DrawRow2, DrawCol2);
		for (int i = 0;i < DrawCount;i++) {
			m_param->segRegionVector[i + 2].row1 = DrawRow1[i];
			m_param->segRegionVector[i + 2].col1 = DrawCol1[i];
			m_param->segRegionVector[i + 2].row2 = DrawRow2[i];
			m_param->segRegionVector[i + 2].col2 = DrawCol2[i];
		}
		currentView()->setColor(255, 255, 0);
		currentView()->setFilled(false);
		QStringList title;
		if (m_param->LRorUD)
			title << tr("Up") << tr("Down");
		else
			title << tr("Left") << tr("Right");
		currentView()->dispRect1(
			m_param->segRegionVector[0].row1, m_param->segRegionVector[0].col1,
			m_param->segRegionVector[0].row2, m_param->segRegionVector[0].col2, title[0]);
		currentView()->dispRect1(
			m_param->segRegionVector[1].row1, m_param->segRegionVector[1].col1,
			m_param->segRegionVector[1].row2, m_param->segRegionVector[1].col2, title[1]);
		currentView()->update();
	}

	//log(LogL1, str0);
}

void pinLength1Wdg::bt_testPin_toggled(bool check)
{
	//log(LogL1, ui->bt_testPin->text());
	if (check) {
		ui->bt_selectRoi->setEnabled(false);
		ui->bt_selectPin->setEnabled(false);
		ui->bt_testPin->setText(tr("exit test"));
		drawTestPin();
	}
	else {
		ui->bt_selectRoi->setEnabled(true);
		ui->bt_selectPin->setEnabled(true);
		ui->bt_testPin->setText(tr("test lead"));
		//currentView()->finishDraw();
		currentView()->update();
	}
}

void pinLength1Wdg::bt_toggled(bool checked)
{
	QPushButton *bt = qobject_cast<QPushButton *>(sender());

	if (bt == ui->bt_selectRoi)
		bt_selectRoi_toggled(checked);
	else if (bt == ui->bt_selectPin)
		bt_selectPin_toggled(checked);
	else if (bt == ui->bt_selectRotate)
		bt_selectRotate_toggled(checked);
	else if (bt == ui->bt_selectSearch)
		bt_selectSearch_toggled(checked);
	else if (bt == ui->bt_testPin)
		bt_testPin_toggled(checked);

	m_module->setParamChanged(ProductLevel);
}

void pinLength1Wdg::comboBox_Index_changed(int Index)
{
	QComboBox* cob = qobject_cast<QComboBox*>(sender());
	QString str0 = QString("");
	QString str1 = QString("");

	if (cob == ui->cob_startAngle)
	{
		str0 = ui->label->text();
		str1 = tr(" change from %1 to %2").arg(m_param->startAngle * 90).arg(Index * 90);
		//log(LogL1, str0 + str1);
		if (m_param->AngleType == 0)
			m_param->startAngle = Index < 2 ? 0 : 1;
		else if (m_param->AngleType == 1)
			m_param->startAngle = Index;
	}

	m_module->setParamChanged(ProductLevel);
}

void pinLength1Wdg::spinBox_valueChanged(int val)
{
	QSpinBox* sp = qobject_cast<QSpinBox*>(sender());

	QString str0 = QString("");
	QString str1 = QString("");

	if (sp == m_positionSet->ui.sp_up) {
		str0 = m_positionSet->ui.label_3->text();
		str1 = tr(" change from %1 to %2").arg(QString::number(m_param->offsetUp)).arg(QString::number(val));
		m_param->offsetUp = val;
	}
	else if (sp == m_positionSet->ui.sp_down) {
		str0 = m_positionSet->ui.label_4->text();
		str1 = tr(" change from %1 to %2").arg(QString::number(m_param->offsetDown)).arg(QString::number(val));
		m_param->offsetDown = val;
	}
	else if (sp == m_positionSet->ui.sp_left) {
		str0 = m_positionSet->ui.label_5->text();
		str1 = tr(" change from %1 to %2").arg(QString::number(m_param->offsetLeft)).arg(QString::number(val));
		m_param->offsetLeft = val;
	}
	else if (sp == m_positionSet->ui.sp_right) {
		str0 = m_positionSet->ui.label_6->text();
		str1 = tr(" change from %1 to %2").arg(QString::number(m_param->offsetRight)).arg(QString::number(val));
		m_param->offsetRight = val;
	}
	else if (sp == m_advanceSet->ui.sp_roioffUp) {
		str0 = m_advanceSet->ui.label_3->text();
		str1 = tr("advance set") + tr(" change from %1 to %2").arg(QString::number(m_param->roiOffUp)).arg(QString::number(val));
		m_param->roiOffUp = val;
	}
	else if (sp == m_advanceSet->ui.sp_roioffDown) {
		str0 = m_advanceSet->ui.label_4->text();
		str1 = tr("advance set") + tr(" change from %1 to %2").arg(QString::number(m_param->roiOffDown)).arg(QString::number(val));
		m_param->roiOffDown = val;
	}
	else if (sp == m_advanceSet->ui.sp_roioffLeft) {
		str0 = m_advanceSet->ui.label_5->text();
		str1 = tr("advance set") + tr(" change from %1 to %2").arg(QString::number(m_param->roiOffLeft)).arg(QString::number(val));
		m_param->roiOffLeft = val;
	}
	else if (sp == m_advanceSet->ui.sp_roioffRight) {
		str0 = m_advanceSet->ui.label_6->text();
		str1 = tr("advance set") + tr(" change from %1 to %2").arg(QString::number(m_param->roiOffRight)).arg(QString::number(val));
		m_param->roiOffRight = val;
	}
	else if (sp == m_advanceSet->ui.sp_crossGray) {
		str0 = m_advanceSet->ui.cb_crossGray->text();
		str1 = tr("advance set") + tr(" change from %1 to %2").arg(QString::number(m_param->crossGray)).arg(QString::number(val));
		m_param->crossGray = val;
	}
	else if (sp == m_advanceSet->ui.sp_crossStep) {
		str0 = m_advanceSet->ui.lb_crossStep->text();
		str1 = tr("advance set") + tr(" change from %1 to %2").arg(QString::number(m_param->crossStep)).arg(QString::number(val));
		m_param->crossStep = val;
	}
	else if (sp == ui->sp_pinContLfUp) {
		if (val == m_param->pinCont[0]) {
			return;
		}
		if (!m_param->IsThreshold)
		{
			m_param->pinCont[0] = val;
			m_paramTables->setGroupLeadNumber(m_param->pinCont[0], m_param->pinCont[1]);
			m_corrTable->setGroupLeadNumber(m_param->pinCont[0], m_param->pinCont[1]);
			refreshTableWidget();
			if (currentView() && this->isVisible() && ui->bt_selectPin->isChecked()) {
				currentView()->finishDraw();
				currentView()->clearLayer(m_layerIdx);
				currentView()->update();
			}
			pinLength1Param::segRegion seg;
			seg.row1 = seg.col1 = 90;
			seg.row2 = seg.col2 = 140;
			vector<pinLength1Param::segRegion>::iterator itbegin = m_param->segRegionVector.begin();
			if (val > m_param->pinCont[0]) {
				for (int i = 0;i < val - m_param->pinCont[0];i++)
				{
					seg.row1 = seg.col1 += 10;
					seg.row2 = seg.col2 += 10;
					m_param->segRegionVector.insert(itbegin + 2 + m_param->pinCont[0] + i, seg);
				}
			}
			else if (val < m_param->pinCont[0]) {
				m_param->segRegionVector.erase(itbegin + 2 + val, itbegin + 2 + m_param->pinCont[0]);
			}
			if (currentView() && this->isVisible() && ui->bt_selectPin->isChecked()) {
				currentView()->setColor(255, 255, 0);
				currentView()->setFilled(false);
				QStringList title;
				if (m_param->LRorUD) {
					title << tr("Up") << tr("Down");
					// 					sprintf(buf[0], "上区域");
					// 					sprintf(buf[1], "下区域");
				}
				else {
					title << tr("Left") << tr("Right");
					// 					sprintf(buf[0], "左区域");
					// 					sprintf(buf[1], "右区域");
				}
				currentView()->dispRect1(
					m_param->segRegionVector[0].row1, m_param->segRegionVector[0].col1,
					m_param->segRegionVector[0].row2, m_param->segRegionVector[0].col2, title[0]);
				currentView()->dispRect1(
					m_param->segRegionVector[1].row1, m_param->segRegionVector[1].col1,
					m_param->segRegionVector[1].row2, m_param->segRegionVector[1].col2, title[1]);

				int DrawCount = m_param->pinCont[0] + m_param->pinCont[1];
				double DrawRow1[40], DrawRow2[40], DrawCol1[40], DrawCol2[40];
				QStringList titles;
				for (int i = 0;i < DrawCount;i++) {
					DrawRow1[i] = m_param->segRegionVector[i + 2].row1;
					DrawCol1[i] = m_param->segRegionVector[i + 2].col1;
					DrawRow2[i] = m_param->segRegionVector[i + 2].row2;
					DrawCol2[i] = m_param->segRegionVector[i + 2].col2;
					if (i < m_param->pinCont[0]) {
						if (!m_param->LRorUD) {
							titles << tr("LeftLead%1").arg(i + 1);
							//sprintf(roibuf[i], "左脚%d", i + 1);
						}
						else {
							titles << tr("UpLead%1").arg(i + 1);
							//sprintf(roibuf[i], "上脚%d", i + 1);
						}
					}
					else {
						if (!m_param->LRorUD) {
							titles << tr("RightLead%1").arg(i + 1 - m_param->pinCont[0]);
							//sprintf(roibuf[i], "右脚%d", i + 1 - m_param->pinCont[0]);
						}
						else {
							titles << tr("DownLead%1").arg(i + 1 - m_param->pinCont[0]);
							//sprintf(roibuf[i], "下脚%d", i + 1 - m_param->pinCont[0]);
						}
					}
				}
				currentView()->setColor(255, 60, 255);
				currentView()->drawRects1(DrawCount, DrawRow1, DrawCol1, DrawRow2, DrawCol2, titles);
				currentView()->update();
			}
		}

		str0 = ui->label_8->text() + QString(" ") + ui->label_7->text();
		str1 = tr(" change from %1 to %2").arg(QString::number(m_param->pinCont[0])).arg(QString::number(val));
		m_param->pinCont[0] = val;
		m_paramTables->setGroupLeadNumber(m_param->pinCont[0], m_param->pinCont[1]);
		m_corrTable->setGroupLeadNumber(m_param->pinCont[0], m_param->pinCont[1]);
		refreshTableWidget();
	}
	else if (sp == ui->sp_pinContRtDw) {
		if (val == m_param->pinCont[1]) {
			return;
		}
		if (!m_param->IsThreshold)
		{
			m_param->pinCont[1] = val;
			m_paramTables->setGroupLeadNumber(m_param->pinCont[0], m_param->pinCont[1]);
			m_corrTable->setGroupLeadNumber(m_param->pinCont[0], m_param->pinCont[1]);
			refreshTableWidget();
			if (currentView() && this->isVisible() && ui->bt_selectPin->isChecked()) {
				currentView()->finishDraw();
				currentView()->clearLayer(m_layerIdx);
			}
			pinLength1Param::segRegion seg;
			seg.row1 = seg.col1 = 90;
			seg.row2 = seg.col2 = 140;
			vector<pinLength1Param::segRegion>::iterator itbegin = m_param->segRegionVector.begin();
			if (val > m_param->pinCont[1]) {
				for (int i = 0;i < val - m_param->pinCont[1];i++)
				{
					seg.row1 = seg.col1 += 10;
					seg.row2 = seg.col2 += 10;
					m_param->segRegionVector.push_back(seg);
				}
			}
			else if (val < m_param->pinCont[1]) {
				m_param->segRegionVector.erase(itbegin + 2 + m_param->pinCont[0] + val, itbegin + 2 + m_param->pinCont[0] + m_param->pinCont[1]);
			}
			if (currentView() && this->isVisible() && ui->bt_selectPin->isChecked()) {
				int DrawCount = m_param->pinCont[0] + m_param->pinCont[1];
				double DrawRow1[40], DrawRow2[40], DrawCol1[40], DrawCol2[40];
				QStringList titles;
				for (int i = 0;i < DrawCount;i++) {
					DrawRow1[i] = m_param->segRegionVector[i + 2].row1;
					DrawCol1[i] = m_param->segRegionVector[i + 2].col1;
					DrawRow2[i] = m_param->segRegionVector[i + 2].row2;
					DrawCol2[i] = m_param->segRegionVector[i + 2].col2;
					if (i < m_param->pinCont[0]) {
						if (!m_param->LRorUD) {
							titles << tr("LeftLead%1").arg(i + 1);
							//sprintf(roibuf[i], "左脚%d", i + 1);
						}
						else {
							titles << tr("UpLead%1").arg(i + 1);
							//sprintf(roibuf[i], "上脚%d", i + 1);
						}
					}
					else {
						if (!m_param->LRorUD) {
							titles << tr("RightLead%1").arg(i + 1 - m_param->pinCont[0]);
							//sprintf(roibuf[i], "右脚%d", i + 1 - m_param->pinCont[0]);
						}
						else {
							titles << tr("DownLead%1").arg(i + 1 - m_param->pinCont[0]);
							//sprintf(roibuf[i], "下脚%d", i + 1 - m_param->pinCont[0]);
						}
					}
				}
				currentView()->setColor(255, 60, 255);
				currentView()->drawRects1(DrawCount, DrawRow1, DrawCol1, DrawRow2, DrawCol2, titles);
				currentView()->update();
			}
		}
		str0 = ui->label_9->text() + QString(" ") + ui->label_7->text();
		str1 = tr(" change from %1 to %2").arg(QString::number(m_param->pinCont[1])).arg(QString::number(val));
		m_param->pinCont[1] = val;
		m_paramTables->setGroupLeadNumber(m_param->pinCont[0], m_param->pinCont[1]);
		m_corrTable->setGroupLeadNumber(m_param->pinCont[0], m_param->pinCont[1]);
		refreshTableWidget();
	}
	else if (sp == ui->sp_dirtyMin) {
		str0 = ui->label_12->text();
		str1 = tr(" change from %1 to %2").arg(QString::number(m_param->leadAreaMin)).arg(QString::number(val));
		m_param->leadAreaMin = val;
	}
	else if (sp == ui->sp_widthMin) {
		str0 = ui->cb_width->text();
		str1 = tr(" change from %1 to %2").arg(QString::number(m_param->widthMin)).arg(QString::number(val));
		m_param->widthMin = val;
	}
	else if (sp == m_advanceSet->ui.sp_openAndClosing) {
		str0 = m_advanceSet->ui.label_16->text();
		str1 = tr(" change from %1 to %2").arg(QString::number(m_param->openAndClosing)).arg(QString::number(val));
		m_param->openAndClosing = val;
	}
	else if (sp == m_advanceSet->ui.sp_pinSize) {
		str0 = m_advanceSet->ui.label_10->text();
		str1 = tr(" change from %1 to %2").arg(QString::number(m_param->pinSize)).arg(QString::number(val));
		m_param->pinSize = val;
	}
	else if (sp == ui->sp_contrast) {
		str0 = ui->label_21->text();
		str1 = tr(" change from %1 to %2").arg(QString::number(m_param->leadContrast)).arg(QString::number(val));
		m_param->leadContrast = val;
	}
	else if (sp == m_advanceSet->ui.sp_widthOff) {
		str0 = m_advanceSet->ui.label_22->text();
		str1 = tr(" change from %1 to %2").arg(QString::number(m_param->widthOffset)).arg(QString::number(val));
		m_param->widthOffset = val;
	}
	else if (sp == ui->sp_threshold) {
		m_param->threValue = val;
		ui->hs_threshold->setValue(val);
	}
	else if (sp == m_advanceSet->ui.sp_leadLossIndex) {
		str0 = m_advanceSet->ui.label_23->text();
		str1 = tr(" change from %1 to %2").arg(QString::number(m_param->leadLossIndex)).arg(QString::number(val));
		int maxnum = m_param->pinCont[0] >= m_param->pinCont[1] ? m_param->pinCont[0] : m_param->pinCont[1];
		if (val > maxnum) {
			disconnect(m_advanceSet->ui.sp_leadLossIndex, SIGNAL(valueChanged(int)), this, SLOT(spinBox_valueChanged(int)));
			m_advanceSet->ui.sp_leadLossIndex->setValue(m_param->leadLossIndex);
			connect(m_advanceSet->ui.sp_leadLossIndex, SIGNAL(valueChanged(int)), this, SLOT(spinBox_valueChanged(int)));
			return;
		}
		m_param->leadLossIndex = val;
	}
	m_module->setParamChanged(ProductLevel);
	//m_param->saveProject();

	//log(LogL1, str0 + str1);
}

void pinLength1Wdg::douSpinBox_valueChanged(double val)
{
	QDoubleSpinBox* dsp = qobject_cast<QDoubleSpinBox*>(sender());
	QString str0 = QString("");
	QString str1 = QString("");
	QString str2 = QString("");
	if (dsp == ui->dsp_ratio0) {
		str0 = ui->groupBox_5->title();
		str1 = ui->label_19->text();
		str2 = tr(" change from %1 to %2").arg(QString::number(m_param->SizeRation[0], 'f', 3)).arg(QString::number(val, 'f', 3));
		m_param->SizeRation[0] = val;
	}
	else if (dsp == ui->dsp_ratio1) {
		str0 = ui->groupBox_5->title();
		str1 = ui->label_20->text();
		str2 = tr(" change from %1 to %2").arg(QString::number(m_param->SizeRation[1], 'f', 3)).arg(QString::number(val, 'f', 3));
		m_param->SizeRation[1] = val;
	}
	m_module->setParamChanged(ProductLevel);
	//log(LogL1, str0 + str1 + str2);
}

void pinLength1Wdg::checkBox_valueChanged(bool check)
{
	QCheckBox* cb = qobject_cast<QCheckBox*>(sender());

	QString str0 = QString("");
	QString str1 = cb->text();
	QString str2;
	if (check)
		str2 = tr(" Check");
	else
		str2 = tr(" unCheck");
	if (cb == m_positionSet->ui.cb_useRefer) {
		m_param->IsReferSection = check;
		//if (check){
		// ui->cb_usePinAreaRefer->setChecked(false);
		//}
	}
	else if (cb == m_positionSet->ui.cb_onlyReferAngle_0) {
		str1 = m_positionSet->ui.lb_onlyReferAngle->text();
		str1 += tr(" of group 1");
		m_param->IsOnlyReferEdge[0] = check;
	}
	else if (cb == m_positionSet->ui.cb_onlyReferAngle_1) {
		str1 = m_positionSet->ui.lb_onlyReferAngle->text();
		str1 += tr(" of group 2");
		m_param->IsOnlyReferEdge[1] = check;
	}
	else if (cb == m_positionSet->ui.cb_usePinAreaRefer) {
		m_param->PinAreaRefer = check;
		//if (check){
		// ui->cb_useRefer->setChecked(false);
		//}
	}
	else if (cb == m_positionSet->ui.cb_referEdgeRev) {
		m_param->IsReferEdgeRev = check;
	}
	else if (cb == ui->cb_delOffSearchROI) {
		m_param->IsDelOffSearchRoi = check;
	}
	else if (cb == m_positionSet->ui.cb_interUseEdgeAngle) {
		m_param->IsInterUseEdgeAngle = check;
	}
	else if (cb == m_advanceSet->ui.cb_IgnoreRoot) {
		m_param->IsIgnoreRoot = check;
	}
	else if (cb == m_advanceSet->ui.cb_whitePin) {
		m_param->IsWhitePin = check;
	}
	else if (cb == ui->cb_width) {
		m_param->IsWidth = check;
	}
	else if (cb == ui->cb_threshold) {
		m_param->IsThreshold = check;
		//ui.hs_threshold->setEnabled(check);
		//ui.sp_threshold->setEnabled(check);
		if (check) {

		}
		else {    //当采用手动区域时，将管脚数目置为自动分割时候的分割数1目
			m_param->pinCont[0] = m_param->pinCont[0];
			m_param->pinCont[1] = m_param->pinCont[1];
			m_paramTables->setGroupLeadNumber(m_param->pinCont[0], m_param->pinCont[1]);
			m_corrTable->setGroupLeadNumber(m_param->pinCont[0], m_param->pinCont[1]);
			refreshTableWidget();
			disconnect(ui->sp_pinContLfUp, SIGNAL(valueChanged(int)), this, SLOT(spinBox_valueChanged(int)));
			disconnect(ui->sp_pinContRtDw, SIGNAL(valueChanged(int)), this, SLOT(spinBox_valueChanged(int)));
			ui->sp_pinContLfUp->setValue(m_param->pinCont[0]);
			ui->sp_pinContRtDw->setValue(m_param->pinCont[1]);
			connect(ui->sp_pinContLfUp, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
			connect(ui->sp_pinContRtDw, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
		}
	}
	else if (cb == ui->cb_errWidth) {
		if (m_param->IsGroupEnableSame) {
			str0 = tr("all group:");
			m_param->errWidthValid[0] = m_param->errWidthValid[1] = check;
		}
		else {
			str0 = m_grpIndex ? ui->rb_grp0->text() : ui->rb_grp1->text();
			m_param->errWidthValid[m_grpIndex] = check;
		}
	}
	else if (cb == ui->cb_errLength) {
		if (m_param->IsGroupEnableSame) {
			str0 = tr("all group:");
			m_param->errLengthValid[0] = m_param->errLengthValid[1] = check;
		}
		else {
			str0 = m_grpIndex ? ui->rb_grp0->text() : ui->rb_grp1->text();
			m_param->errLengthValid[m_grpIndex] = check;
		}

	}
	else if (cb == ui->cb_errInterval) {
		if (m_param->IsGroupEnableSame) {
			str0 = tr("all group:");
			m_param->errInterValid[0] = m_param->errInterValid[1] = check;
		}
		else {
			str0 = m_grpIndex ? ui->rb_grp0->text() : ui->rb_grp1->text();
			m_param->errInterValid[m_grpIndex] = check;
		}
	}
	else if (cb == ui->cb_errSpan) {
		m_param->errSpanValid = check;
	}
	else if (cb == ui->cb_errMaxDiff) {
		if (m_param->IsGroupEnableSame) {
			str0 = tr("all group:");
			m_param->errMaxDiffValid[0] = m_param->errMaxDiffValid[1] = check;
		}
		else {
			str0 = m_grpIndex ? ui->rb_grp0->text() : ui->rb_grp1->text();
			m_param->errMaxDiffValid[m_grpIndex] = check;
		}
	}
	else if (cb == ui->cb_errAllMaxDiff) {
		m_param->errAllMaxDiffValid = check;
	}
	else if (cb == m_advanceSet->ui.cb_inputRatio) {
		m_param->IsInputSize = check;
	}
	else if (cb == ui->cb_useDir) {
		m_param->IsUseDir = check;
	}
	else if (cb == m_advanceSet->ui.cb_showAfterDet) {
		m_param->IsShowAfterDet = check;
	}
	else if (cb == m_advanceSet->ui.cb_useInputRegion) {
		m_param->IsUseInputRegion = check;
	}
	else if (cb == m_advanceSet->ui.cb_NoDetectTop) {
		m_param->IsNoDetectTop = check;
	}
	else if (cb == m_advanceSet->ui.cb_crossGray) {
		m_param->CrossGrayValid = check;
	}
	else if (cb == m_advanceSet->ui.cb_groupSame) {
		m_param->IsGroupEnableSame = check;
		if (check) {
			ui->rb_grp0->setChecked(1);
			m_param->errWidthValid[1] = m_param->errWidthValid[0];
			m_param->errLengthValid[1] = m_param->errLengthValid[0];
			m_param->errInterValid[1] = m_param->errInterValid[0];
			m_param->errMaxDiffValid[1] = m_param->errMaxDiffValid[0];
			m_param->errSpanValid = m_param->errSpanValid;
		}
		ui->rb_grp0->setVisible(!m_param->IsGroupEnableSame);
		ui->rb_grp1->setVisible(!m_param->IsGroupEnableSame);
	}
	m_module->setParamChanged(ProductLevel);
	//log(LogL1, str0 + str1 + str2);
}

void pinLength1Wdg::slider_valueChanged(int val)
{
	m_param->threValue = val;
	disconnect(ui->sp_threshold, SIGNAL(valueChanged(int)), this, SLOT(spinBox_valueChanged(int)));
	ui->sp_threshold->setValue(val);
	connect(ui->sp_threshold, SIGNAL(valueChanged(int)), this, SLOT(spinBox_valueChanged(int)));
	currentView()->clearLayer(m_layerIdx);
	if (m_param->IsThreshold&&ui->bt_selectPin->isChecked()) {
		for (int g = 0;g < 2;g++) {
			Hobject threRect, threRectRt, tmpImage, tmpImageRt;
			gen_rectangle1(&threRect, 
				m_param->segRegionVector[g].row1, m_param->segRegionVector[g].col1,
				m_param->segRegionVector[g].row2, m_param->segRegionVector[g].col2);
			currentView()->setColor(0, 0, 0);
			currentView()->setFilled(true);
			currentView()->dispRect1(
				m_param->segRegionVector[g].row1, m_param->segRegionVector[g].col1,
				m_param->segRegionVector[g].row2, m_param->segRegionVector[g].col2);  //把背景涂黑
			reduce_domain(*m_image, threRect, &tmpImage);
			if (m_param->IsWhitePin) {
				threshold(tmpImage, &tmpImage, m_param->threValue, 255);
			}
			else {
				threshold(tmpImage, &tmpImage, 0, m_param->threValue);
			}

			currentView()->setColor(255, 255, 255);
			Hlong num;
			count_obj(tmpImage, &num);
			if (num > 0) {
				Hobject dispReg;
				union1(tmpImage, &dispReg);
				RlRegion disprlreg;
				HalOpenCV::hal2cvRegion(dispReg, &disprlreg);
				currentView()->dispRegion(&disprlreg);
			}
			opening_rectangle1(tmpImage, &tmpImage, 3, 3);
			if (m_param->pinSize > 0) {
				if (m_param->LRorUD) {
					closing_rectangle1(tmpImage, &tmpImage, m_param->pinSize, m_param->pinSize * 2);
				}
				else {
					closing_rectangle1(tmpImage, &tmpImage, m_param->pinSize * 2, m_param->pinSize);
				}
			}
			connection(tmpImage, &tmpImage);
			select_shape(tmpImage, &tmpImage, "area", "and", m_param->leadAreaMin, 999999);
			if (m_param->IsWidth) {
				select_shape(tmpImage, &tmpImage, "width", "and", m_param->widthMin, 9999);
				select_shape(tmpImage, &tmpImage, "height", "and", m_param->widthMin, 9999);
			}
			Hlong regionCount;
			count_obj(tmpImage, &regionCount);
			if (regionCount > 20) {
				return;
			}
			else {
				m_param->pinCont[g] = regionCount;
				m_paramTables->setGroupLeadNumber(m_param->pinCont[0], m_param->pinCont[1]);
				m_corrTable->setGroupLeadNumber(m_param->pinCont[0], m_param->pinCont[1]);
				refreshTableWidget();
			}
			HTuple lfRow1, lfCol1, lfRow2, lfCol2;
			smallest_rectangle1(tmpImage, &lfRow1, &lfCol1, &lfRow2, &lfCol2);
			HTuple rowsort, sortindex;
			//tuple_sort(lfRow1,&rowsort);
			if (m_param->LRorUD == 0) {    //上下排布
				tuple_sort_index(lfRow1, &sortindex);
			}
			else {
				tuple_sort_index(lfCol1, &sortindex);
			}
			HTuple mrow1, mrow2, mcol1, mcol2;
			for (int e = 0;e < regionCount;e++) {
				mrow1[e] = lfRow1[sortindex[e].I()].D();
				mrow2[e] = lfRow2[sortindex[e].I()].D();
				mcol1[e] = lfCol1[sortindex[e].I()].D();
				mcol2[e] = lfCol2[sortindex[e].I()].D();
			}
			if (g == 0) {
				vector<pinLength1Param::segRegion>::iterator itbegin = m_param->segRegionVector.begin() + 2;
				vector<pinLength1Param::segRegion>::iterator itEnd = m_param->segRegionVector.end();
				m_param->segRegionVector.erase(itbegin, itEnd);
			}
			pinLength1Param::segRegion seg;
			for (int i = 0;i < m_param->pinCont[g];i++)
			{
				int dirval[4] = { 1,1,1,1 };
				if (m_param->IsSOPType) {
					if (!m_param->LRorUD) {  //左右管脚
						if (g == 0) {
							dirval[3] = 0;
						}
						else if (g == 1) {
							dirval[2] = 0;
						}
					}
					else {
						if (g == 0) {
							dirval[1] = 0;
						}
						else if (g == 1) {
							dirval[0] = 0;
						}
					}
				}
				seg.row1 = mrow1[i].D() - m_param->pinSize / 2 * dirval[0];
				seg.row2 = mrow2[i].D() + m_param->pinSize / 2 * dirval[1];
				seg.col1 = mcol1[i].D() - m_param->pinSize / 2 * dirval[2];
				seg.col2 = mcol2[i].D() + m_param->pinSize / 2 * dirval[3];
				m_param->segRegionVector.push_back(seg);
			}

			currentView()->setColor(255, 255, 0);
			currentView()->setFilled(false);
			char buf[2][32];
			if (m_param->LRorUD) {
				if (g == 0) {
					sprintf(buf[g], "up");
				}
				else {
					sprintf(buf[g], "down");
				}
			}
			else {
				if (g == 0) {
					sprintf(buf[g], "left");
				}
				else {
					sprintf(buf[g], "right");
				}
			}
			currentView()->dispRect1(m_param->segRegionVector[g].row1, m_param->segRegionVector[g].col1, m_param->segRegionVector[g].row2,
				m_param->segRegionVector[g].col2, buf[g]);
		}


		int DrawCount = m_param->pinCont[0] + m_param->pinCont[1];
		double DrawRow1[40], DrawRow2[40], DrawCol1[40], DrawCol2[40];
		QStringList titles;
		for (int i = 0;i < DrawCount;i++) {
			DrawRow1[i] = m_param->segRegionVector[i + 2].row1;
			DrawCol1[i] = m_param->segRegionVector[i + 2].col1;
			DrawRow2[i] = m_param->segRegionVector[i + 2].row2;
			DrawCol2[i] = m_param->segRegionVector[i + 2].col2;
			if (i < m_param->pinCont[0]) {
				if (!m_param->LRorUD) {
					titles << tr("Lf-%1").arg(i + 1);
					//sprintf(roibuf[i], "Lf-%d", i + 1);
				}
				else {
					titles << tr("Up-%1").arg(i + 1);
					//sprintf(roibuf[i], "Up-%d", i + 1);
				}
			}
			else {
				if (!m_param->LRorUD) {
					titles << tr("Rt-%1").arg(i + 1 - m_param->pinCont[0]);
					//sprintf(roibuf[i], "Rt-%d", i + 1 - m_param->pinCont[0]);
				}
				else {
					titles << tr("Dw-%1").arg(i + 1 - m_param->pinCont[0]);
					//sprintf(roibuf[i], "Dw-%d", i + 1 - m_param->pinCont[0]);
				}
			}
		}
		currentView()->setColor(255, 60, 255);
		currentView()->drawRects1(DrawCount, DrawRow1, DrawCol1, DrawRow2, DrawCol2, titles);
		currentView()->setFilled(false);
		currentView()->setColor(255, 255, 0);
		currentView()->update();
		connectSlots(false);
		ui->sp_pinContLfUp->setValue(m_param->pinCont[0]);
		ui->sp_pinContRtDw->setValue(m_param->pinCont[1]);
		connectSlots(true);
	}
	else if (ui->bt_testPin->isChecked()) {
		drawTestPin();
	}

	m_module->setParamChanged(ProductLevel);
}

void pinLength1Wdg::radioButton_toggled(bool check)
{
	QRadioButton* rb = qobject_cast<QRadioButton*>(sender());

	QString str0 = QString("");
	QString str1 = QString("");
	if (check) {
		str0 = rb->text() + " check ";
		if (rb == ui->rb_grp0) {
			m_grpIndex = 0;
			connectSlots(false);
			changeErrGroup();
			connectSlots(true);
		}
		else if (rb == ui->rb_grp1) {
			m_grpIndex = 1;
			connectSlots(false);
			changeErrGroup();
			connectSlots(true);
		}
		else if (rb == ui->rb_2angle) {
			m_param->AngleType = 0;
			m_param->startAngle = (m_param->startAngle < 2) ? 0 : 1;
		}
		else if (rb == ui->rb_4angle) {
			m_param->AngleType = 1;
			m_param->startAngle *= 2;        //两方向时，m_param->startAngle=1代表180°，改成四方向后，需要更换为2
		}
		else if (rb == m_advanceSet->ui.rb_LFRT) {
			m_param->LRorUD = 0;
			changeLeadDir(check);
		}
		else if (rb == m_advanceSet->ui.rb_UPDW) {
			m_param->LRorUD = 1;
			changeLeadDir(check);
		}
		else if (rb == m_advanceSet->ui.rb_SOP) {
			m_param->IsSOPType = 1;
		}
		else if (rb == m_advanceSet->ui.rb_QFN) {
			m_param->IsSOPType = 0;
		}
		else if (rb == m_positionSet->ui.rb_EdgeSOP) {
			m_param->IsEdgeSopType = 1;
			m_positionSet->ui.grp_qfn->setVisible(0);
		}
		else if (rb == m_positionSet->ui.rb_EdgeQFN) {
			m_param->IsEdgeSopType = 0;
			m_positionSet->ui.grp_qfn->setVisible(1);
		}
		m_module->setParamChanged(ProductLevel);
		//log(LogL1, str0 + str1);
	}
}

void pinLength1Wdg::on_bt_highLevel_toggled(bool val)
{
	ui->grp_high->setVisible(val);
}

void pinLength1Wdg::bt_selectRotate_toggled(bool val)
{
	if (val) {
		if (m_selState != UnSel) {
			connectSlots(false);
			ui->bt_selectRotate->setChecked(false);
			connectSlots(true);
			return;
		}
		m_selState = SelRotate;
		currentView()->clearLayer(m_layerIdx);
		currentView()->setDrawColor(0, 255, 0);
		ui->bt_selectRotate->setText(tr("Sure Rotate"));
		//ui.bt_selectRotate->setText(tr("确认旋转区域"));
		ui->bt_testPin->setEnabled(false);
		currentView()->drawRect1(m_param->rotateRoi.row1, m_param->rotateRoi.col1, m_param->rotateRoi.row2, m_param->rotateRoi.col2);
		currentView()->update();
	}
	else {
		m_selState = UnSel;
		currentView()->finishDraw();
		currentView()->update();
		ui->bt_selectRotate->setText(tr("Select Rotating Region"));
		ui->bt_testPin->setEnabled(true);
		currentView()->getDrawRect1(m_param->rotateRoi.row1, m_param->rotateRoi.col1, m_param->rotateRoi.row2, m_param->rotateRoi.col2);
	}
}

void pinLength1Wdg::bt_selectSearch_toggled(bool val)
{
	if (val) {
		if (m_selState != UnSel) {
			connectSlots(false);
			ui->bt_selectSearch->setChecked(false);
			connectSlots(true);
			return;
		}
		m_selState = SelSearch;

		currentView()->clearLayer(m_layerIdx);
		ui->bt_selectSearch->setText(tr("Search ROI OK"));
		char buf[32];
		currentView()->setFilled(false);
		currentView()->setColor(255, 60, 255);
		sprintf(buf, "searchROI");
		currentView()->drawRect2(m_param->SearchEdgeRoi.row, m_param->SearchEdgeRoi.col, m_param->SearchEdgeRoi.angle
			, m_param->SearchEdgeRoi.len1, m_param->SearchEdgeRoi.len2, buf);
		currentView()->setColor(255, 255, 0);
		currentView()->update();
	}
	else {
		m_selState = UnSel;
		ui->bt_selectSearch->setText(tr("Search ROI"));
		currentView()->clearLayer(m_layerIdx);
		currentView()->finishDraw();
		currentView()->getDrawRect2(m_param->SearchEdgeRoi.row, m_param->SearchEdgeRoi.col, m_param->SearchEdgeRoi.angle
			, m_param->SearchEdgeRoi.len1, m_param->SearchEdgeRoi.len2);
		currentView()->setColor(255, 255, 0);
		currentView()->setFilled(false);
		currentView()->update();
	}
}

void pinLength1Wdg::on_bt_locate_clicked()
{
	m_positionSet->exec();
}

void pinLength1Wdg::on_bt_advanceLead_clicked()
{
	m_advanceSet->exec();
}

void pinLength1Wdg::on_bt_table_clicked()
{
	m_dialog->exec();
}

void pinLength1Wdg::on_bt_corrTable_clicked()
{
	m_corrDialog->exec();
}

void pinLength1Wdg::tableWidget_valueChanged(int checkIdx, int grpIdx, int leadIdx, bool minOrMax, double val)
{
	ParamTables* pt = qobject_cast<ParamTables*>(sender());
	QString str0 = QString("");
	QString str1 = QString("");
	QString cbname, leadname, minmaxName;
	pt->getCheckBoxName(checkIdx, cbname);
	double exval;
	if (pt == m_paramTables) {
		leadname = tr("lead%1 of group%2: ").arg(leadIdx + 1).arg(grpIdx + 1);
		if (minOrMax)
			minmaxName = tr(" min val");
		else
			minmaxName = tr(" max val");
		str0 = leadname + cbname + minmaxName;
		if (checkIdx == 0) {
			if (!minOrMax) {
				exval = m_param->errLengthMin[grpIdx][leadIdx];
				m_param->errLengthMin[grpIdx][leadIdx] = val;
			}
			else {
				exval = m_param->errLengthMax[grpIdx][leadIdx];
				m_param->errLengthMax[grpIdx][leadIdx] = val;
			}
		}
		else if (checkIdx == 1) {
			if (!minOrMax) {
				exval = m_param->errWidthMin[grpIdx][leadIdx];
				m_param->errWidthMin[grpIdx][leadIdx] = val;
			}
			else {
				exval = m_param->errWidthMax[grpIdx][leadIdx];
				m_param->errWidthMax[grpIdx][leadIdx] = val;
			}
		}
		else if (checkIdx == 2) {
			if (!minOrMax) {
				exval = m_param->errInterMin[grpIdx][leadIdx];
				m_param->errInterMin[grpIdx][leadIdx] = val;
			}
			else {
				exval = m_param->errInterMax[grpIdx][leadIdx];
				m_param->errInterMax[grpIdx][leadIdx] = val;
			}
		}
		else if (checkIdx == 3) {
			if (!minOrMax) {
				exval = m_param->errSpanMin[leadIdx];
				m_param->errSpanMin[leadIdx] = val;
				if (m_param->pinCont[grpIdx == 0 ? 1 : 0] > leadIdx) {
					m_paramTables->setValue(checkIdx, grpIdx == 0 ? 1 : 0, leadIdx, minOrMax, val);
				}

			}
			else {
				exval = m_param->errSpanMax[leadIdx];
				m_param->errSpanMax[leadIdx] = val;
				if (m_param->pinCont[grpIdx == 0 ? 1 : 0] > leadIdx) {
					m_paramTables->setValue(checkIdx, grpIdx == 0 ? 1 : 0, leadIdx, minOrMax, val);
				}
			}
		}
		else if (checkIdx == 4) {
			if (!minOrMax) {
				exval = m_param->errMaxDiffMin[grpIdx];
				m_param->errMaxDiffMin[grpIdx] = val;
			}
			else {
				exval = m_param->errMaxDiff[grpIdx];
				m_param->errMaxDiff[grpIdx] = val;
			}
		}
		else if (checkIdx == 5) {
			if (!minOrMax) {
				exval = m_param->errAllMaxDiffMin;
				m_param->errAllMaxDiffMin = val;
			}
			else {
				exval = m_param->errAllMaxDiff;
				m_param->errAllMaxDiff = val;
			}
		}
	}
	else if (pt == m_corrTable) {
		leadname = tr("lead%1 of group%2: ").arg(leadIdx + 1).arg(grpIdx + 1);
		cbname = tr("compensation") + cbname;
		str0 = leadname + cbname;
		if (checkIdx == 0) {
			exval = m_param->LeadLengthCorrNew[grpIdx][leadIdx];
			m_param->LeadLengthCorrNew[grpIdx][leadIdx] = val;
		}
		else if (checkIdx == 1) {
			exval = m_param->LeadWidthCorrNew[grpIdx][leadIdx];
			m_param->LeadWidthCorrNew[grpIdx][leadIdx] = val;
		}
		else if (checkIdx == 2) {
			exval = m_param->LeadInterCorrNew[grpIdx][leadIdx];
			m_param->LeadInterCorrNew[grpIdx][leadIdx] = val;
		}
		else if (checkIdx == 3) {
			exval = m_param->LeadSpanCorr[leadIdx];
			m_param->LeadSpanCorr[leadIdx] = val;
			if (m_param->pinCont[grpIdx == 0 ? 1 : 0] > leadIdx) {
				m_corrTable->setValue(checkIdx, grpIdx == 0 ? 1 : 0, leadIdx, minOrMax, val);
			}
		}
	}
	str1 = tr(" change from %1 to %2").arg(QString::number(exval, 'f', 3)).arg(QString::number(val, 'f', 3));

	m_module->setParamChanged(ProductLevel);
	//log(LogL1, str0 + str1);
}

void pinLength1Wdg::tableWidget_checkToggled(int index, bool check) {}

void pinLength1Wdg::tableWidget_CopyItems(bool IsAll, int startCheck, int CheckRange, int starLead, int leadRange)
{
	ParamTables* pt = qobject_cast<ParamTables*>(sender());
	QString str0 = QString("");
	QString str1 = QString("");
	QString cbname, leadname, minmaxName;
	if (pt == m_paramTables) {
		int stc, edc, stl, edl;
		if (IsAll) {
			stc = 0;edc = 12;
			stl = 0;edl = m_param->pinCont[0] + m_param->pinCont[1];
		}
		else {
			stc = startCheck;edc = startCheck + CheckRange;
			stl = starLead;edl = starLead + leadRange;
		}
		for (int i = stc;i < edc;i++) {
			int checkIdx = i / 2;
			bool minormax = bool(i % 2);
			double exval, nowval;
			pt->getCheckBoxName(checkIdx, cbname);
			for (int j = stl;j < edl;j++) {
				if (j == 0 || j == m_param->pinCont[0]) {
					continue;
				}
				int grpIdx;
				if (j >= m_param->pinCont[0]) {
					grpIdx = 1;
				}
				else {
					grpIdx = 0;
				}
				int leadIdx = j - grpIdx*m_param->pinCont[0];

				leadname = tr("lead%1 of group%2: ").arg(leadIdx + 1).arg(grpIdx + 1);
				if (minormax)
					minmaxName = tr(" min val");
				else
					minmaxName = tr(" max val");
				str0 = leadname + cbname + minmaxName;
				if (i == 0) {
					exval = m_param->errLengthMin[grpIdx][leadIdx];
					m_param->errLengthMin[grpIdx][leadIdx] = nowval = m_param->errLengthMin[grpIdx][0];

				}
				else if (i == 1) {
					exval = m_param->errLengthMax[grpIdx][leadIdx];
					m_param->errLengthMax[grpIdx][leadIdx] = nowval = m_param->errLengthMax[grpIdx][0];
				}
				else if (i == 2) {
					exval = m_param->errWidthMin[grpIdx][leadIdx];
					m_param->errWidthMin[grpIdx][leadIdx] = nowval = m_param->errWidthMin[grpIdx][0];
				}
				else if (i == 3) {
					exval = m_param->errWidthMax[grpIdx][leadIdx];
					m_param->errWidthMax[grpIdx][leadIdx] = nowval = m_param->errWidthMax[grpIdx][0];
				}
				else if (i == 4) {
					exval = m_param->errInterMin[grpIdx][leadIdx];
					m_param->errInterMin[grpIdx][leadIdx] = nowval = m_param->errInterMin[grpIdx][0];
				}
				else if (i == 5) {
					exval = m_param->errInterMax[grpIdx][leadIdx];
					m_param->errInterMax[grpIdx][leadIdx] = nowval = m_param->errInterMax[grpIdx][0];
				}
				else if (i == 6) {
					exval = m_param->errSpanMin[leadIdx];
					m_param->errSpanMin[leadIdx] = nowval = m_param->errSpanMin[0];
					if (m_param->pinCont[grpIdx == 0 ? 1 : 0] > leadIdx) {  //针对修改一组另一组也要修改的情况，两组管脚数目不同会出现问题
						m_paramTables->setValue(3, grpIdx == 0 ? 1 : 0, leadIdx, 0, nowval);
					}
				}
				else if (i == 7) {
					exval = m_param->errSpanMax[leadIdx];
					m_param->errSpanMax[leadIdx] = nowval = m_param->errSpanMax[0];
					if (m_param->pinCont[grpIdx == 0 ? 1 : 0] > leadIdx) {
						m_paramTables->setValue(3, grpIdx == 0 ? 1 : 0, leadIdx, 1, nowval);
					}
				}
				if (i == 8 || i == 9 || i == 10 || i == 11) {
					continue;
				}
				else {
					str1 = tr(" change from %1 to %2").arg(QString::number(exval, 'f', 3)).arg(QString::number(nowval, 'f', 3));
					//log(LogL1, str0 + str1);
				}
			}
		}
	}
	else if (pt == m_corrTable) {
		int stc, edc, stl, edl;
		if (IsAll) {
			stc = 0;edc = 4;
			stl = 0;edl = m_param->pinCont[0] + m_param->pinCont[1];
		}
		else {
			stc = startCheck;edc = startCheck + CheckRange;
			stl = starLead;edl = starLead + leadRange;
		}
		for (int i = stc;i < edc;i++) {
			int checkIdx = i;
			bool minormax = false;
			double exval, nowval;
			pt->getCheckBoxName(checkIdx, cbname);
			cbname = tr("compensation") + cbname;
			for (int j = stl;j < edl;j++) {
				if (j == 0 || j == m_param->pinCont[0]) {
					continue;
				}
				int grpIdx;
				if (j >= m_param->pinCont[0]) {
					grpIdx = 1;
				}
				else {
					grpIdx = 0;
				}
				int leadIdx = j - grpIdx*m_param->pinCont[0];

				leadname = tr("lead%1 of group%2: ").arg(leadIdx + 1).arg(grpIdx + 1);

				str0 = leadname + cbname;
				if (i == 0) {
					exval = m_param->LeadLengthCorrNew[grpIdx][leadIdx];
					m_param->LeadLengthCorrNew[grpIdx][leadIdx] = nowval = m_param->LeadLengthCorrNew[grpIdx][0];
				}
				else if (i == 1) {
					exval = m_param->LeadWidthCorrNew[grpIdx][leadIdx];
					m_param->LeadWidthCorrNew[grpIdx][leadIdx] = nowval = m_param->LeadWidthCorrNew[grpIdx][0];
				}
				else if (i == 2) {
					exval = m_param->LeadInterCorrNew[grpIdx][leadIdx];
					m_param->LeadInterCorrNew[grpIdx][leadIdx] = nowval = m_param->LeadInterCorrNew[grpIdx][0];
				}
				if (i == 3) {
					continue;
				}
				else {
					str1 = tr(" change from %1 to %2").arg(QString::number(exval, 'f', 3)).arg(QString::number(nowval, 'f', 3));
					//log(LogL1, str0 + str1);
				}
			}
		}
	}
	m_module->setParamChanged(ProductLevel);
}