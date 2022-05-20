#include "polarWdg.h"
#include "polarMod.h"
#include "polarParam.h"
#include "ui_polarWdg.h"
#include <QMessageBox>
#include "ImageView.h"
#include "imageViewItem.h"
#include "customView.h"

polarWdg::polarWdg(int i) :
	ModSetWidget(),ui(new Ui::polarWdg),m_widgetType(i)
{
	m_hasConnect = false;
	ui->setupUi(this);
	iniUi();
	m_layerIdx = 0;
	m_param = nullptr;
	m_module = nullptr;
	m_rectList = nullptr;
}

polarWdg::~polarWdg()
{
	delete ui;
}

void polarWdg::setModule(UnitModule *module)
{
	m_module=dynamic_cast<polarMod*>(module);
	m_param = m_module->m_param;
	//currentView()->setLayer(m_layerIdx);
	//updateUi();
}

void polarWdg::showEvent(QShowEvent * event)
{
	updateUi();
}

void polarWdg::connectSlots(bool link)
{
	if(m_hasConnect==link)
		return;
	m_hasConnect=link;

	connectOrNot(link, ui->bt_allUes0Param, SIGNAL(clicked()), SLOT(bt_clicked()));

	connectOrNot(link, ui->bt_select, SIGNAL(clicked()), SLOT(bt_lineSelect_clicked()));
	connectOrNot(link, ui->bt_ensure, SIGNAL(clicked()), SLOT(bt_lineSelect_clicked()));
	connectOrNot(link, ui->bt_cancel, SIGNAL(clicked()), SLOT(bt_lineSelect_clicked()));

	connectOrNot(link, ui->bt_selectAssistRect, SIGNAL(clicked()), SLOT(bt_assistRectSelect_clicked()));
	connectOrNot(link, ui->bt_cancelAssistRect, SIGNAL(clicked()), SLOT(bt_assistRectSelect_clicked()));
	connectOrNot(link, ui->bt_ensureAssistRect, SIGNAL(clicked()), SLOT(bt_assistRectSelect_clicked()));

	connectOrNot(link, ui->sp_minAng0, SIGNAL(valueChanged(int)), SLOT(sp_valueChanged(int)));
	connectOrNot(link, ui->sp_minAng1, SIGNAL(valueChanged(int)), SLOT(sp_valueChanged(int)));
	connectOrNot(link, ui->sp_maxAng0, SIGNAL(valueChanged(int)), SLOT(sp_valueChanged(int)));
	connectOrNot(link, ui->sp_maxAng1, SIGNAL(valueChanged(int)), SLOT(sp_valueChanged(int)));
	connectOrNot(link, ui->sp_step, SIGNAL(valueChanged(int)), SLOT(sp_valueChanged(int)));
	connectOrNot(link, ui->sp_rmDist, SIGNAL(valueChanged(int)), SLOT(sp_valueChanged(int)));
	connectOrNot(link, ui->sp_gradient, SIGNAL(valueChanged(int)), SLOT(sp_valueChanged(int)));
	connectOrNot(link, ui->sp_degRange, SIGNAL(valueChanged(int)), SLOT(sp_valueChanged(int)));
	connectOrNot(link, ui->sp_iteratNum, SIGNAL(valueChanged(int)), SLOT(sp_valueChanged(int)));

	connectOrNot(link, ui->cb_pol, SIGNAL(currentIndexChanged(int)), SLOT(cb_currentIndexChanged(int)));
	connectOrNot(link, ui->cb_first, SIGNAL(currentIndexChanged(int)), SLOT(cb_currentIndexChanged(int)));
	connectOrNot(link, ui->cb_lineIndex, SIGNAL(currentIndexChanged(int)), SLOT(cb_currentIndexChanged(int)));

	connectOrNot(link, ui->rb_left_0, SIGNAL(clicked()), SLOT(rb_clicked()));
	connectOrNot(link, ui->rb_right_0, SIGNAL(clicked()), SLOT(rb_clicked()));

	connectOrNot(link, ui->bt_moreParam, SIGNAL(toggled(bool)), SLOT(bt_toggled(bool)));

	connectOrNot(link, ui->cb_defaultSig, SIGNAL(toggled(bool)), SLOT(cb_toggled(bool)));
	connectOrNot(link, ui->cb_matrixRefer, SIGNAL(toggled(bool)), SLOT(cb_toggled(bool)));
	connectOrNot(link, ui->cb_dispAfterDet, SIGNAL(toggled(bool)), SLOT(cb_toggled(bool)));
	connectOrNot(link, ui->cb_range0Enable, SIGNAL(toggled(bool)), SLOT(cb_toggled(bool)));
	connectOrNot(link, ui->cb_range1Enable, SIGNAL(toggled(bool)), SLOT(cb_toggled(bool)));
}

