#include "orientAreaWidget.h"
#include "orientAreaModule.h"
#include "orientAreaParam.h"
#include "ui_orientAreaWidget.h"
#include "ImageView.h"

orientAreaWidget::orientAreaWidget(int i) :
	ModSetWidget(),ui(new Ui::orientAreaWidget), m_widgetType(i)
{
	m_hasConnect = false;
	ui->setupUi(this);
	iniUi();
	m_layerIdx = 0;
	m_param = nullptr;
	m_module = nullptr;
}

orientAreaWidget::~orientAreaWidget()
{
	delete ui;
}

void orientAreaWidget::setModule(UnitModule *module)
{
	m_module = dynamic_cast<orientAreaModule*>(module);
	m_param = m_module->m_param;
	//currentView()->setLayer(m_layerIdx);
	//updateUi();
}

void orientAreaWidget::updateUi()
{
	connectSlots(false);
	setUiValue();
	connectSlots(true);
}

void orientAreaWidget::showEvent(QShowEvent * event)
{
	updateUi();
}

void orientAreaWidget::connectSlots(bool link)
{
	if (m_hasConnect == link)
		return;
	m_hasConnect = link;

	connectOrNot(link, ui->bt_selectROI, SIGNAL(toggled(bool)), SLOT(bt_toggled(bool)));
	connectOrNot(link, ui->bt_advance, SIGNAL(toggled(bool)), SLOT(bt_toggled(bool)));
	connectOrNot(link, ui->bt_test, SIGNAL(toggled(bool)), SLOT(bt_toggled(bool)));

	connectOrNot(link, ui->cb_open, SIGNAL(toggled(bool)), SLOT(cb_toggled(bool)));
	connectOrNot(link, ui->cb_close, SIGNAL(toggled(bool)), SLOT(cb_toggled(bool)));
	connectOrNot(link, ui->cb_useRefer, SIGNAL(toggled(bool)), SLOT(cb_toggled(bool)));
	connectOrNot(link, ui->cb_errCount, SIGNAL(toggled(bool)), SLOT(cb_toggled(bool)));
	connectOrNot(link, ui->cb_errArea, SIGNAL(toggled(bool)), SLOT(cb_toggled(bool)));
	connectOrNot(link, ui->cb_whiteGuy, SIGNAL(toggled(bool)), SLOT(cb_toggled(bool)));
	connectOrNot(link, ui->cb_fillUp, SIGNAL(toggled(bool)), SLOT(cb_toggled(bool)));
	connectOrNot(link, ui->cb_useDir, SIGNAL(toggled(bool)), SLOT(cb_toggled(bool)));
	connectOrNot(link, ui->cb_reverse, SIGNAL(toggled(bool)), SLOT(cb_toggled(bool)));
	connectOrNot(link, ui->cb_areaCompare, SIGNAL(toggled(bool)), SLOT(cb_toggled(bool)));
	connectOrNot(link, ui->cb_showRegion, SIGNAL(toggled(bool)), SLOT(cb_toggled(bool)));
	connectOrNot(link, ui->cb_findTarget, SIGNAL(toggled(bool)), SLOT(cb_toggled(bool)));
	connectOrNot(link, ui->cb_noneValid, SIGNAL(toggled(bool)), SLOT(cb_toggled(bool)));
	//connectOrNot(link, ui->cb_noneTest, SIGNAL(toggled(bool)), SLOT(cb_toggled(bool)));

	connectOrNot(link, ui->sp_areaMax, SIGNAL(valueChanged(int)), SLOT(sp_valueChanged(int)));
	connectOrNot(link, ui->sp_areaMin, SIGNAL(valueChanged(int)), SLOT(sp_valueChanged(int)));
	connectOrNot(link, ui->sp_widthMax, SIGNAL(valueChanged(int)), SLOT(sp_valueChanged(int)));
	connectOrNot(link, ui->sp_widthMin, SIGNAL(valueChanged(int)), SLOT(sp_valueChanged(int)));
	connectOrNot(link, ui->sp_heightMax, SIGNAL(valueChanged(int)), SLOT(sp_valueChanged(int)));
	connectOrNot(link, ui->sp_heightMin, SIGNAL(valueChanged(int)), SLOT(sp_valueChanged(int)));
	connectOrNot(link, ui->sp_grayMax, SIGNAL(valueChanged(int)), SLOT(sp_valueChanged(int)));
	connectOrNot(link, ui->sp_grayMin, SIGNAL(valueChanged(int)), SLOT(sp_valueChanged(int)));
	connectOrNot(link, ui->sp_open, SIGNAL(valueChanged(int)), SLOT(sp_valueChanged(int)));
	connectOrNot(link, ui->sp_close, SIGNAL(valueChanged(int)), SLOT(sp_valueChanged(int)));
	connectOrNot(link, ui->sp_errCountMax, SIGNAL(valueChanged(int)), SLOT(sp_valueChanged(int)));
	connectOrNot(link, ui->sp_errCountMin, SIGNAL(valueChanged(int)), SLOT(sp_valueChanged(int)));
	connectOrNot(link, ui->sp_errAreaMax, SIGNAL(valueChanged(int)), SLOT(sp_valueChanged(int)));
	connectOrNot(link, ui->sp_errAreaMin, SIGNAL(valueChanged(int)), SLOT(sp_valueChanged(int)));

 	//connectOrNot(link, ui->cb_pin1, SIGNAL(currentIndexChanged(int)), SLOT(cb_currentIndexChanged(int)));
 	//connectOrNot(link, ui->cb_pin2, SIGNAL(currentIndexChanged(int)), SLOT(cb_currentIndexChanged(int)));
	connectOrNot(link, ui->cob_areaCompare, SIGNAL(currentIndexChanged(int)), SLOT(cb_currentIndexChanged(int)));
	//connectOrNot(link, ui->cob_noneNG, SIGNAL(currentIndexChanged(int)), SLOT(cb_currentIndexChanged(int)));
}

