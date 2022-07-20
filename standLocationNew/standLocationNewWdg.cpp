#include "standLocationNewWdg.h"
#include "standLocationNewMod.h"
#include "standLocationNewParam.h"
#include "ui_standLocationNewWdg.h"
#include "hom3d5s.h"
#include "paramTables.h"
#include "ImageView.h"
#include "RlRegion.h"
#include "halOpenCV.h"

standLocationNewWdg::standLocationNewWdg(int i) :
	ModSetWidget(),ui(new Ui::standLocationNewWdg),m_widgetType(i)
{
	m_hasConnect = false;
	m_layerIdx = 0;
	m_image = nullptr;
	ui->setupUi(this);
	iniUi();
	m_param = nullptr;
	m_module = nullptr;
}

standLocationNewWdg::~standLocationNewWdg()
{
	delete ui;
	delete m_dialog;
	delete m_corrDialog;
	delete m_offTable;
	delete m_corrTable;
	delete m_paramTables;
}

void standLocationNewWdg::setModule(UnitModule *module)
{
	m_module=dynamic_cast<standLocationNewMod*>(module);
	m_param = m_module->m_param;
	m_image = m_module->m_image;
	connectSlots(false);
	setUiValue();
	connectSlots(true);
}

void standLocationNewWdg::showEvent(QShowEvent * event)
{
	connectSlots(false);
	setUiValue();
	connectSlots(true);
}

void standLocationNewWdg::radioButton_toggled(bool check)
{
	QRadioButton* rb = qobject_cast<QRadioButton*>(sender());
	QString str = QString("");
	if (check) {
		if (rb == ui->rb_upDw) {
			str = tr(" change from %1 to %2").arg(ui->rb_lfRt->text()).arg(ui->rb_upDw->text());
			m_param->upDwOrLfRt = 0;
		}
		else if (rb == ui->rb_lfRt) {
			str = tr(" change from %1 to %2").arg(ui->rb_upDw->text()).arg(ui->rb_lfRt->text());
			m_param->upDwOrLfRt = 1;
		}
		//og(LogL1, str);
		m_module->setParamChanged(ProductLevel);
	}
}

void standLocationNewWdg::checkBox_toggled(bool check)
{
	QCheckBox* cb = qobject_cast<QCheckBox*>(sender());
	QString str0 = QString("");
	QString str1 = cb->text();
	QString str2 = QString("");
	if (check)
		str2 = tr(" Check");
	else
		str2 = tr(" unCheck");
	if (cb == ui->cb_standoff) {
		m_param->errPinStandOffValid = check;
	}
	else if (cb == ui->cb_standoffDiff) {
		m_param->errPinStandOffDiffValid = check;
	}
	else if (cb == ui->cb_coplanar) {
		m_param->errCoplanarValid = check;
	}
	else if (cb == ui->cb_LMS) {
		m_param->IsLMSValid = check;
	}
	//log(LogL1, str0 + str1 + str2);
	m_module->setParamChanged(ProductLevel);
}

void standLocationNewWdg::doubleSpinBox_valueChanged(double val)
{
	QDoubleSpinBox* dsp = qobject_cast<QDoubleSpinBox*>(sender());
	QString str1, str2;
	str1 = ("");
	str2 = ("");
	if (dsp == ui->dsp_packageOffset) {
		str1 = ui->label_44->text();
		str2 = tr(" change from %1 to %2").arg(QString::number(m_param->packageOff)).arg(QString::number(val));
		m_param->packageOff = val;
	}
	if (dsp == ui->dsp_LMSFactor) {
		str1 = ui->cb_LMS->text() + " factor value ";
		str2 = tr(" change from %1 to %2").arg(QString::number(m_param->LMSFactor)).arg(QString::number(val));
		m_param->LMSFactor = val;
	}
	//log(LogL1, str1 + str2);
	m_module->setParamChanged(ProductLevel);
}

