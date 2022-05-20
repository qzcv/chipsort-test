#include "calib3D5SMod.h"
#include "calib3D5SParam.h"
#include <QObject>
#include <ImageView.h>
#include <resultText.h>
#include "halOpenCV.h"

calib3D5SMod::calib3D5SMod()
{
	iniData();
}

calib3D5SMod::calib3D5SMod(const QString& dirPath)
{
	iniData();
	load(dirPath, MachineLevel);
}

calib3D5SMod::~calib3D5SMod()
{
	delete m_param;
	delete m_image;
}

int calib3D5SMod::command(int cmdID, void* dataIn, void* dataOut)
{
	return 0;
}

int calib3D5SMod::afterSetProperty(MetaProperty*)
{
	return 0;
}

void calib3D5SMod::save(const QString &dirPath)
{
	m_param->readWriteParam(false, dirPath, MachineLevel);
	resetParamChanged();
}

void calib3D5SMod::load(const QString &dirPath, QvsParamLevel level)
{
	m_param->readWriteParam(true,dirPath,level);

	if (level == MachineLevel)
	{
		p_pixelSize->clear();
		p_calibInfo->clear();
		p_lineAngle->clear();
		p_prjRatio->clear();
		p_actualDepth->clear();

		for (int i = 0;i < 5;i++) {
			p_pixelSize->push_back(m_param->pixelSize[i]);
			p_calibInfo->push_back(m_param->pixelSize[i]);
		}

		for (int i = 0;i < 4;i++) {
			p_lineAngle->push_back(m_param->angle[i]);
			p_calibInfo->push_back(m_param->angle[i]);
		}

		for (int i = 0;i < 4;i++) {
			p_prjRatio->push_back(m_param->prjRatio[i]);
			p_calibInfo->push_back(m_param->prjRatio[i]);
		}
		
		for (int i = 0;i < 4;i++) {
			p_actualDepth->push_back(m_param->actualDepth[i]);
			p_calibInfo->push_back(m_param->actualDepth[i]);
		}
	}
}

int calib3D5SMod::run(const QString &funName)
{
	if (p_im->empty())
		return -1;

	HalOpenCV::cv2halImg(*p_im, m_image, false);
	detectSize();
	detectLine();
	detectPrj();

	setParamChanged(MachineLevel);

	return 0;
}