void orientAreaWidget::iniUi()
{

}

void orientAreaWidget::setUiValue()
{
	if (!m_param)
		return;

	//QStringList lst;
	//for (auto i = 0;i < 32;++i)
	//	lst << QString("out%1").arg(i);
	//ui->cb_pin1->clear();
	//ui->cb_pin2->clear();
	//ui->cob_noneNG->clear();
	//ui->cb_pin1->addItems(lst);
	//ui->cb_pin2->addItems(lst);
	//ui->cob_noneNG->addItems(lst);

	ui->sp_open->setEnabled(m_param->openValid);
	ui->sp_close->setEnabled(m_param->closeValid);
	ui->sp_errAreaMin->setEnabled(m_param->errAreaValid);
	ui->sp_errAreaMax->setEnabled(m_param->errAreaValid);
	ui->sp_errCountMin->setEnabled(m_param->errRegionValid);
	ui->sp_errCountMax->setEnabled(m_param->errRegionValid);

	ui->sp_open->setValue(m_param->Opening);
	ui->sp_close->setValue(m_param->Closing);
	ui->sp_areaMin->setValue(m_param->areaMin);
	ui->sp_areaMax->setValue(m_param->areaMax);
	ui->sp_grayMin->setValue(m_param->grayMin);
	ui->sp_grayMax->setValue(m_param->grayMax);
	ui->sp_widthMin->setValue(m_param->widthMin);
	ui->sp_widthMax->setValue(m_param->widthMax);
	ui->sp_heightMin->setValue(m_param->heightMin);
	ui->sp_heightMax->setValue(m_param->heightMax);
	ui->sp_errAreaMin->setValue(m_param->errAreaMin);
	ui->sp_errAreaMax->setValue(m_param->errAreaMax);
	ui->sp_errCountMin->setValue(m_param->errRegionMin);
	ui->sp_errCountMax->setValue(m_param->errRegionMax);

	ui->cb_open->setChecked(m_param->openValid);
	ui->cb_close->setChecked(m_param->closeValid);
	ui->cb_fillUp->setChecked(m_param->IsFillUp);
	ui->cb_useDir->setChecked(m_param->usePinDir);
	ui->cb_reverse->setChecked(m_param->IsReverse);
	ui->cb_errArea->setChecked(m_param->errAreaValid);
	ui->cb_useRefer->setChecked(m_param->referSection);
	ui->cb_whiteGuy->setChecked(m_param->whiteGuy);
	ui->cb_errCount->setChecked(m_param->errRegionValid);
	ui->cb_showRegion->setChecked(m_param->IsShowRegion);
	ui->cb_findTarget->setChecked(m_param->IsSingleTarget);
	ui->cb_areaCompare->setChecked(m_param->errAreaCompareValid);
	ui->cb_noneValid->setChecked(m_param->noneNgValid);

 	//ui->cb_pin1->setVisible(m_param->usePinDir);
 	//ui->cb_pin2->setVisible(m_param->usePinDir);
	ui->cob_areaCompare->setEnabled(m_param->errAreaCompareValid);

 	//ui->cb_pin1->setCurrentIndex(m_param->pin1Idx);
 	//ui->cb_pin2->setCurrentIndex(m_param->pin2Idx);
	ui->cob_areaCompare->setCurrentIndex(m_param->errAreaCompareIndex);

	ui->cb_reverse->setVisible(m_param->noneNgValid);

	//ui->cob_noneNG->setCurrentIndex(m_param->noneNgIdx);
	//ui->cob_noneNG->setVisible(m_param->noneNgValid);
	//ui->cb_noneTest->setVisible(m_param->noneNgValid);
	//ui->cb_dirLfTest->setVisible(m_param->usePinDir);
	//ui->cb_dirRtTest->setVisible(m_param->usePinDir);

	if (ui->bt_advance->isChecked())
		ui->grp_advance->show();
	else
		ui->grp_advance->hide();
}

