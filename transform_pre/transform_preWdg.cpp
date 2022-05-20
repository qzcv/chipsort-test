#include "transform_preWdg.h"
#include "transform_preMod.h"
#include "transform_preParam.h"
#include "ui_transform_preWdg.h"

transform_preWdg::transform_preWdg(int i) :
	ModSetWidget(),ui(new Ui::transform_preWdg),m_widgetType(i)
{
	m_hasConnect = false;
	ui->setupUi(this);
	iniUi();
	m_param = nullptr;
	m_module = nullptr;
}

transform_preWdg::~transform_preWdg()
{
	delete ui;
}

void transform_preWdg::setModule(UnitModule *module)
{
	m_module=dynamic_cast<transform_preMod*>(module);
	m_param = m_module->m_param;
// 	connectSlots(false);
// 	setUiValue();
// 	connectSlots(true);
}

void transform_preWdg::showEvent(QShowEvent * event)
{
	connectSlots(false);
	setUiValue();
	connectSlots(true);
}

void transform_preWdg::connectSlots(bool link)
{
	if(m_hasConnect==link)
		return;
	m_hasConnect=link;

}

void transform_preWdg::iniUi()
{

}

void transform_preWdg::setUiValue()
{
	if(!m_param)
		return;

	ui->cb_type->setCurrentIndex(m_param->transformType);
}

void transform_preWdg::on_cb_type_currentIndexChanged(int val)
{
	m_param->transformType = val;

	m_module->setParamChanged(ProductLevel);
}