void calib3D5SMod::viewResult(ImageView *iv, const QString &funName, int)
{
	iv->setColor(Qt::red);
	for (int i = 0;i < 5;i++) {
		if (m_sizeOK[i]) {
			for (int j = 0;j < 2;j++) {
				if (i < 3) {
					if (i == 0 && m_param->horizontalHorVerticalH == 0) {
						iv->dispLine(m_sizeRow[i][j] - 5, m_sizeCol[i][j], m_sizeRow[i][j] + 5, m_sizeCol[i][j]);
					}
					else {
						iv->dispLine(m_sizeRow[i][j], m_sizeCol[i][j] - 5, m_sizeRow[i][j], m_sizeCol[i][j] + 5);
					}
				}
				else
					iv->dispLine(m_sizeRow[i][j] - 5, m_sizeCol[i][j], m_sizeRow[i][j] + 5, m_sizeCol[i][j]);
			}
			iv->dispLine(m_sizeRow[i][0], m_sizeCol[i][0], m_sizeRow[i][1], m_sizeCol[i][1]);
		}
	}

	for (int i = 0;i < 4;i++) {
		if (m_lineOK[i]) {
			iv->dispLine(m_lineRow[i][0], m_lineCol[i][0], m_lineRow[i][1], m_lineCol[i][1]);
		}
	}

	for (int i = 0;i < 4;i++) {
		if (m_prjOK[i]) {
			if (i < 2) {
				double len = (m_param->depthRow2[i] - m_param->depthRow1[i]) / 2;
				iv->dispLine(m_prjRow[i] - len, m_prjCol[i], m_prjRow[i] + len, m_prjCol[i]);
			}
			else {
				double len = (m_param->depthCol2[i] - m_param->depthCol1[i]) / 2;
				iv->dispLine(m_prjRow[i], m_prjCol[i] - len, m_prjRow[i], m_prjCol[i] + len);
			}
		}
	}
// 	for (int i = 0;i < 5;i++) {
// 		HTuple ok, pr[2], pc[2];
// 		getDispOutData(SizeOk, i, ok);
// 		if (ok[0].I()) {
// 			for (int j = 0;j < 2;j++) {
// 				getDispOutData(SizeRow, i, j, pr[j]);
// 				getDispOutData(SizeCol, i, j, pc[j]);
// 				if (i < 3) {
// 					if (i == 0 && m_param.horizontalHorVerticalH == 0) {
// 						m_view->dispLine(pr[j][0].D() - 5, pc[j][0].D(), pr[j][0].D() + 5, pc[j][0].D());
// 					}
// 					else {
// 						m_view->dispLine(pr[j][0].D(), pc[j][0].D() - 5, pr[j][0].D(), pc[j][0].D() + 5);
// 					}
// 				}
// 				else
// 					m_view->dispLine(pr[j][0].D() - 5, pc[j][0].D(), pr[j][0].D() + 5, pc[j][0].D());
// 			}
// 			m_view->dispLine(pr[0][0].D(), pc[0][0].D(), pr[1][0].D(), pc[1][0].D());
// 		}
// 	}
// 
// 	for (int i = 0;i < 4;i++) {
// 		HTuple ok, pr[2], pc[2];
// 		getDispOutData(LineOk, i, ok);
// 		if (ok[0].I()) {
// 			for (int j = 0;j < 2;j++) {
// 				getDispOutData(LineRow, i, j, pr[j]);
// 				getDispOutData(LineCol, i, j, pc[j]);
// 			}
// 			m_view->dispLine(pr[0][0].D(), pc[0][0].D(), pr[1][0].D(), pc[1][0].D());
// 		}
// 	}
// 
// 	for (int i = 0;i < 4;i++) {
// 		HTuple ok, pr, pc;
// 		getDispOutData(PrjOK, i, ok);
// 		if (ok[0].I()) {
// 			getDispOutData(PrjRow, i, pr);
// 			getDispOutData(PrjCol, i, pc);
// 			if (i < 2) {
// 				double len = (m_param.depthRow2[i] - m_param.depthRow1[i]) / 2;
// 				m_view->dispLine(pr[0].D() - len, pc[0].D(), pr[0].D() + len, pc[0].D());
// 			}
// 			else {
// 				double len = (m_param.depthCol2[i] - m_param.depthCol1[i]) / 2;
// 				m_view->dispLine(pr[0].D(), pc[0].D() - len, pr[0].D(), pc[0].D() + len);
// 			}
// 
// 		}
// 	}
}