void orientAreaWidget::testParam()
{
	if (m_module->p_img->empty())
		return;

	RlRegion reg, lReg, rReg, dispRectReg;
	lReg.genRect(m_param->RoiCol1[0], m_param->RoiRow1[0], m_param->RoiCol2[0], m_param->RoiRow2[0]);
	rReg.genRect(m_param->RoiCol1[1], m_param->RoiRow1[1], m_param->RoiCol2[1], m_param->RoiRow2[1]);

	QString text;
	if (m_param->IsSingleTarget)
	{
		dispRectReg.unionReg(dispRectReg, lReg);
		m_module->selectShape(lReg);
		reg.unionReg(reg, m_module->m_reg[0]);

		RlRegions regs;
		reg.connect(regs);
		int num = regs.size();

		int area;
		float cx, cy;
		reg.areaCenter(area, cx, cy);
		text = tr("num=%1,area=%2").arg(num).arg(area);
	}
	else
	{
		dispRectReg.unionReg(dispRectReg, lReg);
		dispRectReg.unionReg(dispRectReg, rReg);
		m_module->selectShape(lReg, rReg);
		reg.unionReg(reg, m_module->m_reg[0]);
		reg.unionReg(reg, m_module->m_reg[1]);

		int num[2], area[2];
		for (auto i = 0;i < 2;++i)
		{
			RlRegions regs;
			m_module->m_reg[i].connect(regs);
			num[i] = regs.size();

			float cx, cy;
			m_module->m_reg[i].areaCenter(area[i], cx, cy);
		}
		text = tr("left: num=%1,area=%2 right:num=%3,area=%4").arg(num[0]).arg(area[0]).arg(num[1]).arg(area[1]);
	}

	//currentView()->clear();
	currentView()->clearLayer(m_layerIdx);
	currentView()->setFilled(true);
	currentView()->setColor(Qt::black);
	currentView()->dispRegion(&dispRectReg);
	currentView()->setColor(Qt::white);
	currentView()->dispRegion(&reg);
	currentView()->setColor(Qt::blue);
	currentView()->setTextSize(10);
	currentView()->dispText(100, 0, text);
	currentView()->update();	
}

