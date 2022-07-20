#include "calib3D5S_homWdg.h"
#include "calib3D5S_homMod.h"
#include "calib3D5S_homParam.h"
#include "ui_calib3D5S_homWdg.h"
#include "ImageView.h"
#include "halOpenCV.h"
#include "hom3d5s.h"
#include <QMessageBox>

calib3D5S_homWdg::calib3D5S_homWdg(int i) :
	ModSetWidget(),ui(new Ui::calib3D5S_homWdg),m_widgetType(i)
{
	m_hasConnect = false;
	m_layerIdx = 0;
	m_image = nullptr;
	ui->setupUi(this);
	iniUi();
	m_param = nullptr;
	m_module = nullptr;
}

calib3D5S_homWdg::~calib3D5S_homWdg()
{
	delete m_image;
	delete ui;
}

void calib3D5S_homWdg::setModule(UnitModule *module)
{
	m_module=dynamic_cast<calib3D5S_homMod*>(module);
	m_param = m_module->m_param;
	m_image = m_module->m_image;
	connectSlots(false);
	setUiValue();
	connectSlots(true);
}

void calib3D5S_homWdg::machinePosChanged(double val)
{
	QDoubleSpinBox* sp = qobject_cast<QDoubleSpinBox*>(sender());
	for (int i = 0; i < BtPointsNum; i++) {
		if (sp == m_btX[i]) {
			m_param->bottomMachineX[i] = val;
			return;
		}
		else if (sp == m_btY[i]) {
			m_param->bottomMachineY[i] = val;
			return;
		}
	}
	if (sp == ui->dsp_B_Z_4)
	{
		m_param->bottomMachineZ_5 = val;
		return;
	}
	for (int i = 0; i < PointsNum; i++) {
		if (sp == m_btX[i]) {
			m_param->bottomMachineX[i] = val;
		}
		else if (sp == m_btY[i]) {
			m_param->bottomMachineY[i] = val;
		}
		else if (sp == m_abcdX[i]) {
			m_param->abcdMachineX[i] = val;
		}
		else if (sp == m_abcdY[i]) {
			m_param->abcdMachineY[i] = val;
		}
		else if (sp == m_abcdZ[i]) {
			m_param->abcdMachineZ[i] = val;
		}
		else {
			for (int j = 0; j < 2; j++)
			{
				if (sp == m_sideX[j][i])
				{
					m_param->sideMachineX[j][i] = val;
				}
				else if (sp == m_sideY[j][i])
				{
					m_param->sideMachineY[j][i] = val;
				}
			}
		}
	}
}

void calib3D5S_homWdg::on_bt_Select_clicked()
{
	setRoi();
}

void calib3D5S_homWdg::on_bt_OK_clicked()
{
	getRoi();
	if (!m_param->IsTransHom)
	{
		for (int i = 0; i < BottomNum; i++)
		{
			m_module->m_BottomFitLine[i]->setRoi(1, &m_param->bottomRow1[i], &m_param->bottomCol1[i], &m_param->bottomRow2[i], &m_param->bottomCol2[i]);
		}
	}
	else {
		for (int i = 0; i < BtTransNum; i++)
		{
			m_module->m_transSideFitLine[i]->setRoi(1, &m_param->btTransRow1[i], &m_param->btTransCol1[i], &m_param->btTransRow2[i], &m_param->btTransCol2[i]);
		}
		for (int i = 0; i < SideTransNum; i++)
		{
			m_module->m_transSideFitLine[i]->setRoi(1, &m_param->sideTransRow1[i], &m_param->sideTransCol1[i], &m_param->sideTransRow2[i], &m_param->sideTransCol2[i]);
		}
	}

	for (int i = 0; i < SideNum; i++)
	{
		for (int j = 0; j < 2; j++)
		{
			m_module->m_sideFitLine[j][i]->setRoi(1, &m_param->sideRow1[j][i], &m_param->sideCol1[j][i], &m_param->sideRow2[j][i], &m_param->sideCol2[j][i]);
		}
	}
}

