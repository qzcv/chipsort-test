#include "locationWdg.h"
#include "locationMod.h"
#include "locationParam.h"
#include "ui_locationWdg.h"
#include <QMessageBox>
#include "ImageView.h"
#include "halOpenCV.h"
#include "paramTables.h"

locationWdg::locationWdg(int i) :
	ModSetWidget(),ui(new Ui::locationWdg),m_widgetType(i)
{
	m_hasConnect = false;
	ui->setupUi(this);
	iniData();
	iniUi();
	m_param = nullptr;
	m_module = nullptr;
}

locationWdg::~locationWdg()
{
	delete ui;
}

void locationWdg::setModule(UnitModule *module)
{
	m_module=dynamic_cast<locationMod*>(module);
	m_param = m_module->m_param;
	m_image = m_module->m_image;
	//currentView()->setLayer(m_layerIdx);
	//updateUi();
}

void locationWdg::showEvent(QShowEvent * event)
{
	updateUi();
}

void locationWdg::connectSlots(bool link)
{
	if(m_hasConnect==link)
		return;
	m_hasConnect=link;

	connectOrNot(link, ui->rb_upDw, SIGNAL(toggled(bool)), SLOT(radioButton_toggled(bool)));
	connectOrNot(link, ui->rb_lfRt, SIGNAL(toggled(bool)), SLOT(radioButton_toggled(bool)));

	connectOrNot(link, ui->cb_errWidth, SIGNAL(toggled(bool)), SLOT(checkBox_toggled(bool)));
	connectOrNot(link, ui->cb_errHeight, SIGNAL(toggled(bool)), SLOT(checkBox_toggled(bool)));
	connectOrNot(link, ui->cb_errAngle, SIGNAL(toggled(bool)), SLOT(checkBox_toggled(bool)));
	connectOrNot(link, ui->cb_errCenterOffset, SIGNAL(toggled(bool)), SLOT(checkBox_toggled(bool)));
	connectOrNot(link, ui->cb_leadSize, SIGNAL(toggled(bool)), SLOT(checkBox_toggled(bool)));

	connectOrNot(link, ui->cb_padStand, SIGNAL(toggled(bool)), SLOT(checkBox_toggled(bool)));
	connectOrNot(link, ui->cb_standoff, SIGNAL(toggled(bool)), SLOT(checkBox_toggled(bool)));
	connectOrNot(link, ui->cb_pinInterval, SIGNAL(toggled(bool)), SLOT(checkBox_toggled(bool)));
	connectOrNot(link, ui->cb_pinWidth, SIGNAL(toggled(bool)), SLOT(checkBox_toggled(bool)));
	connectOrNot(link, ui->cb_standoffDiff, SIGNAL(toggled(bool)), SLOT(checkBox_toggled(bool)));
	connectOrNot(link, ui->cb_coplanar, SIGNAL(toggled(bool)), SLOT(checkBox_toggled(bool)));

	//connectOrNot(link, ui->dsp_UpDwGap,SIGNAL(valueChanged(double)),SLOT(doubleSpinBox_valueChanged(double)));
	//connectOrNot(link, ui->dsp_LfRtGap,SIGNAL(valueChanged(double)),SLOT(doubleSpinBox_valueChanged(double)));
	connectOrNot(link, ui->sp_leadSize, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_maxAngle, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_errWidthMin, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_errWidthMax, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_errAngle, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_errHeightMin, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_errHeightMax, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_errCenterOffset, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_grp1pinNum, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_grp2pinNum, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));

	connectOrNot(link, ui->dsp_pinLength, SIGNAL(valueChanged(double)), SLOT(doubleSpinBox_valueChanged(double)));
	connectOrNot(link, ui->dsp_scalePaichu, SIGNAL(valueChanged(double)), SLOT(doubleSpinBox_valueChanged(double)));


	connectOrNot(link, ui->sp_minGrayDiff, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_searchLineGap, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_lineOffset, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_offsetPlus, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_minArea, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_standoffOffset, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_grayval, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_edgeGray, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));

	connectOrNot(link, ui->dsp_factor, SIGNAL(valueChanged(double)), SLOT(doubleSpinBox_valueChanged(double)));
	connectOrNot(link, ui->sp_paichu, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->cb_paichu, SIGNAL(toggled(bool)), SLOT(checkBox_toggled(bool)));

	connectOrNot(link, ui->tabWidget, SIGNAL(currentChanged(int)), SLOT(tabWidget_indexChange(int)));

	connectOrNot(link, m_paramTables, SIGNAL(CellChanged(int, int, int, bool, double)), SLOT(tableWidget_valueChanged(int, int, int, bool, double)));
	connectOrNot(link, m_corrTable, SIGNAL(CellChanged(int, int, int, bool, double)), SLOT(tableWidget_valueChanged(int, int, int, bool, double)));
	connectOrNot(link, m_offTable, SIGNAL(CellChanged(int, int, int, bool, double)), SLOT(tableWidget_valueChanged(int, int, int, bool, double)));

	connectOrNot(link, m_paramTables, SIGNAL(CopyItems(bool, int, int, int, int)), SLOT(tableWidget_CopyItems(bool, int, int, int, int)));
	connectOrNot(link, m_corrTable, SIGNAL(CopyItems(bool, int, int, int, int)), SLOT(tableWidget_CopyItems(bool, int, int, int, int)));
	connectOrNot(link, m_offTable, SIGNAL(CopyItems(bool, int, int, int, int)), SLOT(tableWidget_CopyItems(bool, int, int, int, int)));
}

void locationWdg::iniUi()
{

}