void orientAreaWidget::bt_toggled(bool checked)
{
	QPushButton *bt = qobject_cast<QPushButton *>(sender());

	m_module->setParamChanged(ProductLevel);

	QString logStr;
	if (ui->bt_selectROI == bt)
	{
		ui->cb_findTarget->setEnabled(!checked);
		ui->bt_test->setEnabled(!checked);

		logStr = tr("clicked button: %1").arg(ui->bt_selectROI->text());

		if (checked)
		{

			ui->bt_selectROI->setText(tr("ROI OK"));

			//currentView()->clear();
			currentView()->clearLayer(m_layerIdx);
			currentView()->setColor(255, 0, 128);
			currentView()->setFilled(false);
			if (m_param->IsSingleTarget)
				currentView()->drawRect1(
					m_param->RoiRow1[0], m_param->RoiCol1[0],
					m_param->RoiRow2[0], m_param->RoiCol2[0]);
			else
			{
				QStringList titles = { QObject::tr("left"),QObject::tr("right") };
				currentView()->drawRects1(2,
					m_param->RoiRow1, m_param->RoiCol1,
					m_param->RoiRow2, m_param->RoiCol2, titles);
			}
			currentView()->update();
		}
		else
		{
			ui->bt_selectROI->setText(tr("Select ROI"));

			currentView()->setFilled(false);
			currentView()->finishDraw();
			//currentView()->clear();
			currentView()->clearLayer(m_layerIdx);
			currentView()->setColor(255, 255, 0);
			if (m_param->IsSingleTarget)
			{
				currentView()->getDrawRect1(
					m_param->RoiRow1[0], m_param->RoiCol1[0],
					m_param->RoiRow2[0], m_param->RoiCol2[0]);
				currentView()->dispRect1(
					m_param->RoiRow1[0], m_param->RoiCol1[0],
					m_param->RoiRow2[0], m_param->RoiCol2[0]);
			}
			else
			{
				currentView()->getDrawRects1(2,
					m_param->RoiRow1, m_param->RoiCol1,
					m_param->RoiRow2, m_param->RoiCol2);
				QStringList titles = { QObject::tr("left"),QObject::tr("right") };
				for (auto i = 0;i < 2;++i)
				{
					currentView()->dispRect1(
						m_param->RoiRow1[i], m_param->RoiCol1[i],
						m_param->RoiRow2[i], m_param->RoiCol2[i], titles[i]);
				}
			}
			currentView()->update();
		}
	}
	else if (ui->bt_test == bt)
	{
		ui->bt_selectROI->setEnabled(!checked);
		if (checked)
		{
			ui->bt_test->setText(tr("exit test"));
			testParam();
		}
		else
		{
			ui->bt_test->setText(tr("test mode"));
			//currentView()->clear();
			currentView()->clearLayer(m_layerIdx);
			currentView()->update();
		}
	}
	else if (ui->bt_advance == bt)
	{
		ui->grp_advance->setVisible(checked);
		if (checked)
			ui->bt_advance->setText(tr("exit advance"));
		else
			ui->bt_advance->setText(tr("advance"));
	}
}

