#include "locationGuiWdg.h"
#include "locationGuiMod.h"
#include "locationGuiParam.h"
#include "ui_locationGuiWdg.h"
#include <QMessageBox>
#include "ImageView.h"
#include "halOpenCV.h"

locationGuiWdg::locationGuiWdg(int i) :
	ModSetWidget(),ui(new Ui::locationGuiWdg),m_widgetType(i)
{
	m_hasConnect = false;
	ui->setupUi(this);
	m_grpIndex = 0;
	m_leadIndex = 0;
	m_layerIdx = 0;
	iniUi();
	m_param = nullptr;
	m_module = nullptr;
	m_image = nullptr;
}

locationGuiWdg::~locationGuiWdg()
{
	delete ui;
}

void locationGuiWdg::setModule(UnitModule *module)
{
	m_module=dynamic_cast<locationGuiMod*>(module);
	m_param = m_module->m_param;
	m_image = m_module->m_image;
	//currentView()->setLayer(m_layerIdx);
	//updateUi();
}

void locationGuiWdg::showEvent(QShowEvent * event)
{
	updateUi();
}

void locationGuiWdg::connectSlots(bool link)
{
	if(m_hasConnect==link)
		return;
	m_hasConnect=link;


	connectOrNot(link, ui->rb_upDw, SIGNAL(toggled(bool)), SLOT(radioButton_toggled(bool)));
	connectOrNot(link, ui->rb_lfRt, SIGNAL(toggled(bool)), SLOT(radioButton_toggled(bool)));
	connectOrNot(link, ui->cb_detectEndPoint, SIGNAL(toggled(bool)), SLOT(checkBox_toggled(bool)));

	connectOrNot(link, ui->cb_errWidth, SIGNAL(toggled(bool)), SLOT(checkBox_toggled(bool)));
	connectOrNot(link, ui->cb_errHeight, SIGNAL(toggled(bool)), SLOT(checkBox_toggled(bool)));
	connectOrNot(link, ui->cb_errAngle, SIGNAL(toggled(bool)), SLOT(checkBox_toggled(bool)));
	connectOrNot(link, ui->cb_errCenterOffset, SIGNAL(toggled(bool)), SLOT(checkBox_toggled(bool)));

	connectOrNot(link, ui->sp_maxAngle, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_errWidthMin, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_errWidthMax, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_errAngle, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_errHeightMin, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_errHeightMax, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_errCenterOffset, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_grp1pinNum, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_grp2pinNum, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->dsp_standOffCorrLF, SIGNAL(valueChanged(double)), SLOT(doubleSpinBox_valueChanged(double)));
	connectOrNot(link, ui->dsp_standOffCorrRT, SIGNAL(valueChanged(double)), SLOT(doubleSpinBox_valueChanged(double)));
	connectOrNot(link, ui->dsp_widthCorrLF, SIGNAL(valueChanged(double)), SLOT(doubleSpinBox_valueChanged(double)));
	connectOrNot(link, ui->dsp_widthCorrRT, SIGNAL(valueChanged(double)), SLOT(doubleSpinBox_valueChanged(double)));
	connectOrNot(link, ui->dsp_InterCorrLF, SIGNAL(valueChanged(double)), SLOT(doubleSpinBox_valueChanged(double)));
	connectOrNot(link, ui->dsp_InterCorrRT, SIGNAL(valueChanged(double)), SLOT(doubleSpinBox_valueChanged(double)));

	connectOrNot(link, ui->dsp_pinLength, SIGNAL(valueChanged(double)), SLOT(doubleSpinBox_valueChanged(double)));

	for (int i = 0;i<4;i++) {
		connectOrNot(link, rb_blackToWhite[i], SIGNAL(toggled(bool)), SLOT(radioButton_toggled(bool)));
		connectOrNot(link, rb_whiteToBlack[i], SIGNAL(toggled(bool)), SLOT(radioButton_toggled(bool)));

		connectOrNot(link, rb_LfToRt[i], SIGNAL(toggled(bool)), SLOT(radioButton_toggled(bool)));
		connectOrNot(link, rb_UpToDw[i], SIGNAL(toggled(bool)), SLOT(radioButton_toggled(bool)));

		connectOrNot(link, sp_extendSearchLine[i], SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
		connectOrNot(link, sp_minGrayDiff[i], SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
		connectOrNot(link, sp_searchLineGap[i], SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
		connectOrNot(link, sp_lineOffset[i], SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
		connectOrNot(link, sp_offsetPlus[i], SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
		connectOrNot(link, sp_minArea[i], SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
		connectOrNot(link, sp_standoffOffset[i], SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
		connectOrNot(link, sp_grayval[i], SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
		connectOrNot(link, sp_edgeSearchWidth[i], SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
		connectOrNot(link, sp_pinSearchWidth[i], SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));

		connectOrNot(link, dsp_factor[i], SIGNAL(valueChanged(double)), SLOT(doubleSpinBox_valueChanged(double)));

		connectOrNot(link, cb_UseStandOff[i], SIGNAL(toggled(bool)), SLOT(checkBox_toggled(bool)));
	}
	connectOrNot(link, ui->sp_LTnum, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_RBnum, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
}

void locationGuiWdg::iniUi()
{
	for (int i = 0;i<4;i++) {
		rb_blackToWhite[i] = findChild<QRadioButton*>(QString("rb_blackToWhite_%1").arg(i));
		rb_whiteToBlack[i] = findChild<QRadioButton*>(QString("rb_whiteToBlack_%1").arg(i));

		rb_LfToRt[i] = findChild<QRadioButton*>(QString("rb_LfToRt_%1").arg(i));
		rb_UpToDw[i] = findChild<QRadioButton*>(QString("rb_UpToDw_%1").arg(i));

		sp_extendSearchLine[i] = findChild<QSpinBox*>(QString("sp_extendSearchLine_%1").arg(i));
		sp_minGrayDiff[i] = findChild<QSpinBox*>(QString("sp_minGrayDiff_%1").arg(i));
		sp_searchLineGap[i] = findChild<QSpinBox*>(QString("sp_searchLineGap_%1").arg(i));
		sp_lineOffset[i] = findChild<QSpinBox*>(QString("sp_lineOffset_%1").arg(i));
		sp_offsetPlus[i] = findChild<QSpinBox*>(QString("sp_offsetPlus_%1").arg(i));
		sp_minArea[i] = findChild<QSpinBox*>(QString("sp_minArea_%1").arg(i));
		sp_standoffOffset[i] = findChild<QSpinBox*>(QString("sp_standoffOffset_%1").arg(i));
		sp_grayval[i] = findChild<QSpinBox*>(QString("sp_grayval_%1").arg(i));
		sp_edgeSearchWidth[i] = findChild<QSpinBox*>(QString("sp_edgeSearchWidth_%1").arg(i));
		sp_pinSearchWidth[i] = findChild<QSpinBox*>(QString("sp_pinSearchWidth_%1").arg(i));

		dsp_factor[i] = findChild<QDoubleSpinBox*>(QString("dsp_factor_%1").arg(i));

		cb_UseStandOff[i] = findChild<QCheckBox*>(QString("cb_UseStandOff_%1").arg(i));
	}
}

void locationGuiWdg::setUiValue()
{
	if(!m_param)
		return;

	ui->cb_detectEndPoint->setChecked(m_param->IsEndPoint == 1);
	ui->sp_maxAngle->setValue(m_param->maxAngle);

	if (m_param->upDwOrLfRt == 0) {
		ui->rb_upDw->setChecked(true);
	}
	else {
		ui->rb_lfRt->setChecked(true);
	}
	ui->dsp_standOffCorrLF->setValue(m_param->standOffCorr[0]);
	ui->dsp_standOffCorrRT->setValue(m_param->standOffCorr[1]);

	ui->dsp_widthCorrLF->setValue(m_param->widthCorr[0]);
	ui->dsp_widthCorrRT->setValue(m_param->widthCorr[1]);

	ui->dsp_InterCorrLF->setValue(m_param->InterCorr[0]);
	ui->dsp_InterCorrRT->setValue(m_param->InterCorr[1]);

	ui->dsp_pinLength->setValue(m_param->pinlength);

	for (int i = 0;i < 4;i++) {
		if (m_param->whiteBlackDir[i] == 0) {
			rb_blackToWhite[i]->setChecked(true);
		}
		else if (m_param->whiteBlackDir[i] == 1) {
			rb_whiteToBlack[i]->setChecked(true);
		}
		if (m_param->searchDir[i] == 0) {
			rb_LfToRt[i]->setChecked(true);
		}
		else if (m_param->searchDir[i] == 1) {
			rb_UpToDw[i]->setChecked(true);
		}
		sp_extendSearchLine[i]->setValue(m_param->extendSearchLine[i]);
		sp_searchLineGap[i]->setValue(m_param->searchLineGap[i]);
		sp_minGrayDiff[i]->setValue(m_param->minGrayDiff[i]);
		sp_lineOffset[i]->setValue(m_param->lineOffset[i]);
		sp_offsetPlus[i]->setValue(m_param->offsetPlus[i]);
		sp_minArea[i]->setValue(m_param->minArea[i]);
		sp_standoffOffset[i]->setValue(m_param->standoffOffset[i]);
		sp_grayval[i]->setValue(m_param->grayVal[i]);
		sp_edgeSearchWidth[i]->setValue(m_param->edgesearchWidth[i]);
		sp_pinSearchWidth[i]->setValue(m_param->pinSearchWidth[i]);
		dsp_factor[i]->setValue(m_param->clipfactor[i]);
		cb_UseStandOff[i]->setChecked(m_param->IsUseStand[i]);
	}
	ui->cb_errWidth->setChecked(m_param->IsErrWidth == 1);
	ui->sp_errWidthMin->setValue(m_param->errWidthMin);
	ui->sp_grp1pinNum->setValue(m_param->grpPinNum[0]);
	ui->sp_grp2pinNum->setValue(m_param->grpPinNum[1]);
	ui->sp_errWidthMin->setEnabled(m_param->IsErrWidth == 1);
	ui->sp_errWidthMax->setValue(m_param->errWidthMax);
	ui->sp_errWidthMax->setEnabled(m_param->IsErrWidth == 1);

	ui->cb_errHeight->setChecked(m_param->IsErrHeight == 1);
	ui->sp_errHeightMin->setValue(m_param->errHeightMin);
	ui->sp_errHeightMin->setEnabled(m_param->IsErrHeight == 1);
	ui->sp_errHeightMax->setValue(m_param->errHeightmax);
	ui->sp_errHeightMax->setEnabled(m_param->IsErrHeight == 1);

	ui->cb_errAngle->setChecked(m_param->IsErrAngle == 1);
	ui->sp_errAngle->setValue(m_param->errAngle);
	ui->sp_errAngle->setEnabled(m_param->IsErrAngle == 1);

	ui->cb_errCenterOffset->setChecked(m_param->IsErrCenterOffset == 1);
	ui->sp_errCenterOffset->setValue(m_param->errCenterOffset);
	ui->sp_errCenterOffset->setEnabled(m_param->IsErrCenterOffset == 1);

	ui->sp_LTnum->setValue(m_param->LTnum);
	ui->sp_RBnum->setValue(m_param->RBnum);
}

void locationGuiWdg::updateUi()
{
	connectSlots(false);
	setUiValue();
	connectSlots(true);
}

void locationGuiWdg::IniTableUI()
{
	for (int i = 0;i < ChildTableMaxNum;i++)
	{
		m_childTable[i] = NULL;
	}
	if (m_param->ChildTablenum > ChildTableMaxNum)
	{
		printf("ERROR: Table num overflow! \n ");
		return;
	}
	int ParentWid = 80;
	int ParentHei = 20;
	m_param->ChildTablenum = m_param->LTnum + m_param->RBnum;
	ui->table_error->setColumnCount(m_param->ChildTablenum + 1);
	ui->table_error->setRowCount(CheckBoxNum + 1);
	ui->table_error->horizontalHeader()->setVisible(true);
	ui->table_error->verticalHeader()->setVisible(false);
	for (int i = 0;i < m_param->ChildTablenum;i++)
	{
		ui->table_error->setSpan(0, i + 1, CheckBoxNum + 1, 1);
	}
	ui->table_error->setShowGrid(false);
	QTableWidgetItem *itemNull = new QTableWidgetItem();
	ui->table_error->setItem(0, 0, itemNull);
	ui->table_error->item(0, 0)->setFlags(Qt::NoItemFlags);
	//ui->table_error->horizontalHeader()->set
	QStringList sListHeader;
	sListHeader << QString::fromLocal8Bit("误差范围");
	for (int i = 0;i < m_param->LTnum;i++)
	{
		QString str = "LT-lead" + QString::number(i + 1);
		sListHeader.push_back(str);
	}
	for (int i = 0;i < m_param->RBnum;i++)
	{
		QString str = "RB-lead" + QString::number(i + 1);
		sListHeader.push_back(str);
	}
	ui->table_error->setHorizontalHeaderLabels(sListHeader);
	for (int i = 0;i < m_param->ChildTablenum + 1;i++)
	{
		ui->table_error->setColumnWidth(i, ParentWid);
	}
	for (int i = 0;i < CheckBoxNum + 1;i++)
	{
		ui->table_error->setRowHeight(i, ParentHei);
	}


	for (int i = 0;i < m_param->ChildTablenum;i++)
	{
		m_childTable[i] = new QTableWidget();
		m_childTable[i]->setColumnCount(2);
		m_childTable[i]->setRowCount(CheckBoxNum + 1);
		m_childTable[i]->horizontalHeader()->setVisible(false);
		m_childTable[i]->verticalHeader()->setVisible(false);
		QTableWidgetItem *item = new QTableWidgetItem("Min");
		m_childTable[i]->setItem(0, 0, item);
		QTableWidgetItem *item1 = new QTableWidgetItem("Max");
		m_childTable[i]->setItem(0, 1, item1);
		m_childTable[i]->item(0, 0)->setFlags(Qt::NoItemFlags);
		m_childTable[i]->item(0, 1)->setFlags(Qt::NoItemFlags);
		//childTable1->setShowGrid(false);
		m_childTable[i]->setFrameShape(QFrame::NoFrame);
		m_childTable[i]->setColumnWidth(0, 0.5*ParentWid);
		m_childTable[i]->setColumnWidth(1, 0.5*ParentWid);
		for (int j = 0;j < 6;j++)
		{
			m_childTable[i]->setRowHeight(j, ParentHei);
		}
		ui->table_error->setCellWidget(0, i + 1, m_childTable[i]);
	}

	QStringList cbName;
	cbName << QString::fromLocal8Bit(" 站立度") << QString::fromLocal8Bit(" 管脚宽度") << QString::fromLocal8Bit(" 管脚间距")
		<< QString::fromLocal8Bit(" 共线性差异");
	for (int i = 0;i < CheckBoxNum;i++)
	{
		m_cbBox[i] = new QCheckBox();
		m_cbBox[i]->setChecked(false);
		ui->table_error->setCellWidget(i + 1, 0, m_cbBox[i]);
		m_cbBox[i]->setText(cbName[i]);
	}
	//corresponding
	m_cbBox[0]->setChecked(m_param->errPinStandOffValid);
	m_cbBox[1]->setChecked(m_param->errPinWidthValid);
	m_cbBox[2]->setChecked(m_param->errPinGapValid);
	m_cbBox[3]->setChecked(m_param->errPinStandOffDiffValid);
	//corresponding
	for (int i = 0;i < CheckBoxNum;i++)
	{
		for (int j = 0;j < m_param->ChildTablenum;j++) {
			QTableWidgetItem *item = new QTableWidgetItem();
			m_childTable[j]->setItem(i + 1, 0, item);
			item->setFlags(Qt::NoItemFlags);
			QTableWidgetItem *item1 = new QTableWidgetItem();
			m_childTable[j]->setItem(i + 1, 1, item1);
			item1->setFlags(Qt::NoItemFlags);
		}
	}
}

void locationGuiWdg::IniTableSlots()
{
	for (int i = 0;i < CheckBoxNum;i++)
	{
		connect(m_cbBox[i], SIGNAL(toggled(bool)), SLOT(Cb_Toggled(bool)));

	}
	for (int i = 0;i < m_param->ChildTablenum;i++)
	{
		connect(m_childTable[i], SIGNAL(itemChanged(QTableWidgetItem *)), SLOT(Child_Changed(QTableWidgetItem *)));
	}
}

void locationGuiWdg::IniTableData()
{
	for (int i = 0;i < m_param->ChildTablenum;i++)
	{
		int grepidx = ((i + 1) > m_param->LTnum);
		int leadInndex;
		if (grepidx)
		{
			leadInndex = i - m_param->LTnum;
		}
		else {
			leadInndex = i;
		}
		//corresponding
		m_childTable[i]->item(1, 0)->setText(QString::number(m_param->errPinStandOffMin[grepidx][leadInndex]));
		m_childTable[i]->item(1, 1)->setText(QString::number(m_param->errPinStandOffMax[grepidx][leadInndex]));
		m_childTable[i]->item(2, 0)->setText(QString::number(m_param->errPinWidthMin[grepidx][leadInndex]));
		m_childTable[i]->item(2, 1)->setText(QString::number(m_param->errPinWidthMax[grepidx][leadInndex]));
		m_childTable[i]->item(3, 0)->setText(QString::number(m_param->errPinGapMin[grepidx][leadInndex]));
		m_childTable[i]->item(3, 1)->setText(QString::number(m_param->errPinGapMax[grepidx][leadInndex]));
		m_childTable[i]->item(4, 0)->setText(QString::number(m_param->errPinStandOffDiffMax[grepidx]));
		m_childTable[i]->item(4, 1)->setText("NULL");
		//corresponding
	}
}

void locationGuiWdg::refrenshTable()
{
	ui->table_error->clear();
	IniTableUI();
	IniTableData();
	IniTableSlots();
}

void locationGuiWdg::on_bt_setOrigin_clicked()
{
	HTuple centerRow, centerCol;
	m_module->getDispOutData(CenterRow, centerRow);
	m_module->getDispOutData(CenterCol, centerCol);
	if (centerRow.Num() == 0)
		return;

	if (QMessageBox::information(this, tr("Hint"), 
		tr("Are you sure to change the current center point (%1,%2) to the template center?")
		.arg(centerCol[0].D(), 0, 'f', 3).arg(centerRow[0].D(), 0, 'f', 3),
		QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
			m_param->TmpCenterRow = centerRow[0].D();
			m_param->TmpCenterCol = centerCol[0].D();

			m_module->setParamChanged(ProductLevel);
	}
}

void locationGuiWdg::radioButton_toggled(bool check)
{
	QRadioButton* rb = qobject_cast<QRadioButton*>(sender());
	if (check) {
		for (int i = 0;i < 4;i++)
		{
			if (rb == rb_blackToWhite[i]) {
				m_param->whiteBlackDir[i] = !check;
				break;
			}
			else if (rb == rb_whiteToBlack[i]) {
				m_param->whiteBlackDir[i] = check;
				break;
			}
			else if (rb == rb_LfToRt[i]) {
				m_param->searchDir[i] = 0;
				break;
			}
			else if (rb == rb_UpToDw[i]) {
				m_param->searchDir[i] = 1;
				break;
			}
		}
		if (rb == ui->rb_upDw) {
			m_param->upDwOrLfRt = 0;
		}
		else if (rb == ui->rb_lfRt) {
			m_param->upDwOrLfRt = 1;
		}
		m_module->setParamChanged(ProductLevel);
	}
}

void locationGuiWdg::checkBox_toggled(bool check)
{
	QCheckBox* cb = qobject_cast<QCheckBox*>(sender());
	if (cb == ui->cb_detectEndPoint) {
		m_param->IsEndPoint = check;
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
	else {
		for (int i = 0;i<4;i++) {
			if (cb == cb_UseStandOff[i]) {
				m_param->IsUseStand[i] = check;
				return;
			}
		}
	}
	m_module->setParamChanged(ProductLevel);
}

void locationGuiWdg::doubleSpinBox_valueChanged(double val)
{
	QDoubleSpinBox* dsp = qobject_cast<QDoubleSpinBox*>(sender());

	if (dsp == ui->dsp_standOffCorrLF) {
		m_param->standOffCorr[0] = val;
	}
	else if (dsp == ui->dsp_standOffCorrRT) {
		m_param->standOffCorr[1] = val;
	}
	if (dsp == ui->dsp_widthCorrLF) {
		m_param->widthCorr[0] = val;
	}
	else if (dsp == ui->dsp_widthCorrRT) {
		m_param->widthCorr[1] = val;
	}
	if (dsp == ui->dsp_InterCorrLF) {
		m_param->InterCorr[0] = val;
	}
	else if (dsp == ui->dsp_InterCorrRT) {
		m_param->InterCorr[1] = val;
	}
	else if (dsp == ui->dsp_pinLength) {
		m_param->pinlength = val;
	}
	else {
		for (int i = 0;i < 4;i++) {
			if (dsp == dsp_factor[i]) {
				m_param->clipfactor[i] = val;
				break;
			}
		}
	}
	m_module->setParamChanged(ProductLevel);
}

void locationGuiWdg::spinBox_valueChanged(int val)
{
	QSpinBox* sp = qobject_cast<QSpinBox*>(sender());
	if (sp == ui->sp_maxAngle) {
		m_param->maxAngle = val;
	}
	else if (sp == ui->sp_grp1pinNum) {
		m_param->grpPinNum[0] = val;
	}
	else if (sp == ui->sp_grp2pinNum) {
		m_param->grpPinNum[1] = val;
	}
	else if (sp == ui->sp_errWidthMin) {
		m_param->errWidthMin = val;
	}
	else if (sp == ui->sp_errWidthMax) {
		m_param->errWidthMax = val;
	}
	else if (sp == ui->sp_errHeightMin) {
		m_param->errHeightMin = val;
	}
	else if (sp == ui->sp_errHeightMax) {
		m_param->errHeightmax = val;
	}
	else if (sp == ui->sp_errAngle) {
		m_param->errAngle = val;
	}
	else if (sp == ui->sp_errCenterOffset) {
		m_param->errCenterOffset = val;
	}
	else if (sp == ui->sp_LTnum) {
		m_param->LTnum = val;
		m_param->ChildTablenum = m_param->LTnum + m_param->RBnum;
		refrenshTable();
	}
	else if (sp == ui->sp_RBnum) {
		m_param->RBnum = val;
		m_param->ChildTablenum = m_param->LTnum + m_param->RBnum;
		refrenshTable();
	}
	else {
		for (int i = 0;i<4;i++) {
			if (sp == sp_extendSearchLine[i]) {
				m_param->extendSearchLine[i] = val;
				break;
			}
			else if (sp == sp_minGrayDiff[i]) {
				m_param->minGrayDiff[i] = val;
				break;
			}
			else if (sp == sp_offsetPlus[i]) {
				m_param->offsetPlus[i] = val;
				break;
			}
			else if (sp == sp_searchLineGap[i]) {
				m_param->searchLineGap[i] = val;
				break;
			}
			else if (sp == sp_lineOffset[i]) {
				m_param->lineOffset[i] = val;
				break;
			}
			else if (sp == sp_minArea[i]) {
				m_param->minArea[i] = val;

				if (m_module->p_im->empty())
					return;
				HalOpenCV::cv2halImg(*m_module->p_im, m_image, false);

				//currentView()->clear();
				currentView()->clearLayer(m_layerIdx);
				currentView()->setColor(0, 0, 0);
				currentView()->setFilled(true);
				currentView()->dispRect1(
					m_param->RoiRegionVector[i + 1].row1, m_param->RoiRegionVector[i + 1].col1,
					m_param->RoiRegionVector[i + 1].row2, m_param->RoiRegionVector[i + 1].col2);
				currentView()->setFilled(false);
				currentView()->setColor(255, 255, 0);
				currentView()->dispRect1(
					m_param->RoiRegionVector[i + 1].row1, m_param->RoiRegionVector[i + 1].col1,
					m_param->RoiRegionVector[i + 1].row2, m_param->RoiRegionVector[i + 1].col2, "ROI");
				currentView()->setFilled(true);
				currentView()->setColor(255, 255, 255);
				Hobject polyImg;
				Hobject recregion;
				gen_rectangle1(&recregion,
					m_param->RoiRegionVector[i + 1].row1, m_param->RoiRegionVector[i + 1].col1,
					m_param->RoiRegionVector[i + 1].row2, m_param->RoiRegionVector[i + 1].col2);
				reduce_domain(*m_image, recregion, &polyImg);
				invert_image(polyImg, &polyImg);
				threshold(polyImg, &polyImg, m_param->grayVal[i], 255);
				connection(polyImg, &polyImg);
				select_shape(polyImg, &polyImg, "area", "and", m_param->minArea[i], 10000000);
				RlRegion reg;
				Hobject dispReg;
				union1(polyImg, &dispReg);
				HalOpenCV::hal2cvRegion(dispReg, &reg);
				currentView()->dispRegion(&reg);
				currentView()->update();
				break;
			}
			else if (sp == sp_standoffOffset[i]) {
				m_param->standoffOffset[i] = val;
				break;
			}
			else if (sp == sp_grayval[i]) {
				m_param->grayVal[i] = val;

				if (m_module->p_im->empty())
					return;
				HalOpenCV::cv2halImg(*m_module->p_im, m_image, false);

				//currentView()->clear();
				currentView()->clearLayer(m_layerIdx);
				currentView()->setColor(0, 0, 0);
				currentView()->setFilled(true);
				currentView()->dispRect1(
					m_param->RoiRegionVector[i + 1].row1, m_param->RoiRegionVector[i + 1].col1,
					m_param->RoiRegionVector[i + 1].row2, m_param->RoiRegionVector[i + 1].col2);
				currentView()->setFilled(false);
				currentView()->setColor(255, 255, 0);
				currentView()->dispRect1(
					m_param->RoiRegionVector[i + 1].row1, m_param->RoiRegionVector[i + 1].col1,
					m_param->RoiRegionVector[i + 1].row2, m_param->RoiRegionVector[i + 1].col2, "ROI");
				currentView()->setFilled(true);
				currentView()->setColor(255, 255, 255);
				Hobject polyImg;
				Hobject recregion;
				gen_rectangle1(&recregion, 
					m_param->RoiRegionVector[i + 1].row1, m_param->RoiRegionVector[i + 1].col1,
					m_param->RoiRegionVector[i + 1].row2, m_param->RoiRegionVector[i + 1].col2);
				reduce_domain(*m_image, recregion, &polyImg);
				invert_image(polyImg, &polyImg);
				threshold(polyImg, &polyImg, m_param->grayVal[i], 255);
				connection(polyImg, &polyImg);
				select_shape(polyImg, &polyImg, "area", "and", m_param->minArea[i], 10000000);
				RlRegion reg;
				Hobject dispReg;
				union1(polyImg, &dispReg);
				HalOpenCV::hal2cvRegion(dispReg, &reg);
				currentView()->dispRegion(&reg);
				currentView()->update();
				break;
			}
			else if (sp == sp_edgeSearchWidth[i]) {
				m_param->edgesearchWidth[i] = val;
				break;
			}
			else if (sp == sp_pinSearchWidth[i]) {
				m_param->pinSearchWidth[i] = val;
				break;
			}
		}
	}
	m_module->setParamChanged(ProductLevel);
}

void locationGuiWdg::on_bt_selectRoi_toggled(bool check)
{
	if (check) {
		//currentView()->clear();
		currentView()->clearLayer(m_layerIdx);
		double row1[5], col1[5], row2[5], col2[5];
		QStringList title;
		title << tr("Search Roi") << tr("Up") << tr("Down") << tr("Left") << tr("Right");
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

void locationGuiWdg::comboBox_currentIndexChanged(int index)
{

}

void locationGuiWdg::on_bt_LTuseLead1_clicked()
{
	m_grpIndex = 0;
	if (QMessageBox::warning(this, tr("Warning!"), tr("make sure to set all leads' param the same as lead 1?"), QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
		for (int j = 0;j < GRP_MAX_LEADNUM;j++) {
			if (j == 0) {
				continue;
			}
			m_param->errPinStandOffMin[m_grpIndex][j] = m_param->errPinStandOffMin[m_grpIndex][0];
			m_param->errPinStandOffMax[m_grpIndex][j] = m_param->errPinStandOffMax[m_grpIndex][0];
			m_param->errPinWidthMin[m_grpIndex][j] = m_param->errPinWidthMin[m_grpIndex][0];
			m_param->errPinWidthMax[m_grpIndex][j] = m_param->errPinWidthMax[m_grpIndex][0];
			m_param->errPinGapMin[m_grpIndex][j] = m_param->errPinGapMin[m_grpIndex][0];
			m_param->errPinGapMax[m_grpIndex][j] = m_param->errPinGapMax[m_grpIndex][0];
		}
		connectSlots(false);
		refrenshTable();
		connectSlots(true);

		m_module->setParamChanged(ProductLevel);
	}
}

void locationGuiWdg::on_bt_RBuseLead1_clicked()
{
	m_grpIndex = 1;
	if (QMessageBox::warning(this, tr("Warning!"), tr("make sure to set all leads' param the same as lead 1?"), QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
		for (int j = 0;j < GRP_MAX_LEADNUM;j++) {
			if (j == 0) {
				continue;
			}
			m_param->errPinStandOffMin[m_grpIndex][j] = m_param->errPinStandOffMin[m_grpIndex][0];
			m_param->errPinStandOffMax[m_grpIndex][j] = m_param->errPinStandOffMax[m_grpIndex][0];
			m_param->errPinWidthMin[m_grpIndex][j] = m_param->errPinWidthMin[m_grpIndex][0];
			m_param->errPinWidthMax[m_grpIndex][j] = m_param->errPinWidthMax[m_grpIndex][0];
			m_param->errPinGapMin[m_grpIndex][j] = m_param->errPinGapMin[m_grpIndex][0];
			m_param->errPinGapMax[m_grpIndex][j] = m_param->errPinGapMax[m_grpIndex][0];
		}
		connectSlots(false);
		refrenshTable();
		connectSlots(true);

		m_module->setParamChanged(ProductLevel);
	}
}

void locationGuiWdg::Cb_Toggled(bool val)
{
	int CbIdx;
	QCheckBox* cbbox = qobject_cast<QCheckBox*>(sender());
	for (int i = 0;i < CheckBoxNum;i++)
	{
		if (cbbox == m_cbBox[i]) {
			CbIdx = i;
			printf("cb_%d triggerd \n", i);

		}
	}
	//corresponding
	if (CbIdx == 0)
	{
		m_param->errPinStandOffValid = val;
	}
	else if (CbIdx == 1)
	{
		m_param->errPinWidthValid = val;
	}
	else if (CbIdx == 2)
	{
		m_param->errPinGapValid = val;
	}
	else if (CbIdx == 3)
	{
		m_param->errPinStandOffDiffValid = val;
	}
	//corresponding
	Qt::ItemFlags Edit;
	if (val == 0) {
		Edit = Qt::NoItemFlags;
	}
	else {
		Edit = (Qt::ItemFlags) 63;
	}

	for (int i = 0;i < m_param->ChildTablenum;i++)
	{
		m_childTable[i]->item(CbIdx + 1, 0)->setFlags(Edit);
		m_childTable[i]->item(CbIdx + 1, 1)->setFlags(Edit);
	}

	m_module->setParamChanged(ProductLevel);
}

void locationGuiWdg::Child_Changed(QTableWidgetItem * item)
{
	int ChildIdx;
	QTableWidget *tablewid = qobject_cast<QTableWidget*>(sender());
	for (int i = 0;i < m_param->ChildTablenum;i++)
	{
		if (tablewid == m_childTable[i])
		{
			ChildIdx = i + 1;
			printf("childbox %d Changed triggerd \n", i);
		}
	}
	if (m_param->ChildTablenum == (m_param->LTnum + m_param->RBnum)) {
		m_grpIndex = ChildIdx > m_param->LTnum;
		if (m_grpIndex)
		{
			m_leadIndex = ChildIdx - m_param->LTnum - 1;
		}
		else {
			m_leadIndex = ChildIdx - 1;
		}

	}

	if (item)
	{
		int itemrow = item->row();
		int itemcol = item->column();
		QString itemtext = item->text();
		double val = itemtext.toDouble();
		//corresponding
		if ((itemrow == 1) && (itemcol == 0))
		{
			m_param->errPinStandOffMin[m_grpIndex][m_leadIndex] = val;
		}
		else if ((itemrow == 1) && (itemcol == 1))
		{
			m_param->errPinStandOffMax[m_grpIndex][m_leadIndex] = val;
		}
		else if ((itemrow == 2) && (itemcol == 0))
		{
			m_param->errPinWidthMin[m_grpIndex][m_leadIndex] = val;
		}
		else if ((itemrow == 2) && (itemcol == 1))
		{
			m_param->errPinWidthMax[m_grpIndex][m_leadIndex] = val;
		}
		else if ((itemrow == 3) && (itemcol == 0))
		{
			m_param->errPinGapMin[m_grpIndex][m_leadIndex] = val;
		}
		else if ((itemrow == 3) && (itemcol == 1))
		{
			m_param->errPinGapMax[m_grpIndex][m_leadIndex] = val;
		}
		else if ((itemrow == 4) && (itemcol == 0))
		{
			m_param->errPinStandOffDiffMax[m_grpIndex] = val;
		}
		//corresponding
	}

	m_module->setParamChanged(ProductLevel);
}