void standLocationNewWdg::spinBox_valueChanged(int val)
{
	QSpinBox* sp = qobject_cast<QSpinBox*>(sender());

	QString str0, str1;
	str0 = QString("");
	str1 = QString("");
	if (sp == ui->sp_grp1pinNum) {
		str0 = ui->label_2->text() + ui->label_3->text();
		str1 = tr(" change from %1 to %2").arg(QString::number(m_param->grpPinNum[0])).arg(QString::number(val));
		m_param->grpPinNum[0] = val;
		m_paramTables->setGroupLeadNumber(m_param->grpPinNum[0], m_param->grpPinNum[1]);
		m_corrTable->setGroupLeadNumber(m_param->grpPinNum[0], m_param->grpPinNum[1]);
		m_offTable->setGroupLeadNumber(m_param->grpPinNum[0], m_param->grpPinNum[1]);
		refreshTableWidget();
	}
	else if (sp == ui->sp_grp2pinNum) {
		str0 = ui->label_2->text() + ui->label_8->text();
		str1 = tr(" change from %1 to %2").arg(QString::number(m_param->grpPinNum[1])).arg(QString::number(val));
		m_param->grpPinNum[1] = val;
		m_paramTables->setGroupLeadNumber(m_param->grpPinNum[0], m_param->grpPinNum[1]);
		m_corrTable->setGroupLeadNumber(m_param->grpPinNum[0], m_param->grpPinNum[1]);
		m_offTable->setGroupLeadNumber(m_param->grpPinNum[0], m_param->grpPinNum[1]);
		refreshTableWidget();
	}
	else if (sp == ui->sp_minGrayDiff) {
		str0 = ui->tabWidget->tabText(m_param->NowGrp) + ui->label_25->text();
		str1 = tr(" change from %1 to %2").arg(QString::number(m_param->minGrayDiff[m_param->NowGrp])).arg(QString::number(val));
		m_param->minGrayDiff[m_param->NowGrp] = val;
	}
	else if (sp == ui->sp_minArea) {
		str0 = ui->tabWidget->tabText(m_param->NowGrp) + ui->label_27->text();
		str1 = tr(" change from %1 to %2").arg(QString::number(m_param->minArea[m_param->NowGrp])).arg(QString::number(val));			
		m_param->minArea[m_param->NowGrp] = val;

		if (m_module->p_im->empty())
			return;
		HalOpenCV::cv2halImg(*m_module->p_im, m_image, false);

		currentView()->clearLayer(m_layerIdx);
		currentView()->setColor(0, 0, 0);
		currentView()->setFilled(true);
		currentView()->dispRect1(
			m_param->RoiRegionVector[m_param->NowGrp + 1].row1, m_param->RoiRegionVector[m_param->NowGrp + 1].col1,
			m_param->RoiRegionVector[m_param->NowGrp + 1].row2, m_param->RoiRegionVector[m_param->NowGrp + 1].col2);
		currentView()->setFilled(false);
		currentView()->setColor(255, 255, 0);
		currentView()->dispRect1(
			m_param->RoiRegionVector[m_param->NowGrp + 1].row1, m_param->RoiRegionVector[m_param->NowGrp + 1].col1,
			m_param->RoiRegionVector[m_param->NowGrp + 1].row2, m_param->RoiRegionVector[m_param->NowGrp + 1].col2, "ROI");
		currentView()->setFilled(true);
		currentView()->setColor(255, 255, 255);
		Hobject polyImg;
		Hobject recregion;
		gen_rectangle1(&recregion, 
			m_param->RoiRegionVector[m_param->NowGrp + 1].row1, m_param->RoiRegionVector[m_param->NowGrp + 1].col1,
			m_param->RoiRegionVector[m_param->NowGrp + 1].row2, m_param->RoiRegionVector[m_param->NowGrp + 1].col2);
		reduce_domain(*m_image, recregion, &polyImg);
		invert_image(polyImg, &polyImg);
		threshold(polyImg, &polyImg, m_param->grayVal[m_param->NowGrp], 255);
		connection(polyImg, &polyImg);
		select_shape(polyImg, &polyImg, "area", "and", m_param->minArea[m_param->NowGrp], 10000000);
		RlRegion dispReg;
		HalOpenCV::hal2cvRegion(polyImg, &dispReg);
		currentView()->dispRegion(&dispReg);
		currentView()->update();
	}
	else if (sp == ui->sp_standoffOffset) {
		str0 = ui->tabWidget->tabText(m_param->NowGrp) + ui->label_43->text();
		str1 = tr(" change from %1 to %2").arg(QString::number(m_param->standoffOffset[m_param->NowGrp])).arg(QString::number(val));
		m_param->standoffOffset[m_param->NowGrp] = val;
	}
	else if (sp == ui->sp_grayval) {
		str0 = ui->tabWidget->tabText(m_param->NowGrp) + ui->label_9->text();
		str1 = tr(" change from %1 to %2").arg(QString::number(m_param->grayVal[m_param->NowGrp])).arg(QString::number(val));
		m_param->grayVal[m_param->NowGrp] = val;

		if (m_module->p_im->empty())
			return;
		HalOpenCV::cv2halImg(*m_module->p_im, m_image, false);

		currentView()->clearLayer(m_layerIdx);
		currentView()->setColor(0, 0, 0);
		currentView()->setFilled(true);
		currentView()->dispRect1(
			m_param->RoiRegionVector[m_param->NowGrp + 1].row1, m_param->RoiRegionVector[m_param->NowGrp + 1].col1,
			m_param->RoiRegionVector[m_param->NowGrp + 1].row2, m_param->RoiRegionVector[m_param->NowGrp + 1].col2);
		currentView()->setFilled(false);
		currentView()->setColor(255, 255, 0);
		currentView()->dispRect1(
			m_param->RoiRegionVector[m_param->NowGrp + 1].row1, m_param->RoiRegionVector[m_param->NowGrp + 1].col1,
			m_param->RoiRegionVector[m_param->NowGrp + 1].row2, m_param->RoiRegionVector[m_param->NowGrp + 1].col2, "ROI");
		currentView()->setFilled(true);
		currentView()->setColor(255, 255, 255);
		Hobject polyImg;
		Hobject recregion;
		gen_rectangle1(&recregion, 
			m_param->RoiRegionVector[m_param->NowGrp + 1].row1, m_param->RoiRegionVector[m_param->NowGrp + 1].col1,
			m_param->RoiRegionVector[m_param->NowGrp + 1].row2, m_param->RoiRegionVector[m_param->NowGrp + 1].col2);
		reduce_domain(*m_image, recregion, &polyImg);
		invert_image(polyImg, &polyImg);
		threshold(polyImg, &polyImg, m_param->grayVal[m_param->NowGrp], 255);
		connection(polyImg, &polyImg);
		select_shape(polyImg, &polyImg, "area", "and", m_param->minArea[m_param->NowGrp], 10000000);
		RlRegion dispReg;
		HalOpenCV::hal2cvRegion(polyImg, &dispReg);
		currentView()->dispRegion(&dispReg);
		currentView()->update();
	}
	//log(LogL1, str0 + str1);
	m_module->setParamChanged(ProductLevel);
}

