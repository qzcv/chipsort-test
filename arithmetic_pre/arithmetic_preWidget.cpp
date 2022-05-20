#include "arithmetic_preWidget.h"
#include "arithmetic_preModule.h"
#include "arithmetic_preParam.h"
#include "ui_arithmetic_preWidget.h"
#include "ImageView.h"

arithmetic_preWidget::arithmetic_preWidget(int i) :
	ModSetWidget(),ui(new Ui::arithmetic_preWidget),m_widgetType(i)
{
	m_hasConnect = false;
	ui->setupUi(this);
	iniUi();
	m_layerIdx = 0;
	m_param = nullptr;
	m_module = nullptr;
}

arithmetic_preWidget::~arithmetic_preWidget()
{
	delete ui;
}

void arithmetic_preWidget::setModule(UnitModule *module)
{
	m_module=dynamic_cast<arithmetic_preModule*>(module);
	m_param = m_module->m_param;
	//currentView()->setLayer(m_layerIdx);
// 	connectSlots(false);
// 	setUiValue();
// 	connectSlots(true);
}

void arithmetic_preWidget::showEvent(QShowEvent * event)
{
	connectSlots(false);
	setUiValue();
	connectSlots(true);
}

void arithmetic_preWidget::connectSlots(bool link)
{
	if(m_hasConnect==link)
		return;
	m_hasConnect=link;

	connectOrNot(link, ui->cb_allImgValid, SIGNAL(toggled(bool)), SLOT(cb_toggled(bool)));
	connectOrNot(link, ui->cb_enable, SIGNAL(toggled(bool)), SLOT(cb_toggled(bool)));

	connectOrNot(link, ui->dsp_a, SIGNAL(valueChanged(double)), SLOT(dsp_valueChanged(double)));
	connectOrNot(link, ui->dsp_b, SIGNAL(valueChanged(double)), SLOT(dsp_valueChanged(double)));
	connectOrNot(link, ui->dsp_c, SIGNAL(valueChanged(double)), SLOT(dsp_valueChanged(double)));

	connectOrNot(link, ui->sp_ROInum, SIGNAL(valueChanged(int)), SLOT(sp_valueChanged(int)));

	connectOrNot(link, ui->cb_img1, SIGNAL(currentIndexChanged(int)), SLOT(cb_currentIndexChanged(int)));
	connectOrNot(link, ui->cb_img2, SIGNAL(currentIndexChanged(int)), SLOT(cb_currentIndexChanged(int)));
	connectOrNot(link, ui->cb_operator, SIGNAL(currentIndexChanged(int)), SLOT(cb_currentIndexChanged(int)));
	connectOrNot(link, ui->cb_outputIdx, SIGNAL(currentIndexChanged(int)), SLOT(cb_currentIndexChanged(int)));

	connectOrNot(link, ui->bt_select, SIGNAL(toggled(bool)), SLOT(bt_toggled(bool)));
}

void arithmetic_preWidget::iniUi()
{

}

void arithmetic_preWidget::setUiValue()
{
	if(!m_param)
		return;

	ui->cb_enable->setChecked(m_param->enable);
	ui->cb_allImgValid->setChecked(m_param->allImgValid);

	ui->gb_edit->setVisible(m_param->enable);
	ui->cb_outputIdx->setVisible(!m_param->enable);
	ui->lb_output->setVisible(!m_param->enable);

	ui->cb_img1->setCurrentIndex(m_param->idx1);
	ui->cb_img2->setCurrentIndex(m_param->idx2);
	ui->cb_operator->setCurrentIndex(m_param->operatorIdx);
	ui->cb_outputIdx->setCurrentIndex(m_param->outputIdx);

	ui->sp_ROInum->setVisible(!m_param->allImgValid);
	ui->lb_ROInum->setVisible(!m_param->allImgValid);
	ui->bt_select->setVisible(!m_param->allImgValid);
	ui->sp_ROInum->setValue(m_param->roiNum);

	int idx = m_param->operatorIdx;
	visible(idx);
	ui->dsp_a->setValue(m_param->pA[idx]);
	ui->dsp_b->setValue(m_param->pB[idx]);
	ui->dsp_c->setValue(m_param->pC[idx]);
}