void polarWdg::iniUi()
{

}

void polarWdg::setUiValue()
{
	if(!m_param)
		return;

	ui->bt_ensure->setEnabled(!ui->bt_select->isEnabled());
	ui->bt_cancel->setEnabled(!ui->bt_select->isEnabled());
	ui->bt_cancelAssistRect->setEnabled(!ui->bt_selectAssistRect->isEnabled());
	ui->bt_ensureAssistRect->setEnabled(!ui->bt_selectAssistRect->isEnabled());

	ui->sp_minAng0->setEnabled(m_param->isRangeEnable[0]);
	ui->sp_maxAng0->setEnabled(m_param->isRangeEnable[0]);
	ui->sp_minAng1->setEnabled(m_param->isRangeEnable[1]);
	ui->sp_maxAng1->setEnabled(m_param->isRangeEnable[1]);

	ui->sp_minAng0->setValue(m_param->minAng[0]);
	ui->sp_minAng1->setValue(m_param->minAng[1]);
	ui->sp_maxAng0->setValue(m_param->maxAng[0]);
	ui->sp_maxAng1->setValue(m_param->maxAng[1]);
	ui->sp_step->setValue(m_param->step[m_param->lineIndex]);
	ui->sp_rmDist->setValue(m_param->rmDist[m_param->lineIndex]);
	ui->sp_gradient->setValue(m_param->gradient[m_param->lineIndex]);
	ui->sp_degRange->setValue(m_param->degRange[m_param->lineIndex]);
	ui->sp_iteratNum->setValue(m_param->iteratNum[m_param->lineIndex]);

	ui->cb_pol->setCurrentIndex(m_param->pol[m_param->lineIndex]);
	ui->cb_first->setCurrentIndex(m_param->isFirst[m_param->lineIndex]);
	ui->cb_lineIndex->setCurrentIndex(m_param->lineIndex);

	ui->rb_left_0->setChecked(m_param->isLeft0);
	ui->rb_right_0->setChecked(!m_param->isLeft0);

	setParamVisible(false);

	ui->cb_defaultSig->setChecked(m_param->isDefaultSig);
	ui->cb_matrixRefer->setChecked(m_param->isMatrixRefer);
	ui->cb_dispAfterDet->setChecked(m_param->isDispAfterDet);
	ui->cb_range0Enable->setChecked(m_param->isRangeEnable[0]);
	ui->cb_range1Enable->setChecked(m_param->isRangeEnable[1]);
}

void polarWdg::updateUi()
{
	connectSlots(false);
	setUiValue();
	connectSlots(true);
}

void polarWdg::drawRects2(int num, double * row, double * col, double * angle, double * len1, double * len2, const QStringList & titles, int arrowSize)
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

void polarWdg::setParamVisible(bool isVisible)
{
	ui->sp_step->setVisible(isVisible);
	ui->lb_step->setVisible(isVisible);

	ui->sp_iteratNum->setVisible(isVisible);
	ui->lb_iteratNum->setVisible(isVisible);

	ui->sp_rmDist->setVisible(isVisible);
	ui->lb_rmDist->setVisible(isVisible);
}

void polarWdg::updateLineParam(int index)
{
	ui->sp_step->setValue(m_param->step[index]);
	ui->sp_rmDist->setValue(m_param->rmDist[index]);
	ui->sp_degRange->setValue(m_param->degRange[index]);
	ui->sp_gradient->setValue(m_param->gradient[index]);
	ui->sp_iteratNum->setValue(m_param->iteratNum[index]);

	ui->cb_pol->setCurrentIndex(m_param->pol[index]);
	ui->cb_first->setCurrentIndex(m_param->isFirst[index]);
}

void polarWdg::bt_toggled(bool checked)
{
	setParamVisible(checked);
}

