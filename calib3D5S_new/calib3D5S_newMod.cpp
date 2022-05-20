#include "calib3D5S_newMod.h"
#include "calib3D5S_newParam.h"
#include <QObject>
#include <ImageView.h>
#include <resultText.h>
#include "halOpenCV.h"

#define SizeOk "sizeOk"
#define SizeRow "sizeRow"
#define SizeCol "sizeCol"
#define SizeDist "sizeDist"
#define LineOk "lineOk"
#define LineRow "lineRow"
#define LineCol "lineCol"

#define StandLineOk "standlineOk"
#define StandLineRow "standlineRow"
#define StandLineCol "standlineCol"

#define PrjOK "prjOK"
#define PrjRow "prjRow"
#define PrjCol "prjCol"
#define PrjOffset "prjOffset"
#define PrjActualOffset "prjActualOffset"
#define StandSize "standSize"  //测出的四个台阶的值
#define StandOffset "standOffset" //识别的站高差值

calib3D5S_newMod::calib3D5S_newMod()
{
	iniData();
}

calib3D5S_newMod::calib3D5S_newMod(const QString& dirPath)
{
	iniData();
	load(dirPath, StructLevel);
}

calib3D5S_newMod::~calib3D5S_newMod()
{
	delete m_param;
	delete m_image;
}

int calib3D5S_newMod::command(int cmdID, void* dataIn, void* dataOut)
{
	return 0;
}

int calib3D5S_newMod::afterSetProperty(MetaProperty*)
{
	return 0;
}

void calib3D5S_newMod::save(const QString &dirPath)
{
	m_param->readWriteParam(false, dirPath, StructLevel);
	resetParamChanged();
}

void calib3D5S_newMod::load(const QString &dirPath, QvsParamLevel level)
{
	m_param->readWriteParam(true,dirPath,level);

	if (level == MachineLevel)
	{
		p_pixSize->clear();
		p_info->clear();
		p_lAng->clear();
		p_ratio->clear();
		p_aDepth->clear();

		for (int i = 0;i < 5;i++) {
			p_pixSize->push_back(m_param->pixelSize[i]);
			p_info->push_back(m_param->pixelSize[i]);
		}

		for (int i = 0;i < 4;i++) {
			p_lAng->push_back(m_param->angle[i]);
			p_info->push_back(m_param->angle[i]);
		}

		for (int i = 0;i < 4;i++) {
			p_ratio->push_back(m_param->prjRatio[i]);
			p_info->push_back(m_param->prjRatio[i]);
		}

		for (int i = 0;i < 4;i++) {
			p_aDepth->push_back(m_param->actualDepth[i]);
			p_info->push_back(m_param->actualDepth[i]);
		}
	}
}

int calib3D5S_newMod::run(const QString &funName)
{
	if (p_im->empty())
		return 0;
	HalOpenCV::cv2halImg(*p_im, m_image, false);

	detectSize();
	detectLine();
	detectPrj();
	detectStand();

	//setDetectOutData(OutTuple, m_outTuple);
	//return true;  //标定的返回结果无影响

	return 0;
}