void standLocationNewWdg::on_bt_selectRoi_toggled(bool check)
{
	if (check) {
		currentView()->clearLayer(m_layerIdx);
		double row1[5], col1[5], row2[5], col2[5];
		//const char* title[5] = {};
		//char buf[5][32];
		QStringList title = { tr("Search ROI"),tr("Up"),tr("Down"),tr("Left"),tr("Right") };
		for (int i = 0; i < 5; i++) {
			row1[i] = m_param->RoiRegionVector[i].row1;
			col1[i] = m_param->RoiRegionVector[i].col1;
			row2[i] = m_param->RoiRegionVector[i].row2;
			col2[i] = m_param->RoiRegionVector[i].col2;
// 			if (i == 0) {
// 				//				sprintf(buf[i],"ËÑË÷ÇøÓò",i+1);
// 				strcpy(buf[i], "Search ROI");
// 			}
// 			else if (i == 1) {
// 				//				sprintf(buf[i],"ÉÏ",i+1);
// 				strcpy(buf[i], "Up");
// 			}
// 			else if (i == 2) {
// 				//				sprintf(buf[i],"ÏÂ",i+1);
// 				strcpy(buf[i], "Down");
// 			}
// 			else if (i == 3) {
// 				//				sprintf(buf[i],"×ó",i+1);
// 				strcpy(buf[i], "Left");
// 			}
// 			else if (i == 4) {
// 				//				sprintf(buf[i],"ÓÒ",i+1);
// 				strcpy(buf[i], "Right");
// 			}
// 			title[i] = buf[i];
		}
		currentView()->setColor(255, 0, 128);
		currentView()->drawRects1(5, row1, col1, row2, col2, title);
		currentView()->update();
	}
	else {

		//log(LogL1, ui->bt_selectRoi->text());

		currentView()->finishDraw();
		double row1[5], col1[5], row2[5], col2[5];
		currentView()->getDrawRects1(5, row1, col1, row2, col2);
		for (int i = 0; i < 5; i++) {
			m_param->RoiRegionVector[i].row1 = row1[i];
			m_param->RoiRegionVector[i].col1 = col1[i];
			m_param->RoiRegionVector[i].row2 = row2[i];
			m_param->RoiRegionVector[i].col2 = col2[i];
		}
		currentView()->clearLayer(m_layerIdx);
		currentView()->setColor(0, 255, 0);
		currentView()->setFilled(false);
		for (int i = 0; i < 5; i++) {
			currentView()->dispRect1(row1[i], col1[i], row2[i], col2[i]);
		}
		currentView()->update();
	}
	m_module->setParamChanged(ProductLevel);
}

