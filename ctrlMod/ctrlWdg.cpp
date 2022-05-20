#include "CtrlWdg.h"
#include "CtrlMod.h"
#include "ui_CtrlWdg.h"
#include <QLineEdit>
#include <QMessageBox>
#include <QFileDialog>
#include <fstream>
#include <iomanip>
#include <QTreeWidgetItem>

using namespace std;

CtrlWdg::CtrlWdg(int i) :
	ModSetWidget(), ui(new Ui::CtrlWdg), m_widgetType(i)
{
	m_hasConnect = false;
	ui->setupUi(this);
	iniUi();
	le_stName.clear();
	lb_stIdx.clear();
	m_param = nullptr;
	m_module = nullptr;
}

CtrlWdg::~CtrlWdg()
{
	delete ui;

	for (auto it : lb_stIdx)
		delete it;
	for (auto it : le_stName)
		delete it;
	delete m_layout;
	delete treeWdg_item[0];
	delete treeWdg_item[1];
}

void CtrlWdg::setModule(UnitModule *module)
{
	m_module = dynamic_cast<CtrlMod*>(module);
	m_param = m_module->m_param;
// 	connectSlots(false);
// 	setUiValue();
// 	connectSlots(true);
}

void CtrlWdg::showEvent(QShowEvent * event)
{
	connectSlots(false);
	setUiValue();
	connectSlots(true);
}

bool CtrlWdg::eventFilter(QObject * watched, QEvent * event)
{
	QSpinBox *sp = qobject_cast<QSpinBox *>(watched);
	QComboBox *cob = qobject_cast<QComboBox *>(watched);
	if (sp == ui->sp_stNum&&event->type() == QEvent::Wheel)
		return true;
	else if (cob == ui->cb_stIdx&&event->type() == QEvent::Wheel)
		return true;
	else
		return QObject::eventFilter(watched, event);
}

void CtrlWdg::sp_valueChanged(int val)
{
	QSpinBox *sp = qobject_cast<QSpinBox *>(sender());

	if (sp == ui->sp_stNum)
	{
		int idx = ui->cb_stIdx->currentIndex();
		if (val > m_param->stNum)
			for (auto i = m_param->stNum;i < val;++i)
			{
				m_param->stName << QString("station%1").arg(i);
				ui->cb_stIdx->addItem(QString("station%1").arg(i));
			}
		else if (val < m_param->stNum)
			for (auto i = m_param->stNum - 1;i >= val;--i)
			{
				m_param->stName.pop_back();
				int cnt = ui->cb_stIdx->count();
				ui->cb_stIdx->removeItem(cnt - 1);
			}
		int cnt = ui->cb_stIdx->count();
		if (cnt > 0 && idx > cnt)
			ui->cb_stIdx->setCurrentIndex(0);
		else if (cnt > 0 && idx <= cnt)
			ui->cb_stIdx->setCurrentIndex(idx);

		ui->gb_advance->setVisible(val);
		m_param->stNum = val;
		ui->bt_genCalibReport->setEnabled(m_param->stNum);
		setTextEditNum();
		m_module->setStationNum();
		m_module->setParamChanged(StructLevel);
	}
}