void calib3D5S_newMod::viewResult(ImageView *iv, const QString &funName, int)
{
	iv->setColor(255, 0, 0);
	for (int i = 0;i < 5;i++) {
		HTuple ok, pr[2], pc[2];
		getDispOutData(SizeOk, i, ok);
		if (ok[0].I()) {
			for (int j = 0;j < 2;j++) {
				getDispOutData(SizeRow, i, j, pr[j]);
				getDispOutData(SizeCol, i, j, pc[j]);
				if (i < 3) {
					if (i == 0 && m_param->horizontalHorVerticalH == 0) {
						iv->dispLine(pr[j][0].D() - 5, pc[j][0].D(), pr[j][0].D() + 5, pc[j][0].D());
					}
					else {
						iv->dispLine(pr[j][0].D(), pc[j][0].D() - 5, pr[j][0].D(), pc[j][0].D() + 5);
					}
				}
				else
					iv->dispLine(pr[j][0].D() - 5, pc[j][0].D(), pr[j][0].D() + 5, pc[j][0].D());
			}
			iv->dispLine(pr[0][0].D(), pc[0][0].D(), pr[1][0].D(), pc[1][0].D());
		}
	}

	for (int i = 0;i < 4;i++) {
		HTuple ok, pr[2], pc[2];
		getDispOutData(LineOk, i, ok);
		if (ok[0].I()) {
			for (int j = 0;j < 2;j++) {
				getDispOutData(LineRow, i, j, pr[j]);
				getDispOutData(LineCol, i, j, pc[j]);
			}
			iv->dispLine(pr[0][0].D(), pc[0][0].D(), pr[1][0].D(), pc[1][0].D());
		}
		HTuple sok, spr[2], spc[2];
		getDispOutData(StandLineOk, i, sok);
		if (sok[0].I()) {
			for (int j = 0;j < 2;j++) {
				getDispOutData(StandLineRow, i, j, spr[j]);
				getDispOutData(StandLineCol, i, j, spc[j]);
			}
			iv->setColor(255, 0, 0);
			iv->dispLine(spr[0][0].D(), spc[0][0].D(), spr[1][0].D(), spc[1][0].D());
			iv->setColor(255, 0, 255);
			iv->dispLine(spr[0][0].D() - 3, spc[0][0].D(), spr[0][0].D() + 3, spc[0][0].D());
			iv->dispLine(spr[0][0].D(), spc[0][0].D() - 3, spr[0][0].D(), spc[0][0].D() + 3);
			iv->dispLine(spr[1][0].D() - 3, spc[1][0].D(), spr[1][0].D() + 3, spc[1][0].D());
			iv->dispLine(spr[1][0].D(), spc[1][0].D() - 3, spr[1][0].D(), spc[1][0].D() + 3);
		}
	}
	for (int i = 0;i < 4;i++) {
		HTuple ok, pr[2], pc[2];
		getDispOutData(PrjOK, i, ok);
		if (ok[0].I()) {
			for (int j = 0;j < 2;j++) {
				getDispOutData(PrjRow, i, j, pr[j]);
				getDispOutData(PrjCol, i, j, pc[j]);
			}
			iv->dispLine(pr[0][0].D(), pc[0][0].D(), pr[1][0].D(), pc[1][0].D());
		}
	}
}

