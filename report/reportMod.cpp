#include "reportMod.h"
#include "reportParam.h"
#include <QObject>
#include <ImageView.h>
#include <resultText.h>
#include <algorithm>
#include <QMessageBox>
#include <QFileDialog>
#include <fstream>
#include <iomanip>
#include <QTextCodec>
#include "iniParam.h"

using namespace std;

#define PIN_OK "ok"
#define PIN_IT "it"

GRRreportMod::GRRreportMod()
{
	iniData();
}

GRRreportMod::GRRreportMod(const QString& dirPath)
{
	iniData();
	load(dirPath, StructLevel);
}

GRRreportMod::~GRRreportMod()
{
	delete m_param;
}

int GRRreportMod::command(int cmdID, void* dataIn, void* dataOut)
{
	return 0;
}

int GRRreportMod::afterSetProperty(MetaProperty*)
{
	return 0;
}

void GRRreportMod::save(const QString &dirPath)
{
	m_param->readWriteParam(false, dirPath, StructLevel);
	resetParamChanged();
}

void GRRreportMod::load(const QString &dirPath, QvsParamLevel level)
{
	m_param->readWriteParam(true,dirPath,level);

	if (level == StructLevel)
		updatePin();
}

int GRRreportMod::run(const QString &funName)
{
	m_detRes = NoRun;
	if (!m_ready)
	{
		if (isFinish())
			m_status = completed;
		else
		{
			m_status = idle;
			m_count = 0;
		}
		return 0;
	}
 
	bool r = true;
	for (auto i = 0; i < m_param->modNum; ++i)
		r = r && (***p_ok.at(i) == 0);
	if (r == false)
	{
		reset();
		m_detRes = OkFail;
		return -1;
	}

	if (m_count == 0)
	{
		m_size = QVector<QVector<int>>(m_param->modNum, QVector<int>(0));
		m_data = QVector<QVector<QVector<QVector<double>>>>(m_param->modNum, QVector<QVector<QVector<double>>>(0));
		for (auto i = 0; i < m_param->modNum; ++i)
		{
			int idx = 0;
			auto map = &(***p_it.at(i));
			m_data[i] = QVector<QVector<QVector<double>>>(map->size(), QVector<QVector<double>>(0));
			m_size[i] = QVector<int>(map->size(), 0);
			for (auto it = map->cbegin(); it != map->cend(); ++it, ++idx)
			{
				QList<QVariant> data = transToList(it.value());
				m_size[i][idx] = data.size();
				if (m_param->enable[i][idx])
					m_data[i][idx] = QVector<QVector<double>>(data.size(), QVector<double>(0));
				else
					m_data[i][idx] = QVector<QVector<double>>(0);
			}
		}
	}

 	m_count++;
	m_status = logging;
	for (auto i = 0; i < m_param->modNum; ++i)
	{
		int idx = 0;
		for (auto it = (***p_it.at(i)).cbegin(); it != (***p_it.at(i)).cend(); ++it, ++idx)
		{
			if(!m_param->enable[i][idx])
				continue;
			QList<QVariant> data = transToList(it.value());
			if (data.size() != m_size[i][idx])
			{
				reset();
				m_detRes = SizeWrong;
				return -1;
			}
			for (auto k = 0; k < data.size(); ++k)
				m_data[i][idx][k].append(data[k].toDouble());
		}
	}
 
	if (isFinish())
	{
		m_ready = false;
		m_status = completed;
		resetWidgets();
		calGrrData();
	}
	resetWidgets();

	return 0;
}

void GRRreportMod::viewResult(ImageView *iv, const QString &funName, int) {}

void GRRreportMod::textResult(ResultText *text, const QString &funName)
{
	if (!m_count)
		return;

	if (m_detRes == NoRun)
		return;
	else
	{
		text->setTextColor(Qt::red);
		if (m_detRes == OkFail)
			text->append(QObject::tr("GRR data error: Module detection is failed! Please restart GRR again!"));
		else if (m_detRes == SizeWrong)
			text->append(QObject::tr("GRR data error: The unit's size of data is not equal to the No.1! Please restart GRR again!"));
		else
		{
			text->setTextColor(Qt::green);
			text->append(QObject::tr("Count: %1/%2").arg(m_count).arg(m_param->InputUnits * 9));
		}
	}
}

void GRRreportMod::iniData()
{
	m_param = new GRRreportParam;
	createPins();
	resetGrrData();
	reset();
	m_ready = false;
	m_size.clear();
	m_detRes = NoRun;
	m_count = 0;

	QTextCodec::setCodecForLocale(QTextCodec::codecForName("GBK"));
}

void GRRreportMod::createPins()
{

}

void GRRreportMod::reset()
{
	m_ready = false;
	m_count = 0;
	m_status = idle;
	m_data.clear();
	resetWidgets();
}

bool GRRreportMod::isFinish()
{
	return m_count == m_param->InputUnits * 9;
}

