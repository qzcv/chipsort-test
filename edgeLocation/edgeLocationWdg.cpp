#include "edgeLocationWdg.h"
#include "edgeLocationMod.h"
#include "edgeLocationParam.h"
#include "ui_edgeLocationWdg.h"
#include <QMessageBox>

edgeLocationWdg::edgeLocationWdg(int i) :
	ModSetWidget(),ui(new Ui::edgeLocationWdg),m_widgetType(i)
{
	m_hasConnect = false;
	ui->setupUi(this);
	m_nowGrpIndex = 0;
	iniUi();
	m_param = nullptr;
	m_module = nullptr;
	for (auto i = 0;i < 4;++i)
		m_edgeLine[i] = nullptr;
}

edgeLocationWdg::~edgeLocationWdg()
{
	delete ui;
}

void edgeLocationWdg::setModule(UnitModule *module)
{
	m_module=dynamic_cast<edgeLocationMod*>(module);
	m_param = m_module->m_param;
	for (auto i = 0;i < 4;++i)
		m_edgeLine[i] = m_module->m_edgeLine[i];
}

void edgeLocationWdg::showEvent(QShowEvent * event)
{
	connectSlots(false);
	setUiValue();
	connectSlots(true);
}

void edgeLocationWdg::connectSlots(bool link)
{
	if(m_hasConnect==link)
		return;
	m_hasConnect=link;

}

void edgeLocationWdg::iniUi()
{

}

void edgeLocationWdg::setUiValue()
{
	if(!m_param)
		return;

}

void edgeLocationWdg::on_bt_setOrigin_clicked()
{
	HTuple centerRow, centerCol;
	getDispOutData(CenterRow, centerRow);
	getDispOutData(CenterCol, centerCol);
	if (centerRow.Num() != 0)
	{
		if (QMessageBox::information(this, tr("Hint"),
			tr("Are you sure to change the current center point(%1,%2) to the template center?")
			.arg(centerCol[0].D(), 0, 'f', 3).arg(centerRow[0].D(), 0, 'f', 3),
			QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
		{
			m_param->TmpCenterRow = centerRow[0].D();
			m_param->TmpCenterCol = centerCol[0].D();
		}
	}
}

void edgeLocationWdg::radioButton_toggled(bool check)
{
	QString str0 = QString("");
	QString str1 = QString("");
	QString str2 = QString("");

	QRadioButton* rb = qobject_cast<QRadioButton*>(sender());
	bool matched = false;
	if (check)
	{
		m_module->setParamChanged(ProductLevel);

		if (rb == m_advDialog->ui.rb_onlyUpDw) {
			m_param->SideDetectType = 0;
			changeFixWidthValid();
			matched = true;
		}
		else if (rb == m_advDialog->ui.rb_onlyLfRt) {
			m_param->SideDetectType = 1;
			changeFixWidthValid();
			matched = true;
		}
		else if (rb == m_advDialog->ui.rb_onlyOneSide) {
			m_param->SideDetectType = 2;
			changeFixWidthValid();
			matched = true;
		}
		if (matched)
		{
			str1 = tr(" change to %1 ").arg(rb->text());
			//log(LogL1, str1);
			return;
		}
		if (rb == ui.rb_blackToWhite) {
			str0 = ui.label_20->text();
			str1 = tr(" change to %1 ").arg(rb->text());
			str2 = ui.tabWidget->tabText(m_nowGrpIndex);
			m_param->whiteBlackDir[m_nowGrpIndex] = !check;
		}
		else if (rb == ui.rb_whiteToBlack) {
			str0 = ui.label_20->text();
			str1 = tr(" change to %1 ").arg(rb->text());
			str2 = ui.tabWidget->tabText(m_nowGrpIndex);
			m_param->whiteBlackDir[m_nowGrpIndex] = check;
		}
		if (rb == ui.rb_inToOut) {
			str0 = ui.label_27->text();
			str1 = tr(" change to %1 ").arg(rb->text());
			str2 = ui.tabWidget->tabText(m_nowGrpIndex);
			m_param->searchDir[m_nowGrpIndex] = !check;
		}
		else if (rb == ui.rb_outToIn) {
			str0 = ui.label_27->text();
			str1 = tr(" change to %1 ").arg(rb->text());
			str2 = ui.tabWidget->tabText(m_nowGrpIndex);
			m_param->searchDir[m_nowGrpIndex] = check;
		}
		m_edgeLine[m_nowGrpIndex]->setPolarAndOrient((fitEdgeLine::Polarity)m_param->whiteBlackDir[m_nowGrpIndex], m_dir[m_param->searchDir[m_nowGrpIndex]][m_nowGrpIndex]);
		//log(LogL1, str2 + str0 + str1);
	}
}

void edgeLocationWdg::checkBox_toggled(bool check)
{

}