void polarWdg::bt_clicked()
{
	QPushButton *bt = qobject_cast<QPushButton *>(sender());

	if (ui->bt_allUes0Param == bt)
	{
		int r = QMessageBox::information(this, tr("information"),
			tr("Whether to apply the base line parameters to all regions?"),
			QMessageBox::Yes | QMessageBox::No);

		if (r == QMessageBox::Yes)
		{
			for (size_t i = 1;i < 4;++i)
			{
				m_param->pol[i] = m_param->pol[0];
				m_param->step[i] = m_param->step[0];
				m_param->rmDist[i] = m_param->rmDist[0];
				m_param->isFirst[i] = m_param->isFirst[0];
				m_param->degRange[i] = m_param->degRange[0];
				m_param->gradient[i] = m_param->gradient[0];
				m_param->iteratNum[i] = m_param->iteratNum[0];
			}
			m_module->setParamChanged(ProductLevel);
		}
	}
}

void polarWdg::bt_lineSelect_clicked()
{
	QPushButton *bt = qobject_cast<QPushButton *>(sender());

	ui->bt_select->setEnabled(bt != ui->bt_select);
	ui->bt_ensure->setEnabled(bt == ui->bt_select);
	ui->bt_cancel->setEnabled(bt == ui->bt_select);

	if (bt == ui->bt_select)
	{
		//currentView()->clear();
		currentView()->clearLayer(m_layerIdx);
		currentView()->setColor(255, 0, 0);
		currentView()->setTextSize(5);
		drawRects2(4, m_param->rectRow, m_param->rectCol, m_param->rectAng,
			m_param->rectLe1, m_param->rectLe2, m_title, 5);
	}
	else if (bt == ui->bt_ensure)
	{
		ui->bt_selectAssistRect->setEnabled(1);
		ui->bt_ensureAssistRect->setEnabled(0);
		ui->bt_cancelAssistRect->setEnabled(0);

		currentView()->getDrawRects2(4, m_param->rectRow, m_param->rectCol, m_param->rectAng,
			m_param->rectLe1, m_param->rectLe2);
		currentView()->finishDraw();
		//currentView()->clear();
		currentView()->clearLayer(m_layerIdx);
		currentView()->setColor(0, 255, 0);

		for (size_t i = 0;i < 4;++i)
		{
			currentView()->dispRect2(m_param->rectRow[i], m_param->rectCol[i], m_param->rectAng[i],
				m_param->rectLe1[i], m_param->rectLe2[i], m_title[i]);
		}
		m_module->setParamChanged(ProductLevel);
	}
	else if (bt == ui->bt_cancel)
	{
		ui->bt_selectAssistRect->setEnabled(1);
		ui->bt_ensureAssistRect->setEnabled(0);
		ui->bt_cancelAssistRect->setEnabled(0);

		currentView()->finishDraw();
		//currentView()->clear();
		currentView()->clearLayer(m_layerIdx);
	}
	currentView()->update();
}

