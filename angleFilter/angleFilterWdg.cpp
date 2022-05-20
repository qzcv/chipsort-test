#include "angleFilterWdg.h"
#include "angleFilterMod.h"
#include "angleFilterParam.h"
#include "ui_angleFilterWdg.h"
#include <QMessageBox>
#include "ImageView.h"
#include "imageViewItem.h"
#include "customView.h"

angleFilterWdg::angleFilterWdg(int i) :
	ModSetWidget(),ui(new Ui::angleFilterWdg),m_widgetType(i)
{
	m_hasConnect = false;
	ui->setupUi(this);
	iniUi();
	m_layerIdx = 0;
	m_param = nullptr;
	m_module = nullptr;
	m_rectList = nullptr;
}

angleFilterWdg::~angleFilterWdg()
{
	delete ui;
}

void angleFilterWdg::setModule(UnitModule *module)
{
	m_module=dynamic_cast<angleFilterMod*>(module);
	m_param = m_module->m_param;
	//currentView()->setLayer(m_layerIdx);
	//updateUi();
}

void angleFilterWdg::showEvent(QShowEvent * event)
{
	updateUi();
}

void angleFilterWdg::connectSlots(bool link)
{
	if(m_hasConnect==link)
		return;
	m_hasConnect=link;

	connectOrNot(link, ui->bt_select, SIGNAL(clicked()), SLOT(bt_lineSelect_clicked()));
	connectOrNot(link, ui->bt_cancel, SIGNAL(clicked()), SLOT(bt_lineSelect_clicked()));
	connectOrNot(link, ui->bt_ensure, SIGNAL(clicked()), SLOT(bt_lineSelect_clicked()));

	connectOrNot(link, ui->bt_advance, SIGNAL(toggled(bool)), SLOT(bt_toggled(bool)));

	connectOrNot(link, ui->bt_allUes0Param, SIGNAL(clicked()), SLOT(bt_clicked()));

	connectOrNot(link, ui->sp_step, SIGNAL(valueChanged(int)), SLOT(sp_valueChanged(int)));
	connectOrNot(link, ui->sp_rmDist, SIGNAL(valueChanged(int)), SLOT(sp_valueChanged(int)));
	connectOrNot(link, ui->sp_gradient, SIGNAL(valueChanged(int)), SLOT(sp_valueChanged(int)));
	connectOrNot(link, ui->sp_iteratNum, SIGNAL(valueChanged(int)), SLOT(sp_valueChanged(int)));
	connectOrNot(link, ui->sp_degRange, SIGNAL(valueChanged(int)), SLOT(sp_valueChanged(int)));
	for (size_t i = 0;i < 2;++i)
	{
		connectOrNot(link, sp_maxAng[i], SIGNAL(valueChanged(int)), SLOT(sp_valueChanged(int)));
		connectOrNot(link, sp_minAng[i], SIGNAL(valueChanged(int)), SLOT(sp_valueChanged(int)));
	}

	connectOrNot(link, ui->cb_dispAfterDet, SIGNAL(toggled(bool)), SLOT(cb_toggled(bool)));
	connectOrNot(link, ui->cb_matrixRefer, SIGNAL(toggled(bool)), SLOT(cb_toggled(bool)));

	connectOrNot(link, ui->cb_pol, SIGNAL(currentIndexChanged(int)), SLOT(cb_currentIndexChanged(int)));
	connectOrNot(link, ui->cb_first, SIGNAL(currentIndexChanged(int)), SLOT(cb_currentIndexChanged(int)));
	connectOrNot(link, ui->cb_lineIndex, SIGNAL(currentIndexChanged(int)), SLOT(cb_currentIndexChanged(int)));
}

void angleFilterWdg::iniUi()
{
	for (size_t i = 0;i < 2;++i)
	{
		sp_maxAng[i] = findChild<QSpinBox *>(QString("sp_maxAng%1").arg(i));
		sp_minAng[i] = findChild<QSpinBox *>(QString("sp_minAng%1").arg(i));
	}
}

void angleFilterWdg::setUiValue()
{
	if(!m_param)
		return;

	ui->bt_cancel->setEnabled(!ui->bt_select->isEnabled());
	ui->bt_ensure->setEnabled(!ui->bt_select->isEnabled());

	int idx = ui->cb_lineIndex->currentIndex();
	ui->sp_step->setValue(m_param->step[idx]);
	ui->sp_rmDist->setValue(m_param->rmDist[idx]);
	ui->sp_gradient->setValue(m_param->gradient[idx]);
	ui->sp_degRange->setValue(m_param->degRange[idx]);
	ui->sp_iteratNum->setValue(m_param->iteratNum[idx]);

	ui->bt_advance->setChecked(false);
	setParamVisible(false);

	for (size_t i = 0;i < 2;++i)
	{
		sp_maxAng[i]->setValue(m_param->maxAng[i]);
		sp_minAng[i]->setValue(m_param->minAng[i]);
	}

	ui->cb_dispAfterDet->setChecked(m_param->isDispAfterDet);
	ui->cb_matrixRefer->setChecked(m_param->isMatrixRefer);

	ui->cb_first->setCurrentIndex(m_param->isFirst[idx]);
	ui->cb_pol->setCurrentIndex(m_param->pol[idx]);
}