void calib3D5SMod::textResult(ResultText *text, const QString &funName)
{
	m_allok = true;
	QString nameStr[5] =
	{ QObject::tr("mid"),QObject::tr("left"),QObject::tr("right"),QObject::tr("Top"),QObject::tr("Bottom") };

	text->setTextColor(Qt::black);
	for (int i = 0;i < 5;i++) {
		if (m_sizeOK[i]) {
			text->setTextColor(Qt::black);
			text->append(QObject::tr("%1:").arg(nameStr[i]));
			text->append(QObject::tr("Image Size= %1      Pixel Size= %2")
				.arg(m_sizeDist[i], 0, 'f', 3).arg(m_param->pixelSize[i], 0, 'f', 5));
		}
		else {
			text->setTextColor(Qt::red);
			text->append(QObject::tr(" Failed to calculate the pixel size of region %1\n").arg(nameStr[i]));
			m_allok = false;
		}
	}

	for (int i = 0;i < 4;i++) {
		text->setTextColor(Qt::black);
		text->append(QObject::tr("%1's angle = %2\n").arg(nameStr[i + 1])
			.arg(m_param->angle[i] / PI*180.0, 0, 'f', 2));
	}
	
	for (int i = 0;i < 4;i++) {
		if (m_prjOK[i] && m_lineOK[i]) {
			text->append(QObject::tr("%1:").arg(nameStr[i + 1]));
			text->append(QObject::tr("Image offset= %1      Actual offset= %2")
				.arg(m_param->prjOffset[i], 0, 'f', 5).arg(m_param->prjActualOffset[i], 0, 'f', 5));
			text->append(QObject::tr("projected scale= %1").arg(m_param->prjRatio[i], 0, 'f', 5));
		}
		else {
			m_allok = false;
			text->setTextColor(Qt::red);
			text->append(QObject::tr(" Failed to calculate the projected scale of region %1 ").arg(nameStr[i + 1]));
		}
	}
	if (m_allok) {
		text->setTextColor(Qt::green);
		text->append(QObject::tr("Calibration is successful!"));
	}
	else {
		text->setTextColor(QColor(255, 0, 0));
		text->append(QObject::tr("Calibration is failed"));
	}
// 	HTuple pixelSize;
// 	char buf[256];
// 	getDispOutData(PixelSize, pixelSize);
// 	m_text->setTextColor(QColor(0, 0, 0));
// 	bool allok = true;
// 
// 	QString nameStr[5] = { tr("mid"),tr("left"),tr("right"),tr("Top"),tr("Bottom") };
// 
// 	for (int i = 0;i < 5;i++) {
// 		HTuple ok, dist;
// 		getDispOutData(SizeOk, i, ok);
// 		getDispOutData(SizeDist, i, dist);
// 		if (ok[0].I()) {
// 			sprintf(buf, "%.1f", dist[0].D());
// 			m_lbImageSize[i]->setText(buf);
// 			sprintf(buf, "%.5f", pixelSize[i].D());
// 			m_lbPixelSize[i]->setText(buf);
// 			m_text->setTextColor(QColor(0, 0, 0));
// 			m_text->append(tr("%1:").arg(nameStr[i]));
// 			m_text->append(tr("Image Size= %1      Pixel Size= %2").arg(QString::number(dist[0].D(), 'f', 3)).arg(QString::number(pixelSize[i].D(), 'f', 5)));
// 		}
// 		else {
// 			m_text->setTextColor(QColor(255, 0, 0));
// 			m_text->append(tr(" Failed to calculate the pixel size of region %1 ").arg(nameStr[i]));
// 			allok = false;
// 		}
// 	}
// 	m_text->append(tr(" "));
// 	HTuple lineAngle;
// 	getDispOutData(LineAngle, lineAngle);
// 	for (int i = 0;i < 4;i++) {
// 		sprintf(buf, "%.1f", lineAngle[i].D() / PI * 180);
// 		m_lbAngle[i]->setText(buf);
// 		m_text->setTextColor(QColor(0, 0, 0));
// 		m_text->append(tr("%1's angle = %2").arg(nameStr[i + 1]).arg(QString::number(lineAngle[i].D() / PI*180.0, 'f', 2)));
// 	}
// 	m_text->append(tr(" "));
// 	HTuple prjRatio;
// 	getDispOutData(PrjRatio, prjRatio);
// 	for (int i = 0;i < 4;i++) {
// 		char buf[32];
// 		HTuple ok, lineOk, offset, aOffset;
// 		getDispOutData(PrjOK, i, ok);
// 		getDispOutData(LineOk, i, lineOk);
// 		if (ok[0].I() && lineOk[0].I()) {
// 			getDispOutData(PrjOffset, i, offset);
// 			getDispOutData(PrjActualOffset, i, aOffset);
// 			sprintf(buf, "%.1f", offset[0].D());
// 			m_lbOffset[i]->setText(buf);
// 			sprintf(buf, "%.3f", aOffset[0].D());
// 			m_lbaOffset[i]->setText(buf);
// 			m_text->append(tr("%1:").arg(nameStr[i + 1]));
// 			m_text->append(tr("Image offset= %1      Actual offset= %2").arg(QString::number(offset[0].D(), 'f', 5)).arg(QString::number(aOffset[0].D(), 'f', 5)));
// 			m_text->append(tr("projected scale= %1").arg(QString::number(prjRatio[i].D(), 'f', 5)));
// 		}
// 		else {
// 			allok = false;
// 			m_text->setTextColor(QColor(255, 0, 0));
// 			m_text->append(tr(" Failed to calculate the projected scale of region %1 ").arg(nameStr[i + 1]));
// 		}
// 		sprintf(buf, "%.4f", prjRatio[i].D());
// 		m_lbPrjRatio[i]->setText(buf);
// 
// 	}
// 	if (allok) {
// 		m_text->setTextColor(QColor(0, 255, 0));
// 		m_text->append(tr("Calibration is successful!"));
// 		QMessageBox::information(this, tr("Message"), tr("Calibration is successful!"), QMessageBox::Yes);
// 	}
// 	else {
// 		m_text->setTextColor(QColor(255, 0, 0));
// 		m_text->append(tr("Calibration is failed"));
// 		QMessageBox::warning(this, tr("Warning"), tr("Calibration is failed!"), QMessageBox::Yes);
// 	}
}