void GRRreportMod::resetGrrData()
{
	m_R = QVector<QVector<QVector<QVector<double>>>>(m_param->modNum, QVector<QVector<QVector<double>>>());
	m_Rabar = QVector<QVector<QVector<QVector<double>>>>(m_param->modNum, QVector<QVector<QVector<double>>>());
	m_Rbar = QVector<QVector<QVector<double>>>(m_param->modNum, QVector<QVector<double>>());

	m_Xabar = QVector<QVector<QVector<QVector<double>>>>(m_param->modNum, QVector<QVector<QVector<double>>>());
	m_Xbardiff = QVector<QVector<QVector<double>>>(m_param->modNum, QVector<QVector<double>>());

	m_cycle1Mean = QVector<QVector<QVector<QVector<double>>>>(m_param->modNum, QVector<QVector<QVector<double>>>());
	m_cycle3Mean = QVector<QVector<QVector<QVector<double>>>>(m_param->modNum, QVector<QVector<QVector<double>>>());
	m_Rp = QVector<QVector<QVector<double>>>(m_param->modNum, QVector<QVector<double>>());

	m_GRR = QVector<QVector<QVector<double>>>(m_param->modNum, QVector<QVector<double>>());
	m_perGRR = QVector<QVector<QVector<double>>>(m_param->modNum, QVector<QVector<double>>());
	for (auto i = 0; i < m_param->modNum; ++i)
	{
		m_R[i] = QVector<QVector<QVector<double>>>(m_param->itemNum[i], QVector<QVector<double>>());
		m_Rabar[i] = QVector<QVector<QVector<double>>>(m_param->itemNum[i], QVector<QVector<double>>());
		m_Rbar[i] = QVector<QVector<double>>(m_param->itemNum[i], QVector<double>());

		m_Xabar[i] = QVector<QVector<QVector<double>>>(m_param->itemNum[i], QVector<QVector<double>>());
		m_Xbardiff[i] = QVector<QVector<double>>(m_param->itemNum[i], QVector<double>());

		m_cycle1Mean[i] = QVector<QVector<QVector<double>>>(m_param->itemNum[i], QVector<QVector<double>>());
		m_cycle3Mean[i] = QVector<QVector<QVector<double>>>(m_param->itemNum[i], QVector<QVector<double>>());
		m_Rp[i] = QVector<QVector<double>>(m_param->itemNum[i], QVector<double>());

		m_GRR[i] = QVector<QVector<double>>(m_param->itemNum[i], QVector<double>());
		m_perGRR[i] = QVector<QVector<double>>(m_param->itemNum[i], QVector<double>());

		for (auto k = 0; k < m_param->itemNum[i]; ++k)
		{
			m_R[i][k] = QVector<QVector<double>>(m_size[i][k], QVector<double>());
			m_Rabar[i][k] = QVector<QVector<double>>(m_size[i][k], QVector<double>());
			m_Rbar[i][k] = QVector<double>(m_size[i][k], 0);

			m_Xabar[i][k] = QVector<QVector<double>>(m_size[i][k], QVector<double>());
			m_Xbardiff[i][k] = QVector<double>(m_size[i][k], 0);

			m_cycle1Mean[i][k] = QVector<QVector<double>>(m_size[i][k], QVector<double>());
			m_cycle3Mean[i][k] = QVector<QVector<double>>(m_size[i][k], QVector<double>());
			m_Rp[i][k] = QVector<double>(m_size[i][k], 0);

			m_GRR[i][k] = QVector<double>(m_size[i][k], 0);
			m_perGRR[i][k] = QVector<double>(m_size[i][k], 0);
		}
	}
}

