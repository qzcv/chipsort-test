#include "pinDetect1Wdg.h"
#include "pinDetect1Mod.h"
#include "pinDetect1Param.h"
#include "ui_pinDetect1Wdg.h"
#include "autothredialog.h"
#include "AdvanceDialog.h"
#include "halOpenCV.h"
#include "ImageView.h"
#include "paramTables.h"

pinDetect1Wdg::pinDetect1Wdg(int i) :
	ModSetWidget(),ui(new Ui::pinDetect1Wdg),m_widgetType(i)
{
	m_hasConnect = false;
	ui->setupUi(this);
	iniData();
	iniUi();

}

pinDetect1Wdg::~pinDetect1Wdg()
{
	delete m_advDialog;
	delete m_autoThreDialog;
	delete ui;
}

void pinDetect1Wdg::setModule(UnitModule *module)
{
	m_module=dynamic_cast<pinDetect1Mod*>(module);
	m_param = m_module->m_param;
	m_image = m_module->m_image;
	//currentView()->setLayer(m_layerIdx);
	//updateUi();
}

void pinDetect1Wdg::showEvent(QShowEvent * event)
{
	updateUi();
}

void pinDetect1Wdg::connectSlots(bool link)
{
	if(m_hasConnect==link)
		return;
	m_hasConnect=link;

	connectOrNot(link, ui->bt_selectRoi, SIGNAL(toggled(bool)), SLOT(bt_selectRoi_toggled(bool)));

	connectOrNot(link, m_autoThreDialog->ui.bt_selAutoThre, SIGNAL(toggled(bool)), SLOT(autoThre_bt_toggled(bool)));
	connectOrNot(link, m_autoThreDialog->ui.bt_selectPin, SIGNAL(toggled(bool)), SLOT(autoThre_bt_toggled(bool)));
	connectOrNot(link, m_autoThreDialog->ui.bt_selSearchRoi, SIGNAL(toggled(bool)), SLOT(autoThre_bt_toggled(bool)));

	connectOrNot(link, m_advDialog->ui.cb_delOffSearchROI, SIGNAL(toggled(bool)), SLOT(checkBox_toggled(bool)));
	connectOrNot(link, m_advDialog->ui.cb_useRefer, SIGNAL(toggled(bool)), SLOT(checkBox_toggled(bool)));
	connectOrNot(link, m_advDialog->ui.cb_referEdgeRev, SIGNAL(toggled(bool)), SLOT(checkBox_toggled(bool)));
	connectOrNot(link, m_advDialog->ui.cb_ampBest, SIGNAL(toggled(bool)), SLOT(checkBox_toggled(bool)));
	connectOrNot(link, m_advDialog->ui.cb_inputRatio, SIGNAL(toggled(bool)), SLOT(checkBox_toggled(bool)));
	connectOrNot(link, m_autoThreDialog->ui.cb_whitePin, SIGNAL(toggled(bool)), SLOT(checkBox_toggled(bool)));
	connectOrNot(link, m_autoThreDialog->ui.cb_useInputRegion, SIGNAL(toggled(bool)), SLOT(checkBox_toggled(bool)));

	connectOrNot(link, ui->cb_length, SIGNAL(toggled(bool)), SLOT(checkBox_toggled(bool)));
	connectOrNot(link, ui->cb_pinInterval, SIGNAL(toggled(bool)), SLOT(checkBox_toggled(bool)));
	connectOrNot(link, ui->cb_pinWidth, SIGNAL(toggled(bool)), SLOT(checkBox_toggled(bool)));
	connectOrNot(link, ui->cb_pinSweep, SIGNAL(toggled(bool)), SLOT(checkBox_toggled(bool)));
	connectOrNot(link, ui->cb_standoffDiff, SIGNAL(toggled(bool)), SLOT(checkBox_toggled(bool)));
	connectOrNot(link, m_advDialog->ui.cb_baseInvert, SIGNAL(toggled(bool)), SLOT(checkBox_toggled(bool)));

	connectOrNot(link, m_advDialog->ui.rb_LFRT, SIGNAL(toggled(bool)), SLOT(radioButton_toggled(bool)));
	connectOrNot(link, m_advDialog->ui.rb_UPDW, SIGNAL(toggled(bool)), SLOT(radioButton_toggled(bool)));

	connectOrNot(link, m_advDialog->ui.sp_offset, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_pinNum, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	//connectOrNot(link, ui->sp_extendSearchLine,SIGNAL(valueChanged(int)),SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, m_advDialog->ui.sp_searchLineGap, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_minGrayDiff, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, m_advDialog->ui.sp_widLineOffset, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, m_advDialog->ui.sp_widSearchLength, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));

	connectOrNot(link, m_advDialog->ui.sp_widRange, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, m_advDialog->ui.sp_SHShift, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));

	connectOrNot(link, m_advDialog->ui.bt_corrTable, SIGNAL(clicked()), SLOT(bt_corrTable_clicked()));

	connectOrNot(link, ui->sp_widthGrayDiff, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	//connectOrNot(link, ui->sp_standoffOffset,SIGNAL(valueChanged(int)),SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_pinArea, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_noiseGray, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_multGray, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));

	connectOrNot(link, ui->sp_deleteEdge, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, m_autoThreDialog->ui.sp_leadSize, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, m_autoThreDialog->ui.sp_minArea, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, m_autoThreDialog->ui.sp_threshold, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, m_autoThreDialog->ui.sp_up, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, m_autoThreDialog->ui.sp_down, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, m_autoThreDialog->ui.sp_left, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, m_autoThreDialog->ui.sp_right, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));

	connectOrNot(link, ui->hs_minGrayDiff, SIGNAL(valueChanged(int)), SLOT(slider_valueChanged(int)));
	connectOrNot(link, ui->hs_widthGrayDiff, SIGNAL(valueChanged(int)), SLOT(slider_valueChanged(int)));
	connectOrNot(link, m_autoThreDialog->ui.hs_threshold, SIGNAL(valueChanged(int)), SLOT(slider_valueChanged(int)));

	//connectOrNot(link, ui->dsp_factor,SIGNAL(valueChanged(double)),SLOT(doubleSpinBox_valueChanged(double)));
	connectOrNot(link, m_advDialog->ui.dsp_ratio, SIGNAL(valueChanged(double)), SLOT(doubleSpinBox_valueChanged(double)));

	connectOrNot(link, m_paramTables, SIGNAL(CellChanged(int, int, int, bool, double)), SLOT(tableWidget_valueChanged(int, int, int, bool, double)));
	connectOrNot(link, m_corrTable, SIGNAL(CellChanged(int, int, int, bool, double)), SLOT(tableWidget_valueChanged(int, int, int, bool, double)));

	connectOrNot(link, m_paramTables, SIGNAL(CopyItems(bool, int, int, int, int)), SLOT(tableWidget_CopyItems(bool, int, int, int, int)));
	connectOrNot(link, m_corrTable, SIGNAL(CopyItems(bool, int, int, int, int)), SLOT(tableWidget_CopyItems(bool, int, int, int, int)));

	connectOrNot(link, ui->tabWidget_lead, SIGNAL(currentChanged(int)), SLOT(tabWidget_indexChange(int)));
	connectOrNot(link, m_advDialog->ui.tabWidget_lead, SIGNAL(currentChanged(int)), SLOT(tabWidget_indexChange(int)));

	connectOrNot(link, m_advDialog->ui.cob_baseLine, SIGNAL(currentIndexChanged(int)), SLOT(comboBox_IndexChanged(int)));
	connectOrNot(link, m_advDialog->ui.cob_widthMethod, SIGNAL(currentIndexChanged(int)), SLOT(comboBox_IndexChanged(int)));
}

void pinDetect1Wdg::iniUi()
{

}