void calib3D5S_newMod::textResult(ResultText *text, const QString &funName)
{
	//getDispOutData(PixelSize, pixelSize);
	text->setTextColor(QColor(0, 0, 0));
	bool allok = true;

	QString nameStr[5] = { QObject::tr("mid"),QObject::tr("left"),QObject::tr("right"),QObject::tr("Top"),QObject::tr("Bottom") };

	for (int i = 0;i < 5;i++) {
		HTuple ok, dist;
		getDispOutData(SizeOk, i, ok);
		getDispOutData(SizeDist, i, dist);
		if (ok[0].I()) {
			text->setTextColor(QColor(0, 0, 0));
			text->append(QObject::tr("%1:").arg(nameStr[i]));
			text->append(QObject::tr("Image Size= %1      Pixel Size= %2")
				.arg(QString::number(dist[0].D(), 'f', 3)).arg(m_param->pixelSize[i], 0, 'f', 5));
		}
		else {
			text->setTextColor(QColor(255, 0, 0));
			text->append(QObject::tr(" Failed to calculate the pixel size of region %1 ").arg(nameStr[i]));
			allok = false;
		}
	}
	text->append(QObject::tr(" "));

	//getDispOutData(LineAngle, lineAngle);
	for (int i = 0;i < 4;i++) {
		text->setTextColor(QColor(0, 0, 0));
		text->append(QObject::tr("%1's angle = %2").arg(nameStr[i + 1]).arg(m_param->angle[i] / PI*180.0, 0, 'f', 2));
	}
	text->append(QObject::tr(" "));

	//getDispOutData(PrjRatio, prjRatio);
	for (int i = 0;i < 4;i++) {
		HTuple ok, lineOk, standLineOk, offset, aOffset, standSize, standOffset;
		getDispOutData(PrjOK, i, ok);
		getDispOutData(LineOk, i, lineOk);
		getDispOutData(StandLineOk, i, standLineOk);
		if (ok[0].I() && lineOk[0].I() && standLineOk[0].I()) {
			getDispOutData(PrjOffset, i, offset);
			getDispOutData(PrjActualOffset, i, aOffset);

			text->append(QObject::tr("%1:").arg(nameStr[i + 1]));
			text->append(QObject::tr("Image offset= %1      Actual offset= %2")
				.arg(QString::number(offset[0].D(), 'f', 5)).arg(QString::number(aOffset[0].D(), 'f', 5)));
			text->append(QObject::tr("projected scale= %1").arg(m_param->prjRatio[i], 0, 'f', 5));

// 			getDispOutData(StandSize, i, standSize);
// 			getDispOutData(StandOffset, i, standOffset);
// 			sprintf(buf, "%.3f", standSize[0].D());
// 			m_lbRS[i]->setText(buf);
// 			sprintf(buf, "%.3f", standOffset[0].D());
// 			m_lbES[i]->setText(buf);
		}
		else {
			allok = false;
			text->setTextColor(QColor(255, 0, 0));
			text->append(QObject::tr(" Failed to calculate the projected scale of region %1 ").arg(nameStr[i + 1]));
		}
	}
	m_allok = allok;
	if (allok) {
		text->setTextColor(QColor(0, 255, 0));
		text->append(QObject::tr("Calibration is successful!"));
//		QMessageBox::information(this, QObject::tr("Message"), QObject::tr("Calibration is successful!"), QMessageBox::Yes);
	}
	else {
		text->setTextColor(QColor(255, 0, 0));
		text->append(QObject::tr("Calibration is failed"));
//		QMessageBox::warning(this, QObject::tr("Warning"), QObject::tr("Calibration is failed!"), QMessageBox::Yes);
	}
}

void calib3D5S_newMod::iniData()
{
	m_param = new calib3D5S_newParam;
	m_image = new Hobject;

	m_outTuple.Reset();

	for (auto i = 0;i < 4;++i)
	{
		m_lineOK[i] = false;
		m_standLineOK[i] = false;
		m_depthRow[i] = 0;
		m_depthCol[i] = 0;
		m_standAngle[i] = 0;
		for (auto j = 0;j < 2;++j)
		{
			m_lineR[i][j] = 0;
			m_lineC[i][j] = 0;
			m_depLineR[i][j] = 0;
			m_depLineC[i][j] = 0;
			m_standLineR[i][j] = 0;
			m_standLineC[i][j] = 0;
		}
	}
	m_allok = false;

	createPins();
	createEvents();
}

void calib3D5S_newMod::createPins()
{
	addPin(&p_im, "im");

	addPin(&p_pixSize, "Hsz");
	addPin(&p_lAng, "lAng");
	addPin(&p_ratio, "ratio");
	addPin(&p_aDepth, "aDepth");
	addPin(&p_info, "info");

	p_lAng.setVisible(false);
	p_ratio.setVisible(false);
	p_aDepth.setVisible(false);
	p_info.setVisible(false);
}

void calib3D5S_newMod::createEvents()
{
	addEvent(ENT_CALIB);
	addEvent(ENT_PRE);
}