void standLocationNewWdg::comboBox_currentIndexChanged(int index)
{

}

void standLocationNewWdg::tabWidget_indexChange(int index)
{
	m_param->NowGrp = index;
	connectSlots(false);
	setCurrentGroup(index);
	connectSlots(true);
}

void standLocationNewWdg::on_bt_table_clicked()
{
	m_dialog->exec();
}

void standLocationNewWdg::on_bt_corrTable_clicked()
{
	m_corrDialog->exec();
}

void standLocationNewWdg::on_bt_advance_toggled(bool val)
{
	if (val) {
		ui->bt_advance->setText(tr("exit advanced"));
	}
	else {
		ui->bt_advance->setText(tr("advanced setup"));
	}
	ui->grp_advance->setVisible(val);
}

void standLocationNewWdg::tableWidget_valueChanged(int checkIdx, int grpIdx, int leadIdx, bool minOrMax, double val)
{
	ParamTables* pt = qobject_cast<ParamTables*>(sender());
	QString str0 = QString("");
	QString str1 = QString("");
	QString cbname, leadname, minmaxName;
	pt->getCheckBoxName(checkIdx, cbname);
	double exval;
	if (pt == m_paramTables) {
		leadname = tr("lead%1 of group%2: ").arg(leadIdx + 1).arg(grpIdx + 1);
		if (minOrMax)
			minmaxName = tr(" min val");
		else
			minmaxName = tr(" max val");
		str0 = leadname + cbname + minmaxName;
		if (checkIdx == 0) {
			if (!minOrMax) {
				exval = m_param->errPinStandOffMin[grpIdx][leadIdx];
				m_param->errPinStandOffMin[grpIdx][leadIdx] = val;
			}
			else {
				exval = m_param->errPinStandOffMax[grpIdx][leadIdx];
				m_param->errPinStandOffMax[grpIdx][leadIdx] = val;
			}
		}
		else if (checkIdx == 1) {
			if (!minOrMax) {
				exval = m_param->errPinStandOffDiffMin[grpIdx];
				m_param->errPinStandOffDiffMin[grpIdx] = val;
			}
			else {
				exval = m_param->errPinStandOffDiffMax[grpIdx];
				m_param->errPinStandOffDiffMax[grpIdx] = val;
			}
		}
		else if (checkIdx == 2) {
			if (!minOrMax) {
				exval = m_param->errCoplanarMin;
				m_param->errCoplanarMin = val;
			}
			else {
				exval = m_param->errCoplanar;
				m_param->errCoplanar = val;
			}
		}
	}
	else if (pt == m_corrTable) {
		//leadname=tr(" group%1: ").arg(grpIdx+1);
		leadname = tr("lead%1 of group%2: ").arg(leadIdx + 1).arg(grpIdx + 1);
		cbname = tr("compensation") + cbname;
		str0 = leadname + cbname;
		if (checkIdx == 0) {
			exval = m_param->standOffCorr[grpIdx][leadIdx];
			m_param->standOffCorr[grpIdx][leadIdx] = val;
		}
	}
	else if (pt == m_offTable) {
		//leadname=tr(" group%1: ").arg(grpIdx+1);
		leadname = tr("lead%1 of group%2: ").arg(leadIdx + 1).arg(grpIdx + 1);
		cbname = tr("offset") + cbname;
		str0 = leadname + cbname;
		if (checkIdx == 0) {
			exval = m_param->offStandOffCorr[grpIdx][leadIdx];
			m_param->offStandOffCorr[grpIdx][leadIdx] = val;
		}
	}
	str1 = tr(" change from %1 to %2").arg(QString::number(exval, 'f', 3)).arg(QString::number(val, 'f', 3));
	//log(LogL1, str0 + str1);
	m_module->setParamChanged(ProductLevel);
}

