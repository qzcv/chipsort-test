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

	connectOrNot(link, ui->sp_allRegNum, SIGNAL(valueChanged(int)), SLOT(sp_valueChanged(int)));
	connectOrNot(link, ui->cb_baseImg, SIGNAL(currentIndexChanged(int)), SLOT(cb_currentIndexChanged(int)));

	for (auto i = 0;i < MAX_REGION;++i)
	{
		connectOrNot(link, bt_Sel[i], SIGNAL(clicked()), SLOT(bt_clicked()));
		connectOrNot(link, bt_Ok[i], SIGNAL(clicked()), SLOT(bt_clicked()));
		connectOrNot(link, bt_Cancle[i], SIGNAL(clicked()), SLOT(bt_clicked()));
		connectOrNot(link, sp_roiNum[i], SIGNAL(valueChanged(int)), SLOT(sp_valueChanged(int)));
		connectOrNot(link, cb_imgIdx[i], SIGNAL(currentIndexChanged(int)), SLOT(cb_currentIndexChanged(int)));
	}
}

void composeND_preWidget::iniUi()
{
	iniQtObject();
}

void composeND_preWidget::setUiValue()
{
	if(!m_param)
		return;

	for (auto i = 0;i < MAX_REGION;++i)
	{
		gb_block[i]->setVisible(m_param->allRegNum > i);
		bt_Ok[i]->setEnabled(!bt_Sel[i]->isEnabled());
		bt_Cancle[i]->setEnabled(!bt_Sel[i]->isEnabled());
		sp_roiNum[i]->setMaximum(ROI_MAX_NUM);
		sp_roiNum[i]->setValue(m_param->roiNum[i]);
		cb_imgIdx[i]->setCurrentIndex(m_param->imageIndex[i]);
	}
	ui->sp_allRegNum->setValue(m_param->allRegNum);
	ui->cb_baseImg->setCurrentIndex(m_param->baseImgIdx);
}

void composeND_preWidget::iniQtObject()
{
	for (int i = 0;i < MAX_REGION;i++)
	{
		cb_imgIdx[i] = findChild<QComboBox*>(QString("cb_image%1").arg(i));
		bt_Sel[i] = findChild<QPushButton*>(QString("bt_select%1").arg(i));
		bt_Ok[i] = findChild<QPushButton*>(QString("bt_ok%1").arg(i));
		bt_Cancle[i] = findChild<QPushButton*>(QString("bt_cancel%1").arg(i));
		sp_roiNum[i] = findChild<QSpinBox*>(QString("sp_roiNum_%1").arg(i));
		gb_block[i] = findChild<QGroupBox*>(QString("gb_block%1").arg(i));

		cb_imgIdx[i]->clear();
		for (auto j = 0;j < 4;++j)
			cb_imgIdx[i]->addItem(tr("img%1").arg(j + 1));
	}
	for (auto j = 0;j < 4;++j)
		ui->cb_baseImg->addItem(tr("img%1").arg(j + 1));
}

void composeND_preWidget::bt_clicked()
{
	QPushButton *bt = qobject_cast<QPushButton *>(sender());

	m_module->setParamChanged(ProductLevel);

	QString logStr = QString();
	for (auto i = 0;i < MAX_REGION;++i)
	{
		if (bt == bt_Sel[i])
		{
			logStr = tr("%1 select ROI").arg(gb_block[i]->title());

			bt_Sel[i]->setEnabled(false);
			bt_Ok[i]->setEnabled(true);
			bt_Cancle[i]->setEnabled(true);
			sp_roiNum[i]->setEnabled(false);

			QStringList textList;
			for (auto k = 0;k < m_param->roiNum[i];++k)
				textList << tr("ROI%1_%2").arg(i).arg(k + 1);

			//currentView()->clear();
			currentView()->clearLayer(m_layerIdx);
			currentView()->drawRects1(m_param->roiNum[i],
				m_param->regRow0[i], m_param->regCol0[i],
				m_param->regRow1[i], m_param->regCol1[i], textList);
			currentView()->update();
		}
		else if (bt == bt_Ok[i])
		{
			logStr = tr("%1 ROI ok").arg(gb_block[i]->title());

			bt_Sel[i]->setEnabled(true);
			bt_Ok[i]->setEnabled(false);
			bt_Cancle[i]->setEnabled(false);
			sp_roiNum[i]->setEnabled(true);

			currentView()->finishDraw();
			currentView()->getDrawRects1(m_param->roiNum[i],
				m_param->regRow0[i], m_param->regCol0[i],
				m_param->regRow1[i], m_param->regCol1[i]);
			checkROI(m_param->roiNum[i],
				m_param->regRow0[i], m_param->regCol0[i],
				m_param->regRow1[i], m_param->regCol1[i]);
			//currentView()->clear();
			currentView()->clearLayer(m_layerIdx);
			currentView()->setColor(Qt::green);
			for (auto k = 0;k < m_param->roiNum[i];++k)
			{
				currentView()->dispRect1(
					m_param->regRow0[i][k], m_param->regCol0[i][k],
					m_param->regRow1[i][k], m_param->regCol1[i][k], tr("ROI%1_%2").arg(i).arg(k + 1));
			}
			currentView()->update();
		}
		else if (bt == bt_Cancle[i])
		{
			logStr = tr("%1 ROI cancel").arg(gb_block[i]->title());

			bt_Sel[i]->setEnabled(true);
			bt_Ok[i]->setEnabled(false);
			bt_Cancle[i]->setEnabled(false);
			sp_roiNum[i]->setEnabled(true);

			//currentView()->clear();
			currentView()->clearLayer(m_layerIdx);
			currentView()->finishDraw();
			currentView()->update();
		}
	}
}

void composeND_preWidget::cb_currentIndexChanged(int idx)
{
	QComboBox *cb = qobject_cast<QComboBox *>(sender());

	m_module->setParamChanged(ProductLevel);

	QString logStr = QString();
	if (cb == ui->cb_baseImg)
	{
		logStr = tr("%1 val changed from %2 to %3").arg(ui->label_8->text())
			.arg(ui->cb_baseImg->itemText(m_param->baseImgIdx)).arg(ui->cb_baseImg->itemText(idx));
		m_param->baseImgIdx = idx;
	}
	else
	{
		for (auto i = 0;i < MAX_REGION;++i)
		{
			if (cb_imgIdx[i] == cb)
			{
				logStr = tr("%1 val changed from %2 to %3").arg(gb_block[i]->title()+tr(" img"))
					.arg(ui->cb_baseImg->itemText(m_param->baseImgIdx)).arg(ui->cb_baseImg->itemText(idx));
				m_param->imageIndex[i] = idx;
			}
		}
	}
}

void composeND_preWidget::sp_valueChanged(int val)
{
	QSpinBox *sp = qobject_cast<QSpinBox *>(sender());

	m_module->setParamChanged(ProductLevel);

	QString logStr = QString();
	if (ui->sp_allRegNum == sp)
	{
		for (auto i = 0;i < MAX_REGION;++i)
			gb_block[i]->setVisible(val > i);
		logStr = tr("%1 val changed from %2 to %3").arg(ui->label_4->text())
			.arg(m_param->allRegNum).arg(val);
		m_param->allRegNum = val;
	}
	else
	{
		for (auto i = 0;i < MAX_REGION;++i)
		{
			if (sp_roiNum[i] == sp)
			{
				logStr = tr("%1 val changed from %2 to %3").arg(gb_block[i]->title() + ui->label_7->text())
					.arg(m_param->roiNum[i]).arg(val);

				m_param->roiNum[i] = val;
			}
		}
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