void pinDetect1Wdg::iniData()
{
	m_param = nullptr;
	m_module = nullptr;

	m_autoThreDialog = new autoThreDialog(this);
	m_advDialog = new AdvancedDialog(this);

	m_selState = UnSel;
	m_testing = false;
	m_layerIdx = 0;
	m_image = nullptr;

	QStringList cbname;
	cbname << tr("length") << tr("interval") << tr("width") << tr("sweep") << tr("length diff");

	QStringList corrname;
	corrname << tr("length offset") << tr("interval offset") << tr("width offset");

	m_dialog = new QDialog(this);
	m_corrDialog = new QDialog(this);
	m_dialog->setMinimumSize(910, 500);
	m_dialog->setWindowTitle(tr("range of param"));
	m_corrDialog->setMinimumSize(910, 400);
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
	for (int i = 0;i < 5;i++) {
		ParamTables::ParamType tmptype = ParamTables::singleParam;
		cbtype.push_back(tmptype);
	}
	cbtype[4] = ParamTables::groupParam;
	m_paramTables = new ParamTables(5, cbtype, false, true, GRP_MAXLEAD, GRP_MAXLEAD, cbname, grpParam);
	m_paramTables->setHeadName(tr("param range"));

	vector<ParamTables::ParamType> corrtype;
	for (int i = 0;i < 3;i++) {
		ParamTables::ParamType tmptype = ParamTables::singleParam;
		corrtype.push_back(tmptype);
	}
	m_corrTable = new ParamTables(3, corrtype, false, false, GRP_MAXLEAD, GRP_MAXLEAD, corrname, grpCorr);
	m_corrTable->setValueRange(-999.999, 999.999, 3);
	m_corrTable->setHeadName(tr("compensation"));

	grpV1->addWidget(grpParam);
	grpV2->addWidget(grpCorr);
	V1Lay->addLayout(grpV1);
	V2Lay->addLayout(grpV2);
	m_dialog->setLayout(V1Lay);
	m_corrDialog->setLayout(V2Lay);
}

void pinDetect1Wdg::setUiValue()
{
	if(!m_param)
		return;

	m_paramTables->setGroupLeadNumber(m_param->grpPinNum[0], m_param->grpPinNum[1]);
	m_corrTable->setGroupLeadNumber(m_param->grpPinNum[0], m_param->grpPinNum[1]);
	refreshTableWidget();

	ui->grp_high->setVisible(false);
	m_advDialog->ui.cb_delOffSearchROI->setChecked(m_param->IsDelOffSearchRoi);
	m_advDialog->ui.cb_referEdgeRev->setChecked(m_param->IsReferEdgeRev);
	QString name[2];
	if (m_param->LRorUD == 0) {
		m_advDialog->ui.rb_LFRT->setChecked(true);
		name[0] = tr("left");
		name[1] = tr("right");
	}
	else if (m_param->LRorUD == 1) {
		m_advDialog->ui.rb_UPDW->setChecked(true);
		name[0] = tr("up");
		name[1] = tr("down");
	}
	changeLeadOrient(m_param->LRorUD);
	ui->tabWidget_lead->setCurrentIndex(m_param->currentGrp);
	for (int i = 0;i < 2;i++) {
		ui->tabWidget_lead->setTabText(i, name[i]);
		m_advDialog->ui.tabWidget_lead->setTabText(i, name[i]);
	}
	m_advDialog->ui.cob_baseLine->setCurrentIndex(m_param->IsDetBase);
	m_advDialog->ui.cob_widthMethod->setCurrentIndex(m_param->widthMethod);
	setCurrentGrp(m_param->currentGrp);
}

void pinDetect1Wdg::updateUi()
{
	connectSlots(false);
	setUiValue();
	connectSlots(true);
}

void pinDetect1Wdg::autoThreRegion()
{
	if (m_module->p_im->empty())
		return;
	HalOpenCV::cv2halImg(*m_module->p_im, m_image, false);

	int g = m_param->currentGrp;
	Hobject threRect, threRectRt, tmpImage, tmpImageRt;
	gen_rectangle1(&threRect, m_param->AutoThreReg[g].row1, m_param->AutoThreReg[g].col1,
		m_param->AutoThreReg[g].row2, m_param->AutoThreReg[g].col2);
	currentView()->finishDraw();
	currentView()->setColor(0, 0, 0);
	currentView()->setFilled(true);
	currentView()->dispRect1(m_param->AutoThreReg[g].row1, m_param->AutoThreReg[g].col1,
		m_param->AutoThreReg[g].row2, m_param->AutoThreReg[g].col2);  //把背景涂黑
	reduce_domain(*m_image, threRect, &tmpImage);
	if (m_param->IsAutoWhite[g]) {
		threshold(tmpImage, &tmpImage, m_param->ThreGrayVal[g], 255);
	}
	else {
		threshold(tmpImage, &tmpImage, 0, m_param->ThreGrayVal[g]);
	}
	currentView()->setColor(255, 255, 255);
	Hlong num;
	count_obj(tmpImage, &num);
	if (num > 0) {
		Hobject dispReg;
		union1(tmpImage, &dispReg);
		RlRegion reg;
		HalOpenCV::hal2cvRegion(dispReg, &reg);
		currentView()->dispRegion(&reg);
	}
	opening_rectangle1(tmpImage, &tmpImage, 3, 3);
	if (m_param->ThreLeadSize[g] > 0) {
		if (m_param->searchDir[g] == 2 || m_param->searchDir[g] == 3) {  //上下
			closing_rectangle1(tmpImage, &tmpImage, m_param->ThreLeadSize[g], m_param->ThreLeadSize[g] * 2);
		}
		else {
			closing_rectangle1(tmpImage, &tmpImage, m_param->ThreLeadSize[g] * 2, m_param->ThreLeadSize[g]);
		}
	}
	connection(tmpImage, &tmpImage);
	select_shape(tmpImage, &tmpImage, "area", "and", m_param->ThreMinArea[g], 999999);
	Hlong regionCount;
	count_obj(tmpImage, &regionCount);
	if (regionCount > GRP_MAXLEAD) {
		return;
	}
	else {
		m_param->grpPinNum[g] = regionCount;
		if (g == 0 || g == 1) {
			m_paramTables->setGroupLeadNumber(m_param->grpPinNum[0], m_param->grpPinNum[1]);
			m_corrTable->setGroupLeadNumber(m_param->grpPinNum[0], m_param->grpPinNum[1]);
			refreshTableWidget();
		}
	}
	HTuple lfRow1, lfCol1, lfRow2, lfCol2;
	smallest_rectangle1(tmpImage, &lfRow1, &lfCol1, &lfRow2, &lfCol2);
	HTuple rowsort, sortindex;
	//tuple_sort(lfRow1,&rowsort);
	if (m_param->searchDir[g] == 2 || m_param->searchDir[g] == 3) {    //上下排布
		tuple_sort_index(lfCol1, &sortindex);
	}
	else {
		tuple_sort_index(lfRow1, &sortindex);
	}
	HTuple mrow1, mrow2, mcol1, mcol2;
	for (int e = 0;e < regionCount;e++) {
		mrow1[e] = lfRow1[sortindex[e].I()].D();
		mrow2[e] = lfRow2[sortindex[e].I()].D();
		mcol1[e] = lfCol1[sortindex[e].I()].D();
		mcol2[e] = lfCol2[sortindex[e].I()].D();
	}
	vector<pinDetect1Param::RoiRegion>::iterator itbegin = m_param->RoiVec[g].begin() + 1;
	vector<pinDetect1Param::RoiRegion>::iterator itEnd = m_param->RoiVec[g].end();
	m_param->RoiVec[g].erase(itbegin, itEnd);

	pinDetect1Param::RoiRegion seg;
	for (int i = 0;i < m_param->grpPinNum[g];i++)
	{
		double dirval[4] = { 1,1,1,1 };  //上下左右
		if (m_param->searchDir[g] == 0) {
			dirval[3] = 0;  //左到右
			dirval[2] = 2;
		}
		else if (m_param->searchDir[g] == 1) {
			dirval[2] = 0; //右到左
			dirval[3] = 2;
		}
		else if (m_param->searchDir[g] == 2) {
			dirval[1] = 0; //上到下
			dirval[0] = 2;
		}
		else if (m_param->searchDir[g] == 3) {
			dirval[0] = 0;
			dirval[1] = 2;
		}
		seg.row1 = mrow1[i].D() - m_param->ThreLeadSize[g] * dirval[0];
		seg.row2 = mrow2[i].D() + m_param->ThreLeadSize[g] * dirval[1];
		seg.col1 = mcol1[i].D() - m_param->ThreLeadSize[g] * dirval[2];
		seg.col2 = mcol2[i].D() + m_param->ThreLeadSize[g] * dirval[3];
		m_param->RoiVec[g].push_back(seg);
	}

	currentView()->setColor(255, 255, 0);
	currentView()->setFilled(false);

	char buf[32];
	sprintf(buf, "autoROI%d", g + 1);

	currentView()->dispRect1(m_param->AutoThreReg[g].row1, m_param->AutoThreReg[g].col1, m_param->AutoThreReg[g].row2,
		m_param->AutoThreReg[g].col2, buf);
	int DrawCount = m_param->grpPinNum[g];
	double DrawRow1[GRP_MAXLEAD], DrawRow2[GRP_MAXLEAD], DrawCol1[GRP_MAXLEAD], DrawCol2[GRP_MAXLEAD];
	QStringList title;
	for (int i = 0;i < DrawCount;i++) {
		DrawRow1[i] = m_param->RoiVec[g][i + 1].row1;
		DrawCol1[i] = m_param->RoiVec[g][i + 1].col1;
		DrawRow2[i] = m_param->RoiVec[g][i + 1].row2;
		DrawCol2[i] = m_param->RoiVec[g][i + 1].col2;
		title << QString("Lead%1").arg(i + 1);
	}
	currentView()->setColor(255, 60, 255);
	currentView()->drawRects1(DrawCount, DrawRow1, DrawCol1, DrawRow2, DrawCol2, title);
	currentView()->setFilled(false);
	currentView()->setColor(255, 255, 0);
	currentView()->update();
	connectSlots(false);
	ui->sp_pinNum->setValue(m_param->grpPinNum[g]);
	connectSlots(true);
}