void standLocationNewWdg::tableWidget_checkToggled(int index, bool check)
{

}

void standLocationNewWdg::tableWidget_CopyItems(bool IsAll, int startCheck, int CheckRange, int starLead, int leadRange)
{
	ParamTables* pt = qobject_cast<ParamTables*>(sender());
	QString str0 = QString("");
	QString str1 = QString("");
	QString cbname, leadname, minmaxName;
	if (pt == m_paramTables) {
		int stc, edc, stl, edl;
		if (IsAll) {
			stc = 0; edc = 6;
			stl = 0; edl = m_param->grpPinNum[0] + m_param->grpPinNum[1];
		}
		else {
			stc = startCheck; edc = startCheck + CheckRange;
			stl = starLead; edl = starLead + leadRange;
		}
		for (int i = stc; i < edc; i++) {
			int checkIdx = i / 2;
			bool minormax = bool(i % 2);
			double exval, nowval;
			pt->getCheckBoxName(checkIdx, cbname);
			for (int j = stl; j < edl; j++) {
				if (j == 0 || j == m_param->grpPinNum[0]) {
					continue;
				}
				int grpIdx;
				if (j >= m_param->grpPinNum[0]) {
					grpIdx = 1;
				}
				else {
					grpIdx = 0;
				}
				int leadIdx = j - grpIdx*m_param->grpPinNum[0];

				leadname = tr("lead%1 of group%2: ").arg(leadIdx + 1).arg(grpIdx + 1);
				if (minormax)
					minmaxName = tr(" min val");
				else
					minmaxName = tr(" max val");
				str0 = leadname + cbname + minmaxName;

				if (i == 0) {
					exval = m_param->errPinStandOffMin[grpIdx][leadIdx];
					m_param->errPinStandOffMin[grpIdx][leadIdx] = nowval = m_param->errPinStandOffMin[grpIdx][0];

				}
				else if (i == 1) {
					exval = m_param->errPinStandOffMax[grpIdx][leadIdx];
					m_param->errPinStandOffMax[grpIdx][leadIdx] = nowval = m_param->errPinStandOffMax[grpIdx][0];
				}
				if (i == 2 || i == 3 || i == 4 || i == 5) {
					continue;
				}
				else {
					str1 = tr(" change from %1 to %2").arg(QString::number(exval, 'f', 3)).arg(QString::number(nowval, 'f', 3));
					//log(LogL1, str0 + str1);
				}
			}
		}
	}
	else if (pt == m_corrTable) {
		int stc, edc, stl, edl;
		if (IsAll) {
			stc = 0; edc = 1;
			stl = 0; edl = m_param->grpPinNum[0] + m_param->grpPinNum[1];
		}
		else {
			stc = startCheck; edc = startCheck + CheckRange;
			stl = starLead; edl = starLead + leadRange;
		}
		for (int i = stc; i < edc; i++) {
			int checkIdx = i;
			bool minormax = false;
			double exval, nowval;
			pt->getCheckBoxName(checkIdx, cbname);
			cbname = tr("compensation") + cbname;
			for (int j = stl; j < edl; j++) {
				if (j == 0 || j == m_param->grpPinNum[0]) {
					continue;
				}
				int grpIdx;
				if (j >= m_param->grpPinNum[0]) {
					grpIdx = 1;
				}
				else {
					grpIdx = 0;
				}
				int leadIdx = j - grpIdx*m_param->grpPinNum[0];

				leadname = tr("lead%1 of group%2: ").arg(leadIdx + 1).arg(grpIdx + 1);

				str0 = leadname + cbname;

				if (i == 0) {
					exval = m_param->standOffCorr[grpIdx][leadIdx];
					m_param->standOffCorr[grpIdx][leadIdx] = nowval = m_param->standOffCorr[grpIdx][0];
				}
				str1 = tr(" change from %1 to %2").arg(QString::number(exval, 'f', 3)).arg(QString::number(nowval, 'f', 3));
				//log(LogL1, str0 + str1);
			}
		}
	}
	else if (pt == m_offTable) {
		int stc, edc, stl, edl;
		if (IsAll) {
			stc = 0; edc = 1;
			stl = 0; edl = m_param->grpPinNum[0] + m_param->grpPinNum[1];
		}
		else {
			stc = startCheck; edc = startCheck + CheckRange;
			stl = starLead; edl = starLead + leadRange;
		}
		for (int i = stc; i < edc; i++) {
			int checkIdx = i;
			bool minormax = false;
			double exval, nowval;
			pt->getCheckBoxName(checkIdx, cbname);
			cbname = tr("offset") + cbname;
			for (int j = stl; j < edl; j++) {
				if (j == 0 || j == m_param->grpPinNum[0]) {
					continue;
				}
				int grpIdx;
				if (j >= m_param->grpPinNum[0]) {
					grpIdx = 1;
				}
				else {
					grpIdx = 0;
				}
				int leadIdx = j - grpIdx*m_param->grpPinNum[0];

				leadname = tr("lead%1 of group%2: ").arg(leadIdx + 1).arg(grpIdx + 1);

				str0 = leadname + cbname;

				if (i == 0) {
					exval = m_param->offStandOffCorr[grpIdx][leadIdx];
					m_param->offStandOffCorr[grpIdx][leadIdx] = nowval = m_param->offStandOffCorr[grpIdx][0];
				}
				str1 = tr(" change from %1 to %2").arg(QString::number(exval, 'f', 3)).arg(QString::number(nowval, 'f', 3));
				//log(LogL1, str0 + str1);
			}
		}
	}
	m_module->setParamChanged(ProductLevel);
}