void arithmetic_preWidget::visible(int idx)
{
	typedef arithmetic_preModule::Type Ty;
	Ty tIdx = (Ty)idx;
	if (Ty::ADD == tIdx || Ty::SUB == tIdx)
	{
		ui->dsp_a->setVisible(true);
		ui->dsp_b->setVisible(true);
		ui->dsp_c->setVisible(true);
		ui->lb_a->setVisible(true);
		ui->lb_b->setVisible(true);
		ui->lb_c->setVisible(true);
	}
	else if (Ty::MULT == tIdx || Ty::DIV == tIdx)
	{
		ui->dsp_a->setVisible(true);
		ui->dsp_b->setVisible(false);
		ui->dsp_c->setVisible(false);
		ui->lb_a->setVisible(true);
		ui->lb_b->setVisible(false);
		ui->lb_c->setVisible(false);
	}
	else
	{
		ui->dsp_a->setVisible(false);
		ui->dsp_b->setVisible(false);
		ui->dsp_c->setVisible(false);
		ui->lb_a->setVisible(false);
		ui->lb_b->setVisible(false);
		ui->lb_c->setVisible(false);
	}

	if (Ty::NOT == tIdx)
		ui->cb_img2->setVisible(false);
	else
		ui->cb_img2->setVisible(true);
}

void arithmetic_preWidget::bt_toggled(bool checked)
{
	QPushButton *bt = qobject_cast<QPushButton *>(sender());

	QString logStr;
	if (ui->bt_select == bt)
	{
		logStr = bt->text();
		ui->sp_ROInum->setEnabled(!checked);
		if (checked)
		{
			ui->bt_select->setText(tr("ROI OK"));

			double row1[ROI_MAX_NUM], col1[ROI_MAX_NUM], row2[ROI_MAX_NUM], col2[ROI_MAX_NUM];
			QStringList ltext;
			for (auto i = 0;i < m_param->roiNum;++i)
			{
				row1[i] = m_param->segRegArray[i].row1;
				col1[i] = m_param->segRegArray[i].col1;
				row2[i] = m_param->segRegArray[i].row2;
				col2[i] = m_param->segRegArray[i].col2;
				ltext << tr("roi%1").arg(i + 1);
			}
			//currentView()->clear();
			currentView()->clearLayer(m_layerIdx);
			currentView()->drawRects1(m_param->roiNum, row1, col1, row2, col2, ltext);
			currentView()->update();
		}
		else
		{
			ui->bt_select->setText(tr("select ROI"));

			double row1[ROI_MAX_NUM], col1[ROI_MAX_NUM], row2[ROI_MAX_NUM], col2[ROI_MAX_NUM];
			//currentView()->clear();
			currentView()->clearLayer(m_layerIdx);
			currentView()->getDrawRects1(m_param->roiNum, row1, col1, row2, col2);
			currentView()->finishDraw();
			currentView()->setColor(Qt::green);
			for (auto i = 0;i < m_param->roiNum;++i)
			{
				currentView()->dispRect1(row1[i], col1[i], row2[i], col2[i], tr("roi%1").arg(i + 1));
				m_param->segRegArray[i].row1 = row1[i];
				m_param->segRegArray[i].col1 = col1[i];
				m_param->segRegArray[i].row2 = row2[i];
				m_param->segRegArray[i].col2 = col2[i];
			}
			currentView()->update();

			m_module->setParamChanged(ProductLevel);
		}
	}
}