void pinDetect1Wdg::changeLeadOrient(bool updown)
{
	if (updown) {
		m_param->searchDir[0] = 2;
		m_param->searchDir[1] = 3;
		m_param->whiteBlackDir[0] = 0;
		m_param->whiteBlackDir[1] = 0;
		m_param->InputBaseIndex[0] = 0;
		m_param->InputBaseIndex[1] = 2;
	}
	else {
		m_param->searchDir[0] = 0;
		m_param->searchDir[1] = 1;
		m_param->whiteBlackDir[0] = 0;
		m_param->whiteBlackDir[1] = 0;
		m_param->InputBaseIndex[0] = 3;
		m_param->InputBaseIndex[1] = 1;
	}
}

void pinDetect1Wdg::setCurrentGrp(int index)
{
	refreshAutoDiaUi(index);

	m_advDialog->ui.cb_ampBest->setChecked(m_param->IsAmpBest[index]);
	ui->cb_length->setChecked(m_param->errPinLengthValid[index]);
	ui->cb_pinInterval->setChecked(m_param->errPinGapValid[index]);
	ui->cb_pinWidth->setChecked(m_param->errPinWidthValid[index]);
	ui->cb_pinSweep->setChecked(m_param->errPinSweepValid[index]);
	ui->cb_standoffDiff->setChecked(m_param->errPinStandOffDiffValid[index]);
	m_advDialog->ui.cb_baseInvert->setChecked(m_param->IsBaseInvert[index]);
	m_advDialog->ui.cb_useRefer->setChecked(m_param->IsRefer[index]);
	m_advDialog->ui.cb_inputRatio->setChecked(m_param->IsInputRatio[index]);

	m_advDialog->ui.dsp_ratio->setValue(m_param->sizeRatio[index]);

	ui->sp_pinNum->setValue(m_param->grpPinNum[index]);
	m_advDialog->ui.sp_offset->setValue(m_param->baseLineOffset[index]);
	m_advDialog->ui.sp_searchLineGap->setValue(m_param->searchLineGap[index]);
	ui->sp_minGrayDiff->setValue(m_param->minGrayDiff[index]);
	ui->hs_minGrayDiff->setValue(m_param->minGrayDiff[index]);
	m_advDialog->ui.sp_widLineOffset->setValue(m_param->widLineOffset[index]);
	m_advDialog->ui.sp_widRange->setValue(m_param->widOffsetRange[index]);
	m_advDialog->ui.sp_SHShift->setValue(m_param->shoulderShift[index]);
	ui->sp_pinArea->setValue(m_param->minArea[index]);
	ui->sp_noiseGray->setValue(m_param->noiseGray[index]);
	ui->sp_multGray->setValue(m_param->multGrayTimes[index]);
	ui->sp_deleteEdge->setValue(m_param->deleteEdge[index]);

	m_advDialog->ui.sp_widSearchLength->setValue(m_param->widSearchLength[index]);
	ui->sp_widthGrayDiff->setValue(m_param->WidthGrayDiff[index]);
	ui->hs_widthGrayDiff->setValue(m_param->WidthGrayDiff[index]);
}

void pinDetect1Wdg::refreshAutoDiaUi(int index)
{
	m_autoThreDialog->ui.sp_leadSize->setValue(m_param->ThreLeadSize[index]);
	m_autoThreDialog->ui.sp_minArea->setValue(m_param->ThreMinArea[index]);
	m_autoThreDialog->ui.sp_threshold->setValue(m_param->ThreGrayVal[index]);
	m_autoThreDialog->ui.hs_threshold->setValue(m_param->ThreGrayVal[index]);
	m_autoThreDialog->ui.cb_whitePin->setChecked(m_param->IsAutoWhite[index]);
	m_autoThreDialog->ui.cb_useInputRegion->setChecked(m_param->IsUseInputRegion);
	m_autoThreDialog->ui.sp_up->setValue(m_param->offsetUp);
	m_autoThreDialog->ui.sp_down->setValue(m_param->offsetDown);
	m_autoThreDialog->ui.sp_left->setValue(m_param->offsetLeft);
	m_autoThreDialog->ui.sp_right->setValue(m_param->offsetRight);
}

void pinDetect1Wdg::refreshTableWidget()
{
	for (int i = 0;i < 2;i++) {
		for (int j = 0;j < m_param->grpPinNum[i];j++) {
			m_paramTables->setValue(0, i, j, 0, m_param->errPinLengthMin[i][j]);
			m_paramTables->setValue(0, i, j, 1, m_param->errPinLengthMax[i][j]);
			m_paramTables->setValue(1, i, j, 0, m_param->errPinGapMin[i][j]);
			m_paramTables->setValue(1, i, j, 1, m_param->errPinGapMax[i][j]);
			m_paramTables->setValue(2, i, j, 0, m_param->errPinWidthMin[i][j]);
			m_paramTables->setValue(2, i, j, 1, m_param->errPinWidthMax[i][j]);
			m_paramTables->setValue(3, i, j, 0, m_param->errPinSweepMin[i][j]);
			m_paramTables->setValue(3, i, j, 1, m_param->errPinSweepMax[i][j]);

			m_corrTable->setValue(0, i, j, 0, m_param->lengthCorrNew[i][j]);
			m_corrTable->setValue(1, i, j, 0, m_param->InterCorrNew[i][j]);
			m_corrTable->setValue(2, i, j, 0, m_param->widthCorrNew[i][j]);
		}
		m_paramTables->setValue(4, i, 0, 0, m_param->errPinStandOffDiffMin[i]);
		m_paramTables->setValue(4, i, 0, 1, m_param->errPinStandOffDiffMax[i]);
	}
}