void locationWdg::iniData()
{
	m_image = nullptr;
	m_layerIdx = 0;

	QStringList cbname;
	cbname << tr("standoff") << tr("width") << tr("interval") << tr("standoff diff") << tr("coplanar");

	QStringList corrname;
	corrname << tr("standoff COMP") << tr("width COMP") << tr("interval COMP");

	QStringList offname;
	offname << tr("standoff offset") << tr("width offset") << tr("interval offset");

	m_dialog = new QDialog(this);
	m_corrDialog = new QDialog(this);
	m_dialog->setMinimumSize(910, 600);
	m_dialog->setWindowTitle(tr("range of param"));

	m_corrDialog->setMinimumSize(910, 400);
	m_corrDialog->setWindowTitle(tr("range of corr param"));

	QVBoxLayout* V1Lay = new QVBoxLayout;
	QVBoxLayout* V2Lay = new QVBoxLayout;

	QGroupBox *grpParam, *grpCorr, *grpOff;
	QVBoxLayout *grpV1, *grpV2, *grpV3;
	grpV1 = new QVBoxLayout;
	grpV2 = new QVBoxLayout;
	grpV3 = new QVBoxLayout;
	grpParam = new QGroupBox();
	grpCorr = new QGroupBox();
	grpOff = new QGroupBox();
	grpParam->setMinimumWidth(900);
	grpCorr->setMinimumWidth(900);
	grpOff->setMinimumWidth(900);

	vector<ParamTables::ParamType> cbtype;
	for (int i = 0;i < cbname.size();i++) {
		ParamTables::ParamType tmptype = ParamTables::singleParam;
		cbtype.push_back(tmptype);
	}
	cbtype[3] = ParamTables::groupParam;
	cbtype[4] = ParamTables::sameParam;
	m_paramTables = new ParamTables(cbname.size(), cbtype, false, true, GRP_MAX_LEADNUM, GRP_MAX_LEADNUM, cbname, grpParam);
	m_paramTables->setHeadName(tr("param range"));
	m_paramTables->setTableHeight(365);

	vector<ParamTables::ParamType> corrtype;
	for (int i = 0;i < corrname.size();i++) {
		ParamTables::ParamType tmptype = ParamTables::singleParam;
		corrtype.push_back(tmptype);
	}
	m_corrTable = new ParamTables(corrname.size(), corrtype, false, false, GRP_MAX_LEADNUM, GRP_MAX_LEADNUM, corrname, grpCorr);
	m_corrTable->setValueRange(-999.999, 999.999, 3);
	m_corrTable->setHeadName(tr("compensation"));
	m_corrTable->setTableHeight(300);

	vector<ParamTables::ParamType> offtype;
	for (int i = 0;i < offname.size();i++) {
		ParamTables::ParamType tmptype = ParamTables::singleParam;
		offtype.push_back(tmptype);
	}
	m_offTable = new ParamTables(offname.size(), offtype, false, false, GRP_MAX_LEADNUM, GRP_MAX_LEADNUM, offname, grpOff);
	m_offTable->setValueRange(-999.999, 999.999, 3);
	m_offTable->setHeadName(tr("offset"));
	m_offTable->setTableHeight(175);

	grpV1->addWidget(grpParam);
	grpV2->addWidget(grpCorr);
	grpV3->addWidget(grpOff);
	V1Lay->addLayout(grpV1, 2);
	V1Lay->addLayout(grpV3, 1);

	V2Lay->addLayout(grpV2, 1);
	m_dialog->setLayout(V1Lay);
	m_corrDialog->setLayout(V2Lay);
}

void locationWdg::setUiValue()
{
	if(!m_param)
		return;

	m_paramTables->setGroupLeadNumber(m_param->grpPinNum[0], m_param->grpPinNum[1]);
	m_corrTable->setGroupLeadNumber(m_param->grpPinNum[0], m_param->grpPinNum[1]);
	m_offTable->setGroupLeadNumber(m_param->grpPinNum[0], m_param->grpPinNum[1]);
	refreshTableWidget();

	if (m_param->NowGrp < 4) {
		setCurrentGroup(m_param->NowGrp);
	}
	ui->tabWidget->setCurrentIndex(m_param->NowGrp);
	ui->grp_high->setVisible(false);
	ui->sp_maxAngle->setValue(m_param->maxAngle);

	if (m_param->upDwOrLfRt == 0) {
		ui->rb_upDw->setChecked(true);
	}
	else {
		ui->rb_lfRt->setChecked(true);
	}

	ui->cb_padStand->setChecked(m_param->IsPadStand);
	if (m_param->IsPadStand) {
		m_param->errPinWidthValid = false;
		m_param->errPinGapValid = false;
	}

	ui->cb_pinWidth->setVisible(!m_param->IsPadStand);
	ui->cb_pinInterval->setVisible(!m_param->IsPadStand);

	ui->dsp_pinLength->setValue(m_param->pinlength);
	ui->cb_standoff->setChecked(m_param->errPinStandOffValid == 1);
	ui->cb_pinInterval->setChecked(m_param->errPinGapValid == 1);
	ui->cb_pinWidth->setChecked(m_param->errPinWidthValid == 1);
	ui->cb_standoffDiff->setChecked(m_param->errPinStandOffDiffValid == 1);
	ui->cb_coplanar->setChecked(m_param->errCoplanarValid);
	ui->cb_errWidth->setChecked(m_param->IsErrWidth == 1);
	ui->sp_errWidthMin->setValue(m_param->errWidthMin);
	ui->sp_grp1pinNum->setValue(m_param->grpPinNum[0]);
	ui->sp_grp2pinNum->setValue(m_param->grpPinNum[1]);
	ui->sp_errWidthMax->setValue(m_param->errWidthMax);
	ui->sp_errWidthMin->setEnabled(m_param->IsErrWidth==1);
	ui->sp_errWidthMax->setEnabled(m_param->IsErrWidth==1);

	ui->cb_errHeight->setChecked(m_param->IsErrHeight == 1);
	ui->sp_errHeightMin->setValue(m_param->errHeightMin);
	ui->sp_errHeightMax->setValue(m_param->errHeightmax);
	ui->sp_errHeightMin->setEnabled(m_param->IsErrHeight==1);
	ui->sp_errHeightMax->setEnabled(m_param->IsErrHeight==1);

	ui->cb_errAngle->setChecked(m_param->IsErrAngle == 1);
	ui->sp_errAngle->setValue(m_param->errAngle);
	ui->sp_errAngle->setEnabled(m_param->IsErrAngle==1);

	ui->cb_errCenterOffset->setChecked(m_param->IsErrCenterOffset == 1);
	ui->sp_errCenterOffset->setValue(m_param->errCenterOffset);
	ui->sp_errCenterOffset->setEnabled(m_param->IsErrCenterOffset==1);
}