void arithmetic_preWidget::cb_toggled(bool checked)
{
	QCheckBox *cb = qobject_cast<QCheckBox *>(sender());

	QString logStr = cb->text() + (checked ? tr(" Check") : tr(" Unchecked"));
	if (ui->cb_enable == cb)
	{
		ui->gb_edit->setVisible(checked);
		ui->cb_outputIdx->setVisible(!checked);
		ui->lb_output->setVisible(!checked);

		m_param->enable = checked;
	}
	else if (ui->cb_allImgValid == cb)
	{
		ui->bt_select->setVisible(!checked);
		ui->lb_ROInum->setVisible(!checked);
		ui->sp_ROInum->setVisible(!checked);

		m_param->allImgValid = checked;
	}
	m_module->setParamChanged(ProductLevel);
}

void arithmetic_preWidget::cb_currentIndexChanged(int idx)
{
	QComboBox *cb = qobject_cast<QComboBox *>(sender());

	QString logStr;
	if (ui->cb_img1 == cb)
	{
		logStr = tr("%1 val changed from %2 to %3").arg(tr("first img")).arg(cb->itemText(m_param->idx1)).arg(cb->itemText(idx));
		m_param->idx1 = idx;
	}
	else if (ui->cb_img2 == cb)
	{
		logStr = tr("%1 val changed from %2 to %3").arg(tr("second img")).arg(cb->itemText(m_param->idx1)).arg(cb->itemText(idx));
		m_param->idx2 = idx;
	}
	else if (ui->cb_operator == cb)
	{
		logStr = tr("%1 val changed from %2 to %3").arg(tr("operator")).arg(cb->itemText(m_param->operatorIdx)).arg(cb->itemText(idx));
		m_param->operatorIdx = idx;		
		visible(idx);
	}
	else if (ui->cb_outputIdx == cb)
	{
		logStr = tr("%1 val changed from %2 to %3").arg(tr("output")).arg(cb->itemText(m_param->outputIdx)).arg(cb->itemText(idx));
		m_param->outputIdx = idx;
	}
	m_module->setParamChanged(ProductLevel);
}

void arithmetic_preWidget::dsp_valueChanged(double val)
{
	QDoubleSpinBox *dsp = qobject_cast<QDoubleSpinBox *>(sender());

	QString logStr;
	int idx = m_param->operatorIdx;
	if (ui->dsp_a == dsp)
	{
		logStr = tr("%1 val changed from %2 to %3").arg(ui->lb_a->text()).arg(m_param->pA[idx], 0, 'f', 3).arg(val, 0, 'f', 3);
		m_param->pA[idx] = val;
	}
	else if (ui->dsp_b == dsp)
	{
		logStr = tr("%1 val changed from %2 to %3").arg(ui->lb_b->text()).arg(m_param->pB[idx], 0, 'f', 3).arg(val, 0, 'f', 3);
		m_param->pB[idx] = val;
	}
	else if (ui->dsp_c == dsp)
	{
		logStr = tr("%1 val changed from %2 to %3").arg(ui->lb_c->text()).arg(m_param->pC[idx], 0, 'f', 3).arg(val, 0, 'f', 3);
		m_param->pC[idx] = val;
	}
	m_module->setParamChanged(ProductLevel);
}
		

void arithmetic_preWidget::sp_valueChanged(int val)
{
	QSpinBox *sp = qobject_cast<QSpinBox *>(sender());

	QString logStr;
	if (ui->sp_ROInum == sp)
	{
		logStr = tr("%1 val changed from %2 to %3").arg(ui->lb_ROInum->text()).arg(m_param->roiNum).arg(val);
		m_param->roiNum = val;

		//currentView()->clear();
		currentView()->clearLayer(m_layerIdx);
		currentView()->setColor(Qt::green);
		for (auto i = 0;i < m_param->roiNum;++i)
		{
			currentView()->dispRect1(
				m_param->segRegArray[i].row1, m_param->segRegArray[i].col1,
				m_param->segRegArray[i].row2, m_param->segRegArray[i].col2, tr("roi%1").arg(i + 1));
		}
		currentView()->update();
	}
	m_module->setParamChanged(ProductLevel);
}