void standLocationNewWdg::connectSlots(bool link)
{
	if(m_hasConnect==link)
		return;
	m_hasConnect=link;

	connectOrNot(link, ui->rb_upDw, SIGNAL(toggled(bool)), SLOT(radioButton_toggled(bool)));
	connectOrNot(link, ui->rb_lfRt, SIGNAL(toggled(bool)), SLOT(radioButton_toggled(bool)));

	connectOrNot(link, ui->cb_standoff, SIGNAL(toggled(bool)), SLOT(checkBox_toggled(bool)));
	connectOrNot(link, ui->cb_standoffDiff, SIGNAL(toggled(bool)), SLOT(checkBox_toggled(bool)));
	connectOrNot(link, ui->cb_coplanar, SIGNAL(toggled(bool)), SLOT(checkBox_toggled(bool)));
	connectOrNot(link, ui->cb_LMS, SIGNAL(toggled(bool)), SLOT(checkBox_toggled(bool)));

	//connectOrNot(link, ui->dsp_UpDwGap,SIGNAL(valueChanged(double)),SLOT(doubleSpinBox_valueChanged(double)));
	connectOrNot(link, ui->dsp_packageOffset, SIGNAL(valueChanged(double)), SLOT(doubleSpinBox_valueChanged(double)));
	connectOrNot(link, ui->dsp_LMSFactor, SIGNAL(valueChanged(double)), SLOT(doubleSpinBox_valueChanged(double)));

	connectOrNot(link, ui->sp_grp1pinNum, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_grp2pinNum, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));

	connectOrNot(link, ui->sp_minGrayDiff, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_minArea, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_standoffOffset, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_grayval, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));

	connectOrNot(link, ui->tabWidget, SIGNAL(currentChanged(int)), SLOT(tabWidget_indexChange(int)));

	connectOrNot(link, m_paramTables, SIGNAL(CellChanged(int, int, int, bool, double)), SLOT(tableWidget_valueChanged(int, int, int, bool, double)));
	connectOrNot(link, m_corrTable, SIGNAL(CellChanged(int, int, int, bool, double)), SLOT(tableWidget_valueChanged(int, int, int, bool, double)));
	connectOrNot(link, m_offTable, SIGNAL(CellChanged(int, int, int, bool, double)), SLOT(tableWidget_valueChanged(int, int, int, bool, double)));

	connectOrNot(link, m_paramTables, SIGNAL(CopyItems(bool, int, int, int, int)), SLOT(tableWidget_CopyItems(bool, int, int, int, int)));
	connectOrNot(link, m_corrTable, SIGNAL(CopyItems(bool, int, int, int, int)), SLOT(tableWidget_CopyItems(bool, int, int, int, int)));
	connectOrNot(link, m_offTable, SIGNAL(CopyItems(bool, int, int, int, int)), SLOT(tableWidget_CopyItems(bool, int, int, int, int)));
}