void orientAreaWidget::cb_toggled(bool check)
{
	QCheckBox* cb = qobject_cast<QCheckBox*>(sender());

	m_module->setParamChanged(ProductLevel);

	QString logStr = QString("");
	logStr = tr("%1 %2").arg(cb->text()).arg(check ? "Check" : "Uncheck");

	if (ui->cb_whiteGuy == cb)
	{
		m_param->whiteGuy = check;

		if (ui->bt_test->isChecked())
			testParam();
	}
	else if (ui->cb_open == cb)
	{
		m_param->openValid = check;
		ui->sp_open->setEnabled(check);

		if (ui->bt_test->isChecked())
			testParam();
	}
	else if (ui->cb_close == cb)
	{
		m_param->closeValid = check;
		ui->sp_close->setEnabled(check);

		if (ui->bt_test->isChecked())
			testParam();
	}
	else if (ui->cb_fillUp == cb)
	{
		m_param->IsFillUp = check;

		if (ui->bt_test->isChecked())
			testParam();
	}
	else if (ui->cb_showRegion == cb)
	{
		m_param->IsShowRegion = check;
	}
	else if (ui->cb_errCount == cb)
	{
		m_param->errRegionValid = check;
		ui->sp_errCountMax->setEnabled(check);
		ui->sp_errCountMin->setEnabled(check);
	}
	else if (ui->cb_errArea == cb)
	{
		m_param->errAreaValid = check;
		ui->sp_errAreaMax->setEnabled(check);
		ui->sp_errAreaMin->setEnabled(check);
	}
	else if (ui->cb_useRefer == cb)
	{
		m_param->referSection = check;
	}
	else if (ui->cb_useDir == cb)
	{
		m_param->usePinDir = check;
		ui->cb_reverse->setVisible(check);

		//ui->cb_pin1->setVisible(check);
		//ui->cb_pin2->setVisible(check);
		//ui->cb_dirLfTest->setVisible(check);
		//ui->cb_dirRtTest->setVisible(check);

//		m_module->setDirPinVisible(check);
	}
	else if (ui->cb_reverse == cb)
	{
		m_param->IsReverse = check;
	}
	else if (ui->cb_areaCompare == cb)
	{
		m_param->errAreaCompareValid = check;
		ui->cob_areaCompare->setEnabled(check);
	}
	else if (ui->cb_findTarget == cb)
	{
		m_param->IsSingleTarget = check;
	}
	else if (ui->cb_noneValid == cb)
	{
		m_param->noneNgValid = check;
		//ui->cob_noneNG->setVisible(check);
		//ui->cb_noneTest->setVisible(check);
	}
// 	else if (ui->cb_noneTest == cb)
// 	{
// 		m_module->p_noneNGValid.setData(check);
// 		m_module->triggerEvent(ENT_WRITE_IO);
// 	}
}