void pinDetect1Wdg::areaPaint()
{
	int grp = m_param->currentGrp;
	HTuple hommat;
	if (m_param->IsRefer[grp]) {
		if (m_module->p_homMat2d[grp]->size() == cv::Size(2, 3))
			HalOpenCV::cv2HalHomMat2D(*m_module->p_homMat2d[grp], &hommat);
		else
			hom_mat2d_identity(&hommat);
// 		try {
// 			hommat = *m_module->p_homMat2d[grp];
// 			//getDetectInData(m_outHomDefine[grp], hommat);
// 			double tv = hommat[0].D();
// 		}
// 		catch (HException& ex) {
// 			hom_mat2d_identity(&hommat);
// 		}
	}
	double len1[11], len2[11], roirow[11], roicol[11], roiangle[11];
	Hobject paintRoi;
	for (int i = 0;i < m_param->grpPinNum[grp];i++) {
		roirow[i] = (m_param->RoiVec[grp][i + 1].row2 + m_param->RoiVec[grp][i + 1].row1) / 2.0;
		roicol[i] = (m_param->RoiVec[grp][i + 1].col2 + m_param->RoiVec[grp][i + 1].col1) / 2.0;
		len1[i] = (m_param->RoiVec[grp][i + 1].col2 - m_param->RoiVec[grp][i + 1].col1) / 2.0;
		len2[i] = (m_param->RoiVec[grp][i + 1].row2 - m_param->RoiVec[grp][i + 1].row1) / 2.0;
		roiangle[i] = 0;
		if (m_param->IsRefer[grp]) {
			affine_trans_pixel(hommat, roirow[i], roicol[i], &roirow[i], &roicol[i]);
			roiangle[i] = atan((hommat[3].D() / hommat[0].D()));
		}
		Hobject roirec;
		gen_rectangle2(&roirec, roirow[i], roicol[i], roiangle[i], len1[i], len2[i]);
		reduce_domain(*m_image, roirec, &roirec);

		//threshold(roirec,&roirec,m_param->noiseGray[grp],255);

		Hobject meanimg;
		mean_image(roirec, &meanimg, 31/*m_param->minArea[2]*/, 31/*m_param->minArea[2]*/);
		dyn_threshold(roirec, meanimg, &roirec, m_param->noiseGray[grp], "light");

		opening_rectangle1(roirec, &roirec, 1, 1);
		connection(roirec, &roirec);
		select_shape(roirec, &roirec, "area", "and", 0, m_param->minArea[grp]);
		Hlong noisenum;
		count_obj(roirec, &noisenum);
		union1(roirec, &roirec);
		union2(paintRoi, roirec, &paintRoi);
	}
	//currentView()->clear();
	currentView()->clearLayer(m_layerIdx);
	currentView()->setFilled(true);
	currentView()->setColor(0, 0, 0);
	RlRegion reg;
	HalOpenCV::hal2cvRegion(paintRoi, &reg);
	currentView()->dispRegion(&reg);
	currentView()->update();
}

void pinDetect1Wdg::radioButton_toggled(bool check)
{
	QRadioButton* rb = qobject_cast<QRadioButton*>(sender());
	QString str0 = QString("");
	QString str1 = QString("");
	if (check) {
		if (rb == m_advDialog->ui.rb_LFRT) {
			str0 = rb->text();
			str1 = tr(" change to %1").arg(rb->text());
			m_param->LRorUD = 0;
			ui->tabWidget_lead->setTabText(0, tr("left"));
			ui->tabWidget_lead->setTabText(1, tr("right"));
			m_advDialog->ui.tabWidget_lead->setTabText(0, tr("left"));
			m_advDialog->ui.tabWidget_lead->setTabText(1, tr("right"));
			changeLeadOrient(0);
		}
		else if (rb == m_advDialog->ui.rb_UPDW) {
			str0 = rb->text();
			str1 = tr(" change to %1").arg(rb->text());
			m_param->LRorUD = 1;
			ui->tabWidget_lead->setTabText(0, tr("up"));
			ui->tabWidget_lead->setTabText(1, tr("down"));
			m_advDialog->ui.tabWidget_lead->setTabText(0, tr("up"));
			m_advDialog->ui.tabWidget_lead->setTabText(1, tr("down"));
			changeLeadOrient(1);
		}
		if (m_testing) {
			m_module->run(QString(""));
			m_module->viewResult(currentView(), QString(""), 0);
			currentView()->update();
			//testVal();
			//emit detect(m_detectIndex);
		}
		//log(LogL1, str0 + str1);
		m_module->setParamChanged(ProductLevel);
	}
}

void pinDetect1Wdg::checkBox_toggled(bool check)
{
	QCheckBox* cb = qobject_cast<QCheckBox*>(sender());
	QString str0 = QString("");
	QString str1 = QString("");
	str0 = ui->tabWidget_lead->tabText(m_param->currentGrp);
	if (check)
		str1 = tr(" Check");
	else
		str1 = tr(" Uncheck");

	if (cb == m_advDialog->ui.cb_delOffSearchROI) {
		m_param->IsDelOffSearchRoi = check;
	}
	else if (cb == m_advDialog->ui.cb_ampBest) {
		m_param->IsAmpBest[m_param->currentGrp] = check;
	}
	else if (cb == m_advDialog->ui.cb_useRefer) {
		for (int i = 0;i < GRP_NUM;i++) {
			m_param->IsRefer[i] = check;
		}
	}
	else if (cb == m_advDialog->ui.cb_baseInvert) {
		m_param->IsBaseInvert[m_param->currentGrp] = check;
	}
	else if (cb == m_advDialog->ui.cb_referEdgeRev) {
		m_param->IsReferEdgeRev = check;
	}

	else if (cb == ui->cb_length) {
		if (m_param->IsSameValid) {
			for (int i = 0;i < GRP_NUM;i++) {
				m_param->errPinLengthValid[i] = check;
			}
		}
		else {
			m_param->errPinLengthValid[m_param->currentGrp] = check;
		}
	}
	else if (cb == ui->cb_pinInterval) {
		if (m_param->IsSameValid) {
			for (int i = 0;i < GRP_NUM;i++) {
				m_param->errPinGapValid[i] = check;
			}
		}
		else {
			m_param->errPinGapValid[m_param->currentGrp] = check;
		}
	}
	else if (cb == ui->cb_pinWidth) {
		if (m_param->IsSameValid) {
			for (int i = 0;i < GRP_NUM;i++) {
				m_param->errPinWidthValid[i] = check;
			}
		}
		else {
			m_param->errPinWidthValid[m_param->currentGrp] = check;
		}
	}
	else if (cb == ui->cb_pinSweep) {
		if (m_param->IsSameValid) {
			for (int i = 0;i < GRP_NUM;i++) {
				m_param->errPinSweepValid[i] = check;
			}
		}
		else {
			m_param->errPinSweepValid[m_param->currentGrp] = check;
		}
	}
	else if (cb == ui->cb_standoffDiff) {
		if (m_param->IsSameValid) {
			for (int i = 0;i < GRP_NUM;i++) {
				m_param->errPinStandOffDiffValid[i] = check;
			}
		}
		else {
			m_param->errPinStandOffDiffValid[m_param->currentGrp] = check;
		}
		if (check) {
			ui->cb_length->setChecked(true);
		}
	}
	else if (cb == m_advDialog->ui.cb_inputRatio) {
		m_param->IsInputRatio[m_param->currentGrp] = check;
	}
	else if (cb == m_autoThreDialog->ui.cb_whitePin) {
		m_param->IsAutoWhite[m_param->currentGrp] = check;
		if (m_selState == SelAutoLead) {
			autoThreRegion();
		}
	}
	else if (cb == m_autoThreDialog->ui.cb_useInputRegion) {
		m_param->IsUseInputRegion = check;
	}
	m_module->setParamChanged(ProductLevel);
	//log(LogL1, str0 + QString(" ") + cb->text() + str1);
}

void pinDetect1Wdg::doubleSpinBox_valueChanged(double val)
{
	QDoubleSpinBox* dsp = qobject_cast<QDoubleSpinBox*>(sender());
	QString str0 = QString("");
	QString str1 = QString("");
	if (dsp == m_advDialog->ui.dsp_ratio) {
		str0 = ui->tabWidget_lead->tabText(m_param->currentGrp) + m_advDialog->ui.lb_ratio->text();
		str1 = tr(" change from %1 to %2")
			.arg(QString::number(m_param->sizeRatio[m_param->currentGrp], 'f', 3))
			.arg(QString::number(val, 'f', 3));
		m_param->sizeRatio[m_param->currentGrp] = val;
	}
	m_module->setParamChanged(ProductLevel);
	//log(LogL1, str0 + str1);
}

