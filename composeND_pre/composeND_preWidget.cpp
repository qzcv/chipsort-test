#include "composeND_preWidget.h"
#include "composeND_preModule.h"
#include "composeND_preParam.h"
#include "ui_composeND_preWidget.h"
#include <QGroupBox>
#include <QPushButton>
#include <QSpinBox>
#include <QComboBox>
#include "ImageView.h"

composeND_preWidget::composeND_preWidget(int i) :
	ModSetWidget(),ui(new Ui::composeND_preWidget),m_widgetType(i)
{
	m_hasConnect = false;
	ui->setupUi(this);
	iniUi();
	m_layerIdx = 0;
	m_param = nullptr;
	m_module = nullptr;
}

composeND_preWidget::~composeND_preWidget()
{
	delete ui;
}

void composeND_preWidget::setModule(UnitModule *module)
{
	m_module=dynamic_cast<composeND_preModule*>(module);
	m_param = m_module->m_param;
	//currentView()->setLayer(m_layerIdx);
	//connectSlots(false);
	//setUiValue();
	//connectSlots(true);
}

void composeND_preWidget::showEvent(QShowEvent * event)
{
	connectSlots(false);
	setUiValue();
	connectSlots(true);
}

void composeND_preWidget::connectSlots(bool link)
{
	if(m_hasConnect==link)
		return;
	m_hasConnect=link;

	connectOrNot(link, ui->sp_imNum, SIGNAL(valueChanged(int)), SLOT(sp_valueChanged(int)));
	connectOrNot(link, ui->sp_roiNum, SIGNAL(valueChanged(int)), SLOT(sp_valueChanged(int)));

	connectOrNot(link, ui->cob_imIdx, SIGNAL(currentIndexChanged(int)), SLOT(cb_currentIndexChanged(int)));

	connectOrNot(link, ui->bt_select, SIGNAL(clicked()), SLOT(bt_clicked()));
	connectOrNot(link, ui->bt_ensure, SIGNAL(clicked()), SLOT(bt_clicked()));
	connectOrNot(link, ui->bt_cancel, SIGNAL(clicked()), SLOT(bt_clicked()));
}

void composeND_preWidget::iniUi()
{
	ui->sp_roiNum->setMaximum(ROI_MAX_NUM);
}

void composeND_preWidget::setUiValue()
{
	if(!m_param)
		return;

	ui->lb_imNum->setVisible(m_widgetType != 1);
	ui->sp_imNum->setVisible(m_widgetType != 1);

	ui->sp_imNum->setValue(m_param->imNum);
	changeImNum(m_param->imNum);

	int idx = ui->cob_imIdx->currentIndex();
	ui->sp_roiNum->setValue(m_param->roiNum[idx]);
	ui->gb_block->setTitle(tr("img%1").arg(idx + 1));
	
// 	ui->bt_ensure->setEnabled(!ui->bt_select->isEnabled());
// 	ui->bt_cancel->setEnabled(!ui->bt_select->isEnabled());
}

void composeND_preWidget::bt_clicked()
{
	QPushButton *bt = qobject_cast<QPushButton *>(sender());

	m_module->setParamChanged(MachineLevel);

	if (bt == ui->bt_select || bt == ui->bt_ensure || bt == ui->bt_cancel)
	{
		bool isEn = (bt != ui->bt_select);
		ui->bt_select->setEnabled(isEn);
		ui->bt_ensure->setEnabled(!isEn);
		ui->bt_cancel->setEnabled(!isEn);
		ui->sp_roiNum->setEnabled(isEn);
		ui->sp_imNum->setEnabled(isEn);
		ui->cob_imIdx->setEnabled(isEn);

		int i = ui->cob_imIdx->currentIndex();

		if (bt == ui->bt_select)
		{
			double row1[ROI_MAX_NUM], col1[ROI_MAX_NUM];
			double row2[ROI_MAX_NUM], col2[ROI_MAX_NUM];
			QStringList titles;
			for (auto k = 0; k < m_param->roiNum[i]; ++k)
			{
				row1[k] = m_param->regRow0[i][k];
				col1[k] = m_param->regCol0[i][k];
				row2[k] = m_param->regRow1[i][k];
				col2[k] = m_param->regCol1[i][k];
				titles << tr("ROI%1_%2").arg(i).arg(k + 1);
			}

			currentView()->finishDraw();
			currentView()->clearLayer(m_layerIdx);
			currentView()->drawRects1(m_param->roiNum[i], row1, col1, row2, col2, titles);
			currentView()->update();
		}
		else if (bt == ui->bt_ensure)
		{
			double row1[ROI_MAX_NUM], col1[ROI_MAX_NUM];
			double row2[ROI_MAX_NUM], col2[ROI_MAX_NUM];
			currentView()->finishDraw();
			currentView()->getDrawRects1(m_param->roiNum[i], row1, col1, row2, col2);
			checkROI(m_param->roiNum[i], row1, col1, row2, col2);
			for (auto k = 0; k < m_param->roiNum[i]; ++k)
			{
				m_param->regRow0[i][k] = row1[k];
				m_param->regCol0[i][k] = col1[k];
				m_param->regRow1[i][k] = row2[k];
				m_param->regCol1[i][k] = col2[k];
			}
			currentView()->clearLayer(m_layerIdx);
			currentView()->setColor(Qt::green);
			for (auto k = 0; k < m_param->roiNum[i]; ++k)
			{
				currentView()->dispRect1(
					m_param->regRow0[i][k], m_param->regCol0[i][k],
					m_param->regRow1[i][k], m_param->regCol1[i][k], tr("ROI%1_%2").arg(i).arg(k + 1));
			}
			currentView()->update();
		}
		else if (bt == ui->bt_cancel)
		{
			currentView()->clearLayer(m_layerIdx);
			currentView()->finishDraw();
			currentView()->update();
		}
	}
}