void calib3D5S_newMod::detectSize()
{
	p_pixSize->clear();
	p_info->clear();

	long w, h;
	double pr[5][2], pc[5][2], dist[5];
	bool ok[5];
	get_image_size(*m_image, &w, &h);
	for (int i = 0;i < 5;i++) {
		double row = (m_param->sizeRow1[i] + m_param->sizeRow2[i]) / 2;
		double col = (m_param->sizeCol1[i] + m_param->sizeCol2[i]) / 2;
		double len1 = (m_param->sizeRow2[i] - m_param->sizeRow1[i]) / 2;
		double len2 = (m_param->sizeCol2[i] - m_param->sizeCol1[i]) / 2;
		long hm;
		HTuple er, ec, ea, ed;
		if (i == 0) {
			if (m_param->horizontalHorVerticalH == 0) {
				gen_measure_rectangle2(row, col, 0, len2, 3, w, h, "bilinear", &hm);
			}
			else {
				gen_measure_rectangle2(row, col, -M_PI / 2, len1, len2, w, h, "bilinear", &hm);
			}
		}
		else if (i > 0 && i < 3)
			gen_measure_rectangle2(row, col, -M_PI / 2, len1, len2, w, h, "bilinear", &hm);
		else
			gen_measure_rectangle2(row, col, 0, len2, len1, w, h, "bilinear", &hm);
		measure_pos(*m_image, hm, 1, 30, "all", "all", &er, &ec, &ea, &ed);
		close_measure(hm);
		if (ed.Num() > 0) {
			ok[i] = true;
			pr[i][0] = er[0].D();
			pc[i][0] = ec[0].D();
			pr[i][1] = er[ed.Num()].D();
			pc[i][1] = ec[ed.Num()].D();
			double dr = pr[i][1] - pr[i][0];
			double dc = pc[i][1] - pc[i][0];
			dist[i] = sqrt(dr*dr + dc*dc);
			m_param->pixelSize[i] = m_param->actualSize[i] / dist[i];
		}
		else {
			ok[i] = false;
			pr[i][0] = 0;
			pc[i][0] = 0;
			pr[i][1] = 0;
			pc[i][1] = 0;
			dist[i] = 0;
		}
		m_param->imgDist[i] = dist[i];
		setDetectOutData(SizeOk, ok[i], i);
		setDetectOutData(SizeDist, dist[i], i);
		for (int j = 0;j < 2;j++) {
			setDetectOutData(SizeRow, pr[i][j], i, j);
			setDetectOutData(SizeCol, pc[i][j], i, j);
		}
		p_pixSize->push_back(m_param->pixelSize[i]);
		p_info->push_back(m_param->pixelSize[i]);
	}
	//setDetectOutData(PixelSize, pixeSize);
}