void pinDetect1Wdg::spinBox_valueChanged(int val)
{
	QSpinBox* sp = qobject_cast<QSpinBox*>(sender());
	QString str0 = QString("");
	QString str1 = QString("");
	if (sp == ui->sp_pinNum) {
		if (m_selState == SelAutoLead) {
			connectSlots(false);
			ui->sp_pinNum->setValue(m_param->grpPinNum[m_param->currentGrp]);
			connectSlots(true);
			return;
		}
		pinDetect1Param::RoiRegion seg;
		seg.row1 = seg.col1 = 100;
		seg.row2 = seg.col2 = 150;  //给新添加的roi固定的开始位1置
		if (val >= m_param->grpPinNum[m_param->currentGrp]) {
			for (int i = m_param->grpPinNum[m_param->currentGrp];i < val;i++) {
				seg.row1 = seg.col1 += 10;
				seg.row2 = seg.col2 += 10;
				m_param->RoiVec[m_param->currentGrp].push_back(seg);
			}
		}
		else if (val < m_param->grpPinNum[m_param->currentGrp]) {
			vector<pinDetect1Param::RoiRegion>::iterator itbegin = m_param->RoiVec[m_param->currentGrp].begin() + val + 1;
			vector<pinDetect1Param::RoiRegion>::iterator itEnd = m_param->RoiVec[m_param->currentGrp].end();
			m_param->RoiVec[m_param->currentGrp].erase(itbegin, itEnd);
		}
		str0 = ui->tabWidget_lead->tabText(m_param->currentGrp) + QString(" ") + ui->lb_pinNum->text();
		str1 = tr(" change from %1 to %2").arg(QString::number(m_param->grpPinNum[m_param->currentGrp])).arg(QString::number(val));
		m_param->grpPinNum[m_param->currentGrp] = val;
		if (m_param->currentGrp == 0 || m_param->currentGrp == 1) {
			m_paramTables->setGroupLeadNumber(m_param->grpPinNum[0], m_param->grpPinNum[1]);
			m_corrTable->setGroupLeadNumber(m_param->grpPinNum[0], m_param->grpPinNum[1]);
			refreshTableWidget();
		}
	}
	else if (sp == m_advDialog->ui.sp_offset) {
		str0 = ui->tabWidget_lead->tabText(m_param->currentGrp) + QString(" ") + m_advDialog->ui.lb_offset->text();
		str1 = tr(" change from %1 to %2").arg(QString::number(m_param->baseLineOffset[m_param->currentGrp])).arg(QString::number(val));
		m_param->baseLineOffset[m_param->currentGrp] = val;
	}
	else if (sp == m_advDialog->ui.sp_SHShift) {
		str0 = ui->tabWidget_lead->tabText(m_param->currentGrp) + QString(" ") + m_advDialog->ui.lb_SHShift->text();
		str1 = tr(" change from %1 to %2").arg(QString::number(m_param->shoulderShift[m_param->currentGrp])).arg(QString::number(val));
		m_param->shoulderShift[m_param->currentGrp] = val;
	}
	//else if(sp==ui->sp_extendSearchLine){
	//	m_param->extendSearchLine[m_param->currentGrp]=val;
	//}
	else if (sp == m_advDialog->ui.sp_widSearchLength) {
		str0 = ui->tabWidget_lead->tabText(m_param->currentGrp) + QString(" ") + m_advDialog->ui.lb_widSearchLength->text();
		str1 = tr(" change from %1 to %2").arg(QString::number(m_param->widSearchLength[m_param->currentGrp])).arg(QString::number(val));
		m_param->widSearchLength[m_param->currentGrp] = val;
	}
	else if (sp == ui->sp_widthGrayDiff) {
		str0 = ui->tabWidget_lead->tabText(m_param->currentGrp) + QString(" ") + ui->lb_widthGrayDiff->text();
		str1 = tr(" change from %1 to %2").arg(QString::number(m_param->WidthGrayDiff[m_param->currentGrp])).arg(QString::number(val));
		m_param->WidthGrayDiff[m_param->currentGrp] = val;
		connectSlots(false);
		ui->hs_widthGrayDiff->setValue(val);
		connectSlots(true);
		if (m_testing) {
			m_module->run(QString(""));
			m_module->viewResult(currentView(), QString(""), 0);
			currentView()->update();
			//testVal();
			//emit detect(m_detectIndex);
		}
	}
	else if (sp == ui->sp_minGrayDiff) {
		str0 = ui->tabWidget_lead->tabText(m_param->currentGrp) + QString(" ") + ui->lb_minGrayDiff->text();
		str1 = tr(" change from %1 to %2").arg(QString::number(m_param->minGrayDiff[m_param->currentGrp])).arg(QString::number(val));
		m_param->minGrayDiff[m_param->currentGrp] = val;
		connectSlots(false);
		ui->hs_minGrayDiff->setValue(val);
		connectSlots(true);
		if (m_testing) {
			m_module->run(QString(""));
			m_module->viewResult(currentView(), QString(""), 0);
			currentView()->update();
			//testVal();
			//emit detect(m_detectIndex);
		}
	}
	else if (sp == m_advDialog->ui.sp_searchLineGap) {
		str0 = ui->tabWidget_lead->tabText(m_param->currentGrp) + QString(" ") + m_advDialog->ui.lb_searchLineGap->text();
		str1 = tr(" change from %1 to %2").arg(QString::number(m_param->searchLineGap[m_param->currentGrp])).arg(QString::number(val));
		m_param->searchLineGap[m_param->currentGrp] = val;
		if (m_testing) {
			m_module->run(QString(""));
			m_module->viewResult(currentView(), QString(""), 0);
			currentView()->update();
			//testVal();
			//emit detect(m_detectIndex);
		}
	}
	else if (sp == m_advDialog->ui.sp_widLineOffset) {
		str0 = ui->tabWidget_lead->tabText(m_param->currentGrp) + QString(" ") + m_advDialog->ui.lb_widLineOffset->text();
		str1 = tr(" change from %1 to %2").arg(QString::number(m_param->widLineOffset[m_param->currentGrp])).arg(QString::number(val));
		m_param->widLineOffset[m_param->currentGrp] = val;
	}
	else if (sp == m_advDialog->ui.sp_widRange) {
		str0 = ui->tabWidget_lead->tabText(m_param->currentGrp) + QString(" ") + m_advDialog->ui.lb_widRange->text();
		str1 = tr(" change from %1 to %2").arg(QString::number(m_param->widOffsetRange[m_param->currentGrp])).arg(QString::number(val));
		m_param->widOffsetRange[m_param->currentGrp] = val;
	}
	else if (sp == ui->sp_deleteEdge) {
		str0 = ui->tabWidget_lead->tabText(m_param->currentGrp) + QString(" ") + ui->lb_deleteEdge->text();
		str1 = tr(" change from %1 to %2").arg(QString::number(m_param->deleteEdge[m_param->currentGrp])).arg(QString::number(val));
		m_param->deleteEdge[m_param->currentGrp] = val;
	}

	else if (sp == ui->sp_pinArea) {
		str0 = ui->tabWidget_lead->tabText(m_param->currentGrp) + QString(" ") + ui->lb_pinArea->text();
		str1 = tr(" change from %1 to %2").arg(QString::number(m_param->minArea[m_param->currentGrp])).arg(QString::number(val));
		m_param->minArea[m_param->currentGrp] = val;
		areaPaint();
	}
	else if (sp == ui->sp_noiseGray) {
		str0 = ui->tabWidget_lead->tabText(m_param->currentGrp) + QString(" ") + ui->lb_noiseGray->text();
		str1 = tr(" change from %1 to %2").arg(QString::number(m_param->noiseGray[m_param->currentGrp])).arg(QString::number(val));
		m_param->noiseGray[m_param->currentGrp] = val;
		areaPaint();
	}
	else if (sp == ui->sp_multGray) {
		str0 = ui->tabWidget_lead->tabText(m_param->currentGrp) + QString(" ") + ui->lb_multGray->text();
		str1 = tr(" change from %1 to %2").arg(QString::number(m_param->multGrayTimes[m_param->currentGrp])).arg(QString::number(val));
		m_param->multGrayTimes[m_param->currentGrp] = val;
		areaPaint();
	}
	else if (sp == m_autoThreDialog->ui.sp_leadSize) {
		str0 = ui->tabWidget_lead->tabText(m_param->currentGrp) + QString(" ") + m_autoThreDialog->ui.lb_leadSize->text();
		str1 = tr(" change from %1 to %2").arg(QString::number(m_param->ThreLeadSize[m_param->currentGrp])).arg(QString::number(val));
		m_param->ThreLeadSize[m_param->currentGrp] = val;
		if (m_selState == SelAutoLead) {
			autoThreRegion();
		}
	}
	else if (sp == m_autoThreDialog->ui.sp_minArea) {
		str0 = ui->tabWidget_lead->tabText(m_param->currentGrp) + QString(" ") + m_autoThreDialog->ui.lb_minarea->text();
		str1 = tr(" change from %1 to %2").arg(QString::number(m_param->ThreMinArea[m_param->currentGrp])).arg(QString::number(val));
		m_param->ThreMinArea[m_param->currentGrp] = val;
		if (m_selState == SelAutoLead) {
			autoThreRegion();
		}
	}
	else if (sp == m_autoThreDialog->ui.sp_threshold) {
		str0 = ui->tabWidget_lead->tabText(m_param->currentGrp) + QString(" ") + tr("auto threshold gray Value");
		str1 = tr(" change from %1 to %2").arg(QString::number(m_param->ThreGrayVal[m_param->currentGrp])).arg(QString::number(val));
		m_param->ThreGrayVal[m_param->currentGrp] = val;
		connectSlots(false);
		m_autoThreDialog->ui.hs_threshold->setValue(val);
		connectSlots(true);
		if (m_selState == SelAutoLead) {
			autoThreRegion();
		}
	}
	else if (sp == m_autoThreDialog->ui.sp_up) {
		str0 = m_autoThreDialog->ui.groupBox->windowIconText() + QString(": ") + m_autoThreDialog->ui.label_3->text();
		str1 = tr(" change from %1 to %2").arg(QString::number(m_param->offsetUp)).arg(QString::number(val));
		m_param->offsetUp = val;
	}
	else if (sp == m_autoThreDialog->ui.sp_down) {
		str0 = m_autoThreDialog->ui.groupBox->windowIconText() + QString(": ") + m_autoThreDialog->ui.label_4->text();
		str1 = tr(" change from %1 to %2").arg(QString::number(m_param->offsetDown)).arg(QString::number(val));
		m_param->offsetDown = val;
	}
	else if (sp == m_autoThreDialog->ui.sp_left) {
		str0 = m_autoThreDialog->ui.groupBox->windowIconText() + QString(": ") + m_autoThreDialog->ui.label_5->text();
		str1 = tr(" change from %1 to %2").arg(QString::number(m_param->offsetLeft)).arg(QString::number(val));
		m_param->offsetLeft = val;
	}
	else if (sp == m_autoThreDialog->ui.sp_right) {
		str0 = m_autoThreDialog->ui.groupBox->windowIconText() + QString(": ") + m_autoThreDialog->ui.label_5->text();
		str1 = tr(" change from %1 to %2").arg(QString::number(m_param->offsetRight)).arg(QString::number(val));
		m_param->offsetRight = val;
	}
	m_module->setParamChanged(ProductLevel);
	//log(LogL1, str0 + str1);
}