void calib3D5S_homWdg::on_bt_Cancel_clicked()
{
	currentView()->clearLayer(m_layerIdx);
	currentView()->finishDraw();
}

void calib3D5S_homWdg::checkBox_toggled(bool val)
{
	QCheckBox* cb = qobject_cast<QCheckBox*>(sender());
	if (cb == ui->cb_transHom)
	{
		m_param->IsTransHom = val;
		m_module->iniDetector();
	}
	else if (cb == ui->cb_blackWang) {
		m_param->IsBlackWang = val;
		m_module->iniDetector();
	}
}

void calib3D5S_homWdg::radioButton_toggled(bool check)
{
	if (check) {
		QRadioButton* rb = qobject_cast<QRadioButton*>(sender());
		if (rb == ui->rb_HorWANG) {
			m_param->horOrVerWANG = 0;
			m_module->iniDetector();
		}
		else if (rb == ui->rb_VerWANG) {
			m_param->horOrVerWANG = 1;
			m_module->iniDetector();
		}
	}
}

void calib3D5S_homWdg::on_bt_selectVerify_toggled(bool val)
{
	if (val)
	{
		currentView()->finishDraw();
		//const char* name[VerifyNum];
		double row1[VerifyNum], row2[VerifyNum], col1[VerifyNum], col2[VerifyNum];
		QStringList name;
		//char btBuf[VerifyNum][32];
		for (int i = 0; i < VerifyNum; i++)
		{
			//sprintf(btBuf[i], "V_%d", i + 1);
			//name[i] = btBuf[i];
			name << QString("V_%d").arg(i + 1);
			row1[i] = m_param->verRow1[i];
			col1[i] = m_param->verCol1[i];
			row2[i] = m_param->verRow2[i];
			col2[i] = m_param->verCol2[i];
		}
		currentView()->drawRects1(VerifyNum, row1, col1, row2, col2, name);
	}
	else {
		double row1[VerifyNum], row2[VerifyNum], col1[VerifyNum], col2[VerifyNum];
		currentView()->getDrawRects1(VerifyNum, row1, col1, row2, col2);
		for (int i = 0; i < VerifyNum; i++)
		{
			m_param->verRow1[i] = row1[i];
			m_param->verCol1[i] = col1[i];
			m_param->verRow2[i] = row2[i];
			m_param->verCol2[i] = col2[i];
		}
		currentView()->finishDraw();
		for (int i = 0; i < VerifyNum; i++)
		{
			m_module->m_veryFitLine[i]->setRoi(1, &m_param->verRow1[i], &m_param->verCol1[i], &m_param->verRow2[i], &m_param->verCol2[i]);
		}
	}
}