void composeND_preWidget::cb_currentIndexChanged(int idx)
{
	QComboBox *cob = qobject_cast<QComboBox *>(sender());

	m_module->setParamChanged(MachineLevel);

	if (cob == ui->cob_imIdx)
	{
		*m_module->p_outIm = **m_module->p_inIm[idx];
		ui->sp_roiNum->setValue(m_param->roiNum[idx]);
		ui->gb_block->setTitle(tr("img%1").arg(idx + 1));
		m_module->triggerEvent(ENT_DISPIMG);
	}
}

void composeND_preWidget::sp_valueChanged(int val)
{
	QSpinBox *sp = qobject_cast<QSpinBox *>(sender());

	m_module->setParamChanged(ProductLevel);

	if (ui->sp_imNum == sp)
	{
		m_param->imNum = val;
		m_module->changePinsNum(val);
		changeImNum(val);

		while (m_param->roiNum.size() < m_param->imNum)
		{
			m_param->roiNum.push_back(1);
			m_param->regRow0.push_back(QVector<double>(1, 100));
			m_param->regCol0.push_back(QVector<double>(1, 100));
			m_param->regRow1.push_back(QVector<double>(1, 200));
			m_param->regCol1.push_back(QVector<double>(1, 200));
		}
		while (m_param->roiNum.size() > m_param->imNum)
		{
			m_param->roiNum.pop_back();
			m_param->regRow0.pop_back();
			m_param->regCol0.pop_back();
			m_param->regRow1.pop_back();
			m_param->regCol1.pop_back();
		}

		m_module->setParamChanged(StructLevel);
		m_module->setParamChanged(MachineLevel);
	}
	else if (sp == ui->sp_roiNum)
	{
		int i = ui->cob_imIdx->currentIndex();
		m_param->roiNum[i] = val;

		while (m_param->regRow0[i].size() < m_param->roiNum[i])
		{
			m_param->regRow0[i].push_back(100);
			m_param->regCol0[i].push_back(100);
			m_param->regRow1[i].push_back(200);
			m_param->regCol1[i].push_back(200);
		}
		while (m_param->regRow0[i].size() > m_param->roiNum[i])
		{
			m_param->regRow0[i].pop_back();
			m_param->regCol0[i].pop_back();
			m_param->regRow1[i].pop_back();
			m_param->regCol1[i].pop_back();
		}
		m_module->setParamChanged(MachineLevel);
	}
}

void composeND_preWidget::checkROI(int num, double * row0, double * col0, double * row1, double * col1)
{
	for (auto i = 0;i < num;++i)
	{
		if (row0[i] > row1[i])
			std::swap(row0[i], row1[i]);
		if (col0[i] > col1[i])
			std::swap(col0[i], col1[i]);
	}
}

void composeND_preWidget::changeImNum(int imNum)
{
	while (ui->cob_imIdx->count() < imNum)
		ui->cob_imIdx->addItem(tr("img%1").arg(ui->cob_imIdx->count() + 1));
	while (ui->cob_imIdx->count() > imNum)
		ui->cob_imIdx->removeItem(ui->cob_imIdx->count() - 1);
}