void pinDetect1Wdg::bt_selectRoi_toggled(bool check)
{
	if (check) {
		if (m_selState != UnSel) {
			connectSlots(false);
			ui->bt_selectRoi->setChecked(false);
			connectSlots(true);
			return;
		}
		m_selState = SelRegion;
		ui->sp_pinNum->setEnabled(false);
		//currentView()->clear();
		currentView()->clearLayer(m_layerIdx);
		double row1[11], col1[11], row2[11], col2[11];
		QStringList title;
		int plusVal, totalVal;
		if (m_param->IsDetBase == 1) {
			plusVal = 0;
			totalVal = m_param->grpPinNum[m_param->currentGrp] + 1;
		}
		else {
			plusVal = 1;
			totalVal = m_param->grpPinNum[m_param->currentGrp];
		}
		for (int i = 0;i < totalVal;i++) {
			row1[i] = m_param->RoiVec[m_param->currentGrp][i + plusVal].row1;
			col1[i] = m_param->RoiVec[m_param->currentGrp][i + plusVal].col1;
			row2[i] = m_param->RoiVec[m_param->currentGrp][i + plusVal].row2;
			col2[i] = m_param->RoiVec[m_param->currentGrp][i + plusVal].col2;
			QString leadName = tr("lead");
			if (m_param->IsDetBase == 1) {
				if (i == 0) {
					title << tr("bottom");
				}
				else {
					title << tr("%1%2").arg(leadName).arg(i);
				}
			}
			else {
				title << tr("%1%2").arg(leadName).arg(i);
				//sprintf(buf[i], "%s%d", leadName.toLocal8Bit().constData(), i + 1);
			}
		}
		currentView()->setColor(255, 0, 128);
		currentView()->drawRects1(totalVal + 1, row1, col1, row2, col2, title);
		currentView()->update();
	}
	else {
		m_selState = UnSel;
		ui->sp_pinNum->setEnabled(true);
		currentView()->finishDraw();
		double row1[11], col1[11], row2[11], col2[11];
		int plusVal, totalVal;
		if (m_param->IsDetBase == 1) {
			plusVal = 0;
			totalVal = m_param->grpPinNum[m_param->currentGrp] + 1;
		}
		else {
			plusVal = 1;
			totalVal = m_param->grpPinNum[m_param->currentGrp];
		}
		currentView()->getDrawRects1(totalVal, row1, col1, row2, col2);
		for (int i = 0;i < totalVal;i++) {
			m_param->RoiVec[m_param->currentGrp][i + plusVal].row1 = row1[i];
			m_param->RoiVec[m_param->currentGrp][i + plusVal].col1 = col1[i];
			m_param->RoiVec[m_param->currentGrp][i + plusVal].row2 = row2[i];
			m_param->RoiVec[m_param->currentGrp][i + plusVal].col2 = col2[i];
		}
		//currentView()->clear();
		currentView()->clearLayer(m_layerIdx);
		currentView()->update();
		m_module->setParamChanged(ProductLevel);
		//log(LogL1, ui->bt_selectRoi->text());
	}
}

void pinDetect1Wdg::comboBox_IndexChanged(int index)
{
	QComboBox* cob = qobject_cast<QComboBox*>(sender());
	QString str0 = QString("");
	QString str1 = QString("");

	if (cob == m_advDialog->ui.cob_baseLine) {
		str0 = cob->itemText(m_param->IsDetBase);
		str1 = m_advDialog->ui.lb_baseLine->text();
		m_param->IsDetBase = index;
	}
	else if (cob == m_advDialog->ui.cob_widthMethod) {
		str0 = cob->itemText(m_param->widthMethod);
		str1 = m_advDialog->ui.lb_widthMethod->text();
		m_param->widthMethod = index;
	}
	m_module->setParamChanged(ProductLevel);
	//log(LogL1, tr("%1 changed from %2 to %3").arg(str1).arg(str0).arg(cob->currentText()));
}

void pinDetect1Wdg::tabWidget_indexChange(int index)
{
	if (m_selState != UnSel) {
		connectSlots(false);
		ui->tabWidget_lead->setCurrentIndex(m_param->currentGrp);
		m_advDialog->ui.tabWidget_lead->setCurrentIndex(m_param->currentGrp);
		connectSlots(true);
		return;
	}
	QString str1 = tr(" change from %1 to %2").arg(ui->tabWidget_lead->tabText(m_param->currentGrp)).arg(ui->tabWidget_lead->tabText(index));
	m_param->currentGrp = index;
	connectSlots(false);
	ui->tabWidget_lead->setCurrentIndex(index);
	m_advDialog->ui.tabWidget_lead->setCurrentIndex(index);
	setCurrentGrp(index);
	connectSlots(true);
}