void locationWdg::updateUi()
{
	connectSlots(false);
	setUiValue();
	connectSlots(true);
}

void locationWdg::setCurrentGroup(int idx)
{
	ui->sp_searchLineGap->setValue(m_param->searchLineGap[idx]);
	ui->sp_minGrayDiff->setValue(m_param->minGrayDiff[idx]);
	ui->sp_lineOffset->setValue(m_param->lineOffset[idx]);
	ui->sp_offsetPlus->setValue(m_param->offsetPlus[idx]);
	ui->sp_minArea->setValue(m_param->minArea[idx]);
	ui->sp_standoffOffset->setValue(m_param->standoffOffset[idx]);
	ui->sp_grayval->setValue(m_param->grayVal[idx]);
	ui->sp_edgeGray->setValue(m_param->edgeGray[idx]);
	ui->dsp_factor->setValue(m_param->clipfactor[idx]);
	ui->dsp_scalePaichu->setValue(m_param->scalePaichu[idx]);
	ui->cb_paichu->setChecked(m_param->IsPaichuLine[idx]);
	ui->sp_paichu->setValue(m_param->paichuLine[idx]);
	ui->cb_leadSize->setChecked(m_param->isLeadSize[idx]);
	ui->sp_leadSize->setEnabled(m_param->isLeadSize[idx]);
	ui->sp_leadSize->setValue(m_param->leadSize[idx]);
}

void locationWdg::refreshTableWidget()
{
	for (int i = 0;i < 2;i++) {
		for (int j = 0;j < m_param->grpPinNum[i];j++) {
			m_paramTables->setValue(0, i, j, 0, m_param->errPinStandOffMin[i][j]);
			m_paramTables->setValue(0, i, j, 1, m_param->errPinStandOffMax[i][j]);
			m_paramTables->setValue(1, i, j, 0, m_param->errPinWidthMin[i][j]);
			m_paramTables->setValue(1, i, j, 1, m_param->errPinWidthMax[i][j]);
			m_paramTables->setValue(2, i, j, 0, m_param->errPinGapMin[i][j]);
			m_paramTables->setValue(2, i, j, 1, m_param->errPinGapMax[i][j]);

			m_corrTable->setValue(0, i, j, 0, m_param->standOffCorr[i][j]);
			m_corrTable->setValue(1, i, j, 0, m_param->widthCorr[i][j]);
			m_corrTable->setValue(2, i, j, 0, m_param->InterCorr[i][j]);

			m_offTable->setValue(0, i, j, 0, m_param->offStandOffCorr[i][j]);
			m_offTable->setValue(1, i, j, 0, m_param->offWidthCorr[i][j]);
			m_offTable->setValue(2, i, j, 0, m_param->offInterCorr[i][j]);
		}
		m_paramTables->setValue(3, i, 0, 0, m_param->errPinStandOffDiffMin[i]);
		m_paramTables->setValue(3, i, 0, 1, m_param->errPinStandOffDiffMax[i]);
	}
	m_paramTables->setValue(4, 0, 0, 0, m_param->errCoplanarMin);
	m_paramTables->setValue(4, 0, 0, 1, m_param->errCoplanar);
}

