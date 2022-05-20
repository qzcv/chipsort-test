#include "calib3D5SWdg.h"
#include "calib3D5SMod.h"
#include "calib3D5SParam.h"
#include "ui_calib3D5SWdg.h"
#include "ImageView.h"
#include <QMessageBox>

calib3D5SWdg::calib3D5SWdg(int i) :
	ModSetWidget(),ui(new Ui::calib3D5SWdg),m_widgetType(i)
{
	m_hasConnect = false;
	ui->setupUi(this);
	iniData();
	iniUi();
	m_param = nullptr;
	m_module = nullptr;
}

calib3D5SWdg::~calib3D5SWdg()
{
	delete ui;
}

void calib3D5SWdg::setModule(UnitModule *module)
{
	m_module=dynamic_cast<calib3D5SMod*>(module);
	m_param = m_module->m_param;
	//updateUi();
}

void calib3D5SWdg::showEvent(QShowEvent * event)
{
	updateUi();
}

void calib3D5SWdg::connectSlots(bool link)
{
	if(m_hasConnect==link)
		return;
	m_hasConnect=link;

	for (int i = 0;i < 5;i++)
		connectOrNot(link, sp_actual[i], SIGNAL(valueChanged(double)), SLOT(actualSizeChanged(double)));
	for (int i = 0;i < 4;i++)
		connectOrNot(link, sp_depth[i], SIGNAL(valueChanged(double)), SLOT(actualSizeChanged(double)));

	connectOrNot(link, ui->rb_HorizontalH, SIGNAL(toggled(bool)), SLOT(radioButton_toggled(bool)));
	connectOrNot(link, ui->rb_VerticalH, SIGNAL(toggled(bool)), SLOT(radioButton_toggled(bool)));
}

void calib3D5SWdg::iniData()
{
	
}

void calib3D5SWdg::iniUi()
{
	for (int i = 0;i < 5;i++) {
		sp_actual[i] = findChild<QDoubleSpinBox*>(QString("sp_l%1").arg(i));
		lb_imageSize[i] = findChild<QLabel*>(QString("lb_pixel%1").arg(i));
		lb_pixelSize[i] = findChild<QLabel*>(QString("lb_size%1").arg(i));
	}
	for (int i = 0;i < 4;i++) {
		sp_depth[i] = findChild<QDoubleSpinBox*>(QString("sp_depth%1").arg(i));
		lb_offset[i] = findChild<QLabel*>(QString("lb_s%1").arg(i + 1));
		lb_aOffset[i] = findChild<QLabel*>(QString("lb_sa%1").arg(i + 1));
		lb_prjRatio[i] = findChild<QLabel*>(QString("lb_k%1").arg(i + 1));
		lb_angle[i] = findChild<QLabel*>(QString("lb_a%1").arg(i + 1));
	}
}

void calib3D5SWdg::setUiValue()
{
	if(!m_param)
		return;

	for (int i = 0;i < 5;i++) {
		sp_actual[i]->setValue(m_param->actualSize[i]);
		lb_imageSize[i]->clear();
		lb_imageSize[i]->setText(QString("%1").arg(m_module->m_sizeDist[i], 0, 'f', 1));
		lb_pixelSize[i]->setText(QString("%1").arg(m_param->pixelSize[i], 0, 'f', 5));
	}
	for (int i = 0;i < 4;i++) {
		sp_depth[i]->setValue(m_param->actualDepth[i]);
		lb_offset[i]->setText(QString("%1").arg(m_param->prjOffset[i], 0, 'f', 1));
		lb_aOffset[i]->setText(QString("%1").arg(m_param->prjActualOffset[i], 0, 'f', 3));
		lb_prjRatio[i]->setText(QString("%1").arg(m_param->prjRatio[i], 0, 'f', 4));
		lb_angle[i]->setText(QString("%1").arg(m_param->angle[i] / PI*180.0, 0, 'f', 1));
	}
	
	if (m_param->horizontalHorVerticalH == 0) 
		ui->rb_HorizontalH->setChecked(true);
	else
		ui->rb_VerticalH->setChecked(true);
}

void calib3D5SWdg::updateUi()
{
	connectSlots(false);
	setUiValue();
	connectSlots(true);
}

void calib3D5SWdg::selectSize()
{
	QStringList title;
	title << QString("Size H");
	for (int i = 1;i < 5;i++) 
		title << QString("Size L%1").arg(i);
	currentView()->drawRects1(5, m_param->sizeRow1, m_param->sizeCol1, m_param->sizeRow2, m_param->sizeCol2, title);
	m_selectState = SelectSize;
}