void standLocationNewWdg::iniUi()
{
	m_grpIndex = 0;
	m_leadIndex = 0;

	QStringList cbname;
	cbname << tr("standoff") << tr("standoff diff") << tr("coplanar");

	QStringList corrname;
	corrname << tr("standoff COMP");

	QStringList offname;
	offname << tr("standoff offset");

	m_dialog = new QDialog(this);
	m_corrDialog = new QDialog(this);
	m_dialog->setMinimumSize(910, 600);
	m_dialog->setWindowTitle(tr("range of param"));
	m_corrDialog->setMinimumSize(910, 400);
	m_corrDialog->setWindowTitle(tr("range of corr param"));
	QVBoxLayout* V1Lay = new QVBoxLayout;
	QVBoxLayout* V2Lay = new QVBoxLayout;
	QGroupBox *grpParam, *grpCorr, *grpOff;
	QVBoxLayout *grpV1, *grpV2, *grpV3;
	grpV1 = new QVBoxLayout;
	grpV2 = new QVBoxLayout;
	grpV3 = new QVBoxLayout;
	grpParam = new QGroupBox();
	grpCorr = new QGroupBox();
	grpOff = new QGroupBox();
	grpParam->setMinimumWidth(900);
	grpCorr->setMinimumWidth(900);
	grpOff->setMinimumWidth(900);

	vector<ParamTables::ParamType> cbtype;
	for (int i = 0; i < cbname.size(); i++) {
		ParamTables::ParamType tmptype = ParamTables::singleParam;
		cbtype.push_back(tmptype);
	}
	cbtype[1] = ParamTables::groupParam;
	cbtype[2] = ParamTables::sameParam;
	m_paramTables = new ParamTables(cbname.size(), cbtype, false, true, GRP_MAX_LEADNUM, GRP_MAX_LEADNUM, cbname, grpParam);
	//m_paramTables->setValueRange(-100.0,999.999,3);
	m_paramTables->setHeadName(tr("param range"));
	m_paramTables->setTableHeight(365);

	vector<ParamTables::ParamType> corrtype;
	for (int i = 0; i < corrname.size(); i++) {
		ParamTables::ParamType tmptype = ParamTables::singleParam;
		corrtype.push_back(tmptype);
	}
	m_corrTable = new ParamTables(corrname.size(), corrtype, false, false, GRP_MAX_LEADNUM, GRP_MAX_LEADNUM, corrname, grpCorr);
	m_corrTable->setValueRange(-999.999, 999.999, 3);
	m_corrTable->setHeadName(tr("compensation"));
	m_corrTable->setTableHeight(300);

	vector<ParamTables::ParamType> offtype;
	for (int i = 0; i < offname.size(); i++) {
		ParamTables::ParamType tmptype = ParamTables::singleParam;
		offtype.push_back(tmptype);
	}
	m_offTable = new ParamTables(offname.size(), offtype, false, false, GRP_MAX_LEADNUM, GRP_MAX_LEADNUM, offname, grpOff);
	m_offTable->setValueRange(-999.999, 999.999, 3);
	m_offTable->setHeadName(tr("offset"));
	m_offTable->setTableHeight(175);

	grpV1->addWidget(grpParam);
	grpV2->addWidget(grpCorr);
	grpV3->addWidget(grpOff);
	V1Lay->addLayout(grpV1, 2);
	V1Lay->addLayout(grpV3, 1);

	V2Lay->addLayout(grpV2, 1);

	m_dialog->setLayout(V1Lay);
	m_corrDialog->setLayout(V2Lay);
}

