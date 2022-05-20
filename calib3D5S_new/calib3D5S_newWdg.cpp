#include "calib3D5S_newWdg.h"
#include "calib3D5S_newMod.h"
#include "calib3D5S_newParam.h"
#include "ui_calib3D5S_newWdg.h"
#include "ImageView.h"
#include <QMessageBox>

calib3D5S_newWdg::calib3D5S_newWdg(int i) :
	ModSetWidget(),ui(new Ui::calib3D5S_newWdg),m_widgetType(i)
{
	m_hasConnect = false;
	ui->setupUi(this);
	iniUi();
	m_param = nullptr;
	m_module = nullptr;
}

calib3D5S_newWdg::~calib3D5S_newWdg()
{
	delete ui;
}

void calib3D5S_newWdg::setModule(UnitModule *module)
{
	m_module=dynamic_cast<calib3D5S_newMod*>(module);
	m_param = m_module->m_param;
	//connectSlots(false);
	//setUiValue();
	//connectSlots(true);
}

void calib3D5S_newWdg::showEvent(QShowEvent * event)
{
	connectSlots(false);
	setUiValue();
	connectSlots(true);
}

void calib3D5S_newWdg::on_bt_Select_clicked()
{
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
	case SelectStand:
		selectStand();
		break;
	}
}

void calib3D5S_newWdg::on_bt_OK_clicked()
{
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
	case SelectStand:
		getSelectStand();
		break;
	}
	m_module->setParamChanged(MachineLevel);
}

void calib3D5S_newWdg::on_bt_Cancel_clicked()
{
	currentView()->finishDraw();
	m_selectState = Idle;
}

void calib3D5S_newWdg::radioButton_toggled(bool val)
{
	if (val) {
		QRadioButton* rb = qobject_cast<QRadioButton*>(sender());
		if (rb == ui->rb_HorizontalH) {
			m_param->horizontalHorVerticalH = 0;
		}
		else if (rb == ui->rb_VerticalH) {
			m_param->horizontalHorVerticalH = 1;
		}
	}
	m_module->setParamChanged(MachineLevel);
}

void calib3D5S_newWdg::on_bt_calib_clicked()
{
	m_module->triggerEvent(ENT_CALIB);

	connectSlots(false);
	setUiValue();
	connectSlots(true);

	if(m_module->m_allok)
		QMessageBox::information(this, QObject::tr("Message"), QObject::tr("Calibration is successful!"), QMessageBox::Yes);
	else
		QMessageBox::warning(this, QObject::tr("Warning"), QObject::tr("Calibration is failed!"), QMessageBox::Yes);
}

void calib3D5S_newWdg::on_bt_compose_clicked()
{
	m_module->triggerEvent(ENT_PRE);
}

void calib3D5S_newWdg::actualSizeChanged(double val)
{
	QDoubleSpinBox* sp = qobject_cast<QDoubleSpinBox*>(sender());
	for (int i = 0;i < 5;i++) {
		if (sp == sp_actual[i]) {
			m_param->actualSize[i] = val;
			break;
		}
	}
	for (int i = 0;i < 4;i++) {
		if (sp == sp_depth[i]) {
			m_param->actualDepth[i] = val;
			break;
		}
		else if (sp == sp_stand[i]) {
			m_param->standH[i] = val;
			break;
		}
	}
	m_module->setParamChanged(MachineLevel);
}

void calib3D5S_newWdg::connectSlots(bool link)
{
	if(m_hasConnect==link)
		return;
	m_hasConnect=link;

	for (int i = 0;i < 5;i++)
	{
		connect(sp_actual[i], SIGNAL(valueChanged(double)), SLOT(actualSizeChanged(double)));
	}
	for (int i = 0;i < 4;i++)
	{
		connect(sp_depth[i], SIGNAL(valueChanged(double)), SLOT(actualSizeChanged(double)));
		connect(sp_stand[i], SIGNAL(valueChanged(double)), SLOT(actualSizeChanged(double)));
	}

	connect(ui->rb_HorizontalH, SIGNAL(toggled(bool)), SLOT(radioButton_toggled(bool)));
	connect(ui->rb_VerticalH, SIGNAL(toggled(bool)), SLOT(radioButton_toggled(bool)));
}