void locationWdg::on_bt_setOrigin_clicked()
{
	HTuple centerRow, centerCol;
	m_module->getDispOutData(CenterRow, centerRow);
	m_module->getDispOutData(CenterCol, centerCol);
	if (centerRow.Num() == 0)
		return;

	if (QMessageBox::information(this, tr("Hint"),
		tr("Are you sure to change the current center point:(%1,%2) to the template center?")
		.arg(centerCol[0].D(), 0, 'f', 3).arg(centerRow[0].D(), 0, 'f', 3),
		QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
		//log(LogL1, ui->bt_setOrigin->text());
		m_param->TmpCenterRow = centerRow[0].D();
		m_param->TmpCenterCol = centerCol[0].D();

		m_module->setParamChanged(ProductLevel);
	}
}

void locationWdg::radioButton_toggled(bool check)
{
	QRadioButton* rb = qobject_cast<QRadioButton*>(sender());
	QString str = QString("");
	if (check) {
		if (rb == ui->rb_upDw) {
			str = tr(" change from %1 to %2").arg(ui->rb_lfRt->text()).arg(ui->rb_upDw->text());
			m_param->upDwOrLfRt = 0;
		}
		else if (rb == ui->rb_lfRt) {
			str = tr(" change from %1 to %2").arg(ui->rb_upDw->text()).arg(ui->rb_lfRt->text());
			m_param->upDwOrLfRt = 1;
		}
		//log(LogL1, str);
		m_module->setParamChanged(ProductLevel);
	}
}

void locationWdg::checkBox_toggled(bool check)
{
	QCheckBox* cb = qobject_cast<QCheckBox*>(sender());
	QString str0 = QString("");
	QString str1 = cb->text();
	QString str2 = QString("");
	if (check)
		str2 = tr(" Check");
	else
		str2 = tr(" unCheck");
	if (cb == ui->cb_paichu) {
		str0 = ui->tabWidget->tabText(m_param->NowGrp);
		m_param->IsPaichuLine[m_param->NowGrp] = check;
	}
	else if (cb == ui->cb_errWidth) {
		m_param->IsErrWidth = check;
		ui->sp_errWidthMin->setEnabled(check);
		ui->sp_errWidthMax->setEnabled(check);
	}
	else if (cb == ui->cb_errHeight) {
		m_param->IsErrHeight = check;
		ui->sp_errHeightMin->setEnabled(check);
		ui->sp_errHeightMax->setEnabled(check);
	}
	else if (cb == ui->cb_errAngle) {
		m_param->IsErrAngle = check;
		ui->sp_errAngle->setEnabled(check);
	}
	else if (cb == ui->cb_errCenterOffset) {
		m_param->IsErrCenterOffset = check;
		ui->sp_errCenterOffset->setEnabled(check);
	}
	else if (cb == ui->cb_standoff) {
		m_param->errPinStandOffValid = check;
	}
	else if (cb == ui->cb_pinInterval) {
		m_param->errPinGapValid = check;
	}
	else if (cb == ui->cb_pinWidth) {
		m_param->errPinWidthValid = check;
	}
	else if (cb == ui->cb_standoffDiff) {
		m_param->errPinStandOffDiffValid = check;
	}
	else if (cb == ui->cb_coplanar) {
		m_param->errCoplanarValid = check;
	}
	else if (cb == ui->cb_leadSize)
	{
		str0 = ui->tabWidget->tabText(m_param->NowGrp);
		m_param->isLeadSize[m_param->NowGrp] = check;
		ui->sp_leadSize->setEnabled(check);
	}
	else if (cb == ui->cb_padStand) {
		m_param->IsPadStand = check;
		if (check)
		{
			ui->cb_pinWidth->setChecked(false);
			ui->cb_pinInterval->setChecked(false);
		}
		ui->cb_pinWidth->setVisible(!check);
		ui->cb_pinInterval->setVisible(!check);
	}
	//log(LogL1, str0 + str1 + str2);
	m_module->setParamChanged(ProductLevel);
}

void locationWdg::doubleSpinBox_valueChanged(double val)
{
	QDoubleSpinBox* dsp = qobject_cast<QDoubleSpinBox*>(sender());
	QString str1, str2;
	str1 = ("");
	str2 = ("");
	if (dsp == ui->dsp_pinLength) {
		str1 = ui->label_23->text();
		str2 = tr(" change from %1 to %2")
			.arg(QString::number(m_param->pinlength, 'f', 3))
			.arg(QString::number(val, 'f', 3));
		m_param->pinlength = val;
	}
	else if (dsp == ui->dsp_factor) {
		str1 = ui->tabWidget->tabText(m_param->NowGrp) + QString(" ") + ui->label_14->text();
		str2 = tr(" change from %1 to %2")
			.arg(QString::number(m_param->clipfactor[m_param->NowGrp], 'f', 1))
			.arg(QString::number(val, 'f', 1));
		m_param->clipfactor[m_param->NowGrp] = val;
	}
	else if (dsp == ui->dsp_scalePaichu) {
		str1 = ui->tabWidget->tabText(m_param->NowGrp) + QString(" ") + ui->label_15->text();
		str2 = tr(" change from %1 to %2")
			.arg(QString::number(m_param->scalePaichu[m_param->NowGrp], 'f', 2))
			.arg(QString::number(val, 'f', 2));
		m_param->scalePaichu[m_param->NowGrp] = val;
	}
	//log(LogL1, str1 + str2);
	m_module->setParamChanged(ProductLevel);
}

void locationWdg::spinBox_valueChanged(int val)
{
	QSpinBox* sp = qobject_cast<QSpinBox*>(sender());

	QString str0, str1;
	str0 = QString("");
	str1 = QString("");

	if (sp == ui->sp_maxAngle) {
		str0 = ui->label->text();
		str1 = tr(" change from %1 to %2").arg(QString::number(m_param->maxAngle)).arg(QString::number(val));
		m_param->maxAngle = val;
	}
	else if (sp == ui->sp_grp1pinNum) {
		str0 = ui->label_2->text() + ui->label_3->text();
		str1 = tr(" change from %1 to %2").arg(QString::number(m_param->grpPinNum[0])).arg(QString::number(val));
		m_param->grpPinNum[0] = val;
		m_paramTables->setGroupLeadNumber(m_param->grpPinNum[0], m_param->grpPinNum[1]);
		m_corrTable->setGroupLeadNumber(m_param->grpPinNum[0], m_param->grpPinNum[1]);
		m_offTable->setGroupLeadNumber(m_param->grpPinNum[0], m_param->grpPinNum[1]);
		refreshTableWidget();
	}
	else if (sp == ui->sp_grp2pinNum) {
		str0 = ui->label_2->text() + ui->label_8->text();
		str1 = tr(" change from %1 to %2").arg(QString::number(m_param->grpPinNum[1])).arg(QString::number(val));
		m_param->grpPinNum[1] = val;
		m_paramTables->setGroupLeadNumber(m_param->grpPinNum[0], m_param->grpPinNum[1]);
		m_corrTable->setGroupLeadNumber(m_param->grpPinNum[0], m_param->grpPinNum[1]);
		m_offTable->setGroupLeadNumber(m_param->grpPinNum[0], m_param->grpPinNum[1]);
		refreshTableWidget();
	}
	else if (sp == ui->sp_errWidthMin) {
		str0 = ui->cb_errWidth->text() + tr(" min val ");
		str1 = tr(" change from %1 to %2").arg(QString::number(m_param->errWidthMin)).arg(QString::number(val));
		m_param->errWidthMin = val;
	}
	else if (sp == ui->sp_errWidthMax) {
		str0 = ui->cb_errWidth->text() + tr(" max val ");
		str1 = tr(" change from %1 to %2").arg(QString::number(m_param->errWidthMax)).arg(QString::number(val));
		m_param->errWidthMax = val;
	}
	else if (sp == ui->sp_errHeightMin) {
		str0 = ui->cb_errHeight->text() + tr(" min val ");
		str1 = tr(" change from %1 to %2").arg(QString::number(m_param->errHeightMin)).arg(QString::number(val));
		m_param->errHeightMin = val;
	}
	else if (sp == ui->sp_errHeightMax) {
		str0 = ui->cb_errHeight->text() + tr(" max val ");
		str1 = tr(" change from %1 to %2").arg(QString::number(m_param->errHeightmax)).arg(QString::number(val));
		m_param->errHeightmax = val;
	}
	else if (sp == ui->sp_errAngle) {
		str0 = ui->cb_errAngle->text();
		str1 = tr(" change from %1 to %2").arg(QString::number(m_param->errAngle)).arg(QString::number(val));
		m_param->errAngle = val;
	}
	else if (sp == ui->sp_errCenterOffset) {
		str0 = ui->cb_errCenterOffset->text();
		str1 = tr(" change from %1 to %2").arg(QString::number(m_param->errCenterOffset)).arg(QString::number(val));
		m_param->errCenterOffset = val;
	}
	else if (sp == ui->sp_minGrayDiff) {
		str0 = ui->tabWidget->tabText(m_param->NowGrp) + ui->label_25->text();
		str1 = tr(" change from %1 to %2").arg(QString::number(m_param->minGrayDiff[m_param->NowGrp])).arg(QString::number(val));
		m_param->minGrayDiff[m_param->NowGrp] = val;
	}
	else if (sp == ui->sp_offsetPlus) {
		str0 = ui->tabWidget->tabText(m_param->NowGrp) + ui->label_4->text();
		str1 = tr(" change from %1 to %2").arg(QString::number(m_param->offsetPlus[m_param->NowGrp])).arg(QString::number(val));
		m_param->offsetPlus[m_param->NowGrp] = val;
	}
	else if (sp == ui->sp_searchLineGap) {
		str0 = ui->tabWidget->tabText(m_param->NowGrp) + ui->label_22->text();
		str1 = tr(" change from %1 to %2").arg(QString::number(m_param->searchLineGap[m_param->NowGrp])).arg(QString::number(val));
		m_param->searchLineGap[m_param->NowGrp] = val;
	}
	else if (sp == ui->sp_lineOffset) {
		str0 = ui->tabWidget->tabText(m_param->NowGrp) + ui->label_24->text();
		str1 = tr(" change from %1 to %2").arg(QString::number(m_param->lineOffset[m_param->NowGrp])).arg(QString::number(val));
		m_param->lineOffset[m_param->NowGrp] = val;
	}
	else if (sp == ui->sp_paichu) {
		str0 = ui->tabWidget->tabText(m_param->NowGrp) + ui->cb_paichu->text();
		str1 = tr(" change from %1 to %2").arg(QString::number(m_param->paichuLine[m_param->NowGrp])).arg(QString::number(val));
		m_param->paichuLine[m_param->NowGrp] = val;
	}
	else if (sp == ui->sp_leadSize) {
		str0 = ui->tabWidget->tabText(m_param->NowGrp) + ui->cb_leadSize->text();
		str1 = tr(" change from %1 to %2").arg(QString::number(m_param->leadSize[m_param->NowGrp])).arg(QString::number(val));
		m_param->leadSize[m_param->NowGrp] = val;

		if (m_module->p_im->empty())
			return;
		HalOpenCV::cv2halImg(*m_module->p_im, m_image, false);
		//currentView()->clear();
		currentView()->clearLayer(m_layerIdx);
		//currentView()->setColor(0,0,0);
		//currentView()->setFilled(true);
		//currentView()->dispRect1(m_param->RoiRegionVector[m_param->NowGrp+1].row1,m_param->RoiRegionVector[m_param->NowGrp+1].col1,
		//	m_param->RoiRegionVector[m_param->NowGrp+1].row2,m_param->RoiRegionVector[m_param->NowGrp+1].col2);
		currentView()->setFilled(false);
		currentView()->setColor(255, 255, 0);
		currentView()->dispRect1(m_param->RoiRegionVector[m_param->NowGrp + 1].row1, m_param->RoiRegionVector[m_param->NowGrp + 1].col1,
			m_param->RoiRegionVector[m_param->NowGrp + 1].row2, m_param->RoiRegionVector[m_param->NowGrp + 1].col2, "ROI");
		currentView()->setFilled(true);
		currentView()->setColor(255, 255, 255);
		Hobject polyImg;
		Hobject recregion;
		gen_rectangle1(&recregion, m_param->RoiRegionVector[m_param->NowGrp + 1].row1, m_param->RoiRegionVector[m_param->NowGrp + 1].col1,
			m_param->RoiRegionVector[m_param->NowGrp + 1].row2, m_param->RoiRegionVector[m_param->NowGrp + 1].col2);
		reduce_domain(*m_image, recregion, &polyImg);
		invert_image(polyImg, &polyImg);
		threshold(polyImg, &polyImg, m_param->grayVal[m_param->NowGrp], 255);
		connection(polyImg, &polyImg);
		if (m_param->isLeadSize[m_param->NowGrp] && m_param->leadSize[m_param->NowGrp] > 0) {
			int wid, hei;
			if (m_param->NowGrp == 0 || m_param->NowGrp == 1) {
				wid = m_param->leadSize[m_param->NowGrp];
				hei = 10;
			}
			else {
				hei = m_param->leadSize[m_param->NowGrp];
				wid = 10;
			}
			Hobject openobj, unionobj;
			opening_rectangle1(polyImg, &openobj, wid, hei);
			//connection(openobj,&openobj);
			union1(openobj, &openobj);
			union1(polyImg, &unionobj);
			difference(unionobj, openobj, &openobj);
			connection(openobj, &openobj);
			dilation_circle(openobj, &openobj, 1);
			Hobject dispReg;
			union1(openobj, &dispReg);
			RlRegion reg;
			HalOpenCV::hal2cvRegion(dispReg, &reg);
			currentView()->dispRegion(&reg);
			currentView()->update();
		}
		else {
			//currentView()->clear();
			currentView()->clearLayer(m_layerIdx);
			currentView()->update();
		}
	}
	else if (sp == ui->sp_minArea) {
		str0 = ui->tabWidget->tabText(m_param->NowGrp) + ui->label_27->text();
		str1 = tr(" change from %1 to %2").arg(QString::number(m_param->minArea[m_param->NowGrp])).arg(QString::number(val));
		m_param->minArea[m_param->NowGrp] = val;

		if (m_module->p_im->empty())
			return;
		HalOpenCV::cv2halImg(*m_module->p_im, m_image, false);
		//currentView()->clear();
		currentView()->clearLayer(m_layerIdx);
		currentView()->setColor(0, 0, 0);
		currentView()->setFilled(true);
		currentView()->dispRect1(
			m_param->RoiRegionVector[m_param->NowGrp + 1].row1, m_param->RoiRegionVector[m_param->NowGrp + 1].col1,
			m_param->RoiRegionVector[m_param->NowGrp + 1].row2, m_param->RoiRegionVector[m_param->NowGrp + 1].col2);
		currentView()->setFilled(false);
		currentView()->setColor(255, 255, 0);
		currentView()->dispRect1(
			m_param->RoiRegionVector[m_param->NowGrp + 1].row1, m_param->RoiRegionVector[m_param->NowGrp + 1].col1,
			m_param->RoiRegionVector[m_param->NowGrp + 1].row2, m_param->RoiRegionVector[m_param->NowGrp + 1].col2, "ROI");
		currentView()->setFilled(true);
		currentView()->setColor(255, 255, 255);
		Hobject polyImg;
		Hobject recregion;
		gen_rectangle1(&recregion, 
			m_param->RoiRegionVector[m_param->NowGrp + 1].row1, m_param->RoiRegionVector[m_param->NowGrp + 1].col1,
			m_param->RoiRegionVector[m_param->NowGrp + 1].row2, m_param->RoiRegionVector[m_param->NowGrp + 1].col2);
		reduce_domain(*m_image, recregion, &polyImg);
		invert_image(polyImg, &polyImg);
		threshold(polyImg, &polyImg, m_param->grayVal[m_param->NowGrp], 255);
		connection(polyImg, &polyImg);
		select_shape(polyImg, &polyImg, "area", "and", m_param->minArea[m_param->NowGrp], 10000000);
		Hobject dispReg;
		union1(polyImg, &dispReg);
		RlRegion reg;
		HalOpenCV::hal2cvRegion(dispReg, &reg);
		currentView()->dispRegion(&reg);
		currentView()->update();
	}
	else if (sp == ui->sp_standoffOffset) {
		str0 = ui->tabWidget->tabText(m_param->NowGrp) + ui->label_43->text();
		str1 = tr(" change from %1 to %2").arg(QString::number(m_param->standoffOffset[m_param->NowGrp])).arg(QString::number(val));
		m_param->standoffOffset[m_param->NowGrp] = val;
	}
	else if (sp == ui->sp_grayval) {
		str0 = ui->tabWidget->tabText(m_param->NowGrp) + ui->label_9->text();
		str1 = tr(" change from %1 to %2").arg(QString::number(m_param->grayVal[m_param->NowGrp])).arg(QString::number(val));
		m_param->grayVal[m_param->NowGrp] = val;

		if (m_module->p_im->empty())
			return;
		HalOpenCV::cv2halImg(*m_module->p_im, m_image, false);
		//currentView()->clear();
		currentView()->clearLayer(m_layerIdx);
		currentView()->setColor(0, 0, 0);
		currentView()->setFilled(true);
		currentView()->dispRect1(
			m_param->RoiRegionVector[m_param->NowGrp + 1].row1, m_param->RoiRegionVector[m_param->NowGrp + 1].col1,
			m_param->RoiRegionVector[m_param->NowGrp + 1].row2, m_param->RoiRegionVector[m_param->NowGrp + 1].col2);
		currentView()->setFilled(false);
		currentView()->setColor(255, 255, 0);
		currentView()->dispRect1(
			m_param->RoiRegionVector[m_param->NowGrp + 1].row1, m_param->RoiRegionVector[m_param->NowGrp + 1].col1,
			m_param->RoiRegionVector[m_param->NowGrp + 1].row2, m_param->RoiRegionVector[m_param->NowGrp + 1].col2, "ROI");
		currentView()->setFilled(true);
		currentView()->setColor(255, 255, 255);
		Hobject polyImg;
		Hobject recregion;
		gen_rectangle1(&recregion, 
			m_param->RoiRegionVector[m_param->NowGrp + 1].row1, m_param->RoiRegionVector[m_param->NowGrp + 1].col1,
			m_param->RoiRegionVector[m_param->NowGrp + 1].row2, m_param->RoiRegionVector[m_param->NowGrp + 1].col2);
		reduce_domain(*m_image, recregion, &polyImg);
		invert_image(polyImg, &polyImg);
		threshold(polyImg, &polyImg, m_param->grayVal[m_param->NowGrp], 255);
		connection(polyImg, &polyImg);
		select_shape(polyImg, &polyImg, "area", "and", m_param->minArea[m_param->NowGrp], 10000000);
		Hobject dispReg;
		union1(polyImg, &dispReg);
		RlRegion reg;
		HalOpenCV::hal2cvRegion(dispReg, &reg);
		currentView()->dispRegion(&reg);
		currentView()->update();
	}
	else if (sp == ui->sp_edgeGray) {
		str0 = ui->tabWidget->tabText(m_param->NowGrp) + ui->label_53->text();
		str1 = tr(" change from %1 to %2").arg(QString::number(m_param->edgeGray[m_param->NowGrp])).arg(QString::number(val));
		m_param->edgeGray[m_param->NowGrp] = val;
	}
	//log(LogL1, str0 + str1);
	m_module->setParamChanged(ProductLevel);
}

void locationWdg::on_bt_selectRoi_toggled(bool check)
{
	if (check) {
		//currentView()->clear();
		currentView()->clearLayer(m_layerIdx);
		double row1[5], col1[5], row2[5], col2[5];
		QStringList title = { tr("SearchROI"),tr("Up"),tr("Down"),tr("Left"),tr("Right") };
		for (int i = 0;i < 5;i++) {
			row1[i] = m_param->RoiRegionVector[i].row1;
			col1[i] = m_param->RoiRegionVector[i].col1;
			row2[i] = m_param->RoiRegionVector[i].row2;
			col2[i] = m_param->RoiRegionVector[i].col2;
		}
		currentView()->setColor(255, 0, 128);
		currentView()->drawRects1(5, row1, col1, row2, col2, title);
		currentView()->update();
	}
	else {

		//log(LogL1, ui->bt_selectRoi->text());

		currentView()->finishDraw();
		double row1[5], col1[5], row2[5], col2[5];
		currentView()->getDrawRects1(5, row1, col1, row2, col2);
		for (int i = 0;i < 5;i++) {
			m_param->RoiRegionVector[i].row1 = row1[i];
			m_param->RoiRegionVector[i].col1 = col1[i];
			m_param->RoiRegionVector[i].row2 = row2[i];
			m_param->RoiRegionVector[i].col2 = col2[i];
		}
		//currentView()->clear();
		currentView()->clearLayer(m_layerIdx);
		currentView()->setColor(0, 255, 0);
		currentView()->setFilled(false);
		for (int i = 0;i < 5;i++) {
			currentView()->dispRect1(row1[i], col1[i], row2[i], col2[i]);
		}
		currentView()->update();

		m_module->setParamChanged(ProductLevel);
	}
}

void locationWdg::comboBox_currentIndexChanged(int index)
{

}

void locationWdg::tabWidget_indexChange(int index)
{
	m_param->NowGrp = index;
	connectSlots(false);
	setCurrentGroup(index);
	connectSlots(true);
}

void locationWdg::on_bt_highLevel_toggled(bool val)
{
	ui->grp_high->setVisible(val);
}

void locationWdg::on_bt_table_clicked()
{
	m_dialog->exec();
}

void locationWdg::on_bt_corrTable_clicked()
{
	m_corrDialog->exec();
}

void locationWdg::tableWidget_valueChanged(int checkIdx, int grpIdx, int leadIdx, bool minOrMax, double val)
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
				exval = m_param->errPinStandOffMin[grpIdx][leadIdx];
				m_param->errPinStandOffMin[grpIdx][leadIdx] = val;
			}
			else {
				exval = m_param->errPinStandOffMax[grpIdx][leadIdx];
				m_param->errPinStandOffMax[grpIdx][leadIdx] = val;
			}
		}
		else if (checkIdx == 1) {
			if (!minOrMax) {
				exval = m_param->errPinWidthMin[grpIdx][leadIdx];
				m_param->errPinWidthMin[grpIdx][leadIdx] = val;
			}
			else {
				exval = m_param->errPinWidthMax[grpIdx][leadIdx];
				m_param->errPinWidthMax[grpIdx][leadIdx] = val;
			}
		}
		else if (checkIdx == 2) {
			if (!minOrMax) {
				exval = m_param->errPinGapMin[grpIdx][leadIdx];
				m_param->errPinGapMin[grpIdx][leadIdx] = val;
			}
			else {
				exval = m_param->errPinGapMax[grpIdx][leadIdx];
				m_param->errPinGapMax[grpIdx][leadIdx] = val;
			}
		}
		else if (checkIdx == 3) {
			if (!minOrMax) {
				exval = m_param->errPinStandOffDiffMin[grpIdx];
				m_param->errPinStandOffDiffMin[grpIdx] = val;
			}
			else {
				exval = m_param->errPinStandOffDiffMax[grpIdx];
				m_param->errPinStandOffDiffMax[grpIdx] = val;
			}
		}
		else if (checkIdx == 4) {
			if (!minOrMax) {
				exval = m_param->errCoplanarMin;
				m_param->errCoplanarMin = val;
			}
			else {
				exval = m_param->errCoplanar;
				m_param->errCoplanar = val;
			}
		}
	}
	else if (pt == m_corrTable) {
		//leadname=tr(" group%1: ").arg(grpIdx+1);
		leadname = tr("lead%1 of group%2: ").arg(leadIdx + 1).arg(grpIdx + 1);
		cbname = tr("compensation") + cbname;
		str0 = leadname + cbname;
		if (checkIdx == 0) {
			exval = m_param->standOffCorr[grpIdx][leadIdx];
			m_param->standOffCorr[grpIdx][leadIdx] = val;
		}
		else if (checkIdx == 1) {
			exval = m_param->widthCorr[grpIdx][leadIdx];
			m_param->widthCorr[grpIdx][leadIdx] = val;
		}
		else if (checkIdx == 2) {
			exval = m_param->InterCorr[grpIdx][leadIdx];
			m_param->InterCorr[grpIdx][leadIdx] = val;
		}
	}
	else if (pt == m_offTable) {
		//leadname=tr(" group%1: ").arg(grpIdx+1);
		leadname = tr("lead%1 of group%2: ").arg(leadIdx + 1).arg(grpIdx + 1);
		cbname = tr("offset") + cbname;
		str0 = leadname + cbname;
		if (checkIdx == 0) {
			exval = m_param->offStandOffCorr[grpIdx][leadIdx];
			m_param->offStandOffCorr[grpIdx][leadIdx] = val;
		}
		else if (checkIdx == 1) {
			exval = m_param->offWidthCorr[grpIdx][leadIdx];
			m_param->offWidthCorr[grpIdx][leadIdx] = val;
		}
		else if (checkIdx == 2) {
			exval = m_param->offInterCorr[grpIdx][leadIdx];
			m_param->offInterCorr[grpIdx][leadIdx] = val;
		}
	}
	str1 = tr(" change from %1 to %2").arg(QString::number(exval, 'f', 3)).arg(QString::number(val, 'f', 3));
	//log(LogL1, str0 + str1);
	m_module->setParamChanged(ProductLevel);
}