void calib3D5S_newMod::detectLine()
{
	p_lAng->clear();

	long w, h;
	get_image_size(*m_image, &w, &h);
	double anlge[4] = { 0, -PI, -PI / 2, PI / 2 };
	for (int i = 0;i < 4;i++) {
		double cr = (m_param->lineRow2[i] + m_param->lineRow1[i]) / 2;
		double cc = (m_param->lineCol2[i] + m_param->lineCol1[i]) / 2;
		double dr = (m_param->lineRow2[i] - m_param->lineRow1[i]) / 2;
		double dc = (m_param->lineCol2[i] - m_param->lineCol1[i]) / 2;
		double len1 = 5, len2 = 5;
		bool ok = true;
		double pr[2], pc[2];
		for (int j = 0;j < 2;j++) {
			HTuple er, ec, ea, ed;
			if (i < 2) {
				len1 = dc;
				cr = j == 0 ? m_param->lineRow1[i] + 2 : m_param->lineRow2[i] - 2;
			}
			else {
				len1 = dr;
				cc = j == 0 ? m_param->lineCol1[i] + 2 : m_param->lineCol2[i] - 2;
			}
			long hm;
			gen_measure_rectangle2(cr, cc, anlge[i], len1, len2, w, h, "bilinear", &hm);
			measure_pos(*m_image, hm, 1, 20, "negative", "first", &er, &ec, &ea, &ed);
			close_measure(hm);
			if (er.Num() > 0) {
				pr[j] = er[0].D();
				pc[j] = ec[0].D();
				m_lineR[i][j] = pr[j];
				m_lineC[i][j] = pc[j];
				setDetectOutData(LineRow, pr[j], i, j);
				setDetectOutData(LineCol, pc[j], i, j);
			}
			else {
				ok = false;
			}
		}
		m_lineOK[i] = ok;
		setDetectOutData(LineOk, ok, i);
		if (ok) {
			double angle;
			if (i < 2) {
				angle = PI / 2 + atan((pc[1] - pc[0]) / (pr[1] - pr[0]));
			}
			else {
				angle = -atan((pr[1] - pr[0]) / (pc[1] - pc[0]));
			}
			m_param->angle[i] = angle;
		}
		p_lAng->push_back(m_param->angle[i]);
		p_info->push_back(m_param->angle[i]);
	}
	//setDetectOutData(LineAngle, lineAngle);

	HTuple standAngle;
	double stanlge[4] = { 0, -PI, -PI / 2, PI / 2 };
	for (int i = 0;i < 4;i++) {

		double cr = (m_param->standRow2[i] + m_param->standRow1[i]) / 2;
		double cc = (m_param->standCol2[i] + m_param->standCol1[i]) / 2;
		double dr = (m_param->standRow2[i] - m_param->standRow1[i]) / 2;
		double dc = (m_param->standCol2[i] - m_param->standCol1[i]) / 2;
		double len1 = 5, len2 = 5;
		bool ok = true;
		double pr[2], pc[2];
		for (int j = 0;j < 2;j++) {
			HTuple er, ec, ea, ed;
			if (m_param->horizontalHorVerticalH == 1) {
				len1 = dc;
				cr = j == 0 ? m_param->standRow1[i] + 2 : m_param->standRow2[i] - 2;
			}
			else {
				len1 = dr;
				cc = j == 0 ? m_param->standCol1[i] + 2 : m_param->standCol2[i] - 2;
			}
			long hm;
			gen_measure_rectangle2(cr, cc, stanlge[((int)!(bool)(m_param->horizontalHorVerticalH)) * 2 + i / 2], len1, len2, w, h, "bilinear", &hm);
			measure_pos(*m_image, hm, 1, 20, "negative", "first", &er, &ec, &ea, &ed);
			close_measure(hm);
			if (er.Num() > 0) {
				pr[j] = er[0].D();
				pc[j] = ec[0].D();
				m_standLineR[i][j] = pr[j];
				m_standLineC[i][j] = pc[j];
				setDetectOutData(StandLineRow, pr[j], i, j);
				setDetectOutData(StandLineCol, pc[j], i, j);
			}
			else {
				ok = false;
			}
		}
		m_standLineOK[i] = ok;
		setDetectOutData(StandLineOk, ok, i);
		if (ok) {
			double angle;
			if (m_param->horizontalHorVerticalH == 1) {
				angle = PI / 2 + atan((pc[1] - pc[0]) / (pr[1] - pr[0]));
			}
			else {
				angle = -atan((pr[1] - pr[0]) / (pc[1] - pc[0]));
			}
			m_standAngle[i] = angle;
		}
	}
	//m_outTuple.Append(lineAngle);
}

