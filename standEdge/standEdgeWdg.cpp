#include "standEdgeWdg.h"
#include "standEdgeMod.h"
#include "standEdgeParam.h"
#include "ui_standEdgeWdg.h"
#include <QMessageBox>
#include "ImageView.h"
#include "halOpenCV.h"

standEdgeWdg::standEdgeWdg(int i) :
	ModSetWidget(),ui(new Ui::standEdgeWdg),m_widgetType(i)
{
	m_hasConnect = false;
	ui->setupUi(this);
	iniUi();
	m_param = nullptr;
	m_module = nullptr;
	m_image = nullptr;
	m_layerIdx = 0;
}

standEdgeWdg::~standEdgeWdg()
{
	delete ui;
}

void standEdgeWdg::setModule(UnitModule *module)
{
	m_module=dynamic_cast<standEdgeMod*>(module);
	m_param = m_module->m_param;
	m_image = m_module->m_image;
	//currentView()->setLayer(m_layIdex);
	//connectSlots(false);
	//setUiValue();
	//connectSlots(true);
}

void standEdgeWdg::showEvent(QShowEvent * event)
{
	connectSlots(false);
	setUiValue();
	connectSlots(true);
}

void standEdgeWdg::on_bt_setOrigin_clicked()
{
	HTuple centerRow, centerCol;
	m_module->getDispOutData(CenterRow, centerRow);
	m_module->getDispOutData(CenterCol, centerCol);
	if (centerRow.Num() == 0) {
		return;
	}

	if (QMessageBox::information(this, tr("Hint"),
		tr("Are you sure to change the current center point:(%1, %2) to the template center?")
		.arg(centerCol[0].D(), 0, 'f', 3).arg(centerRow[0].D(), 0, 'f', 3),
		QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
		//log(LogL1, ui->bt_setOrigin->text());
			m_param->TmpCenterRow = centerRow[0].D();
			m_param->TmpCenterCol = centerCol[0].D();
			m_module->setParamChanged(ProductLevel);
	}
}

void standEdgeWdg::radioButton_toggled(bool check)
{

}

void standEdgeWdg::checkBox_toggled(bool check)
{
	QCheckBox* cb = qobject_cast<QCheckBox*>(sender());
	QString str0 = QString("");
	QString str1 = cb->text();
	QString str2 = QString("");
	if (check)
		str2 = tr(" Check");
	else
		str2 = tr(" unCheck");
	if (cb == ui->cb_detectEndPoint) {
		m_param->IsEndPoint = check;
	}
	else if (cb == ui->cb_paichu) {
		str0 = ui->tabWidget->tabText(m_param->NowGrp);
		m_param->IsPaichuLine[m_param->NowGrp] = check;
	}
	else if (cb == ui->cb_errWidth) {
		m_param->IsErrWidth = check;
		ui->sp_errWidthMin->setEnabled(check);
		ui->sp_errWidthMax->setEnabled(check);
	}
	else if (cb == ui->cb_errHeight) {
		m_param->IsErrHeight = check;
		ui->sp_errHeightMin->setEnabled(check);
		ui->sp_errHeightMax->setEnabled(check);
	}
	else if (cb == ui->cb_errAngle) {
		m_param->IsErrAngle = check;
		ui->sp_errAngle->setEnabled(check);
	}
	else if (cb == ui->cb_errCenterOffset) {
		m_param->IsErrCenterOffset = check;
		ui->sp_errCenterOffset->setEnabled(check);
	}
	else if (cb == ui->cb_leadSize)
	{
		str0 = ui->tabWidget->tabText(m_param->NowGrp);
		m_param->isLeadSize[m_param->NowGrp] = check;
		ui->sp_leadSize->setEnabled(check);
	}
	else if (cb == ui->cb_outlier)
	{
		m_param->IsClearOutlier[m_param->NowGrp] = check;
	}
	//log(LogL1, str0 + str1 + str2);
	m_module->setParamChanged(ProductLevel);
}

void standEdgeWdg::doubleSpinBox_valueChanged(double val)
{
	QDoubleSpinBox* dsp = qobject_cast<QDoubleSpinBox*>(sender());
	QString str1, str2;
	str1 = ("");
	str2 = ("");
	if (dsp == ui->dsp_factor) {
		str1 = ui->tabWidget->tabText(m_param->NowGrp) + QString(" ") + ui->label_14->text();
		str2 = tr(" change from %1 to %2")
			.arg(QString::number(m_param->clipfactor[m_param->NowGrp], 'f', 1))
			.arg(QString::number(val, 'f', 1));
		m_param->clipfactor[m_param->NowGrp] = val;
	}
	else if (dsp == ui->dsp_scalePaichu) {
		str1 = ui->tabWidget->tabText(m_param->NowGrp) + QString(" ") + ui->label_15->text();
		str2 = tr(" change from %1 to %2")
			.arg(QString::number(m_param->scalePaichu[m_param->NowGrp], 'f', 2))
			.arg(QString::number(val, 'f', 2));
		m_param->scalePaichu[m_param->NowGrp] = val;
	}
	m_module->setParamChanged(ProductLevel);
	//log(LogL1, str1 + str2);
}

void standEdgeWdg::spinBox_valueChanged(int val)
{
	QSpinBox* sp = qobject_cast<QSpinBox*>(sender());

	QString str0, str1;
	str0 = QString("");
	str1 = QString("");

	if (sp == ui->sp_maxAngle) {
		str0 = ui->label->text();
		str1 = tr(" change from %1 to %2").arg(QString::number(m_param->maxAngle)).arg(QString::number(val));
		m_param->maxAngle = val;
	}
	else if (sp == ui->sp_errWidthMin) {
		str0 = ui->cb_errWidth->text() + tr(" min val ");
		str1 = tr(" change from %1 to %2").arg(QString::number(m_param->errWidthMin)).arg(QString::number(val));
		m_param->errWidthMin = val;
	}
	else if (sp == ui->sp_errWidthMax) {
		str0 = ui->cb_errWidth->text() + tr(" max val ");
		str1 = tr(" change from %1 to %2").arg(QString::number(m_param->errWidthMax)).arg(QString::number(val));
		m_param->errWidthMax = val;
	}
	else if (sp == ui->sp_errHeightMin) {
		str0 = ui->cb_errHeight->text() + tr(" min val ");
		str1 = tr(" change from %1 to %2").arg(QString::number(m_param->errHeightMin)).arg(QString::number(val));
		m_param->errHeightMin = val;
	}
	else if (sp == ui->sp_errHeightMax) {
		str0 = ui->cb_errHeight->text() + tr(" max val ");
		str1 = tr(" change from %1 to %2").arg(QString::number(m_param->errHeightmax)).arg(QString::number(val));
		m_param->errHeightmax = val;
	}
	else if (sp == ui->sp_errAngle) {
		str0 = ui->cb_errAngle->text();
		str1 = tr(" change from %1 to %2").arg(QString::number(m_param->errAngle)).arg(QString::number(val));
		m_param->errAngle = val;
	}
	else if (sp == ui->sp_errCenterOffset) {
		str0 = ui->cb_errCenterOffset->text();
		str1 = tr(" change from %1 to %2").arg(QString::number(m_param->errCenterOffset)).arg(QString::number(val));
		m_param->errCenterOffset = val;
	}
	else if (sp == ui->sp_minGrayDiff) {
		str0 = ui->tabWidget->tabText(m_param->NowGrp) + ui->label_25->text();
		str1 = tr(" change from %1 to %2").arg(QString::number(m_param->minGrayDiff[m_param->NowGrp])).arg(QString::number(val));
		m_param->minGrayDiff[m_param->NowGrp] = val;
	}
	else if (sp == ui->sp_offsetPlus) {
		str0 = ui->tabWidget->tabText(m_param->NowGrp) + ui->label_4->text();
		str1 = tr(" change from %1 to %2").arg(QString::number(m_param->offsetPlus[m_param->NowGrp])).arg(QString::number(val));
		m_param->offsetPlus[m_param->NowGrp] = val;
	}
	else if (sp == ui->sp_searchLineGap) {
		str0 = ui->tabWidget->tabText(m_param->NowGrp) + ui->label_22->text();
		str1 = tr(" change from %1 to %2").arg(QString::number(m_param->searchLineGap[m_param->NowGrp])).arg(QString::number(val));
		m_param->searchLineGap[m_param->NowGrp] = val;
	}
	else if (sp == ui->sp_lineOffset) {
		str0 = ui->tabWidget->tabText(m_param->NowGrp) + ui->label_24->text();
		str1 = tr(" change from %1 to %2").arg(QString::number(m_param->lineOffset[m_param->NowGrp])).arg(QString::number(val));
		m_param->lineOffset[m_param->NowGrp] = val;
	}
	else if (sp == ui->sp_paichu) {
		str0 = ui->tabWidget->tabText(m_param->NowGrp) + ui->cb_paichu->text();
		str1 = tr(" change from %1 to %2").arg(QString::number(m_param->paichuLine[m_param->NowGrp])).arg(QString::number(val));
		m_param->paichuLine[m_param->NowGrp] = val;
	}
	else if (sp == ui->sp_leadSize) {
		str0 = ui->tabWidget->tabText(m_param->NowGrp) + ui->cb_leadSize->text();
		str1 = tr(" change from %1 to %2").arg(QString::number(m_param->leadSize[m_param->NowGrp])).arg(QString::number(val));
		m_param->leadSize[m_param->NowGrp] = val;
		//currentView()->clear();
		//currentView()->setColor(0,0,0);
		//currentView()->setFilled(true);
		//currentView()->dispRect1(m_param->RoiRegionVector[m_param->NowGrp+1].row1,m_param->RoiRegionVector[m_param->NowGrp+1].col1,
		//	m_param->RoiRegionVector[m_param->NowGrp+1].row2,m_param->RoiRegionVector[m_param->NowGrp+1].col2);
		HalOpenCV::cv2halImg(*m_module->p_im, m_image, false);
		currentView()->clearLayer(m_layerIdx);
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
		if (m_param->isLeadSize[m_param->NowGrp] && m_param->leadSize[m_param->NowGrp] > 0) {
			int wid, hei;
			if (m_param->NowGrp == 0 || m_param->NowGrp == 1) {
				wid = m_param->leadSize[m_param->NowGrp];
				hei = 10;
			}
			else {
				hei = m_param->leadSize[m_param->NowGrp];
				wid = 10;
			}
			Hobject openobj, unionobj;
			opening_rectangle1(polyImg, &openobj, wid, hei);
			//connection(openobj,&openobj);
			union1(openobj, &openobj);
			union1(polyImg, &unionobj);
			difference(unionobj, openobj, &openobj);
			connection(openobj, &openobj);
			dilation_circle(openobj, &openobj, 1);
			union1(openobj, &openobj);
			RlRegion rlReg;
			HalOpenCV::hal2cvRegion(openobj, &rlReg);
			currentView()->dispRegion(/*openobj*/&rlReg);
			currentView()->update();
		}
		else {
			//currentView()->clear();
			currentView()->clearLayer(m_layerIdx);
			currentView()->update();
		}
	}
	else if (sp == ui->sp_grayval) {
		str0 = ui->tabWidget->tabText(m_param->NowGrp) + ui->label_9->text();
		str1 = tr(" change from %1 to %2").arg(QString::number(m_param->grayVal[m_param->NowGrp])).arg(QString::number(val));
		m_param->grayVal[m_param->NowGrp] = val;

		//currentView()->clear();
		HalOpenCV::cv2halImg(*m_module->p_im, m_image, false);
		currentView()->clearLayer(m_layerIdx);
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
		if (m_param->isLeadSize[m_param->NowGrp] && m_param->leadSize[m_param->NowGrp] > 0) {
			int wid, hei;
			if (m_param->NowGrp == 0 || m_param->NowGrp == 1) {
				wid = m_param->leadSize[m_param->NowGrp];
				hei = 10;
			}
			else {
				hei = m_param->leadSize[m_param->NowGrp];
				wid = 10;
			}
			Hobject openobj, unionobj;
			opening_rectangle1(polyImg, &openobj, wid, hei);
			//connection(openobj,&openobj);
			union1(openobj, &openobj);
			union1(polyImg, &unionobj);
			difference(unionobj, openobj, &openobj);
			connection(openobj, &openobj);
			dilation_circle(openobj, &openobj, 1);
			union1(openobj, &openobj);
			RlRegion rlReg;
			HalOpenCV::hal2cvRegion(openobj, &rlReg);
			currentView()->dispRegion(/*openobj*/&rlReg);
			currentView()->update();
		}
		else {
			//currentView()->clear();
			currentView()->clearLayer(m_layerIdx);
			currentView()->update();
		}
	}
	else if (sp == ui->sp_edgeGray) {
		str0 = ui->tabWidget->tabText(m_param->NowGrp) + ui->label_53->text();
		str1 = tr(" change from %1 to %2").arg(QString::number(m_param->edgeGray[m_param->NowGrp])).arg(QString::number(val));
		m_param->edgeGray[m_param->NowGrp] = val;
	}
	m_module->setParamChanged(ProductLevel);
	//log(LogL1, str0 + str1);
}

void standEdgeWdg::on_bt_selectRoi_toggled(bool check)
{
	if (check) {
		currentView()->clearLayer(m_layerIdx);
		//currentView()->clear();
		double row1[5], col1[5], row2[5], col2[5];
		QStringList title = { "Search ROI" ,"Up" ,"Down" ,"Left" ,"Right" };
		for (int i = 0;i < 5;i++) {
			row1[i] = m_param->RoiRegionVector[i].row1;
			col1[i] = m_param->RoiRegionVector[i].col1;
			row2[i] = m_param->RoiRegionVector[i].row2;
			col2[i] = m_param->RoiRegionVector[i].col2;
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
		for (int i = 0;i < 5;i++) {
			m_param->RoiRegionVector[i].row1 = row1[i];
			m_param->RoiRegionVector[i].col1 = col1[i];
			m_param->RoiRegionVector[i].row2 = row2[i];
			m_param->RoiRegionVector[i].col2 = col2[i];
		}
		//currentView()->clear();
		currentView()->clearLayer(m_layerIdx);
		currentView()->setColor(0, 255, 0);
		currentView()->setFilled(false);
		for (int i = 0;i < 5;i++) {
			currentView()->dispRect1(row1[i], col1[i], row2[i], col2[i]);
		}
		currentView()->update();
		m_module->setParamChanged(ProductLevel);
	}
}

void standEdgeWdg::comboBox_currentIndexChanged(int index)
{

}

void standEdgeWdg::tabWidget_indexChange(int index)
{
	m_param->NowGrp = index;
	connectSlots(false);
	setCurrentGroup(index);
	connectSlots(true);
}

void standEdgeWdg::on_bt_highLevel_toggled(bool val)
{
	ui->grp_high->setVisible(val);
}

void standEdgeWdg::on_bt_helpRect_toggled(bool checked)
{
	if (checked) {
		if (ui->bt_selectRoi->isChecked())
			ui->bt_selectRoi->setChecked(false);

		double row1 = m_param->helpRectRow0;
		double col1 = m_param->helpRectCol0;
		double row2 = m_param->helpRectRow1;
		double col2 = m_param->helpRectCol1;
		//currentView()->clear();
		currentView()->clearLayer(m_layerIdx);
		currentView()->setColor(255, 0, 128);
		currentView()->drawRect1(row1, col1, row2, col2, tr("HelpRect"));
		currentView()->update();
	}
	else {
		HalOpenCV::cv2halImg(*m_module->p_im, m_image, false);

		double row[2], col[2];
		currentView()->finishDraw();
		currentView()->getDrawRect1(row[0], col[0], row[1], col[1]);
		//currentView()->clear();
		currentView()->clearLayer(m_layerIdx);
		currentView()->setColor(0, 255, 0);
		currentView()->setFilled(false);
		currentView()->dispRect1(row[0], col[0], row[1], col[1]);

		//生成四个位置的框
		HTuple imgH, imgW;
		get_image_size(*m_image, &imgW, &imgH);
		int imgh = imgH[0].I();
		int imgw = imgW[0].I();

		int wid = (col[1] - col[0]) / 20;         //ROI区域的宽
		wid = std::max(wid, 10);
		for (size_t i = 0; i < 4; ++i)
		{
			double totalRow1, totalCol1, totalRow2, totalCol2;
			if (i == 0 || i == 1)       //0:up 1:down
			{
				totalRow1 = row[i] - wid;
				totalRow2 = row[i] + wid;
				//totalCol1 = m_param->RoiRegionVector[i + 1].col1;//col1 + 1 * (col2 - col1) / 4;
				//totalCol2 = m_param->RoiRegionVector[i + 1].col2;//col2 - 1 * (col2 - col1) / 4;

				if (i == 0)//up
					totalRow1 = std::max(totalRow1, 0.0);
				else if (i == 1)//down
					totalRow2 = std::min(totalRow2, (double)imgh);

				m_param->RoiRegionVector[i + 1].row1 = totalRow1;
				m_param->RoiRegionVector[i + 1].row2 = totalRow2;
				//m_param->RoiRegionVector[i + 1].col1 = totalCol1;
				//m_param->RoiRegionVector[i + 1].col2 = totalCol2;
			}
			else if (i == 2 || i == 3)  //3:right 2:left
			{
				//totalRow1 = m_param->RoiRegionVector[i + 1].row1;//row1 + 1 * (row2 - row1) / 4;
				//totalRow2 = m_param->RoiRegionVector[i + 1].row2;//row2 - 1 * (row2 - row1) / 4;
				totalCol1 = col[i - 2] - wid;
				totalCol2 = col[i - 2] + wid;

				if (i == 2)//left
					totalCol1 = std::max(totalCol1, 0.0);
				else if (i == 3)//right
					totalCol2 = std::min(totalCol2, (double)imgw);

				//m_param->RoiRegionVector[i + 1].row1 = totalRow1;
				//m_param->RoiRegionVector[i + 1].row2 = totalRow2;
				m_param->RoiRegionVector[i + 1].col1 = totalCol1;
				m_param->RoiRegionVector[i + 1].col2 = totalCol2;
			}
		}
		int expand = 15;
		m_param->RoiRegionVector[0].row1 = std::max(m_param->RoiRegionVector[1].row1 - expand, 0.0f);
		m_param->RoiRegionVector[0].col1 = std::max(m_param->RoiRegionVector[3].col1 - expand, 0.0f);
		m_param->RoiRegionVector[0].row2 = std::min(m_param->RoiRegionVector[2].row2 + expand, (float)imgh);
		m_param->RoiRegionVector[0].col2 = std::min(m_param->RoiRegionVector[4].col2 + expand, (float)imgw);

		ui->bt_selectRoi->setChecked(true);
	}
}

void standEdgeWdg::connectSlots(bool link)
{
	if(m_hasConnect==link)
		return;
	m_hasConnect=link;

	connectOrNot(link, ui->cb_detectEndPoint, SIGNAL(toggled(bool)), SLOT(checkBox_toggled(bool)));

	connectOrNot(link, ui->cb_errWidth, SIGNAL(toggled(bool)), SLOT(checkBox_toggled(bool)));
	connectOrNot(link, ui->cb_errHeight, SIGNAL(toggled(bool)), SLOT(checkBox_toggled(bool)));
	connectOrNot(link, ui->cb_errAngle, SIGNAL(toggled(bool)), SLOT(checkBox_toggled(bool)));
	connectOrNot(link, ui->cb_errCenterOffset, SIGNAL(toggled(bool)), SLOT(checkBox_toggled(bool)));
	connectOrNot(link, ui->cb_leadSize, SIGNAL(toggled(bool)), SLOT(checkBox_toggled(bool)));
	connectOrNot(link, ui->cb_outlier, SIGNAL(toggled(bool)), SLOT(checkBox_toggled(bool)));

	//connectOrNot(link, ui->dsp_UpDwGap,SIGNAL(valueChanged(double)),SLOT(doubleSpinBox_valueChanged(double)));
	//connectOrNot(link, ui->dsp_LfRtGap,SIGNAL(valueChanged(double)),SLOT(doubleSpinBox_valueChanged(double)));
	connectOrNot(link, ui->sp_leadSize, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_maxAngle, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_errWidthMin, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_errWidthMax, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_errAngle, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_errHeightMin, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_errHeightMax, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_errCenterOffset, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));

	connectOrNot(link, ui->dsp_scalePaichu, SIGNAL(valueChanged(double)), SLOT(doubleSpinBox_valueChanged(double)));


	connectOrNot(link, ui->sp_minGrayDiff, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_searchLineGap, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_lineOffset, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_offsetPlus, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_grayval, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_edgeGray, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));

	connectOrNot(link, ui->dsp_factor, SIGNAL(valueChanged(double)), SLOT(doubleSpinBox_valueChanged(double)));
	connectOrNot(link, ui->sp_paichu, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->cb_paichu, SIGNAL(toggled(bool)), SLOT(checkBox_toggled(bool)));

	connectOrNot(link, ui->tabWidget, SIGNAL(currentChanged(int)), SLOT(tabWidget_indexChange(int)));
}