void pinDetect1Wdg::on_bt_highLevel_toggled(bool val)
{
	ui->grp_high->setVisible(val);
}

void pinDetect1Wdg::on_bt_test_toggled(bool val)
{
	m_testing = val;
	if (val) {
		ui->bt_test->setText(tr("exit test"));
		m_module->run(QString(""));
		m_module->viewResult(currentView(), QString(""), 0);
		currentView()->update();
		//testVal();
		//emit detect(m_detectIndex);
	}
	else {
		//currentView()->clear();
		currentView()->clearLayer(m_layerIdx);
		currentView()->update();
		ui->bt_test->setText(tr("test"));
	}
}

void pinDetect1Wdg::slider_valueChanged(int val)
{
	QSlider* sl = qobject_cast<QSlider*>(sender());
	if (sl == ui->hs_minGrayDiff) {
		m_param->minGrayDiff[m_param->currentGrp] = val;
		ui->sp_minGrayDiff->setValue(val);
	}
	else if (sl == ui->hs_widthGrayDiff) {
		m_param->WidthGrayDiff[m_param->currentGrp] = val;
		ui->sp_widthGrayDiff->setValue(val);
	}
	else if (sl == m_autoThreDialog->ui.hs_threshold) {
		m_autoThreDialog->ui.sp_threshold->setValue(val);
	}
}

void pinDetect1Wdg::on_bt_table_clicked()
{
	m_dialog->exec();
}

void pinDetect1Wdg::bt_corrTable_clicked()
{
	m_corrDialog->exec();
}

void pinDetect1Wdg::tableWidget_valueChanged(int checkIdx, int grpIdx, int leadIdx, bool minOrMax, double val)
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
				exval = m_param->errPinLengthMin[grpIdx][leadIdx];
				m_param->errPinLengthMin[grpIdx][leadIdx] = val;
			}
			else {
				exval = m_param->errPinLengthMax[grpIdx][leadIdx];
				m_param->errPinLengthMax[grpIdx][leadIdx] = val;
			}
		}
		else if (checkIdx == 1) {
			if (!minOrMax) {
				exval = m_param->errPinGapMin[grpIdx][leadIdx];
				m_param->errPinGapMin[grpIdx][leadIdx] = val;
			}
			else {
				exval = m_param->errPinGapMax[grpIdx][leadIdx];
				m_param->errPinGapMax[grpIdx][leadIdx] = val;
			}
		}
		else if (checkIdx == 2) {
			if (!minOrMax) {
				exval = m_param->errPinWidthMin[grpIdx][leadIdx];
				m_param->errPinWidthMin[grpIdx][leadIdx] = val;
			}
			else {
				exval = m_param->errPinWidthMax[grpIdx][leadIdx];
				m_param->errPinWidthMax[grpIdx][leadIdx] = val;
			}
		}
		else if (checkIdx == 3) {
			if (!minOrMax) {
				exval = m_param->errPinSweepMin[grpIdx][leadIdx];
				m_param->errPinSweepMin[grpIdx][leadIdx] = val;
			}
			else {
				exval = m_param->errPinSweepMax[grpIdx][leadIdx];
				m_param->errPinSweepMax[grpIdx][leadIdx] = val;
			}
		}
		else if (checkIdx == 4) {
			if (!minOrMax) {
				exval = m_param->errPinStandOffDiffMin[grpIdx];
				m_param->errPinStandOffDiffMin[grpIdx] = val;
			}
			else {
				exval = m_param->errPinStandOffDiffMax[grpIdx];
				m_param->errPinStandOffDiffMax[grpIdx] = val;
			}

		}
	}
	else if (pt == m_corrTable) {
		//leadname=tr(" group%1: ").arg(grpIdx+1);
		leadname = tr("lead%1 of group%2: ").arg(leadIdx + 1).arg(grpIdx + 1);
		cbname = tr("compensation") + cbname;
		str0 = leadname + cbname;
		if (checkIdx == 0) {
			exval = m_param->lengthCorrNew[grpIdx][leadIdx];
			m_param->lengthCorrNew[grpIdx][leadIdx] = val;
		}
		else if (checkIdx == 1) {
			exval = m_param->InterCorrNew[grpIdx][leadIdx];
			m_param->InterCorrNew[grpIdx][leadIdx] = val;
		}
		else if (checkIdx == 2) {
			exval = m_param->widthCorrNew[grpIdx][leadIdx];
			m_param->widthCorrNew[grpIdx][leadIdx] = val;
		}
	}
	str1 = tr(" change from %1 to %2").arg(QString::number(exval, 'f', 3)).arg(QString::number(val, 'f', 3));
	m_module->setParamChanged(ProductLevel);
	//log(LogL1, str0 + str1);
}

void pinDetect1Wdg::tableWidget_checkToggled(int index, bool check)
{

}

void pinDetect1Wdg::tableWidget_CopyItems(bool IsAll, int startCheck, int CheckRange, int starLead, int leadRange)
{
	ParamTables* pt = qobject_cast<ParamTables*>(sender());
	QString str0 = QString("");
	QString str1 = QString("");
	QString cbname, leadname, minmaxName;
	if (pt == m_paramTables) {
		int stc, edc, stl, edl;
		if (IsAll) {
			stc = 0;edc = 10;
			stl = 0;edl = m_param->grpPinNum[0] + m_param->grpPinNum[1];
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
				if (j == 0 || j == m_param->grpPinNum[0]) {
					continue;
				}
				int grpIdx;
				if (j >= m_param->grpPinNum[0]) {
					grpIdx = 1;
				}
				else {
					grpIdx = 0;
				}

				int leadIdx = j - grpIdx*m_param->grpPinNum[0];

				leadname = tr("lead%1 of group%2: ").arg(leadIdx + 1).arg(grpIdx + 1);
				if (minormax)
					minmaxName = tr(" min val");
				else
					minmaxName = tr(" max val");
				str0 = leadname + cbname + minmaxName;

				if (i == 0) {
					exval = m_param->errPinLengthMin[grpIdx][leadIdx];
					m_param->errPinLengthMin[grpIdx][leadIdx] = nowval = m_param->errPinLengthMin[grpIdx][0];

				}
				else if (i == 1) {
					exval = m_param->errPinLengthMax[grpIdx][leadIdx];
					m_param->errPinLengthMax[grpIdx][leadIdx] = nowval = m_param->errPinLengthMax[grpIdx][0];
				}
				else if (i == 2) {
					exval = m_param->errPinGapMin[grpIdx][leadIdx];
					m_param->errPinGapMin[grpIdx][leadIdx] = nowval = m_param->errPinGapMin[grpIdx][0];
				}
				else if (i == 3) {
					exval = m_param->errPinGapMax[grpIdx][leadIdx];
					m_param->errPinGapMax[grpIdx][leadIdx] = nowval = m_param->errPinGapMax[grpIdx][0];
				}
				else if (i == 4) {
					exval = m_param->errPinWidthMin[grpIdx][leadIdx];
					m_param->errPinWidthMin[grpIdx][leadIdx] = nowval = m_param->errPinWidthMin[grpIdx][0];
				}
				else if (i == 5) {
					exval = m_param->errPinWidthMax[grpIdx][leadIdx];
					m_param->errPinWidthMax[grpIdx][leadIdx] = nowval = m_param->errPinWidthMax[grpIdx][0];
				}
				else if (i == 6) {
					exval = m_param->errPinSweepMin[grpIdx][leadIdx];
					m_param->errPinSweepMin[grpIdx][leadIdx] = nowval = m_param->errPinSweepMin[grpIdx][0];
				}
				else if (i == 7) {
					exval = m_param->errPinSweepMax[grpIdx][leadIdx];
					m_param->errPinSweepMax[grpIdx][leadIdx] = nowval = m_param->errPinSweepMax[grpIdx][0];
				}
				if (i == 8 || i == 9) {
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
			stc = 0;edc = 3;
			stl = 0;edl = m_param->grpPinNum[0] + m_param->grpPinNum[1];
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
				if (j == 0 || j == m_param->grpPinNum[0]) {
					continue;
				}
				int grpIdx;
				if (j >= m_param->grpPinNum[0]) {
					grpIdx = 1;
				}
				else {
					grpIdx = 0;
				}
				int leadIdx = j - grpIdx*m_param->grpPinNum[0];

				leadname = tr("lead%1 of group%2: ").arg(leadIdx + 1).arg(grpIdx + 1);

				str0 = leadname + cbname;
				//if (i==0)
				//	continue;
				//	//m_param->lengthCorr[grpIdx]=m_param->lengthCorr[grpIdx];
				//else if(i==1)
				//	continue;
				//	//m_param->widthCorr[grpIdx]=m_param->widthCorr[grpIdx];
				//else if(i==2)
				//	continue;
				//	//m_param->InterCorr[grpIdx]=m_param->InterCorr[grpIdx];

				if (i == 0) {
					exval = m_param->lengthCorrNew[grpIdx][leadIdx];
					m_param->lengthCorrNew[grpIdx][leadIdx] = nowval = m_param->lengthCorrNew[grpIdx][0];
				}
				else if (i == 1) {
					exval = m_param->InterCorrNew[grpIdx][leadIdx];
					m_param->InterCorrNew[grpIdx][leadIdx] = nowval = m_param->InterCorrNew[grpIdx][0];
				}
				else if (i == 2) {
					exval = m_param->widthCorrNew[grpIdx][leadIdx];
					m_param->widthCorrNew[grpIdx][leadIdx] = nowval = m_param->widthCorrNew[grpIdx][0];
				}
				str1 = tr(" change from %1 to %2").arg(QString::number(exval, 'f', 3)).arg(QString::number(nowval, 'f', 3));
				//log(LogL1, str0 + str1);
			}
		}
	}
	m_module->setParamChanged(ProductLevel);
}