void locationWdg::tableWidget_checkToggled(int index, bool check)
{

}

void locationWdg::tableWidget_CopyItems(bool IsAll, int startCheck, int CheckRange, int starLead, int leadRange)
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
					exval = m_param->errPinStandOffMin[grpIdx][leadIdx];
					m_param->errPinStandOffMin[grpIdx][leadIdx] = nowval = m_param->errPinStandOffMin[grpIdx][0];

				}
				else if (i == 1) {
					exval = m_param->errPinStandOffMax[grpIdx][leadIdx];
					m_param->errPinStandOffMax[grpIdx][leadIdx] = nowval = m_param->errPinStandOffMax[grpIdx][0];
				}
				else if (i == 2) {
					exval = m_param->errPinWidthMin[grpIdx][leadIdx];
					m_param->errPinWidthMin[grpIdx][leadIdx] = nowval = m_param->errPinWidthMin[grpIdx][0];
				}
				else if (i == 3) {
					exval = m_param->errPinWidthMax[grpIdx][leadIdx];
					m_param->errPinWidthMax[grpIdx][leadIdx] = nowval = m_param->errPinWidthMax[grpIdx][0];
				}
				else if (i == 4) {
					exval = m_param->errPinGapMin[grpIdx][leadIdx];
					m_param->errPinGapMin[grpIdx][leadIdx] = nowval = m_param->errPinGapMin[grpIdx][0];
				}
				else if (i == 5) {
					exval = m_param->errPinGapMax[grpIdx][leadIdx];
					m_param->errPinGapMax[grpIdx][leadIdx] = nowval = m_param->errPinGapMax[grpIdx][0];
				}
				if (i == 6 || i == 7 || i == 8 || i == 9) {
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

				if (i == 0) {
					exval = m_param->standOffCorr[grpIdx][leadIdx];
					m_param->standOffCorr[grpIdx][leadIdx] = nowval = m_param->standOffCorr[grpIdx][0];
				}
				else if (i == 1) {
					exval = m_param->widthCorr[grpIdx][leadIdx];
					m_param->widthCorr[grpIdx][leadIdx] = nowval = m_param->widthCorr[grpIdx][0];
				}
				else if (i == 2) {
					exval = m_param->InterCorr[grpIdx][leadIdx];
					m_param->InterCorr[grpIdx][leadIdx] = nowval = m_param->InterCorr[grpIdx][0];
				}
				str1 = tr(" change from %1 to %2").arg(QString::number(exval, 'f', 3)).arg(QString::number(nowval, 'f', 3));
				//log(LogL1, str0 + str1);
			}
		}
	}
	else if (pt == m_offTable) {
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
			cbname = tr("offset") + cbname;
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

				if (i == 0) {
					exval = m_param->offStandOffCorr[grpIdx][leadIdx];
					m_param->offStandOffCorr[grpIdx][leadIdx] = nowval = m_param->offStandOffCorr[grpIdx][0];
				}
				else if (i == 1) {
					exval = m_param->offWidthCorr[grpIdx][leadIdx];
					m_param->offWidthCorr[grpIdx][leadIdx] = nowval = m_param->offWidthCorr[grpIdx][0];
				}
				else if (i == 2) {
					exval = m_param->offInterCorr[grpIdx][leadIdx];
					m_param->offInterCorr[grpIdx][leadIdx] = nowval = m_param->offInterCorr[grpIdx][0];
				}
				str1 = tr(" change from %1 to %2").arg(QString::number(exval, 'f', 3)).arg(QString::number(nowval, 'f', 3));
				//log(LogL1, str0 + str1);
			}
		}
	}
	m_module->setParamChanged(ProductLevel);
}