void calib3D5S_homWdg::on_bt_verify_clicked()
{
	if (m_module->p_im->empty())
		return;
	HalOpenCV::cv2halImg(*m_module->p_im, m_image, false);

	bool allok = true;
	bool veryLineOK[VerifyNum];
	CvPoint2D32f veryLine[2][VerifyNum];
	cv::Point2d veryInterP[4];
	bool lineok = true;
	for (int i = 0; i < VerifyNum; i++)
	{
		veryLineOK[i] = true;
		CvPoint2D32f line[2];
		int lineNum = 0;
		fitEdgeLine::ResType res = m_module->m_veryFitLine[i]->calculateLine(m_image, line, lineNum);
		if (res != fitEdgeLine::ResOK) {
			veryLineOK[i] = false;
			lineok = false;
		}
		else {
			for (int j = 0; j < 2; j++)
			{
				veryLine[j][i].x = line[j].x;
				veryLine[j][i].y = line[j].y;
			}
		}
	}
	bool veryinterOk = true;
	if (lineok)
	{
		for (int i = 0; i < 4; i++)
		{
			bool interok = true;
			HTuple pa;
			HTuple interRow, interCol;
			int Idx = i * 2;

			intersection_ll(veryLine[0][Idx].y, veryLine[0][Idx].x, veryLine[1][Idx].y, veryLine[1][Idx].x,
				veryLine[0][Idx + 1].y, veryLine[0][Idx + 1].x, veryLine[1][Idx + 1].y, veryLine[1][Idx + 1].x, &interRow, &interCol, &pa);
			int paval = pa[0].I();
			if (paval == 0)
			{
				veryInterP[i].x = interCol[0].D();
				veryInterP[i].y = interRow[0].D();
			}
			else {
				interok = false;
				veryinterOk = false;
				allok = false;
			}
		}
	}
	cv::Point3d world[2];
	double realStand[2] = { 0,0 };
	bool calok = true;
	if (lineok&& veryinterOk)
	{
		if (m_param->IsTransHom)
		{
			if (!m_module->m_hom3D5S[0]->calWorldPos(*m_module->m_homTrans[0], veryInterP[0], *m_module->m_hom3D5S[0], veryInterP[1], world[0])) {
				calok = false;
			}
			if (!m_module->m_hom3D5S[0]->calWorldPos(*m_module->m_homTrans[1], veryInterP[3], *m_module->m_hom3D5S[0], veryInterP[2], world[1])) {
				calok = false;
			}
			if (m_module->m_sideWorldTrans[0].x != 0 && m_module->m_sideWorldTrans[0].y != 0 && 
				m_module->m_sideWorldTrans[1].y != 0 && m_module->m_sideWorldTrans[1].x != 0)
			{
				m_module->m_plane->fit(m_module->m_sideWorldTrans, 4);
				realStand[0] = m_module->m_plane->pointDistance(world[0]);
				realStand[1] = m_module->m_plane->pointDistance(world[1]);
			}
		}
		else {
			if (!m_module->m_hom3D5S[0]->calWorldPos(*m_module->m_hom3D5S[1], veryInterP[0], *m_module->m_hom3D5S[0], veryInterP[1], world[0])) {
				calok = false;
			}
			if (!m_module->m_hom3D5S[0]->calWorldPos(*m_module->m_hom3D5S[2], veryInterP[3], *m_module->m_hom3D5S[0], veryInterP[2], world[1])) {
				calok = false;
			}
		}

	}
	if (lineok&& veryinterOk&&calok)
	{
		QSize sz = currentView()->size();

		currentView()->setColor(QColor(0, 0, 0));
		currentView()->setTextSize(10);
		currentView()->dispText(sz.height() - 20, 0, tr("xyz_1:%1,%2,%3").arg(world[0].x, 0, 'f', 4).arg(world[0].y, 0, 'f', 4).arg(world[0].z, 0, 'f', 4));
		currentView()->dispText(sz.height() - 10, 0, tr("xyz_2:%1,%2,%3").arg(world[1].x, 0, 'f', 4).arg(world[1].y, 0, 'f', 4).arg(world[1].z, 0, 'f', 4));
		if (m_param->IsTransHom)
			currentView()->dispText(sz.height(), 0, tr("real stand: stand-1=%1, stand-2=%2").arg(realStand[0], 0, 'f', 4).arg(realStand[1], 0, 'f', 4));
		currentView()->clearLayer(m_layerIdx);
		currentView()->setColor(255, 0, 0);
		for (int i = 0; i < VerifyNum; i++)
		{
			currentView()->dispLine(veryLine[0][i].y, veryLine[0][i].x, veryLine[1][i].y, veryLine[1][i].x);
		}
		currentView()->update();
	}
}

void calib3D5S_homWdg::on_bt_calib_clicked()
{
	auto r = QMessageBox::information(this, tr("info"), tr("Are you want to calibration?"), QMessageBox::Yes | QMessageBox::No);
	if (r == QMessageBox::Yes)
	{
		m_module->triggerEvent(ENT_CALIB);
	}
}