void calib3D5SWdg::getSelectSize()
{
	currentView()->finishDraw();
	if (m_selectState == SelectSize)
		currentView()->getDrawRects1(5, m_param->sizeRow1, m_param->sizeCol1, m_param->sizeRow2, m_param->sizeCol2);
	m_selectState = Idle;
}

void calib3D5SWdg::selectLine()
{
	QStringList title;
	for (int i = 0;i < 4;i++)
		title << QString("Line%1").arg(i + 1);
	currentView()->drawRects1(4, m_param->lineRow1, m_param->lineCol1, m_param->lineRow2, m_param->lineCol2, title);
	m_selectState = SelectLine;
}

void calib3D5SWdg::getSelectLine()
{
	currentView()->finishDraw();
	if (m_selectState == SelectLine)
		currentView()->getDrawRects1(4, m_param->lineRow1, m_param->lineCol1, m_param->lineRow2, m_param->lineCol2);
	m_selectState = Idle;
}

void calib3D5SWdg::selectPrj()
{
	QStringList title;
	for (int i = 0;i < 4;i++)
		title << tr("slot%1").arg(i + 1);
	currentView()->drawRects1(4, m_param->depthRow1, m_param->depthCol1, m_param->depthRow2, m_param->depthCol2, title);
	m_selectState = SelectPrj;
}

void calib3D5SWdg::getSelectPrj()
{
	currentView()->finishDraw();
	if (m_selectState == SelectPrj)
		currentView()->getDrawRects1(4, m_param->depthRow1, m_param->depthCol1, m_param->depthRow2, m_param->depthCol2);
	m_selectState = Idle;
}

void calib3D5SWdg::actualSizeChanged(double val)
{
	QDoubleSpinBox* sp = qobject_cast<QDoubleSpinBox*>(sender());
	for (int i = 0;i < 5;i++) {
		if (sp == sp_actual[i]) {
			m_param->actualSize[i] = val;
			m_module->setParamChanged(ProductLevel);
			return;
		}
	}
	for (int i = 0;i < 4;i++) {
		if (sp == sp_depth[i]) {
			m_param->actualDepth[i] = val;
			m_module->setParamChanged(MachineLevel);
			return;
		}
	}
}

void calib3D5SWdg::on_bt_Select_clicked()
{
	ui->bt_Select->setEnabled(false);
	ui->bt_OK->setEnabled(true);
	ui->bt_Cancel->setEnabled(true);

	int index = ui->tabWidget->currentIndex() + 1;
	switch (index) {
	case SelectSize:
		selectSize();
		break;
	case SelectLine:
		selectLine();
		break;
	case SelectPrj:
		selectPrj();
		break;
	}
}

void calib3D5SWdg::on_bt_OK_clicked()
{
	ui->bt_Select->setEnabled(true);
	ui->bt_OK->setEnabled(false);
	ui->bt_Cancel->setEnabled(false);

	int index = ui->tabWidget->currentIndex() + 1;
	switch (index) {
	case SelectSize:
		getSelectSize();
		break;
	case SelectLine:
		getSelectLine();
		break;
	case SelectPrj:
		getSelectPrj();
		break;
	}
	m_module->setParamChanged(ProductLevel);
}

void calib3D5SWdg::on_bt_Cancel_clicked()
{
	ui->bt_Select->setEnabled(true);
	ui->bt_OK->setEnabled(false);
	ui->bt_Cancel->setEnabled(false);

	currentView()->finishDraw();
	m_selectState = Idle;
}

void calib3D5SWdg::on_bt_calib_clicked()
{
	m_module->triggerEvent(ENT_CALIB);
	updateUi();

	if (m_module->m_allok)
		QMessageBox::information(this, tr("Message"), tr("Calibration is successful!"), QMessageBox::Yes);
	else
		QMessageBox::warning(this, tr("Warning"), tr("Calibration is failed!"), QMessageBox::Yes);
}

void calib3D5SWdg::radioButton_toggled(bool check)
{
	if (check) {
		QRadioButton* rb = qobject_cast<QRadioButton*>(sender());
		if (rb == ui->rb_HorizontalH) {
			m_param->horizontalHorVerticalH = 0;
		}
		else if (rb == ui->rb_VerticalH) {
			m_param->horizontalHorVerticalH = 1;
		}
		m_module->setParamChanged(ProductLevel);
	}
}