void standEdgeWdg::iniUi()
{

}

void standEdgeWdg::setUiValue()
{
	if(!m_param)
		return;

	if (m_param->NowGrp < 4) {
		setCurrentGroup(m_param->NowGrp);
	}
	ui->tabWidget->setCurrentIndex(m_param->NowGrp);
	ui->grp_high->setVisible(false);
	ui->cb_detectEndPoint->setChecked(m_param->IsEndPoint == 1);
	ui->sp_maxAngle->setValue(m_param->maxAngle);

	ui->cb_errWidth->setChecked(m_param->IsErrWidth == 1);
	ui->sp_errWidthMin->setValue(m_param->errWidthMin);
	ui->sp_errWidthMin->setEnabled(m_param->IsErrWidth == 1);
	ui->sp_errWidthMax->setValue(m_param->errWidthMax);
	ui->sp_errWidthMax->setEnabled(m_param->IsErrWidth == 1);

	ui->cb_errHeight->setChecked(m_param->IsErrHeight == 1);
	ui->sp_errHeightMin->setValue(m_param->errHeightMin);
	ui->sp_errHeightMin->setEnabled(m_param->IsErrHeight == 1);
	ui->sp_errHeightMax->setValue(m_param->errHeightmax);
	ui->sp_errHeightMax->setEnabled(m_param->IsErrHeight == 1);

	ui->cb_errAngle->setChecked(m_param->IsErrAngle == 1);
	ui->sp_errAngle->setValue(m_param->errAngle);
	ui->sp_errAngle->setEnabled(m_param->IsErrAngle == 1);

	ui->cb_errCenterOffset->setChecked(m_param->IsErrCenterOffset == 1);
	ui->sp_errCenterOffset->setValue(m_param->errCenterOffset);
	ui->sp_errCenterOffset->setEnabled(m_param->IsErrCenterOffset == 1);
}

void standEdgeWdg::setCurrentGroup(int i)
{
	ui->sp_searchLineGap->setValue(m_param->searchLineGap[i]);
	ui->sp_minGrayDiff->setValue(m_param->minGrayDiff[i]);
	ui->sp_lineOffset->setValue(m_param->lineOffset[i]);
	ui->sp_offsetPlus->setValue(m_param->offsetPlus[i]);
	ui->sp_grayval->setValue(m_param->grayVal[i]);
	ui->sp_edgeGray->setValue(m_param->edgeGray[i]);
	ui->dsp_factor->setValue(m_param->clipfactor[i]);
	ui->dsp_scalePaichu->setValue(m_param->scalePaichu[i]);
	ui->cb_paichu->setChecked(m_param->IsPaichuLine[i]);
	ui->sp_paichu->setValue(m_param->paichuLine[i]);
	ui->cb_leadSize->setChecked(m_param->isLeadSize[i]);
	ui->sp_leadSize->setEnabled(m_param->isLeadSize[i]);
	ui->sp_leadSize->setValue(m_param->leadSize[i]);
	ui->cb_outlier->setChecked(m_param->IsClearOutlier[i]);
}