void standLocationNewWdg::setUiValue()
{
	if(!m_param)
		return;

	if (m_param->NowGrp < 4) {
		setCurrentGroup(m_param->NowGrp);
	}
	ui->tabWidget->setCurrentIndex(m_param->NowGrp);

	if (m_param->upDwOrLfRt == 0) {
		ui->rb_upDw->setChecked(true);
	}
	else {
		ui->rb_lfRt->setChecked(true);
	}

	ui->cb_standoff->setChecked(m_param->errPinStandOffValid == 1);
	ui->cb_standoffDiff->setChecked(m_param->errPinStandOffDiffValid == 1);
	ui->cb_coplanar->setChecked(m_param->errCoplanarValid);
	ui->cb_LMS->setChecked(m_param->IsLMSValid);
	ui->sp_grp1pinNum->setValue(m_param->grpPinNum[0]);
	ui->sp_grp2pinNum->setValue(m_param->grpPinNum[1]);
	ui->dsp_packageOffset->setValue(m_param->packageOff);
	ui->dsp_LMSFactor->setValue(m_param->LMSFactor);
	ui->bt_advance->setChecked(false);
	ui->grp_advance->setVisible(false);

	m_paramTables->setGroupLeadNumber(m_param->grpPinNum[0], m_param->grpPinNum[1]);
	m_corrTable->setGroupLeadNumber(m_param->grpPinNum[0], m_param->grpPinNum[1]);
	m_offTable->setGroupLeadNumber(m_param->grpPinNum[0], m_param->grpPinNum[1]);
	refreshTableWidget();
}

void standLocationNewWdg::refreshTableWidget()
{
	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < m_param->grpPinNum[i]; j++) {
			m_paramTables->setValue(0, i, j, 0, m_param->errPinStandOffMin[i][j]);
			m_paramTables->setValue(0, i, j, 1, m_param->errPinStandOffMax[i][j]);


			m_corrTable->setValue(0, i, j, 0, m_param->standOffCorr[i][j]);

			m_offTable->setValue(0, i, j, 0, m_param->offStandOffCorr[i][j]);
		}
		m_paramTables->setValue(1, i, 0, 0, m_param->errPinStandOffDiffMin[i]);
		m_paramTables->setValue(1, i, 0, 1, m_param->errPinStandOffDiffMax[i]);
	}
	m_paramTables->setValue(2, 0, 0, 0, m_param->errCoplanarMin);
	m_paramTables->setValue(2, 0, 0, 1, m_param->errCoplanar);
}

void standLocationNewWdg::setCurrentGroup(int i)
{
	ui->sp_minGrayDiff->setValue(m_param->minGrayDiff[i]);
	ui->sp_minArea->setValue(m_param->minArea[i]);
	ui->sp_standoffOffset->setValue(m_param->standoffOffset[i]);
	ui->sp_grayval->setValue(m_param->grayVal[i]);
}