void calib3D5S_newMod::detectPrj()
{
	p_ratio->clear();
	p_aDepth->clear();

	long w, h;
	HTuple ratio;
	HTuple actualDepth;
	get_image_size(*m_image, &w, &h);
	double recangle[4] = { 0, -PI, -PI / 2, PI / 2 };
	for (int i = 0;i < 4;i++) {
		double cr = (m_param->depthRow2[i] + m_param->depthRow1[i]) / 2;
		double cc = (m_param->depthCol2[i] + m_param->depthCol1[i]) / 2;
		double dr = (m_param->depthRow2[i] - m_param->depthRow1[i]) / 2;
		double dc = (m_param->depthCol2[i] - m_param->depthCol1[i]) / 2;
		double len1 = 5, len2 = 5;
		bool ok = true;
		double pr[2], pc[2];

		for (int j = 0;j < 2;j++) {
			HTuple er, ec, ea, ed;
			if (i < 2) {
				len1 = dc;
				cr = j == 0 ? m_param->depthRow1[i] + 2 : m_param->depthRow2[i] - 2;
			}
			else {
				len1 = dr;
				cc = j == 0 ? m_param->depthCol1[i] + 2 : m_param->depthCol2[i] - 2;
			}
			long hm;
			gen_measure_rectangle2(cr, cc, recangle[i], len1, len2, w, h, "bilinear", &hm);
			measure_pos(*m_image, hm, 1, 20, "negative", "first", &er, &ec, &ea, &ed);
			close_measure(hm);
			if (er.Num() > 0) {
				pr[j] = er[0].D();
				pc[j] = ec[0].D();
				m_depLineR[i][j] = pr[j];
				m_depLineC[i][j] = pc[j];
				setDetectOutData(PrjRow, pr[j], i, j);
				setDetectOutData(PrjCol, pc[j], i, j);
			}
			else {
				ok = false;
			}
		}
		if (ok) {
			m_depthRow[i] = (m_depLineR[i][0] + m_depLineR[i][1]) / 2.0;
			m_depthCol[i] = (m_depLineC[i][0] + m_depLineC[i][1]) / 2.0;
			double dist;
			distance_pl(m_depthRow[i], m_depthCol[i], m_lineR[i][0], m_lineC[i][0], m_lineR[i][1], m_lineC[i][1], &dist);
			setDetectOutData(PrjOffset, dist, i);
			dist *= m_param->pixelSize[i + 1];
			setDetectOutData(PrjActualOffset, dist, i);
			m_param->prjRatio[i] = dist / m_param->actualDepth[i];

			p_ratio->push_back(m_param->prjRatio[i]);
			p_aDepth->push_back(m_param->actualDepth[i]);
			//ratio.Append(m_param->prjRatio[i]);
			//actualDepth.Append(m_param->actualDepth[i]);
			setDetectOutData(PrjOK, true, i);
		}
		else {
			setDetectOutData(PrjOK, false, i);
		}
	}
	for (auto i = 0;i < p_ratio->size();++i)
		p_info->push_back(p_ratio->at(i));
	for (auto i = 0;i < p_aDepth->size();++i)
		p_info->push_back(p_aDepth->at(i));

	//setDetectOutData(PrjRatio, ratio);
	//setDetectOutData(ActualDepth, actualDepth);
	//m_outTuple.Append(ratio);
	//m_outTuple.Append(actualDepth);
}

void calib3D5S_newMod::detectStand()
{
	for (int i = 0;i < 4;i++) {
		int DIdx;
		DIdx = !(m_param->horizontalHorVerticalH) * 2 + i / 2;
		if (m_standLineOK[i]) {
			double dist;
			double centerR, centerC;
			centerR = (m_standLineR[i][0] + m_standLineR[i][1]) / 2.0;
			centerC = (m_standLineC[i][0] + m_standLineC[i][1]) / 2.0;
			distance_pl(centerR, centerC, m_depLineR[DIdx][0], m_depLineC[DIdx][0], m_depLineR[DIdx][1], m_depLineC[DIdx][1], &dist);
			double anglestand = m_standAngle[i] / PI*180.0;
			double prjratioo = asin(m_param->prjRatio[DIdx]);
			double k1 = (1.0 / cos(prjratioo))*m_param->pixelSize[DIdx + 1];
			double pinstand = dist*(1.0 / cos(prjratioo))*m_param->pixelSize[DIdx + 1] - m_param->actualDepth[DIdx] * tan(prjratioo);
			
			m_param->standOffset[i] = pinstand - m_param->standH[i];
			m_param->standSize[i] = pinstand;
			setDetectOutData(StandSize, pinstand, i);
			setDetectOutData(StandOffset, pinstand - m_param->standH[i], i);
		}
	}
}