void pinDetect1Wdg::autoThre_bt_toggled(bool check)
{
	QPushButton* bt = qobject_cast<QPushButton*>(sender());
	QString str0 = bt->text();
	if (bt == m_autoThreDialog->ui.bt_selAutoThre) {
		if (check) {
			if (m_selState != UnSel) {
				connectSlots(false);
				m_autoThreDialog->ui.bt_selAutoThre->setChecked(false);
				connectSlots(true);
				return;
			}
			m_selState = SelAutoReg;

			currentView()->finishDraw();
			//currentView()->clear();
			currentView()->clearLayer(m_layerIdx);
			bt->setText(tr("Auto ROI OK"));
			char buf[32];
			currentView()->setFilled(false);
			currentView()->setColor(255, 60, 255);
			int index = m_param->currentGrp;
			sprintf(buf, "autoROI%d", index + 1);
			currentView()->drawRect1(m_param->AutoThreReg[index].row1, m_param->AutoThreReg[index].col1, m_param->AutoThreReg[index].row2
				, m_param->AutoThreReg[index].col2, buf);
			currentView()->setColor(255, 255, 0);
			currentView()->update();
		}
		else {
			m_selState = UnSel;
			bt->setText(tr("Auto ROI"));
			//currentView()->clear();
			currentView()->clearLayer(m_layerIdx);
			currentView()->finishDraw();
			int index = m_param->currentGrp;
			currentView()->getDrawRect1(m_param->AutoThreReg[index].row1, m_param->AutoThreReg[index].col1, m_param->AutoThreReg[index].row2,
				m_param->AutoThreReg[index].col2);
			currentView()->setColor(255, 255, 0);
			currentView()->setFilled(false);
			currentView()->update();
		}
	}
	else if (bt == m_autoThreDialog->ui.bt_selectPin) {
		if (check) {
			if (m_selState != UnSel) {
				connectSlots(false);
				m_autoThreDialog->ui.bt_selectPin->setChecked(false);
				connectSlots(true);
				return;
			}
			m_selState = SelAutoLead;

			bt->setText(tr("Lead ROI OK"));
			currentView()->finishDraw();
			//currentView()->clear();
			currentView()->clearLayer(m_layerIdx);
			currentView()->setColor(255, 255, 0);
			currentView()->setFilled(false);
			char buf[32];
			int index = m_param->currentGrp;
			sprintf(buf, "autoROI%d", index + 1);

			currentView()->dispRect1(m_param->AutoThreReg[index].row1, m_param->AutoThreReg[index].col1, m_param->AutoThreReg[index].row2,
				m_param->AutoThreReg[index].col2, buf);

			int DrawCount = m_param->grpPinNum[index];
			double DrawRow1[GRP_MAXLEAD], DrawRow2[GRP_MAXLEAD], DrawCol1[GRP_MAXLEAD], DrawCol2[GRP_MAXLEAD];
			QStringList title;
			for (int i = 0;i < DrawCount;i++) {
				DrawRow1[i] = m_param->RoiVec[index][i + 1].row1;
				DrawCol1[i] = m_param->RoiVec[index][i + 1].col1;
				DrawRow2[i] = m_param->RoiVec[index][i + 1].row2;
				DrawCol2[i] = m_param->RoiVec[index][i + 1].col2;
				title << QString("Lead%1").arg(i + 1);
			}
			currentView()->setFilled(false);
			currentView()->setColor(255, 60, 255);
			currentView()->drawRects1(DrawCount, DrawRow1, DrawCol1, DrawRow2, DrawCol2, title);
			currentView()->setColor(255, 255, 0);
			currentView()->update();
		}
		else {
			m_selState = UnSel;
			bt->setText(tr("Select Lead"));
			//currentView()->clear();
			currentView()->clearLayer(m_layerIdx);
			currentView()->finishDraw();
			int index = m_param->currentGrp;
			double DrawRow1[GRP_MAXLEAD], DrawRow2[GRP_MAXLEAD], DrawCol1[GRP_MAXLEAD], DrawCol2[GRP_MAXLEAD];
			int DrawCount = m_param->grpPinNum[index];
			currentView()->getDrawRects1(DrawCount, DrawRow1, DrawCol1, DrawRow2, DrawCol2);
			for (int i = 0;i < DrawCount;i++) {
				m_param->RoiVec[index][i + 1].row1 = DrawRow1[i];
				m_param->RoiVec[index][i + 1].col1 = DrawCol1[i];
				m_param->RoiVec[index][i + 1].row2 = DrawRow2[i];
				m_param->RoiVec[index][i + 1].col2 = DrawCol2[i];
			}
			currentView()->setColor(255, 255, 0);
			currentView()->setFilled(false);
			currentView()->update();
		}
	}
	else if (bt = m_autoThreDialog->ui.bt_selSearchRoi) {

		if (check) {
			if (m_selState != UnSel) {
				connectSlots(false);
				m_autoThreDialog->ui.bt_selAutoThre->setChecked(false);
				connectSlots(true);
				return;
			}
			m_selState = SelSearch;

			//currentView()->clear();
			currentView()->clearLayer(m_layerIdx);
			bt->setText(tr("Search ROI OK"));
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
			bt->setText(tr("Search ROI"));
			//currentView()->clear();
			currentView()->clearLayer(m_layerIdx);
			currentView()->finishDraw();
			currentView()->getDrawRect2(m_param->SearchEdgeRoi.row, m_param->SearchEdgeRoi.col, 
				m_param->SearchEdgeRoi.angle, m_param->SearchEdgeRoi.len1, m_param->SearchEdgeRoi.len2);
			currentView()->setColor(255, 255, 0);
			currentView()->setFilled(false);
			currentView()->update();
		}
	}
	m_module->setParamChanged(ProductLevel);
	//log(LogL1, str0);
}

void pinDetect1Wdg::on_bt_autoThreshold_clicked()
{
	if (m_autoThreDialog->isVisible())
		return;
	m_autoThreDialog->show();
}

void pinDetect1Wdg::on_bt_advance_clicked()
{
	if (m_autoThreDialog->isVisible() || m_selState != UnSel)
		return;
	m_advDialog->exec();
}