void calib3D5S_homWdg::connectSlots(bool link)
{
	if(m_hasConnect==link)
		return;
	m_hasConnect=link;

	for (int i = 0; i < PointsNum; i++) {
		connectOrNot(link, m_abcdX[i], SIGNAL(valueChanged(double)), SLOT(machinePosChanged(double)));
		connectOrNot(link, m_abcdY[i], SIGNAL(valueChanged(double)), SLOT(machinePosChanged(double)));
		connectOrNot(link, m_abcdZ[i], SIGNAL(valueChanged(double)), SLOT(machinePosChanged(double)));

		for (int j = 0; j < 2; j++)
		{
			connectOrNot(link, m_sideX[j][i], SIGNAL(valueChanged(double)), SLOT(machinePosChanged(double)));
			connectOrNot(link, m_sideY[j][i], SIGNAL(valueChanged(double)), SLOT(machinePosChanged(double)));
		}
	}
	for (int i = 0; i < BtPointsNum; i++) {
		connectOrNot(link, m_btX[i], SIGNAL(valueChanged(double)), SLOT(machinePosChanged(double)));
		connectOrNot(link, m_btY[i], SIGNAL(valueChanged(double)), SLOT(machinePosChanged(double)));
	}
	connectOrNot(link, ui->rb_HorWANG, SIGNAL(toggled(bool)), SLOT(radioButton_toggled(bool)));
	connectOrNot(link, ui->rb_VerWANG, SIGNAL(toggled(bool)), SLOT(radioButton_toggled(bool)));
	connectOrNot(link, ui->cb_transHom, SIGNAL(toggled(bool)), SLOT(checkBox_toggled(bool)));
	connectOrNot(link, ui->cb_blackWang, SIGNAL(toggled(bool)), SLOT(checkBox_toggled(bool)));
	connectOrNot(link, ui->dsp_B_Z_4, SIGNAL(valueChanged(double)), SLOT(machinePosChanged(double)));
}

void calib3D5S_homWdg::iniUi()
{
	for (int i = 0; i < PointsNum; i++) {
		m_abcdX[i] = findChild<QDoubleSpinBox*>(QString("dsp_ABCD_%1_X").arg(i));
		m_abcdY[i] = findChild<QDoubleSpinBox*>(QString("dsp_ABCD_%1_Y").arg(i));
		m_abcdZ[i] = findChild<QDoubleSpinBox*>(QString("dsp_ABCD_%1_Z").arg(i));

		for (int j = 0; j < 2; j++)
		{
			m_sideX[j][i] = findChild<QDoubleSpinBox*>(QString("dsp_S%1_X_%2").arg(j + 1).arg(i));
			m_sideY[j][i] = findChild<QDoubleSpinBox*>(QString("dsp_S%1_Y_%2").arg(j + 1).arg(i));
		}
	}
	for (int i = 0; i < BtPointsNum; i++) {
		m_btX[i] = findChild<QDoubleSpinBox*>(QString("dsp_B_X_%1").arg(i));
		m_btY[i] = findChild<QDoubleSpinBox*>(QString("dsp_B_Y_%1").arg(i));
	}
}

void calib3D5S_homWdg::setUiValue()
{
	if(!m_param)
		return;

}