void polarWdg::bt_assistRectSelect_clicked()
{
	QPushButton *bt = qobject_cast<QPushButton *>(sender());

	ui->bt_selectAssistRect->setEnabled(bt != ui->bt_selectAssistRect);
	ui->bt_cancelAssistRect->setEnabled(bt == ui->bt_selectAssistRect);
	ui->bt_ensureAssistRect->setEnabled(bt == ui->bt_selectAssistRect);

	//currentView()->clear();
	currentView()->clearLayer(m_layerIdx);
	if (bt == ui->bt_selectAssistRect)
	{
		QStringList title = { tr("left"),tr("right") };
		currentView()->drawRects1(2, 
			m_param->assistRectR1, m_param->assistRectC1, m_param->assistRectR2, m_param->assistRectC2, title);
	}
	else if (bt == ui->bt_ensureAssistRect)
	{
		currentView()->getDrawRects1(2, 
			m_param->assistRectR1, m_param->assistRectC1, m_param->assistRectR2, m_param->assistRectC2);
		currentView()->finishDraw();
		currentView()->setColor(255, 0, 0);
		currentView()->setTextSize(5);

		double row[4], col[4];
		double le1[4], le2[4];
		double ang[4];
		for (size_t i = 0;i < 2;++i)
		{
			//左一，右一直线
			row[2 * i] = m_param->assistRectR1[i] + (m_param->assistRectR2[i] - m_param->assistRectR1[i]) / 4;
			col[2 * i] = m_param->assistRectC2[i] + (m_param->assistRectC1[i] - m_param->assistRectC2[i]) / 2;
			le1[2 * i] = (m_param->assistRectR2[i] - m_param->assistRectR1[i]) / 4;
			le2[2 * i] = (m_param->assistRectC2[i] - m_param->assistRectC1[i]) / 2;
			ang[2 * i] = -CV_PI / 2;

			row[2 * i + 1] = m_param->assistRectR1[i] + (m_param->assistRectR2[i] - m_param->assistRectR1[i]) / 2;
			col[2 * i + 1] = m_param->assistRectC2[i] + (m_param->assistRectC1[i] - m_param->assistRectC2[i]) / 2;
			le1[2 * i + 1] = (m_param->assistRectR2[i] - m_param->assistRectR1[i]) / 4;
			le2[2 * i + 1] = (m_param->assistRectC2[i] - m_param->assistRectC1[i]) / 2;

			ang[2 * i + 1] = 3 * CV_PI / 4 - CV_PI*i / 2;
		}
		drawRects2(4, row, col, ang, le1, le2, m_title, 5);

		ui->bt_select->setEnabled(false);
		ui->bt_ensure->setEnabled(true);
		ui->bt_cancel->setEnabled(true);
	}
	else if (bt == ui->bt_cancelAssistRect)
	{
		currentView()->finishDraw();
		//currentView()->clear();
		currentView()->clearLayer(m_layerIdx);
	}
	currentView()->update();
}

void polarWdg::sp_valueChanged(int value)
{
	QSpinBox *sp = qobject_cast<QSpinBox *>(sender());
	if (sp == ui->sp_step)
		m_param->step[m_param->lineIndex] = value;
	else if (sp == ui->sp_rmDist)
		m_param->rmDist[m_param->lineIndex] = value;
	else if (sp == ui->sp_gradient)
		m_param->gradient[m_param->lineIndex] = value;
	else if (sp == ui->sp_degRange)
		m_param->degRange[m_param->lineIndex] = value;
	else if (sp == ui->sp_iteratNum)
		m_param->iteratNum[m_param->lineIndex] = value;
	else if (sp == ui->sp_minAng0 || sp == ui->sp_minAng1)
		m_param->minAng[sp == ui->sp_minAng1] = value;
	else if (sp == ui->sp_maxAng0 || sp == ui->sp_maxAng1)
		m_param->maxAng[sp == ui->sp_maxAng1] = value;
	m_module->setParamChanged(ProductLevel);
}

void polarWdg::cb_currentIndexChanged(int index)
{
	QComboBox *cb = qobject_cast<QComboBox *>(sender());
	if (cb == ui->cb_pol)
		m_param->pol[m_param->lineIndex] = index;
	else if (cb == ui->cb_first)
		m_param->isFirst[m_param->lineIndex] = index;
	else if (cb == ui->cb_lineIndex)
	{
		m_param->lineIndex = index;

		updateLineParam(index);
	}
	m_module->setParamChanged(ProductLevel);
}

void polarWdg::cb_toggled(bool checked)
{
	QCheckBox *cb = qobject_cast<QCheckBox *>(sender());

	if (cb == ui->cb_matrixRefer)
		m_param->isMatrixRefer = checked;
	else if (cb == ui->cb_dispAfterDet)
		m_param->isDispAfterDet = checked;
	else if (cb == ui->cb_defaultSig)
		m_param->isDefaultSig = checked;
	else if (cb == ui->cb_range0Enable)
	{
		m_param->isRangeEnable[0] = checked;

		ui->sp_minAng0->setEnabled(checked);
		ui->sp_maxAng0->setEnabled(checked);
	}
	else if (cb == ui->cb_range1Enable)
	{
		m_param->isRangeEnable[1] = checked;

		ui->sp_minAng1->setEnabled(checked);
		ui->sp_maxAng1->setEnabled(checked);
	}
	m_module->setParamChanged(ProductLevel);
}

void polarWdg::rb_clicked()
{
	QRadioButton *rb = qobject_cast<QRadioButton *>(sender());

	if (rb == ui->rb_left_0 || rb == ui->rb_right_0)
		m_param->isLeft0 = (rb == ui->rb_left_0);
	m_module->setParamChanged(ProductLevel);
}