void GRRreportMod::calGrrData()
{
	resetGrrData();

	//基于MSA计算方式 3人3轮
	double D4 = 2.58;
	double K1 = 0.5908;
	double K2 = 0.5231;
	double K3[9] = { 0.701, 0.5231, 0.4467, 0.4030, 0.3742, 0.3534, 0.3375, 0.3249, 0.3146 };//取决于料的个数，从2-10依次取值
	
	double min, max, tol, avge;
	for (auto i = 0; i < m_param->modNum; ++i)
		for (auto k = 0; k < m_param->itemNum[i]; ++k)
		{
			if (!m_param->enable[i][k])
				continue;

			for (auto l = 0; l < m_size[i][k]; ++l)
			{
				double sumR = 0, sumX = 0;
				for (auto m = 0; m < m_param->InputUnits * 9;)
				{
					int grpIdx = m / (m_param->InputUnits * 3);
					int grp1 = m;
					int grp2 = m + m_param->InputUnits * 1;
					int grp3 = m + m_param->InputUnits * 2;

					minMaxOf3(m_data[i][k][l][grp1], m_data[i][k][l][grp2], m_data[i][k][l][grp3], min, max, tol);
					avge = (m_data[i][k][l][grp1] + m_data[i][k][l][grp2] + m_data[i][k][l][grp3]) / 3.0;

					m_R[i][k][l].append(tol);
					m_cycle1Mean[i][k][l].append(avge);

					sumR += tol;
					sumX += avge;

					if ((m + 1) % m_param->InputUnits == 0)
					{
						m_Rabar[i][k][l].append(sumR / m_param->InputUnits);
						m_Xabar[i][k][l].append(sumX / m_param->InputUnits);

						sumR = 0;
						sumX = 0;
					}
					if (m >= m_param->InputUnits * 6)
					{
						int unitIdx = m%m_param->InputUnits;
						int cyc1 = unitIdx + m_param->InputUnits * 0;
						int cyc2 = unitIdx + m_param->InputUnits * 1;
						int cyc3 = unitIdx + m_param->InputUnits * 2;
						m_cycle3Mean[i][k][l].append((m_cycle1Mean[i][k][l][cyc1] + m_cycle1Mean[i][k][l][cyc2] + m_cycle1Mean[i][k][l][cyc3]) / 3.0);
					}

					++m;
					if (m % m_param->InputUnits == 0)
						m += m_param->InputUnits * 2;
				}

				m_Rbar[i][k][l] = (m_Rabar[i][k][l][0] + m_Rabar[i][k][l][1] + m_Rabar[i][k][l][2]) / 3.0;
				minMaxOf3(m_Xabar[i][k][l][0], m_Xabar[i][k][l][1], m_Xabar[i][k][l][2], min, max, tol);
				m_Xbardiff[i][k][l] = tol;
				QVector<double> cycle3Mean = m_cycle3Mean[i][k][l];
				std::stable_sort(cycle3Mean.begin(), cycle3Mean.end());
				m_Rp[i][k][l] = fabs(cycle3Mean.front() - cycle3Mean.back());

				double EV = m_Rbar[i][k][l] * K1;								//重复性
				double val = pow(m_Xbardiff[i][k][l] * K2, 2) - (pow(EV, 2) / (m_param->InputUnits * 3));
				double AV = val < 0 ? 0 : sqrt(val);							//再现性
				m_GRR[i][k][l] = sqrt(pow(EV, 2) + pow(AV, 2));
				double tol = m_param->IsGrrByTol ? fabs(m_param->USL[i][k] - m_param->LSL[i][k]) : m_Rp[i][k][l];
				double PV = tol * K3[m_param->InputUnits - 2];					//零件变差
				double TV = sqrt(pow(m_GRR[i][k][l], 2) + pow(PV, 2));			//总变差
				m_perGRR[i][k][l] = 100 * m_GRR[i][k][l] / TV;
				if (_isnan(m_perGRR[i][k][l]))
					m_perGRR[i][k][l] = 0;
			}
		}

	auto select = QMessageBox::information(nullptr, QObject::tr("information"),
		QObject::tr("The default output is a csv file. Should it be changed to an html file (read-only)?"),
		QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
	if (select == QMessageBox::Yes)
		genGrrReport_html();
	else if (select == QMessageBox::No)
		genGrrReport_xlsx();
}

void GRRreportMod::minMaxOf3(double a, double b, double c, double & min, double & max, double & tol)
{
	min = std::min<double>(a, b);
	min = std::min<double>(c, min);

	max = std::max<double>(a, b);
	max = std::max<double>(c, max);

	tol = fabs(max - min);
}

void GRRreportMod::genGrrReport_xlsx()
{
	QString filename = QFileDialog::getSaveFileName(nullptr, QString("save Files"), QString("./%1").arg(m_param->GrrLotName), QString("GRR files (*.csv)"));

	if (filename.size() <= 0)
		return;

	QFile file(filename);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
		return;

	QTextStream textstream(&file);
	textstream << "Report Type:" << ", " << "GRR\n\n";
	textstream << "Report Name:" << ", " << m_param->GrrLotName << ", , " << "Station:" << ", " << "ST_Lead3D5S\n";
	textstream << "Date Time:" << ", " << QDateTime::currentDateTime().toString("yy/MM/dd_HH:mm") << ", , " << "Distance Unit:" << ", " << "MilliMeter\n";

	textstream << "\n\n";
	textstream << "Measurements Summary\n";
	textstream << "Items" << "," << "Xbardiff" << "," << "Avg Rbar" << "," << "Rp" << ", " << "GRR" << ", " << "GRR%\n";
	for (auto i = 0; i < m_param->modNum; ++i)
		for (auto k = 0; k < m_param->itemNum[i]; ++k)
		{
			if (!m_param->enable[i][k])
				continue;
			for (auto l = 0; l < m_size[i][k]; ++l)
			{
				char idx[256] = "";
				if (m_size[i][k] != 1)
					sprintf_s(idx, "%d", l + 1);
				textstream 
					<< m_param->itText[i][k] << idx << ","
					<< QString::number(m_Xbardiff[i][k][l], 'f', 4) << ","
					<< QString::number(m_Rbar[i][k][l], 'f', 4) << ","
					<< QString::number(m_Rp[i][k][l], 'f', 4) << ","
					<< QString::number(m_GRR[i][k][l], 'f', 4) << ","
					<< QString::number(m_perGRR[i][k][l], 'f', 4) << "\n";
			}
			textstream << "\n";
		}

	if (!m_param->IsOutAll)
		return;
	textstream << "Inspection:" << "\n";
	for (auto i = 0; i < m_param->modNum; ++i)
		for (auto k = 0; k < m_param->itemNum[i]; ++k)
		{
			if (!m_param->enable[i][k])
				continue;
			for (auto l = 0; l < m_size[i][k]; ++l)
			{
				char idx[256] = "";
				if (m_size[i][k] != 1)
					sprintf_s(idx, "%d", l + 1);
				textstream << m_param->itText[i][k] << idx << "\n";
				textstream << ","
					<< "Operator1" << "," << "," << "," << ","
					<< "Operator2" << "," << "," << "," << ","
					<< "Operator3" << "," << "," << "," << "\n";
				textstream << " , " 
					<< "Cycle 1" << ", " << "Cycle 2" << ", " << "Cycle 3" << ", " << "Range" << ", "
					<< "Cycle 1" << ", " << "Cycle 2" << ", " << "Cycle 3" << ", " << "Range" << ", "
					<< "Cycle 1" << ", " << "Cycle 2" << ", " << "Cycle 3" << ", " << "Range" << "," 
					<< "Avg" << "\n";
				for (auto j = 0; j < m_param->InputUnits; ++j)
				{
					textstream << "C" << j + 1 << ",";
					textstream
						<< QString::number(m_data[i][k][l][j + m_param->InputUnits * 0], 'f', 4) << ","
						<< QString::number(m_data[i][k][l][j + m_param->InputUnits * 1], 'f', 4) << ","
						<< QString::number(m_data[i][k][l][j + m_param->InputUnits * 2], 'f', 4) << ","
						<< QString::number(m_R[i][k][l][j + m_param->InputUnits * 0], 'f', 4) << ",";
					textstream
						<< QString::number(m_data[i][k][l][j + m_param->InputUnits * 3], 'f', 4) << ","
						<< QString::number(m_data[i][k][l][j + m_param->InputUnits * 4], 'f', 4) << ","
						<< QString::number(m_data[i][k][l][j + m_param->InputUnits * 5], 'f', 4) << ","
						<< QString::number(m_R[i][k][l][j + m_param->InputUnits * 1], 'f', 4) << ",";
					textstream
						<< QString::number(m_data[i][k][l][j + m_param->InputUnits * 6], 'f', 4) << ","
						<< QString::number(m_data[i][k][l][j + m_param->InputUnits * 7], 'f', 4) << ","
						<< QString::number(m_data[i][k][l][j + m_param->InputUnits * 8], 'f', 4) << ","
						<< QString::number(m_R[i][k][l][j + m_param->InputUnits * 2], 'f', 4) << ",";
					textstream << QString::number(m_cycle3Mean[i][k][l][j], 'f', 4) << "\n";
				}
				textstream << "Rbar" << ", " 
					<< ", , , " << QString::number(m_Rabar[i][k][l][0], 'f', 4) << ", "
					<< ", , , " << QString::number(m_Rabar[i][k][l][1], 'f', 4) << ", "
					<< ", , , " << QString::number(m_Rabar[i][k][l][2], 'f', 4) << "\n\n";
				textstream << "Summary" << "\n";
				textstream << "Xbardiff" << ", " << QString::number(m_Xbardiff[i][k][l], 'f', 4) << "\n";
				textstream << "Avg.Rbar" << ", " << QString::number(m_Rbar[i][k][l], 'f', 4) << "\n";
				textstream << "Rp" << ", " << QString::number(m_Rp[i][k][l], 'f', 4) << "\n";
				textstream << "GRR" << ", " << QString::number(m_GRR[i][k][l], 'f', 4) << "\n";
				textstream << "GRR%" << ", " << QString::number(m_perGRR[i][k][l], 'f', 4) << "\n";
			}
			textstream << "\n\n";
		}

}

void GRRreportMod::genGrrReport_html()
{
	QString filename = QFileDialog::getSaveFileName(nullptr, QString("save Files"), QString("./%1").arg(m_param->GrrLotName), QString("GRR files (*.html)"));

	if (filename.size() <= 0)
		return;

	fstream report_html(filename.toLocal8Bit().constData(), ios::out | ios::trunc);

	report_html << "<head>";
	report_html << "<meta http-equiv=\"Content-type\" content=\"text/css;charset=GB2312\"/>";
	report_html << setiosflags(ios::fixed) << setprecision(4);       //保留小数点后4位

	 //title   
	report_html << "<br>" << "<br>";
	report_html << "<table style=\"font-size:24px;font-family:SimHei; border-collapse:collapse;\" >";
	report_html << "<tr align=\"left\">";
	report_html << "<td width=\"300px\">" << "Report Type:" << "</td>";
	report_html << "<th width=\"300px\">" << "GRR" << "</th>";
	report_html << "</tr>";

	report_html << "<tr>";
	report_html << "<td>" << "&nbsp" << "</td>";
	report_html << "<td>" << "&nbsp" << "</td>";
	report_html << "</tr>";

	report_html << "<tr align=\"left\">";
	report_html << "<td>" << "Station:" << "</td>";
	report_html << "<th>" << "ST_Lead3D5S" << "</th>";
	report_html << "</tr>";

	report_html << "<tr align=\"left\">";
	report_html << "<td>" << "Date Time:" << "</td>";
	report_html << "<th  colspan=\"2\">" << QDateTime::currentDateTime().toString("yy/MM/dd_HH:mm").toLocal8Bit().constData() << "</th>";
	report_html << "</tr>";

	report_html << "<tr align=\"left\">";
	report_html << "<td>" << "Distance Unit:" << "</td>";
	report_html << "<th>" << "MilliMeter" << "</th>";
	report_html << "</tr>";
	report_html << "</table>";
	report_html << "<br>" << "<br>" << "<br>" << "<br>";

	report_html << "<table  style=\"font-size:28px;font-family:SimHei;\">";
	report_html << "<tr>";
	report_html << "<th width=\"1280px\" align=\"center\">" << "Measurements Summary" << "</th>";
	report_html << "</tr>";
	report_html << "</table>";
	report_html << "<br>" ;

	report_html << "<table border=\"1\" style=\"font-size:18px;font-family:SimHei; border-collapse:collapse;\" >";
	report_html << "<tr align=\"center\" bgcolor=\"#AAAAAA\">";
	report_html << "<th width=\"230px\">" << "Items" << "</th>";
	report_html << "<th width=\"230px\">" << "Xbardiff" << "</th>";
	report_html << "<th width=\"230px\">" << "Avg.Rbar" << "</th>";
	report_html << "<th width=\"230px\">" << "Rp" << "</th>";
	report_html << "<th width=\"230px\">" << "GRR" << "</th>";
	report_html << "<th width=\"230px\">" << "GRR%" << "</th>";
	report_html << "</tr>";
	for (auto i = 0; i < m_param->modNum; ++i)
		for (auto k = 0; k < m_param->itemNum[i]; ++k)
		{
			if(!m_param->enable[i][k])
				continue;
			for (auto l = 0; l < m_size[i][k]; ++l)
			{
				char idx[256] = "";
				if (m_size[i][k] != 1)
					sprintf_s(idx, "%d", l + 1);

				report_html << "<tr>";
				report_html << "<td align=\"left\">" << m_param->itText[i][k].toLocal8Bit().constData() << idx << "</td>";
				report_html << "<td align=\"right\">" << m_Xbardiff[i][k][l] << "</td>";
				report_html << "<td align=\"right\">" << m_Rbar[i][k][l] << "</td>";
				report_html << "<td align=\"right\">" << m_Rp[i][k][l] << "</td>";
				report_html << "<td align=\"right\">" << m_GRR[i][k][l] << "</td>";
				report_html << "<td align=\"right\">" << m_perGRR[i][k][l] << "</td>";
				report_html << "</tr>";
			}
			if (k != m_param->itemNum[i] - 1)
			{
				report_html << "<tr>";
				report_html << "<td>" << "&nbsp" << "</td>";
				report_html << "<td>" << "&nbsp" << "</td>";
				report_html << "<td>" << "&nbsp" << "</td>";
				report_html << "<td>" << "&nbsp" << "</td>";
				report_html << "<td>" << "&nbsp" << "</td>";
				report_html << "</tr>";
			}

		}
	report_html << "</table>";
	report_html << "<br>";

	if (m_param->IsOutAll)
	{
		report_html << "<br>" << "<br>";
		report_html << "<table style=\"font-size:28px;font-family:SimHei; border-collapse:collapse;\">";
		report_html << "<tr>";
		report_html << "<th width=\"1380px\">" << "Inspection" << "</th>";      //400*3+80+100
		report_html << "</tr>";
		report_html << "</table>";
		report_html << "<br>";

		for (auto i = 0; i < m_param->modNum; ++i)
			for (auto k = 0; k < m_param->itemNum[i]; ++k)
			{
				if (!m_param->enable[i][k])
					continue;

				for (auto l = 0; l < m_size[i][k]; ++l)
				{
					char idx[256] = "";
					if (m_size[i][k] != 1)
						sprintf_s(idx, "%d", l + 1);

					report_html << "<table style=\"font-size:24px;font-family:SimHei; border-collapse:collapse;\">";
					report_html << "<tr>";
					report_html << "<th width=\"1380px\">" << m_param->itText[i][k].toLocal8Bit().constData() << idx << "</th>";      //400*3+80+100
					report_html << "</tr>";
					report_html << "</table>";

					report_html << "<table border=\"1\" style=\"font-size:18px;font-family:SimHei; border-collapse:collapse;\">";

					report_html << "<tr align=\"center\"  bgcolor=\"#AAAAAA\">";
					report_html << "<td width=\"80px\">" << "&nbsp" << "</td>";
					report_html << "<td width=\"400px\" colspan=\"4\">" << "Operator1" << "</td>";       //100px*4
					report_html << "<td width=\"400px\" colspan=\"4\">" << "Operator2" << "</td>";
					report_html << "<td width=\"400px\" colspan=\"4\">" << "Operator3" << "</td>";
					report_html << "<td width=\"100px\">" << "&nbsp" << "</td>";
					report_html << "</tr>";

					report_html << "<tr align=\"center\" bgcolor=\"#AAAAAA\">";
					report_html << "<td>" << "&nbsp" << "</td>";
					report_html << "<td>" << "Cycle1" << "</td>" << "<td>" << "Cycle2" << "</td>" << "<td>" << "Cycle3" << "</td>" << "<td>" << "Range" << "</td>";
					report_html << "<td>" << "Cycle1" << "</td>" << "<td>" << "Cycle2" << "</td>" << "<td>" << "Cycle3" << "</td>" << "<td>" << "Range" << "</td>";
					report_html << "<td>" << "Cycle1" << "</td>" << "<td>" << "Cycle2" << "</td>" << "<td>" << "Cycle3" << "</td>" << "<td>" << "Range" << "</td>";
					report_html << "<td>" << "Avg" << "</td>";
					report_html << "</tr>";

					for (auto j = 0; j < m_param->InputUnits; ++j)
					{
						report_html << "<tr align=\"right\">";
						report_html << "<td align=\"left\">" << "C" << j + 1 << "</td>";

						report_html << "<td>" << m_data[i][k][l][j + m_param->InputUnits * 0] << "</td>";
						report_html << "<td>" << m_data[i][k][l][j + m_param->InputUnits * 1] << "</td>";
						report_html << "<td>" << m_data[i][k][l][j + m_param->InputUnits * 2] << "</td>";
						report_html << "<td>" << m_R[i][k][l][j + m_param->InputUnits * 0] << "</td>";

						report_html << "<td>" << m_data[i][k][l][j + m_param->InputUnits * 3] << "</td>";
						report_html << "<td>" << m_data[i][k][l][j + m_param->InputUnits * 4] << "</td>";
						report_html << "<td>" << m_data[i][k][l][j + m_param->InputUnits * 5] << "</td>";
						report_html << "<td>" << m_R[i][k][l][j + m_param->InputUnits * 1] << "</td>";

						report_html << "<td>" << m_data[i][k][l][j + m_param->InputUnits * 6] << "</td>";
						report_html << "<td>" << m_data[i][k][l][j + m_param->InputUnits * 7] << "</td>";
						report_html << "<td>" << m_data[i][k][l][j + m_param->InputUnits * 8] << "</td>";
						report_html << "<td>" << m_R[i][k][l][j + m_param->InputUnits * 2] << "</td>";

						report_html << "<td>" << m_cycle3Mean[i][k][l][j] << "</td>";
						report_html << "</tr>";
					}
					report_html << "<tr align=\"right\">";
					report_html << "<td align=\"left\">" << "Rbar" << "</td>";
					report_html << "<td>" << "-" << "</td>";
					report_html << "<td>" << "-" << "</td>";
					report_html << "<td>" << "-" << "</td>";
					report_html << "<td>" << m_Rabar[i][k][l][0] << "</td>";

					report_html << "<td>" << "-" << "</td>";
					report_html << "<td>" << "-" << "</td>";
					report_html << "<td>" << "-" << "</td>";
					report_html << "<td>" << m_Rabar[i][k][l][1] << "</td>";

					report_html << "<td>" << "-" << "</td>";
					report_html << "<td>" << "-" << "</td>";
					report_html << "<td>" << "-" << "</td>";
					report_html << "<td>" << m_Rabar[i][k][l][2] << "</td>";
					report_html << "</tr>";
					report_html << "</table>";

					report_html << "<br>";
					report_html << "<table border=\"1\" style=\"font-size:20px;font-family:SimHei; border-collapse:collapse;\">";
					report_html << "<tr>";
					report_html << "<th colspan=\"2\" width=\"380px\" align=\"center\" bgcolor=\"#AAAAAA\">" << "Summary" << "</th>";
					report_html << "</tr>";

					report_html << "<tr>";
					report_html << "<td  width=\"180px\" align=\"left\">" << "Xbardiff" << "</td>";
					report_html << "<td  width=\"200px\" align=\"right\">" << m_Xbardiff[i][k][l] << "</td>";
					report_html << "</tr>";

					report_html << "<tr>";
					report_html << "<td align=\"left\">" << "Avg.Rbar" << "</td>";
					report_html << "<td align=\"right\">" << m_Rbar[i][k][l] << "</td>";
					report_html << "</tr>";

					report_html << "<tr>";
					report_html << "<td align=\"left\">" << "Rp" << "</td>";
					report_html << "<td align=\"right\">" << m_Rp[i][k][l] << "</td>";
					report_html << "</tr>";

					report_html << "<tr>";
					report_html << "<td align=\"left\">" << "GRR" << "</th>";
					report_html << "<td align=\"right\">" << m_GRR[i][k][l] << "</td>";
					report_html << "</tr>";

					report_html << "<tr>";
					report_html << "<td align=\"left\">" << "GRR%" << "</th>";
					report_html << "<td align=\"right\">" << m_perGRR[i][k][l] << "</td>";
					report_html << "</tr>";

					report_html << "</table>";
					report_html << "<br>" << "<br>";
				}
				report_html << "<br>" << "<br>";
			}
	}		

	report_html.close();
}

void GRRreportMod::updatePin()
{
	if (p_ok.size() == m_param->modNum)
		return;
	while (p_ok.size() > m_param->modNum)
	{
		removePin(pinName(PIN_OK, p_ok.size() - 1));
		removePin(pinName(PIN_IT, p_ok.size() - 1));
		p_ok.pop_back();
		p_it.pop_back();
	}
	while (p_ok.size() < m_param->modNum)
	{
		UnitInputPin<int> *ok = new UnitInputPin<int>;
		UnitInputPin<QMap<QString, QVariant>> *it = new UnitInputPin<QMap<QString, QVariant>>;
		addPin(ok, pinName(PIN_OK, p_ok.size()));
		addPin(it, pinName(PIN_IT, p_ok.size()));
		p_ok.push_back(ok);
		p_it.push_back(it);
	}
}

QString GRRreportMod::pinName(QString prefix, int idx)
{
	return QString("%1%2").arg(prefix).arg(idx);
}

QList<QVariant> GRRreportMod::transToList(QVariant var)
{
	if (var.toList().size() != 0)
		return var.toList();
	else
		return QList<QVariant>({ var.toDouble() });
}


calibReportMod::calibReportMod()
{
	iniData();
}

calibReportMod::calibReportMod(const QString& dirPath)
{
	iniData();
	load(dirPath, StructLevel);
}

calibReportMod::~calibReportMod()
{
}

int calibReportMod::command(int cmdID, void* dataIn, void* dataOut)
{
	return 0;
}

int calibReportMod::afterSetProperty(MetaProperty*)
{
	return 0;
}

void calibReportMod::save(const QString &dirPath)
{
	RWParam(false, dirPath, StructLevel);
	resetParamChanged();
}

void calibReportMod::load(const QString &dirPath, QvsParamLevel level)
{
	RWParam(true, dirPath, level);
}

void calibReportMod::iniData()
{
	m_is3D = false;
	m_name = QString("");
	createPins();
}

void calibReportMod::createPins()
{
	addPin(&p_calibInfo, "info");
}

void calibReportMod::RWParam(bool r, QString dirPath, int level)
{
	SETTING_INIT(dirPath + "/GRRreport.ini");

	if (r&&level != ProductLevel)
		return;

	RW_VALUE(r, m_is3D);
	RW_VALUE(r, m_name);
}

void calibReportMod::genCalibReport_html(int ispix)
{
	if ((m_is3D&&p_calibInfo->size() != 17) || (!m_is3D&&p_calibInfo->size() != 1))
	{
		QMessageBox::warning(nullptr, tr("Warning!"), tr(" Data Stream wrong, please check it"), QMessageBox::Yes);
		return;
	}

	QString filename = QFileDialog::getSaveFileName(nullptr, QString::fromLocal8Bit("save Files"), QString::fromLocal8Bit("./CalibLeadReport"), QString::fromLocal8Bit("Calibration Report (*.html)"));
	if (filename.size() <= 0)
		return;

	fstream report_html(filename.toLocal8Bit().constData(), ios::out | ios::trunc);
	if (ispix == 0) {
		int index = -1;

		report_html << "<head>";
		report_html << "<meta http-equiv=\"Content-type\" content=\"text/css;charset=GB2312\"/>";
		report_html << setiosflags(ios::fixed) << setprecision(4);       //保留小数点后四位

		report_html << "<table style=\"font-size:28px;font-family:SimHei\">";
		report_html << "<tr>";
		report_html << "<td width=\"1300px\" align=\"center\">" << "CALIBRATION SUCCEED" << "</td>";
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
		for (int i = 0; i < 4; i++)
			report_html << "<td align=\"right\">" << p_calibInfo->at(++index) << "</td>";
		report_html << "<td align=\"right\">" << p_calibInfo->at(0) << "</td>";
		report_html << "<td align=\"left\">" << "MillimeterPerPixel" << "</td>";
		report_html << "</tr>";

		report_html << "<tr>";
		report_html << "<td align=\"left\">" << "Base Angle" << "</td>";
		for (int i = 0; i < 4; i++)
			report_html << "<td align=\"right\">" << p_calibInfo->at(++index) << "</td>";
		report_html << "<td align=\"right\">" << "&nbsp" << "</td>";
		report_html << "<td align=\"left\">" << "Degree" << "</td>";
		report_html << "</tr>";

		report_html << "<tr>";
		report_html << "<td align=\"left\">" << "Ratio" << "</td>";
		for (int i = 0; i < 4; i++)
			report_html << "<td align=\"right\">" << p_calibInfo->at(++index) << "</td>";
		report_html << "<td align=\"right\">" << "&nbsp" << "</td>";
		report_html << "<td align=\"left\">" << "&nbsp" << "</td>";
		report_html << "</tr>";

		report_html << "<tr>";
		report_html << "<td align=\"left\">" << "Actual Depth" << "</td>";
		for (int i = 0; i < 4; i++)
			report_html << "<td align=\"right\">" << p_calibInfo->at(++index) << "</td>";
		report_html << "<td align=\"right\">" << "&nbsp" << "</td>";
		report_html << "<td align=\"left\">" << "Millimeter" << "</td>";
		report_html << "</tr>";
		report_html << "</table>";
	}
	else if (ispix == 1)
	{
		int index = -1;
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
		report_html << "<td align=\"center\">" << p_calibInfo->at(0) << "</td>";
		report_html << "<td align=\"center\">" << "&nbsp" << "</td>";
		report_html << "<td align=\"center\">" << "&nbsp" << "</td>";
		report_html << "<td align=\"center\">" << "Millimeter/Pixel" << "</td>";
		report_html << "</tr>";
		report_html << "</table>";
	}
	report_html.close();
}

void calibReportMod::genCalibReport_xlsx(int ispix)
{
	if ((m_is3D&&p_calibInfo->size() != 17) || (!m_is3D&&p_calibInfo->size() != 1))
	{
		QMessageBox::warning(nullptr, tr("Warning!"), tr(" Data Stream wrong, please check it"), QMessageBox::Yes);
		return;
	}

	QString filename = QFileDialog::getSaveFileName(nullptr, QString::fromLocal8Bit("save Files"), QString::fromLocal8Bit("./CalibLeadReport"), QString::fromLocal8Bit("Calibration Report (*.csv)"));
	if (filename.size() <= 0)
		return;

	QFile file(filename);
	int index = -1;
	if (file.open(QIODevice::WriteOnly | QIODevice::Text))
	{
		QTextStream textstream(&file);
		if (ispix == 0)
		{
			textstream << "Calibration Success!\n\n";
			textstream << "Report Type:" << ", " << "Calibration Data\n\n";
			textstream << "Report Station:" << ", " << "Lead3D5S\n";
			textstream << "Date Time:" << ", " << QDateTime::currentDateTime().toString("MM/dd_HH:mm").toLocal8Bit().constData() << "\n";/*<<", , "<<"Distance Unit:"<<", "<<"MilliMeter\n"*/

			textstream << "Measurement" << "," << "Left" << "," << "Right" << "," << "Up" << "," << "Down" << "," << "Middle" << "," << "Unit" << "\n";
			textstream << "Pixel Size " << ", ";
			index++;
			for (int i = 0; i < 4; i++) {
				textstream << QString::number(p_calibInfo->at(++index), 'f', 6) << ", ";
			}
			textstream << QString::number(p_calibInfo->at(0), 'f', 6) << ", ";
			textstream << "MillimeterPerPixel" << "\n";

			textstream << "Base Angle " << ", ";
			for (int i = 0; i < 4; i++) {
				textstream << QString::number(p_calibInfo->at(++index), 'f', 6) << ", ";
			}
			textstream << ", ";
			textstream << "degree" << "\n";

			textstream << "Ratio " << ", ";
			for (int i = 0; i < 4; i++) {
				textstream << QString::number(p_calibInfo->at(++index), 'f', 6) << ", ";
			}
			textstream << "\n";

			textstream << "Actual Depth" << ", ";
			for (int i = 0; i < 4; i++) {
				textstream << QString::number(p_calibInfo->at(++index), 'f', 6) << ", ";
			}
			textstream << ", ";
			textstream << "Millimeter" << "\n";
		}
		else
		{
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
			textstream << QString::number(p_calibInfo->at(0), 'f', 6) << ", " << ", " << ", ";
			textstream << "MillimeterPerPixel" << "\n";
		}
		file.close();
	}
}