void calib3D5S_homWdg::setRoi()
{
	if (!m_param->IsTransHom)
	{
		//const char* name[BottomNum + SideNum * 2];
		QStringList name;
		double row1[BottomNum + SideNum * 2], row2[BottomNum + SideNum * 2], col1[BottomNum + SideNum * 2], col2[BottomNum + SideNum * 2];

		//char btBuf[BottomNum][32];
		for (int i = 0; i < BottomNum; i++)
		{
			//sprintf(btBuf[i], "B_%d", i + 1);
			//name[i] = btBuf[i];
			name << QString("B_%1").arg(i + 1);
			row1[i] = m_param->bottomRow1[i];
			col1[i] = m_param->bottomCol1[i];
			row2[i] = m_param->bottomRow2[i];
			col2[i] = m_param->bottomCol2[i];
		}
		//char side1Buf[SideNum][32];
		for (int i = 0; i < SideNum; i++)
		{
			//sprintf(side1Buf[i], "S1_%d", i + 1);
			//name[i + BottomNum] = side1Buf[i];
			name << QString("S1_%1").arg(i + 1);
			row1[i + BottomNum] = m_param->sideRow1[0][i];
			col1[i + BottomNum] = m_param->sideCol1[0][i];
			row2[i + BottomNum] = m_param->sideRow2[0][i];
			col2[i + BottomNum] = m_param->sideCol2[0][i];
		}

		//char side2Buf[SideNum][32];
		for (int i = 0; i < SideNum; i++)
		{
			//sprintf(side2Buf[i], "S2_%d", i + 1);
			//name[i + BottomNum + SideNum] = side2Buf[i];
			name << QString("S2_%1").arg(i + 1);
			row1[i + BottomNum + SideNum] = m_param->sideRow1[1][i];
			col1[i + BottomNum + SideNum] = m_param->sideCol1[1][i];
			row2[i + BottomNum + SideNum] = m_param->sideRow2[1][i];
			col2[i + BottomNum + SideNum] = m_param->sideCol2[1][i];
		}
		currentView()->drawRects1(BottomNum + SideNum * 2, row1, col1, row2, col2, name);
	}
	else {
		//const char* name[BtTransNum + SideTransNum + SideNum * 2];
		QStringList name;
		double row1[BtTransNum + SideTransNum + SideNum * 2], row2[BtTransNum + SideTransNum + SideNum * 2],
			col1[BtTransNum + SideTransNum + SideNum * 2], col2[BtTransNum + SideTransNum + SideNum * 2];

		//char btBuf[BtTransNum][32];
		for (int i = 0; i < BtTransNum; i++)
		{
			//sprintf(btBuf[i], "B_T_%d", i + 1);
			//name[i] = btBuf[i];
			name << QString("B_T_%1").arg(i + 1);
			row1[i] = m_param->btTransRow1[i];
			col1[i] = m_param->btTransCol1[i];
			row2[i] = m_param->btTransRow2[i];
			col2[i] = m_param->btTransCol2[i];
		}
		//char sideTransBuf[SideTransNum][32];
		for (int i = 0; i < SideTransNum; i++)
		{
			//sprintf(sideTransBuf[i], "S_T_%d", i + 1);
			//name[i + BtTransNum] = sideTransBuf[i];
			name << QString("S_T_%1").arg(i + 1);
			row1[i + BtTransNum] = m_param->sideTransRow1[i];
			col1[i + BtTransNum] = m_param->sideTransCol1[i];
			row2[i + BtTransNum] = m_param->sideTransRow2[i];
			col2[i + BtTransNum] = m_param->sideTransCol2[i];
		}
		//char side1Buf[SideNum][32];
		for (int i = 0; i < SideNum; i++)
		{
			//sprintf(side1Buf[i], "S1_%d", i + 1);
			//name[i + BtTransNum + SideTransNum] = side1Buf[i];
			name << QString("S1_%1").arg(i + 1);
			row1[i + BtTransNum + SideTransNum] = m_param->sideRow1[0][i];
			col1[i + BtTransNum + SideTransNum] = m_param->sideCol1[0][i];
			row2[i + BtTransNum + SideTransNum] = m_param->sideRow2[0][i];
			col2[i + BtTransNum + SideTransNum] = m_param->sideCol2[0][i];
		}

		//char side2Buf[SideNum][32];
		for (int i = 0; i < SideNum; i++)
		{
			//sprintf(side2Buf[i], "S2_%d", i + 1);
			//name[i + BtTransNum + SideTransNum + SideNum] = side2Buf[i];
			name << QString("S2_%1").arg(i + 1);
			row1[i + BtTransNum + SideTransNum + SideNum] = m_param->sideRow1[1][i];
			col1[i + BtTransNum + SideTransNum + SideNum] = m_param->sideCol1[1][i];
			row2[i + BtTransNum + SideTransNum + SideNum] = m_param->sideRow2[1][i];
			col2[i + BtTransNum + SideTransNum + SideNum] = m_param->sideCol2[1][i];
		}
		currentView()->drawRects1(BtTransNum + SideTransNum + SideNum * 2, row1, col1, row2, col2, name);
	}
}