void angleFilterWdg::updateUi()
{
	connectSlots(false);
	setUiValue();
	connectSlots(true);
}

void angleFilterWdg::setParamVisible(bool isVisible)
{
	ui->sp_step->setVisible(isVisible);
	ui->lb_step->setVisible(isVisible);

	ui->sp_iteratNum->setVisible(isVisible);
	ui->lb_iteratNum->setVisible(isVisible);

	ui->sp_rmDist->setVisible(isVisible);
	ui->lb_rmDist->setVisible(isVisible);

	ui->cb_first->setVisible(isVisible);
	ui->lb_first->setVisible(isVisible);

	ui->cb_pol->setVisible(isVisible);
	ui->lb_pol->setVisible(isVisible);
}

void angleFilterWdg::updateLineParam(int idx)
{
	connectSlots(false);
	ui->sp_step->setValue(m_param->step[idx]);
	ui->sp_rmDist->setValue(m_param->rmDist[idx]);
	ui->sp_degRange->setValue(m_param->degRange[idx]);
	ui->sp_gradient->setValue(m_param->gradient[idx]);
	ui->sp_iteratNum->setValue(m_param->iteratNum[idx]);

	ui->cb_first->setCurrentIndex(m_param->isFirst[idx]);
	ui->cb_pol->setCurrentIndex(m_param->pol[idx]);
	connectSlots(true);
}

void angleFilterWdg::drawRects2(int num, double * row, double * col, double * angle, double * len1, double * len2, const QStringList & titles, int arrowSize)
{
	QList<ImageViewRotRectList::RotRectListItem> list;
	for (auto i = 0;i < num;++i)
	{
		ImageViewRotRectList::RotRectListItem item;
		item.center = QPointF(col[i], row[i]);
		item.angle = angle[i];
		item.len[0] = len1[i];
		item.len[1] = len2[i];
		item.text = titles[i];

		list.append(item);
	}

	if (m_rectList)
		delete m_rectList;
	m_rectList = new cusViewRotRectList(list, arrowSize);
	currentView()->drawItem(m_rectList, true);
}