void calib3D5S_newWdg::iniUi()
{
	for (int i = 0;i < 5;i++)
	{
		sp_actual[i] = findChild<QDoubleSpinBox*>(QString("sp_l%1").arg(i));
		lb_imageSize[i] = findChild<QLabel*>(QString("lb_pixel%1").arg(i));
		lb_pixelSize[i] = findChild<QLabel*>(QString("lb_size%1").arg(i));
	}
	for (int i = 0;i < 4;i++)
	{
		sp_depth[i] = findChild<QDoubleSpinBox*>(QString("sp_depth%1").arg(i));
		sp_stand[i] = findChild<QDoubleSpinBox*>(QString("sp_stand_%1").arg(i));
		lb_offset[i] = findChild<QLabel*>(QString("lb_s%1").arg(i + 1));
		lb_aOffset[i] = findChild<QLabel*>(QString("lb_sa%1").arg(i + 1));
		lb_prjRatio[i] = findChild<QLabel*>(QString("lb_k%1").arg(i + 1));
		lb_RS[i] = findChild<QLabel*>(QString("lb_RS_%1").arg(i));
		lb_ES[i] = findChild<QLabel*>(QString("lb_ES_%1").arg(i));
	}
	for (int i = 0;i < 4;i++) {
		lb_angle[i] = findChild<QLabel*>(QString("lb_a%1").arg(i + 1));
	}
}

void calib3D5S_newWdg::setUiValue()
{
	if(!m_param)
		return;

	for (int i = 0;i < 5;i++) {
		sp_actual[i]->setValue(m_param->actualSize[i]);
		lb_imageSize[i]->clear();
		lb_imageSize[i]->setText(QString("%1").arg(m_param->imgDist[i], 0, 'f', 1));
		lb_pixelSize[i]->setText(QString("%1").arg(m_param->pixelSize[i], 0, 'f', 5));
	}
	for (int i = 0;i < 2;i++) {
		sp_depth[i]->setValue(m_param->actualDepth[i]);
		lb_prjRatio[i]->setText(QString("%1").arg(m_param->prjRatio[i], 0, 'f', 3));
	}
	for (int i = 0;i < 4;i++) {
		lb_angle[i]->setText(QString("%1").arg(m_param->angle[i] / CV_PI * 180, 0, 'f', 2));
		lb_RS[i]->setText(QString("%1").arg(m_param->standSize[i], 0, 'f', 3));
		lb_ES[i]->setText(QString("%1").arg(m_param->standOffset[i], 0, 'f', 3));
	}
	if (m_param->horizontalHorVerticalH == 0) {
		ui->rb_HorizontalH->setChecked(true);
	}
	else {
		ui->rb_VerticalH->setChecked(true);
	}
}

void calib3D5S_newWdg::selectSize()
{
	QStringList title = { "Size H" };
	for (int i = 1;i < 5;i++)
		title << QString("Size L%1").arg(i);
	currentView()->drawRects1(5, m_param->sizeRow1, m_param->sizeCol1, m_param->sizeRow2, m_param->sizeCol2, title);
	m_selectState = SelectSize;
}

void calib3D5S_newWdg::getSelectSize()
{
	currentView()->finishDraw();
	if (m_selectState == SelectSize)
		currentView()->getDrawRects1(5, m_param->sizeRow1, m_param->sizeCol1, m_param->sizeRow2, m_param->sizeCol2);
	m_selectState = Idle;
}

void calib3D5S_newWdg::selectLine()
{
	QStringList title;
	for (int i = 0;i < 4;i++)
		title << QString("Line%1").arg(i + 1);
	currentView()->drawRects1(4, m_param->lineRow1, m_param->lineCol1, m_param->lineRow2, m_param->lineCol2, title);
	m_selectState = SelectLine;
}

void calib3D5S_newWdg::getSelectLine()
{
	currentView()->finishDraw();
	if (m_selectState == SelectLine)
		currentView()->getDrawRects1(4, m_param->lineRow1, m_param->lineCol1, m_param->lineRow2, m_param->lineCol2);
	m_selectState = Idle;
}

void calib3D5S_newWdg::selectPrj()
{
	QStringList title;
	for (int i = 0;i < 4;i++)
		title << tr("slot%1").arg(i + 1);
	currentView()->drawRects1(4, m_param->depthRow1, m_param->depthCol1, m_param->depthRow2, m_param->depthCol2, title);
	m_selectState = SelectPrj;
}

void calib3D5S_newWdg::getSelectPrj()
{
	currentView()->finishDraw();
	if (m_selectState == SelectPrj)
		currentView()->getDrawRects1(4, m_param->depthRow1, m_param->depthCol1, m_param->depthRow2, m_param->depthCol2);
	m_selectState = Idle;
}

void calib3D5S_newWdg::selectStand()
{
	QStringList title;
	for (int i = 0;i < 2;i++)
	{
		title << QString::fromLocal8Bit("ธ฿%1").arg(i + 1);
		title << QString::fromLocal8Bit("ตอ%1").arg(i + 1);
	}
	currentView()->drawRects1(4, m_param->standRow1, m_param->standCol1, m_param->standRow2, m_param->standCol2, title);
	m_selectState = SelectStand;
}

void calib3D5S_newWdg::getSelectStand()
{
	currentView()->finishDraw();
	if (m_selectState == SelectStand)
		currentView()->getDrawRects1(4, m_param->standRow1, m_param->standCol1, m_param->standRow2, m_param->standCol2);
	m_selectState = Idle;
}