void calib3D5S_homWdg::getRoi()
{
	if (!m_param->IsTransHom)
	{
		double row1[BottomNum + SideNum * 2], row2[BottomNum + SideNum * 2], col1[BottomNum + SideNum * 2], col2[BottomNum + SideNum * 2];
		currentView()->getDrawRects1(BottomNum + SideNum * 2, row1, col1, row2, col2);
		for (int i = 0; i < BottomNum; i++)
		{
			m_param->bottomRow1[i] = row1[i];
			m_param->bottomCol1[i] = col1[i];
			m_param->bottomRow2[i] = row2[i];
			m_param->bottomCol2[i] = col2[i];
		}
		for (int i = 0; i < SideNum; i++)
		{
			m_param->sideRow1[0][i] = row1[i + BottomNum];
			m_param->sideCol1[0][i] = col1[i + BottomNum];
			m_param->sideRow2[0][i] = row2[i + BottomNum];
			m_param->sideCol2[0][i] = col2[i + BottomNum];
		}
		for (int i = 0; i < SideNum; i++)
		{
			m_param->sideRow1[1][i] = row1[i + BottomNum + SideNum];
			m_param->sideCol1[1][i] = col1[i + BottomNum + SideNum];
			m_param->sideRow2[1][i] = row2[i + BottomNum + SideNum];
			m_param->sideCol2[1][i] = col2[i + BottomNum + SideNum];
		}
		currentView()->finishDraw();
	}
	else {
		double row1[BtTransNum + SideTransNum + SideNum * 2], row2[BtTransNum + SideTransNum + SideNum * 2]
			, col1[BtTransNum + SideTransNum + SideNum * 2], col2[BtTransNum + SideTransNum + SideNum * 2];
		currentView()->getDrawRects1(BtTransNum + SideTransNum + SideNum * 2, row1, col1, row2, col2);
		for (int i = 0; i < BtTransNum; i++)
		{
			m_param->btTransRow1[i] = row1[i];
			m_param->btTransCol1[i] = col1[i];
			m_param->btTransRow2[i] = row2[i];
			m_param->btTransCol2[i] = col2[i];
		}
		for (int i = 0; i < SideTransNum; i++)
		{
			m_param->sideTransRow1[i] = row1[i + BtTransNum];
			m_param->sideTransCol1[i] = col1[i + BtTransNum];
			m_param->sideTransRow2[i] = row2[i + BtTransNum];
			m_param->sideTransCol2[i] = col2[i + BtTransNum];
		}
		for (int i = 0; i < SideNum; i++)
		{
			m_param->sideRow1[0][i] = row1[i + SideTransNum + BtTransNum];
			m_param->sideCol1[0][i] = col1[i + SideTransNum + BtTransNum];
			m_param->sideRow2[0][i] = row2[i + SideTransNum + BtTransNum];
			m_param->sideCol2[0][i] = col2[i + SideTransNum + BtTransNum];
		}
		for (int i = 0; i < SideNum; i++)
		{
			m_param->sideRow1[1][i] = row1[i + SideTransNum + BtTransNum + SideNum];
			m_param->sideCol1[1][i] = col1[i + SideTransNum + BtTransNum + SideNum];
			m_param->sideRow2[1][i] = row2[i + SideTransNum + BtTransNum + SideNum];
			m_param->sideCol2[1][i] = col2[i + SideTransNum + BtTransNum + SideNum];
		}
		currentView()->finishDraw();
	}
}

