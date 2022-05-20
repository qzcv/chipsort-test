#include "smooth_preWdg.h"
#include "smooth_preMod.h"
#include "smooth_preParam.h"
#include "ui_smooth_preWdg.h"

smooth_preWdg::smooth_preWdg(int i) :
	ModSetWidget(),ui(new Ui::smooth_preWdg),m_widgetType(i)
{
	m_hasConnect = false;
	ui->setupUi(this);
	iniUi();
	m_param = nullptr;
	m_module = nullptr;
}

smooth_preWdg::~smooth_preWdg()
{
	delete ui;
}

void smooth_preWdg::setModule(UnitModule *module)
{
	m_module=dynamic_cast<smooth_preMod*>(module);
	m_param = m_module->m_param;
	//connectSlots(false);
	//setUiValue();
	//connectSlots(true);
}

void smooth_preWdg::showEvent(QShowEvent * event)
{
	connectSlots(false);
	setUiValue();
	connectSlots(true);
}

void smooth_preWdg::connectSlots(bool link)
{
	if(m_hasConnect==link)
		return;
	m_hasConnect=link;

	connectOrNot(link, ui->cb_filtertype, SIGNAL(currentIndexChanged(int)), SLOT(typeChanged(int)));
	connectOrNot(link, ui->sp_param1, SIGNAL(valueChanged(int)), SLOT(intParamChanged(int)));
	connectOrNot(link, ui->sp_param2, SIGNAL(valueChanged(int)), SLOT(intParamChanged(int)));
	connectOrNot(link, ui->sp_param3, SIGNAL(valueChanged(int)), SLOT(intParamChanged(int)));
	connectOrNot(link, ui->sp_param4, SIGNAL(valueChanged(double)), SLOT(doubleParamChanged(double)));
}

void smooth_preWdg::iniUi()
{

}

void smooth_preWdg::setUiValue()
{
	if(!m_param)
		return;

	ui->sp_param1->setValue(m_param->smoothParam1);
	ui->sp_param2->setValue(m_param->smoothParam2);
	ui->sp_param3->setValue(m_param->smoothParam3);
	ui->sp_param4->setValue(m_param->smoothParam4);
	ui->cb_filtertype->setCurrentIndex(m_param->smoothType);

	setObjVisible();
}

void smooth_preWdg::setObjVisible()
{
	bool V[4] = { false,false,false,false };
	switch (m_param->smoothType) {
	case 0:
		//GAUSSIAN
	case 2:		
		//MEDIAN
		V[0] = true;
		break;
	case 1:
		//mean
		V[0] = V[1] = true;
		break;
	case 3:
		//sigma
		V[0] = V[1] = V[2] = true;
		break;
	case 4:
		//contrast
		V[0] = V[1] = V[3] = true;
		break;
	default:
		V[0] = true;
	}
	ui->lb_param1->setVisible(V[0]);
	ui->lb_param2->setVisible(V[1]);
	ui->lb_param3->setVisible(V[2]);
	ui->lb_param4->setVisible(V[3]);

	ui->sp_param1->setVisible(V[0]);
	ui->sp_param2->setVisible(V[1]);
	ui->sp_param3->setVisible(V[2]);
	ui->sp_param4->setVisible(V[3]);
}

void smooth_preWdg::typeChanged(int val)
{
	m_param->smoothType = val;
	setObjVisible();
	m_module->setParamChanged(ProductLevel);
}

void smooth_preWdg::intParamChanged(int val)
{
	QSpinBox* sp = qobject_cast<QSpinBox*>(sender());
	if (sp == ui->sp_param1)
		m_param->smoothParam1 = val;
	else if (sp == ui->sp_param2)
		m_param->smoothParam2 = val;
	else if (sp == ui->sp_param3)
		m_param->smoothParam3 = val;

	m_module->setParamChanged(ProductLevel);
}

void smooth_preWdg::doubleParamChanged(double val)
{
	QDoubleSpinBox* sp = qobject_cast<QDoubleSpinBox*>(sender());
	if (sp == ui->sp_param4)
		m_param->smoothParam4 = val;

	m_module->setParamChanged(ProductLevel);
}