void calib3D5SMod::iniData()
{
	m_param = new calib3D5SParam;
	createPins();
	createEvents();

	m_image = new Hobject;
	m_allok = true;

	for (auto i = 0;i < 5;++i)
	{
		m_sizeOK[i] = true;
		m_sizeDist[i] = 0;
		for (auto j = 0;j < 2;++j)
		{
			m_sizeRow[i][j] = 100;
			m_sizeCol[i][j] = 100;
		}
	}

	for (auto i = 0;i < 4;++i)
	{
		m_lineOK[i] = true;
		for (auto j = 0;j < 2;++j)
		{
			m_lineRow[i][j] = 100;
			m_lineCol[i][j] = 100;
		}
	}

	for (auto i = 0;i < 4;++i)
	{
		m_prjOK[i] = true;
		m_prjRow[i] = 100;
		m_prjCol[i] = 100;
	}
}

void calib3D5SMod::createPins()
{
	addPin(&p_im, "im");

	addPin(&p_pixelSize, "pxSz", "pxSz", "pixelSize");
	addPin(&p_lineAngle, "lAng", "lAng", "lineAngle");
	addPin(&p_prjRatio, "ratio");
	addPin(&p_actualDepth, "depth", "depth", "actualDepth");
	addPin(&p_calibInfo, "info", "info", QString::fromLocal8Bit("出标定报告的信息"));

	p_lineAngle.setVisible(false);
	p_prjRatio.setVisible(false);
	p_actualDepth.setVisible(false);
	p_calibInfo.setVisible(false);
}

void calib3D5SMod::createEvents()
{
	addEvent(ENT_CALIB);
}

void calib3D5SMod::detectSize()
{
	p_pixelSize->clear();
	p_calibInfo->clear();

	long w, h;
	double pr[5][2], pc[5][2], dist[5];
	bool ok[5];
	HTuple pixelSize;
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
		m_sizeOK[i] = ok[i];
		m_sizeDist[i] = dist[i];
		for (int j = 0;j < 2;j++)
		{
			m_sizeRow[i][j] = pr[i][j];
			m_sizeCol[i][j] = pc[i][j];
		}
		//setDetectOutData(SizeOk, ok[i], i);
		//setDetectOutData(SizeDist, dist[i], i);
		//for (int j = 0;j < 2;j++) {
		//setDetectOutData(SizeRow, pr[i][j], i, j);
		//setDetectOutData(SizeCol, pc[i][j], i, j);
		//}
		//pixelSize.Append(m_param->pixelSize[i]);

		p_pixelSize->push_back(m_param->pixelSize[i]);
		p_calibInfo->push_back(m_param->pixelSize[i]);
	}
	//setDetectOutData(PixelSize, pixeSize);
	//m_outTuple = pixeSize;
}

