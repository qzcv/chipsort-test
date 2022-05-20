#include "spiltImage_preWdg.h"
#include "spiltImage_preMod.h"
#include "spiltImage_preParam.h"
#include "ui_spiltImage_preWdg.h"
#include "ImageView.h"

spiltImage_preWdg::spiltImage_preWdg(int i) :
	ModSetWidget(),ui(new Ui::spiltImage_preWdg),m_widgetType(i)
{
	m_hasConnect = false;
	ui->setupUi(this);
	iniUi();
	m_layerIdx = 0;
	m_param = nullptr;
	m_module = nullptr;
}

spiltImage_preWdg::~spiltImage_preWdg()
{
	delete ui;
}

void spiltImage_preWdg::setModule(UnitModule *module)
{
	m_module=dynamic_cast<spiltImage_preMod*>(module);
	m_param = m_module->m_param;
	//currentView()->setLayer(m_layerIdx);
	//updateUi();
}

void spiltImage_preWdg::showEvent(QShowEvent * event)
{
	updateUi();
}

void spiltImage_preWdg::connectSlots(bool link)
{
	if(m_hasConnect==link)
		return;
	m_hasConnect=link;


	connectOrNot(link, ui->cb_moduleValid, SIGNAL(toggled(bool)), SLOT(cb_toggled(bool)));
	connectOrNot(link, ui->cb_RoiValid, SIGNAL(toggled(bool)), SLOT(cb_toggled(bool)));

	connectOrNot(link, ui->bt_selectRoi, SIGNAL(toggled(bool)), SLOT(bt_toggled(bool)));

	connectOrNot(link, ui->sp_RoiCounts, SIGNAL(valueChanged(int)), SLOT(sp_valueChanged(int)));
}

void spiltImage_preWdg::iniUi()
{

}

void spiltImage_preWdg::setUiValue()
{
	if(!m_param)
		return;

	ui->cb_moduleValid->setChecked(m_param->moduleValid);
	ui->cb_RoiValid->setChecked(m_param->RoiValid);

	ui->sp_RoiCounts->setValue(m_param->RoiCounts);
}

void spiltImage_preWdg::dispRoi()
{
	//currentView()->clear();
	currentView()->clearLayer(m_layerIdx);
	currentView()->setTextSize(12);
	currentView()->setColor(255, 0, 128);
	currentView()->setFilled(false);
	for (int i = 0;i < m_param->RoiCounts;i++)
	{
		currentView()->dispRect1(
			m_param->segRegionVector[i].row1, m_param->segRegionVector[i].col1, 
			m_param->segRegionVector[i].row2, m_param->segRegionVector[i].col2, "pre");
	}
	currentView()->update();
}

void spiltImage_preWdg::updateUi()
{
	connectSlots(false);
	setUiValue();
	connectSlots(true);
}

void spiltImage_preWdg::bt_toggled(bool checked)
{
	QPushButton *pb = qobject_cast<QPushButton *>(sender());

	if (pb == ui->bt_selectRoi)
	{
		if (checked) {
			if (m_param->RoiCounts == 0)
				return;
			
			ui->bt_selectRoi->setText(tr("OK Roi"));
			ui->sp_RoiCounts->setEnabled(false);
			//currentView()->clear();
			currentView()->clearLayer(m_layerIdx);
			double row1[30], col1[30], row2[30], col2[30];
			QStringList title;
			for (int i = 0;i < m_param->RoiCounts;i++) {
				row1[i] = m_param->segRegionVector[i].row1;
				col1[i] = m_param->segRegionVector[i].col1;
				row2[i] = m_param->segRegionVector[i].row2;
				col2[i] = m_param->segRegionVector[i].col2;
				title << QString("roi[%1]").arg(i + 1);
			}
			currentView()->setColor(255, 0, 128);
			currentView()->drawRects1(m_param->RoiCounts, row1, col1, row2, col2, title);
			currentView()->update();
		}
		else {
			if (m_param->RoiCounts == 0)
				return;
			
			ui->bt_selectRoi->setText(tr("Select Roi"));
			ui->sp_RoiCounts->setEnabled(true);
			currentView()->finishDraw();
			double row1[30], col1[30], row2[30], col2[30];
			currentView()->getDrawRects1(m_param->RoiCounts, row1, col1, row2, col2);
			for (int i = 0;i < m_param->RoiCounts;i++) {
				m_param->segRegionVector[i].row1 = row1[i];
				m_param->segRegionVector[i].col1 = col1[i];
				m_param->segRegionVector[i].row2 = row2[i];
				m_param->segRegionVector[i].col2 = col2[i];
			}
			//currentView()->clear();
			currentView()->clearLayer(m_layerIdx);
			dispRoi();
			currentView()->update();
		}
	}

	m_module->setParamChanged(ProductLevel);
}

void spiltImage_preWdg::cb_toggled(bool checked)
{
	QCheckBox *cb = qobject_cast<QCheckBox *>(sender());

	if (cb == ui->cb_moduleValid)
	{
		m_param->moduleValid = checked;
	}
	else if (cb == ui->cb_RoiValid)
	{
		m_param->RoiValid = checked;
		if (checked) {
			dispRoi();
		}
		else {
			currentView()->finishDraw();
			//currentView()->clear();
			currentView()->clearLayer(m_layerIdx);
			currentView()->update();
		}
	}
}

void spiltImage_preWdg::sp_valueChanged(int val)
{
	QSpinBox *sp = qobject_cast<QSpinBox *>(sender());

	if (sp == ui->sp_RoiCounts)
	{
		if (m_param->RoiCounts == val)
			return;
		if (val > m_param->RoiCounts) {
			spiltImage_preParam::segRegion seg;
			for (int i = m_param->RoiCounts;i < val;i++)
			{
				seg.row1 = seg.col1 = 50 + 10 * i;
				seg.row2 = seg.col2 = 100 + 10 * i;
				m_param->segRegionVector.push_back(seg);
			}
		}
		else {
			vector<spiltImage_preParam::segRegion>::iterator itBegin = m_param->segRegionVector.begin() + val;
			vector<spiltImage_preParam::segRegion>::iterator itEnd = m_param->segRegionVector.end();
			m_param->segRegionVector.erase(itBegin, itEnd);
		}
		m_param->RoiCounts = val;
		currentView()->finishDraw();
		dispRoi();
	}
	m_module->setParamChanged(ProductLevel);
}