void orientAreaWidget::sp_valueChanged(int val)
{
	QSpinBox* sp = qobject_cast<QSpinBox*>(sender());

	m_module->setParamChanged(ProductLevel);

	QString logStr = QString("");
	if (ui->sp_areaMin == sp)
	{
		logStr = tr("%1 change from %2 to %3").arg(ui->label_12->text()).arg(m_param->areaMin).arg(val);
		m_param->areaMin = val;
		ui->sp_areaMax->setMinimum(val);

		if (ui->bt_test->isChecked())
			testParam();
	}
	else if (ui->sp_areaMax == sp)
	{
		logStr = tr("%1 change from %2 to %3").arg(ui->label_13->text()).arg(m_param->areaMax).arg(val);
		m_param->areaMax = val;
		ui->sp_areaMin->setMaximum(val);

		if (ui->bt_test->isChecked())
			testParam();
	}
	else if (ui->sp_widthMin == sp)
	{
		logStr = tr("%1 change from %2 to %3").arg(ui->label_18->text()).arg(m_param->widthMin).arg(val);
		m_param->widthMin = val;
		ui->sp_widthMax->setMinimum(val);

		if (ui->bt_test->isChecked())
			testParam();
	}
	else if (ui->sp_widthMax == sp)
	{
		logStr = tr("%1 change from %2 to %3").arg(ui->label_18->text()).arg(m_param->widthMax).arg(val);
		m_param->widthMax = val;
		ui->sp_widthMin->setMaximum(val);

		if (ui->bt_test->isChecked())
			testParam();
	}
	else if (ui->sp_heightMin == sp)
	{
		logStr = tr("%1 change from %2 to %3").arg(ui->label_16->text()).arg(m_param->heightMin).arg(val);
		m_param->heightMin = val;
		ui->sp_heightMax->setMinimum(val);

		if (ui->bt_test->isChecked())
			testParam();
	}
	else if (ui->sp_heightMax == sp)
	{
		logStr = tr("%1 change from %2 to %3").arg(ui->label_17->text()).arg(m_param->heightMax).arg(val);
		m_param->heightMax = val;
		ui->sp_heightMin->setMaximum(val);

		if (ui->bt_test->isChecked())
			testParam();
	}
	else if (ui->sp_grayMin == sp)
	{
		logStr = tr("%1 change from %2 to %3").arg(ui->label_11->text()).arg(m_param->grayMin).arg(val);
		m_param->grayMin = val;
		ui->sp_grayMax->setMinimum(val);

		if (ui->bt_test->isChecked())
			testParam();
	}
	else if (ui->sp_grayMax == sp)
	{
		logStr = tr("%1 change from %2 to %3").arg(ui->label_14->text()).arg(m_param->grayMax).arg(val);
		m_param->grayMax = val;
		ui->sp_grayMin->setMaximum(val);

		if (ui->bt_test->isChecked())
			testParam();
	}
	else if (ui->sp_open == sp)
	{
		logStr = tr("%1 change from %2 to %3").arg(ui->cb_open->text()).arg(m_param->Opening).arg(val);
		m_param->Opening = val;

		if (ui->bt_test->isChecked())
			testParam();
	}
	else if (ui->sp_close == sp)
	{
		logStr = tr("%1 change from %2 to %3").arg(ui->cb_close->text()).arg(m_param->Closing).arg(val);
		m_param->Closing = val;

		if (ui->bt_test->isChecked())
			testParam();
	}
	else if (ui->sp_errCountMax == sp)
	{
		logStr = tr("%1 max val change from %2 to %3").arg(ui->cb_errCount->text()).arg(m_param->errRegionMax).arg(val);
		m_param->errRegionMax = val;

		ui->sp_errCountMin->setMaximum(val);
	}
	else if (ui->sp_errCountMin == sp)
	{
		logStr = tr("%1 min val change from %2 to %3").arg(ui->cb_errCount->text()).arg(m_param->errRegionMin).arg(val);
		m_param->errRegionMin = val;

		ui->sp_errAreaMax->setMinimum(val);
	}
	else if (ui->sp_errAreaMax == sp)
	{
		logStr = tr("%1 max val change from %2 to %3").arg(ui->cb_errArea->text()).arg(m_param->errAreaMax).arg(val);
		m_param->errAreaMax = val;

		ui->sp_errAreaMin->setMaximum(val);
	}
	else if (ui->sp_errAreaMin == sp)
	{
		logStr = tr("%1 min val change from %2 to %3").arg(ui->cb_errArea->text()).arg(m_param->errAreaMin).arg(val);
		m_param->errAreaMin = val;

		ui->sp_errAreaMax->setMinimum(val);
	}
}

void orientAreaWidget::cb_currentIndexChanged(int index)
{
	QComboBox* cob = qobject_cast<QComboBox*>(sender());

	m_module->setParamChanged(ProductLevel);

	QString logStr = QString("");
// 	if (ui->cb_pin1 == cob)
// 		{
// 			logStr = tr("pin1 from out%1 change to out%2").arg(m_param->pin1Idx).arg(index);
// 			m_param->pin1Idx = index;
// 	
// 			//m_module->p_lfIdx.setData(index);
// 		}
// 		else if (ui->cb_pin2 == cob)
// 		{
// 			logStr = tr("pin2 from out%1 change to out%2").arg(m_param->pin2Idx).arg(index);
// 			m_param->pin2Idx = index;
// 	
// 			//m_module->p_rtIdx.setData(index);
// 		}
		/*else */if (ui->cob_areaCompare == cob)
	{
		logStr = tr("%1 change from %2 to %3").arg(ui->cb_areaCompare->text())
			.arg(cob->itemText(m_param->errAreaCompareIndex)).arg(cob->itemText(index));
		m_param->errAreaCompareIndex = index;
	}
// 	else if (ui->cob_noneNG == cob)
// 	{
// 		logStr = tr("%1 change from %2 to %3").arg(ui->cb_noneValid->text())
// 			.arg(cob->itemText(m_param->noneNgIdx)).arg(cob->itemText(index));
// 		m_param->noneNgIdx = index;
// 		//m_module->p_noneNGIdx.setData(index);
// 	}
}