void calib3D5SMod::detectLine()
{
	p_lineAngle->clear();

	long w, h;
	get_image_size(*m_image, &w, &h);
	HTuple lineAngle;
	double anlge[4] = { 0, -PI, -PI / 2, PI / 2 };
	for (int i = 0;i < 4;i++) {
		double cr = (m_param->lineRow2[i] + m_param->lineRow1[i]) / 2;
		double cc = (m_param->lineCol2[i] + m_param->lineCol1[i]) / 2;
		double dr = (m_param->lineRow2[i] - m_param->lineRow1[i]) / 2;
		double dc = (m_param->lineCol2[i] - m_param->lineCol1[i]) / 2;
		double len1 = 10, len2 = 10;
		bool ok = true;
		double pr[2], pc[2];
		for (int j = 0;j < 2;j++) {
			HTuple er, ec, ea, ed;
			if (i < 2) {
				len1 = dc;
				cr = j == 0 ? m_param->lineRow1[i] + len2 : m_param->lineRow2[i] - len2;
			}
			else {
				len1 = dr;
				cc = j == 0 ? m_param->lineCol1[i] + len2 : m_param->lineCol2[i] - len2;
			}
			long hm;
			gen_measure_rectangle2(cr, cc, anlge[i], len1, len2, w, h, "bilinear", &hm);
			measure_pos(*m_image, hm, 1, 20, "negative", "first", &er, &ec, &ea, &ed);
			close_measure(hm);
			if (er.Num() > 0) {
				pr[j] = er[0].D();
				pc[j] = ec[0].D();
				m_lineRow[i][j] = pr[j];
				m_lineCol[i][j] = pc[j];
				//setDetectOutData(LineRow, pr[j], i, j);
				//setDetectOutData(LineCol, pc[j], i, j);
			}
			else {
				ok = false;
			}
		}
		m_lineOK[i] = ok;
		//setDetectOutData(LineOk, ok, i);
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
		//lineAngle.Append(m_param->angle[i]);

		p_lineAngle->push_back(m_param->angle[i]);
		p_calibInfo->push_back(m_param->angle[i]);
	}
	//setDetectOutData(LineAngle, lineAngle);
	//m_outTuple.Append(lineAngle);
}

void calib3D5SMod::detectPrj()
{
	p_prjRatio->clear();
	p_actualDepth->clear();

	long w, h;
	HTuple ratio;
	HTuple actualDepth;
	get_image_size(*m_image, &w, &h);
	for (int i = 0;i < 4;i++) {
		double cr = (m_param->depthRow2[i] + m_param->depthRow1[i]) / 2;
		double cc = (m_param->depthCol2[i] + m_param->depthCol1[i]) / 2;
		double len1 = (m_param->depthCol2[i] - m_param->depthCol1[i]) / 2;
		double len2 = (m_param->depthRow2[i] - m_param->depthRow1[i]) / 2;
		long hm;
		HTuple er, ec, ea, ed;
		double recangle;
		if (i < 2) {
			i == 0 ? recangle = 0 : recangle = PI;
		}
		else {
			i == 2 ? recangle = -PI / 2.0 : recangle = PI / 2.0;
		}
		gen_measure_rectangle2(cr, cc, recangle, len1, len2, w, h, "bilinear", &hm);
		measure_pos(*m_image, hm, 1, 20, "negative", "first", &er, &ec, &ea, &ed);
		close_measure(hm);
		if (er.Num() > 0) {
			m_prjOK[i] = true;
			m_prjRow[i] = er[0].D();
			m_prjCol[i] = ec[0].D();
			//setDetectOutData(PrjOK, true, i);
			//setDetectOutData(PrjRow, er, i);
			//setDetectOutData(PrjCol, ec, i);
			if (m_lineOK[i]) {
				double dist;
				distance_pl(er[0].D(), ec[0].D(), m_lineRow[i][0], m_lineCol[i][0], m_lineRow[i][1], m_lineCol[i][1], &dist);
				m_param->prjOffset[i] = dist;
				//setDetectOutData(PrjOffset, dist, i);
				dist *= m_param->pixelSize[i + 1];
				m_param->prjActualOffset[i] = dist;
				//setDetectOutData(PrjActualOffset, dist, i);
				m_param->prjRatio[i] = dist / m_param->actualDepth[i];
			}
		}
		else {
			//setDetectOutData(PrjOK, false, i);
			m_prjOK[i] = false;
		}
		//ratio.Append(m_param->prjRatio[i]);
		//actualDepth.Append(m_param->actualDepth[i]);

		p_prjRatio->push_back(m_param->prjRatio[i]);
		p_actualDepth->push_back(m_param->actualDepth[i]);
	}

	for (auto i = 0;i < p_prjRatio->size();++i)
		p_calibInfo->push_back(p_prjRatio->at(i));
	for (auto i = 0;i < p_actualDepth->size();++i)
		p_calibInfo->push_back(p_actualDepth->at(i));

	//setDetectOutData(PrjRatio, ratio);
	//setDetectOutData(ActualDepth, actualDepth);
	//m_outTuple.Append(ratio);
	//m_outTuple.Append(actualDepth);
}