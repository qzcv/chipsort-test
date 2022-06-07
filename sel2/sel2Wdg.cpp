#include "sel2Wdg.h"
#include "sel2Mod.h"
#include "sel2Param.h"
#include "ui_sel2Wdg.h"

sel2Wdg::sel2Wdg(int i) :
	ModSetWidget(),ui(new Ui::sel2Wdg),m_widgetType(i)
{
	m_hasConnect = false;
	ui->setupUi(this);
	iniUi();
	m_param = nullptr;
	m_module = nullptr;
}

sel2Wdg::~sel2Wdg()
{
	delete ui;
}

void sel2Wdg::setModule(UnitModule *module)
{
	m_module=dynamic_cast<sel2Mod*>(module);
	m_param = m_module->m_param;
	connectSlots(false);
	setUiValue();
	connectSlots(true);
}

void sel2Wdg::cob_currentIndexChanged(int idx)
{
	m_param->outIdx = idx;
	*m_module->p_int = idx;
}

void sel2Wdg::connectSlots(bool link)
{
	if(m_hasConnect==link)
		return;
	m_hasConnect=link;

	connectOrNot(link, ui->cob_imIdx, SIGNAL(currentIndexChanged(int)), SLOT(cob_currentIndexChanged(int)));
}

void sel2Wdg::iniUi()
{

}

void sel2Wdg::setUiValue()
{
	if(!m_param)
		return;

	ui->cob_imIdx->setCurrentIndex(m_param->outIdx);
}