void angleFilterWdg::bt_clicked()
{
	if (QMessageBox::warning(this, tr("warning"), tr("Whether to apply the base line parameters to all regions?"),
		QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
	{
		QString logStr = tr("Appling the base line parameters to all regions");
		for (size_t i = 1;i < 3;++i)
		{
			m_param->pol[i] = m_param->pol[0];
			m_param->isFirst[i] = m_param->isFirst[0];

			m_param->step[i] = m_param->step[0];
			m_param->rmDist[i] = m_param->rmDist[0];
			m_param->degRange[i] = m_param->degRange[0];
			m_param->gradient[i] = m_param->gradient[0];
			m_param->iteratNum[i] = m_param->iteratNum[0];
		}

		int idx = ui->cb_lineIndex->currentIndex();
		updateLineParam(idx);
		m_module->setParamChanged(ProductLevel);
		//log(LogL1, logStr);
	}
}

void angleFilterWdg::bt_toggled(bool checked)
{
	setParamVisible(checked);
}

void angleFilterWdg::bt_lineSelect_clicked()
{
	QPushButton *bt = qobject_cast<QPushButton *>(sender());

	ui->bt_select->setEnabled(bt != ui->bt_select);
	ui->bt_ensure->setEnabled(bt == ui->bt_select);
	ui->bt_cancel->setEnabled(bt == ui->bt_select);

	if (bt == ui->bt_select)
	{
		//currentView()->clear();
		currentView()->clearLayer(m_layerIdx);
		currentView()->setTextSize(10);
		QStringList title = { tr("base"),tr("left"),tr("right") };
		drawRects2(3, m_param->crow, m_param->ccol, m_param->ang, m_param->len1, m_param->len2, title, 5);
		currentView()->update();
	}
	else if (bt == ui->bt_ensure)
	{
		currentView()->getDrawRects2(3, m_param->crow, m_param->ccol, m_param->ang, m_param->len1, m_param->len2);
		currentView()->finishDraw();
		//currentView()->clear();
		currentView()->clearLayer(m_layerIdx);
		currentView()->setColor(0, 255, 0);
		QStringList title = { tr("base"),tr("left"),tr("right") };
		for (size_t i = 0;i < 3;++i)
			currentView()->dispRect2(m_param->crow[i], m_param->ccol[i], m_param->ang[i], m_param->len1[i], m_param->len2[i], title[i]);
		currentView()->update();
		m_module->setParamChanged(ProductLevel);
		//log(LogL1, ui.bt_ensure->text());
	}
	else if (bt == ui->bt_cancel)
	{
		currentView()->finishDraw();
		//currentView()->clear();
		currentView()->clearLayer(m_layerIdx);
		currentView()->update();
	}
}

void angleFilterWdg::cb_toggled(bool checked)
{
	QCheckBox *cb = qobject_cast<QCheckBox *>(sender());
	QString logStr = cb->text() + (checked ? tr(" Checked") : tr(" Unchecked"));
	if (cb == ui->cb_matrixRefer)
	{
		m_param->isMatrixRefer = checked;
	}
	else if (cb == ui->cb_dispAfterDet)
	{
		m_param->isDispAfterDet = checked;
	}
	m_module->setParamChanged(ProductLevel);
	//log(LogL1, logStr);
}

void angleFilterWdg::sp_valueChanged(int val)
{
	QSpinBox *sp = qobject_cast<QSpinBox *>(sender());
	QString logStr;
	QString logStr1 = QString("");
	int idx = ui->cb_lineIndex->currentIndex();
	if (sp == ui->sp_step)
	{
		logStr1 = ui->cb_lineIndex->currentText();
		logStr = tr(" %1 val changed from %2 to %3").arg(ui->lb_step->text()).arg(m_param->step[idx]).arg(val);
		m_param->step[idx] = val;
	}
	else if (sp == ui->sp_rmDist)
	{
		logStr1 = ui->cb_lineIndex->currentText();
		logStr = tr(" %1 val changed from %2 to %3").arg(ui->lb_rmDist->text()).arg(m_param->rmDist[idx]).arg(val);
		m_param->rmDist[idx] = val;
	}
	else if (sp == ui->sp_gradient)
	{
		logStr1 = ui->cb_lineIndex->currentText();
		logStr = tr(" %1 val changed from %2 to %3").arg(ui->label->text()).arg(m_param->gradient[idx]).arg(val);
		m_param->gradient[idx] = val;
	}
	else if (sp == ui->sp_degRange)
	{
		logStr1 = ui->cb_lineIndex->currentText();
		logStr = tr(" %1 val changed from %2 to %3").arg(ui->label_4->text()).arg(m_param->degRange[idx]).arg(val);
		m_param->degRange[idx] = val;
	}
	else if (sp == ui->sp_iteratNum)
	{
		logStr1 = ui->cb_lineIndex->currentText();
		logStr = tr(" %1 val changed from %2 to %3").arg(ui->lb_iteratNum->text()).arg(m_param->iteratNum[idx]).arg(val);
		m_param->iteratNum[idx] = val;
	}
	else
	{
		for (size_t i = 0;i < 2;++i)
		{
			if (sp == sp_maxAng[i])
			{
				logStr = tr("%1 max val changed from %2 to %3")
					.arg(i == 0 ? ui->label_2->text() : ui->label_3->text())
					.arg(m_param->maxAng[i]).arg(val);
				m_param->maxAng[i] = val;
			}
			else if (sp == sp_minAng[i])
			{
				logStr = tr("%1 min val changed from %2 to %3")
					.arg(i == 0 ? ui->label_2->text() : ui->label_3->text())
					.arg(m_param->minAng[i]).arg(val);
				m_param->minAng[i] = val;
			}
		}
	}
	m_module->setParamChanged(ProductLevel);
	//log(LogL1, logStr1 + logStr);
}

void angleFilterWdg::cb_currentIndexChanged(int idx)
{
	QComboBox *cb = qobject_cast<QComboBox *>(sender());
	QString logStr = QString("");
	QString logStr1 = QString("");
	if (cb == ui->cb_first)
	{
		logStr1 = ui->cb_lineIndex->currentText();
		logStr = tr(" %1 val changed from %2 to %3").arg(ui->lb_first->text()).arg(m_param->isFirst[idx]).arg(idx);
		int lidx = ui->cb_lineIndex->currentIndex();
		m_param->isFirst[lidx] = idx;
	}
	else if (cb == ui->cb_lineIndex)
	{
		updateLineParam(idx);
	}
	else if (cb == ui->cb_pol)
	{
		logStr1 = ui->cb_lineIndex->currentText();
		logStr = tr(" %1 val changed from %2 to %3").arg(ui->lb_pol->text()).arg(m_param->pol[idx]).arg(idx);
		int lidx = ui->cb_lineIndex->currentIndex();
		m_param->pol[lidx] = idx;
	}
	m_module->setParamChanged(ProductLevel);
	//log(LogL1, logStr1 + logStr);
}