void CtrlWdg::bt_clicked()
{
	QPushButton *bt = qobject_cast<QPushButton *>(sender());
	if (bt == ui->bt_genCalibReport)
	{
		bool ispix = ui->rb_st_normal->isChecked();
		int st = ui->cb_stIdx->currentIndex();
		if (st > m_module->p_calibInfo.size())
		{
			QMessageBox::warning(this, QString("warning!"), tr("data wrong!"), QMessageBox::Yes);
			return;
		}

		int r = QMessageBox::information(this, tr("information"),
			tr("The default output is a csv file. Should it be changed to an html file (read-only)?"),
			QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
		if (r == QMessageBox::Cancel)
			return;
		else if (r == QMessageBox::No)
			genCalibReport_xlsx(st, ispix);
		else if (r == QMessageBox::Yes)
			genCalibReport_html(st, ispix);
	}
	else if (ui->bt_clearData)
	{
		int r = QMessageBox::information(this, tr("information"),
			tr("Are you want to clear all product data?"), QMessageBox::Yes | QMessageBox::No);
		if (r == QMessageBox::Yes)
		{
			m_module->triggerEvent(ENT_CLEAR);
		}
	}
}

void CtrlWdg::cb_toggled(bool checked)
{
//	QCheckBox *cb = qobject_cast<QCheckBox *>(sender());
}

void CtrlWdg::le_textEdited(const QString & str)
{
	QLineEdit *le = qobject_cast<QLineEdit *>(sender());

	for (auto i = 0;i < le_stName.size();++i)
		if (le == le_stName[i])
		{
			m_param->stName[i] = str;
			ui->cb_stIdx->setItemText(i, m_param->stName[i]);
			break;
		}

	m_module->setParamChanged(ProductLevel);
}

void CtrlWdg::connectSlots(bool link)
{
	if (m_hasConnect == link)
		return;
	m_hasConnect = link;

	connectOrNot(link, ui->sp_stNum, SIGNAL(valueChanged(int)), SLOT(sp_valueChanged(int)));
	connectOrNot(link, ui->bt_genCalibReport, SIGNAL(clicked()), SLOT(bt_clicked()));
	connectOrNot(link, ui->bt_clearData, SIGNAL(clicked()), SLOT(bt_clicked()));
}

void CtrlWdg::iniUi()
{
	m_layout = new QGridLayout(ui->gb_advance);

	ui->sp_stNum->installEventFilter(this);
	ui->cb_stIdx->installEventFilter(this);

	ui->rb_st_normal->setChecked(true);
	ui->rb_st_3D->setChecked(false);

	treeWdg_item[0] = new QTreeWidgetItem(ui->treeWidget->topLevelItem(0));
	treeWdg_item[1] = new QTreeWidgetItem(ui->treeWidget->topLevelItem(1));
	ui->treeWidget->setHeaderHidden(true);
	ui->treeWidget->setItemWidget(treeWdg_item[0], 0, ui->frame_clearData);		//清楚数据
	ui->treeWidget->setItemWidget(treeWdg_item[1], 0, ui->frame_calibReport);	//标定报告
}

void CtrlWdg::setUiValue()
{
	if (!m_param)
		return;

	if (m_widgetType == 1)	//simple
	{
		ui->gb_advance->setVisible(false);
		ui->lb_stNum->setVisible(false);
		ui->sp_stNum->setVisible(false);
	}
	else
	{
		ui->gb_advance->setVisible(m_param->stNum);
		ui->lb_stNum->setVisible(true);
		ui->sp_stNum->setVisible(true);
	}

	ui->sp_stNum->setValue(m_param->stNum);

	int idx = ui->cb_stIdx->currentIndex();
	int cnt = ui->cb_stIdx->count();
	if (cnt > m_param->stNum)
		for (auto i = cnt - 1;i >= m_param->stNum;--i)
		{
			int cnt_n = ui->cb_stIdx->count();
			ui->cb_stIdx->removeItem(cnt_n - 1);
		}
	else if (cnt < m_param->stNum)
		for (auto i = cnt;i < m_param->stNum;++i)
		{
			ui->cb_stIdx->addItem(m_param->stName[i]);
		}
	int cnt_n = ui->cb_stIdx->count();
	if (idx > cnt_n)
		ui->cb_stIdx->setCurrentIndex(-1);
	else
		ui->cb_stIdx->setCurrentIndex(idx);

	ui->bt_genCalibReport->setEnabled(m_param->stNum);

	setTextEditNum();
}

void CtrlWdg::setTextEditNum()
{
	int sz = le_stName.size();
	if (sz == m_param->stNum)
		return;
	else if (sz > m_param->stNum)
		for (auto i = sz - 1;i >= m_param->stNum;--i)
		{
			m_layout->removeWidget(lb_stIdx[i]);
			m_layout->removeWidget(le_stName[i]);

			delete lb_stIdx[i];
			delete le_stName[i];
			lb_stIdx.pop_back();
			le_stName.pop_back();
		}
	else if (sz < m_param->stNum)
		for (auto i = sz;i < m_param->stNum;++i)
		{
			QLabel *lb = new QLabel;
			lb->setText(QString("st%1 name:").arg(i));
			lb_stIdx.push_back(lb);
			QLineEdit *le = new QLineEdit;
			le->setText(m_param->stName[i]);
			le_stName.push_back(le);

			m_layout->addWidget(lb, i, 0);
			m_layout->addWidget(le, i, 1);

			connect(le, SIGNAL(textEdited(const QString &)), SLOT(le_textEdited(const QString &)));
		}
}

void CtrlWdg::genCalibReport_html(int st, int ispix)
{
	auto info = m_module->p_calibInfo[st];

	int calibErr = 0;   //0正常, 1数据流错误，2计算错误
	int num;
	try {
		HTuple outtuple;
		for (auto i = 0;i < (*info)->size();++i)
			outtuple[i] = (*info)->at(i);
		//m_station[st]->getOutputData(0, outtuple, true);
		try {
			num = outtuple.Num();
		}
		catch (HException& ex) {
			calibErr = 1;
			QMessageBox::warning(this, tr("Warning!"), tr(" Data Stream wrong, please check it"), QMessageBox::Yes);
			return;
		}
		if (ispix == 0) {
			if (num != 17) {
				calibErr = 2;
			}
			else
			{
				QString filename = QFileDialog::getSaveFileName(this, QString::fromLocal8Bit("save Files"), QString::fromLocal8Bit("./CalibLeadReport"), QString::fromLocal8Bit("Calibration Report (*.html)"));
				if (filename.size() <= 0)
					return;

				int index = -1;
				fstream report_html(filename.toLocal8Bit().constData(), ios::out | ios::trunc);

				report_html << "<head>";
				report_html << "<meta http-equiv=\"Content-type\" content=\"text/css;charset=GB2312\"/>";
				report_html << setiosflags(ios::fixed) << setprecision(4);       //保留小数点后四位

				report_html << "<table style=\"font-size:28px;font-family:SimHei\">";
				report_html << "<tr>";
				report_html << "<td width=\"1300px\" align=\"right\">" << "CALIBRATION SUCCEED" << "</td>";
				report_html << "</tr>";
				report_html << "</table>";
				report_html << "<br>";

				report_html << "<table style=\"font-size:28px;font-family:SimHei\">";
				report_html << "<tr>";
				report_html << "<td width=\"1300px\" align=\"center\">" << "CALIBRATION SUMMARY REPORT" << "</td>";
				report_html << "</tr>";
				report_html << "</table>";
				report_html << "<br>";

				report_html << "<table style=\"font-size:22px;font-family:SimHei\">";
				report_html << "<tr align=\"left\">";
				report_html << "<td width=\"200px\">" << "Report Type:" << "</td>";
				report_html << "<th width=\"200px\">" << "Calibration Data" << "</th>";
				report_html << "</tr>";

				report_html << "<tr align=\"left\">";
				report_html << "<td width=\"200px\">" << "Report Station:" << "</td>";
				report_html << "<th width=\"200px\">" << "Lead3D5S" << "</th>";
				report_html << "</tr>";

				report_html << "<tr align=\"left\">";
				report_html << "<td width=\"200px\">" << "Date Time:" << "</td>";
				report_html << "<th width=\"200px\">" << QDateTime::currentDateTime().toString("yy/MM/dd_HH:mm").toLocal8Bit().constData() << "</th>";
				report_html << "</tr>";
				report_html << "</table>";
				report_html << "<br>" << "<br>" << "<br>";

				report_html << "<table border=\"1\" style=\"font-size:20px;font-family:SimHei; border-collapse:collapse;\">";
				//首行参数指标
				report_html << "<tr  align=\"center\" bgcolor=\"#AAAAAA\">";
				report_html << "<td width=\"300px\">" << "Measurement" << "</td>";
				report_html << "<td width=\"150px\">" << "Left" << "</td>";
				report_html << "<td width=\"150px\">" << "Right" << "</td>";
				report_html << "<td width=\"150px\">" << "Up" << "</td>";
				report_html << "<td width=\"150px\">" << "Down" << "</td>";
				report_html << "<td width=\"150px\">" << "Middle" << "</td>";
				report_html << "<td width=\"250px\">" << "Unit" << "</td>";
				report_html << "</tr>";

				index++;

				report_html << "<tr>";
				report_html << "<td align=\"left\">" << "Pixel Size" << "</td>";
				for (int i = 0;i < 4;i++)
					report_html << "<td align=\"right\">" << outtuple[++index].D() << "</td>";
				report_html << "<td align=\"right\">" << outtuple[0].D() << "</td>";
				report_html << "<td align=\"left\">" << "MillimeterPerPixel" << "</td>";
				report_html << "</tr>";

				report_html << "<tr>";
				report_html << "<td align=\"left\">" << "Base Angle" << "</td>";
				for (int i = 0;i < 4;i++)
					report_html << "<td align=\"right\">" << outtuple[++index].D() << "</td>";
				report_html << "<td align=\"right\">" << "&nbsp" << "</td>";
				report_html << "<td align=\"left\">" << "Degree" << "</td>";
				report_html << "</tr>";

				report_html << "<tr>";
				report_html << "<td align=\"left\">" << "Ratio" << "</td>";
				for (int i = 0;i < 4;i++)
					report_html << "<td align=\"right\">" << outtuple[++index].D() << "</td>";
				report_html << "<td align=\"right\">" << "&nbsp" << "</td>";
				report_html << "<td align=\"left\">" << "&nbsp" << "</td>";
				report_html << "</tr>";

				report_html << "<tr>";
				report_html << "<td align=\"left\">" << "Actual Depth" << "</td>";
				for (int i = 0;i < 4;i++)
					report_html << "<td align=\"right\">" << outtuple[++index].D() << "</td>";
				report_html << "<td align=\"right\">" << "&nbsp" << "</td>";
				report_html << "<td align=\"left\">" << "Millimeter" << "</td>";
				report_html << "</tr>";
				report_html << "</table>";

				report_html.close();
			}
		}
		else if (ispix == 1)
		{
			if (num != /*3*/1) {	//station3平台目前只支持输出一个数据
				calibErr = 2;
			}
			else {
				QString filename = QFileDialog::getSaveFileName(this, QString::fromLocal8Bit("save Files"), QString::fromLocal8Bit("./CalibPixelReport"), QString::fromLocal8Bit("Calibration Report (*.html)"));
				if (filename.size() <= 0)
					return;

				int index = -1;
				fstream report_html(filename.toLocal8Bit().constData(), ios::out | ios::trunc);

				report_html << "<head>";
				report_html << "<meta http-equiv=\"Content-type\" content=\"text/css;charset=GB2312\"/>";

				report_html << "<table border=\"1\">";
				report_html << "<tr>";
				report_html << "<th>" << "Report Type:" << "</th>";
				report_html << "<th>" << "Calibration Data" << "</th>";
				report_html << "</tr>";

				report_html << "<tr>";
				report_html << "<th>" << "Report Station:" << "</th>";
				report_html << "<th>" << "LeadPixelSize" << "</th>";
				report_html << "</tr>";

				report_html << "<tr>";
				report_html << "<th>" << "Date Time:" << "</th>";
				report_html << "<th>" << QDateTime::currentDateTime().toString("MM/dd_HH:mm").toLocal8Bit().constData() << "</th>";
				report_html << "</tr>";
				report_html << "</table>";

				report_html << "<table border=\"1\">";
				report_html << "<tr>";
				report_html << "<th>" << "Measurement" << "</th>";
				report_html << "<th>" << "Value" << "</th>";
				report_html << "<th>" << "Min" << "</th>";
				report_html << "<th>" << "Max" << "</th>";
				report_html << "<th>" << "Unit" << "</th>";
				report_html << "</tr>";

				index++;

				// 				report_html << "<tr>";
				// 				report_html << "<th>" << "Actual Size" << "</th>";
				// 				report_html << "<td align=\"center\">" << outtuple[0].D() << "</td>";
				// 				report_html << "<td align=\"center\">" << "&nbsp" << "</td>";
				// 				report_html << "<td align=\"center\">" << "&nbsp" << "</td>";
				// 				report_html << "<td align=\"center\">" << "Millimeter" << "</td>";
				// 				report_html << "</tr>";
				// 
				// 				report_html << "<tr>";
				// 				report_html << "<th>" << "Test Value" << "</th>";
				// 				report_html << "<td align=\"center\">" << outtuple[1].D() << "</td>";
				// 				report_html << "<td align=\"center\">" << outtuple[1].D() << "</td>";
				// 				report_html << "<td align=\"center\">" << outtuple[1].D() << "</td>";
				// 				report_html << "<td align=\"center\">" << "Pixel" << "</td>";
				// 				report_html << "</tr>";
				// 
				// 				report_html << "<tr>";
				// 				report_html << "<th>" << "Pixel Size" << "</th>";
				// 				report_html << "<td align=\"center\">" << outtuple[2].D() << "</td>";
				// 				report_html << "<td align=\"center\">" << "&nbsp" << "</td>";
				// 				report_html << "<td align=\"center\">" << "&nbsp" << "</td>";
				// 				report_html << "<td align=\"center\">" << "MillimeterPerPixel" << "</td>";
				// 				report_html << "</tr>";

				report_html << "<tr>";
				report_html << "<th>" << "Pixel Size" << "</th>";
				report_html << "<td align=\"center\">" << outtuple[0].D() << "</td>";
				report_html << "<td align=\"center\">" << "&nbsp" << "</td>";
				report_html << "<td align=\"center\">" << "&nbsp" << "</td>";
				report_html << "<td align=\"center\">" << "Millimeter/Pixel" << "</td>";
				report_html << "</tr>";
				report_html << "</table>";

				report_html.close();
			}
		}
	}
	catch (HException& ex) {
		calibErr = 2;
	}
	if (calibErr == 1) {

	}
	else if (calibErr == 2) {
		QMessageBox::warning(this, tr("Warning!"), tr(" OutPut Data  wrong, please check it"), QMessageBox::Yes);
		return;
	}
}

void CtrlWdg::genCalibReport_xlsx(int st, int ispix)
{
	auto info = m_module->p_calibInfo[st];

	int calibErr = 0;   //0正常, 1数据流错误，2计算错误
	int num;
	try {
		HTuple outtuple;
		for (auto i = 0;i < (*info)->size();++i)
			outtuple[i] = (*info)->at(i);
		//m_station[st]->getOutputData(0, outtuple, true);
		try {
			num = outtuple.Num();
		}
		catch (HException& ex) {
			calibErr = 1;
			QMessageBox::warning(this, tr("Warning!"), tr(" Data Stream  wrong, please check it"), QMessageBox::Yes);
			return;
		}
		if (ispix == 0) {
			if (num != 17) {
				calibErr = 2;
			}
			else {
				QString filename = QFileDialog::getSaveFileName(this, QString::fromLocal8Bit("save Files"), QString::fromLocal8Bit("./CalibLeadReport"), QString::fromLocal8Bit("Calibration Report (*.csv)"));
				if (filename.size() > 0) {
					QFile file(filename);
					int index = -1;
					if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
						QTextStream textstream(&file);
						textstream << "Calibration Success!\n\n";
						textstream << "Report Type:" << ", " << "Calibration Data\n\n";
						textstream << "Report Station:" << ", " << "Lead3D5S\n";
						textstream << "Date Time:" << ", " << QDateTime::currentDateTime().toString("MM/dd_HH:mm").toLocal8Bit().constData() << "\n";/*<<", , "<<"Distance Unit:"<<", "<<"MilliMeter\n"*/

						textstream << "Measurement" << "," << "Left" << "," << "Right" << "," << "Up" << "," << "Down" << "," << "Middle" << "," << "Unit" << "\n";
						textstream << "Pixel Size " << ", ";
						index++;
						for (int i = 0;i < 4;i++) {
							textstream << QString::number(outtuple[++index].D(), 'f', 6) << ", ";
						}
						textstream << QString::number(outtuple[0].D(), 'f', 6) << ", ";
						textstream << "MillimeterPerPixel" << "\n";

						textstream << "Base Angle " << ", ";
						for (int i = 0;i < 4;i++) {
							textstream << QString::number(outtuple[++index].D(), 'f', 6) << ", ";
						}
						textstream << ", ";
						textstream << "degree" << "\n";

						textstream << "Ratio " << ", ";
						for (int i = 0;i < 4;i++) {
							textstream << QString::number(outtuple[++index].D(), 'f', 6) << ", ";
						}
						textstream << "\n";

						textstream << "Actual Depth" << ", ";
						for (int i = 0;i < 4;i++) {
							textstream << QString::number(outtuple[++index].D(), 'f', 6) << ", ";
						}
						textstream << ", ";
						textstream << "Millimeter" << "\n";
						file.close();
					}
				}
			}
		}
		else if (ispix == 1) {
			if (num != /*3*/1) {
				calibErr = 2;
			}
			else {
				QString filename = QFileDialog::getSaveFileName(this, QString::fromLocal8Bit("save Files"), QString::fromLocal8Bit("./CalibPixelReport"), QString::fromLocal8Bit("Calibration Report (*.csv)"));
				if (filename.size() > 0) {
					QFile file(filename);
					int index = -1;
					if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
						QTextStream textstream(&file);
						textstream << "Calibration Success!\n\n";
						textstream << "Report Type:" << ", " << "Calibration Data\n\n";
						textstream << "Report Station:" << ", " << "LeadPixelSize\n";
						textstream << "Date Time:" << ", " << QDateTime::currentDateTime().toString("MM/dd_HH:mm").toLocal8Bit().constData() << "\n";/*<<", , "<<"Distance Unit:"<<", "<<"MilliMeter\n"*/

						textstream << "Measurement" << "," << "value" << "," << "min" << "," << "max" << "," << "Unit" << "\n";

						// 						textstream << "Actual Size " << ", ";
						// 						textstream << QString::number(outtuple[0].D(), 'f', 6) << ", " << ", " << ", ";
						// 						textstream << "Millimeter" << "\n";
						// 
						// 						textstream << "Test Value" << ", ";
						// 						textstream << QString::number(outtuple[1].D(), 'f', 6) << ", " << QString::number(outtuple[1].D(), 'f', 6) << ", "
						// 							<< QString::number(outtuple[1].D(), 'f', 6) << ", ";
						// 						textstream << "pixel " << "\n";
						// 
						// 						textstream << " Pixel Size " << ", ";
						// 						textstream << QString::number(outtuple[2].D(), 'f', 6) << ", " << ", " << ", ";
						// 						textstream << "MillimeterPerPixel" << "\n";
						textstream << " Pixel Size " << ", ";
						textstream << QString::number(outtuple[0].D(), 'f', 6) << ", " << ", " << ", ";
						textstream << "MillimeterPerPixel" << "\n";
						file.close();
					}
				}
			}
		}
	}
	catch (HException& ex) {
		calibErr = 2;
	}
	if (calibErr == 1) {

	}
	else if (calibErr == 2) {
		QMessageBox::warning(this, tr("Warning!"), tr(" OutPut Data  wrong, please check it"), QMessageBox::Yes);
		return;
	}
}

