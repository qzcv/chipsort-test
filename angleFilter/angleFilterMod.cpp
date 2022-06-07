#include "angleFilterMod.h"
#include "angleFilterParam.h"
#include <QObject>
#include <ImageView.h>
#include <resultText.h>
#include "Measure1D.h"
#include <opencv2/imgproc.hpp>
#include "halOpenCV.h"

angleFilterMod::angleFilterMod()
{
	iniData();
}

angleFilterMod::angleFilterMod(const QString& dirPath)
{
	iniData();
	load(dirPath, StructLevel);
}

angleFilterMod::~angleFilterMod()
{
	for (auto i = 0;i < 3;++i)
		if (m_detector[i])
			delete m_detector[i];
	delete m_param;
}

int angleFilterMod::command(int cmdID, void* dataIn, void* dataOut)
{
	return 0;
}

int angleFilterMod::afterSetProperty(MetaProperty*)
{
	return 0;
}

void angleFilterMod::save(const QString &dirPath)
{
	m_param->readWriteParam(false, dirPath, StructLevel);
	resetParamChanged();
}

void angleFilterMod::load(const QString &dirPath, QvsParamLevel level)
{
	m_param->readWriteParam(true,dirPath,level);
}

int angleFilterMod::run(const QString &funName)
{
	//bool ok = true;
// 	for (size_t i = 0;i < 3;++i)
// 		m_testItemsStatus[i] = 1;
// 
// 	if (!m_image || m_image->Id() == H_EMPTY_REGION)
// 	{
// 		setDetectOutData(EMPTY_IMG, true);
// 		setDetectOutData(OK, false);
// 		m_testItemsStatus[0] = 0;
// 		return false;
// 	}
// 	else
// 		setDetectOutData(EMPTY_IMG, false);

	if (p_im->empty())
		return -1;
	if (m_param->isMatrixRefer&&p_homMat2d->size() != cv::Size(3, 2))
		return -1;

	m_allok = 0;

	cv::Mat im;
	if (p_im->channels() == 3)
		cv::cvtColor(**p_im, im, CV_BGR2GRAY);
	else
		im = *p_im;

	HTuple HomMat;
	if (m_param->isMatrixRefer)
	{
		try
		{
			HalOpenCV::cv2HalHomMat2D(*p_homMat2d, &HomMat);
			double tv = HomMat[0].D();
		}
		catch (HException& ex)
		{
			hom_mat2d_identity(&HomMat);
		}
	}
	else
		hom_mat2d_identity(&HomMat);

	double rotateAng = atan(HomMat[3].D() / HomMat[0].D());

	for (size_t i = 0;i < 3;++i)
	{
		HTuple row, col;
		HTuple ang = m_param->ang[i] + rotateAng;
		affine_trans_pixel(HomMat, m_param->crow[i], m_param->ccol[i], &row, &col);

		m_curRow[i] = row[0].D();
		m_curCol[i] = col[0].D();
		m_curAng[i] = ang[0].D();
		m_curLen1[i] = m_param->len1[i];
		m_curLen2[i] = m_param->len2[i];

		setNewDetector(i, row[0].D(), col[0].D(), ang[0].D(), m_param->len1[i], m_param->len2[i]);
		m_detector[i]->setFirst(m_param->isFirst[i]);
		m_detector[i]->setGradStep(m_param->step[i]);
		m_detector[i]->setMaxIterator(m_param->iteratNum[i]);
		m_detector[i]->setRemoveDistance(m_param->rmDist[i]);
		m_detector[i]->setHoughFilter(true, m_param->degRange[i]);

		if (i == 0)//基准直线
		{
			cv::Point2f line[2];
			LineDetect::Polarity pol = (LineDetect::Polarity)m_param->pol[i];
			bool isOK = m_detector[i]->calucalte(im, pol, m_param->gradient[i], line);

			if (!isOK)
			{
				//setDetectOutData(OK, false);
				//m_testItemsStatus[0] = 0;
				//return false;
				m_allok = -1;
				m_baseLineOk = false;
				return -1;
			}
			else
			{
				m_baseLineOk = true;
				for (int k = 0;k < 2;++k)
				{
					m_baseRow[k] = line[k].y;
					m_baseCol[k] = line[k].x;
				}
			}
		}
		else	//左右管脚
		{
			cv::Point2f line[2][2];
			LineDetect::Polarity pol = (LineDetect::Polarity)m_param->pol[i];
			bool isOK1 = m_detector[i]->calucalte(im, pol, m_param->gradient[i], line[0]);

			//反向
			setNewDetector(i, row[0].D(), col[0].D(), ang[0].D() + CV_PI, m_param->len1[i], m_param->len2[i]);
			m_detector[i]->setFirst(m_param->isFirst[i + 2]);
			m_detector[i]->setGradStep(m_param->step[i + 2]);
			m_detector[i]->setMaxIterator(m_param->iteratNum[i + 2]);
			m_detector[i]->setRemoveDistance(m_param->rmDist[i + 2]);
			m_detector[i]->setHoughFilter(true, m_param->degRange[i + 2]);
			pol = (LineDetect::Polarity)m_param->pol[i + 2];
			bool isOK2 = m_detector[i]->calucalte(im, pol, m_param->gradient[i + 2], line[1]);

			if (!(isOK1&&isOK2))
			{
				//setDetectOutData(i == 1 ? LT_LINE_NG : RT_LINE_NG, true);
				//setDetectOutData(OK, false);
				//m_testItemsStatus[0] = 0;
				//return false;
				m_lineOK[i - 1] = false;
				m_allok = -1;
				return -1;
			}
			else
			{
				//setDetectOutData(i == 1 ? LT_LINE_NG : RT_LINE_NG, false);
				m_lineOK[i - 1] = true;

				for (int k = 0;k < 2;++k)//管脚的上下两边
				{
					HTuple  deg;
					for (int j = 0;j < 2;++j)
					{
						m_lineRow[i - 1][k][j] = line[k][j].y;
						m_lineCol[i - 1][k][j] = line[k][j].x;
					}
					angle_ll(
						m_lineRow[i - 1][k][0], m_lineCol[i - 1][k][0],
						m_lineRow[i - 1][k][1], m_lineCol[i - 1][k][1],
						m_baseRow[0], m_baseCol[0], m_baseRow[1], m_baseCol[1], &deg);
					double ang = fabs(deg[0].D() * 180 / PI);
					while (ang > 360)
						ang -= 360;
					if (ang > 90)//转化成锐角
						ang = 180 - ang;
					m_ang[i - 1][k] = ang;

					if (ang<m_param->minAng[i - 1] || ang>m_param->maxAng[i - 1])
					{
						//setDetectOutData(OK, false);
						//m_testItemsStatus[i] = 0;
						//ok = false;
						m_allok = -1;
					}
				}
			}
		}
	}
	return m_allok;
}

void angleFilterMod::viewResult(ImageView *iv, const QString &funName, int)
{
	if (p_im->empty())
		return;
	if (m_param->isMatrixRefer&&p_homMat2d->size() != cv::Size(3, 2))
		return;
	if (!m_param->isDispAfterDet)
		return;

	QStringList title = { QObject::tr("base"),QObject::tr("left"),QObject::tr("right") };
	iv->setTextSize(9);
	for (size_t i = 0;i < 3;++i)
	{
		if (i == 0)
		{
			if (m_baseLineOk == false)
			{
				iv->setColor(Qt::red);
				iv->dispRect2(m_curRow[i], m_curCol[i], m_curAng[i], m_curLen1[i], m_curLen2[i], title[i]);
				return;
			}
			else
			{
				iv->setColor(Qt::green);
				iv->dispLine(m_baseRow[0], m_baseCol[0], m_baseRow[1], m_baseCol[1]);
			}				
		}
		else
		{
			if (m_lineOK[i - 1] == false)
			{
				iv->setColor(Qt::red);
				iv->dispRect2(m_curRow[i], m_curCol[i], m_curAng[i], m_curLen1[i], m_curLen2[i], title[i]);
				return;
			}
			else
			{
				for (int k = 0;k < 2;++k)
				{
					if (m_ang[i - 1][k] < m_param->minAng[i - 1] || 
						m_ang[i - 1][k] > m_param->maxAng[i - 1])
						iv->setColor(Qt::red);
					else
						iv->setColor(Qt::green);

					iv->dispLine(
						m_lineRow[i - 1][k][0], m_lineCol[i - 1][k][0],
						m_lineRow[i - 1][k][1], m_lineCol[i - 1][k][1], QString("%1").arg(k + 1));
				}
			}
		}
	}

// 	CsVariant isEmpty;
// 	getDispOutData(EMPTY_IMG, isEmpty);
// 	if (isEmpty[0].I())
// 		return;
// 
// 	if (!m_param->isDispAfterDet)
// 		return;
// 
// 	HTuple baseRow, baseCol;
// 	HTuple lineRow[2][2], lineCol[2][2];
// 	HTuple lineAng[2][2];
// 	const char *title[3] = { "base line","left line","right line" };
// 	m_view->setTextSize(9);
// 	for (size_t i = 0;i < 3;++i)
// 	{
// 		HTuple row, col, ang, len1, len2;
// 		getDispOutData(_ROW, i, row);
// 		getDispOutData(_COL, i, col);
// 		getDispOutData(_ANG, i, ang);
// 		getDispOutData(_LEN1, i, len1);
// 		getDispOutData(_LEN2, i, len2);
// 		if (i == 0)
// 		{
// 			CsVariant baseNG;
// 			getDispOutData(BASE_LINE_NG, baseNG);
// 			if (baseNG[0].I())
// 			{
// 				m_view->setColor(255, 0, 0);
// 				m_view->dispRect2(row[0].D(), col[0].D(), ang[0].D(), len1[0].D(), len2[0].D(), title[i], 5);
// 			}
// 			else
// 			{
// 				m_view->setColor(0, 255, 0);
// 
// 				getDispOutData(BASE_LINE_ROW, baseRow);
// 				getDispOutData(BASE_LINE_COL, baseCol);
// 				if (baseRow.Num() >= 2)
// 					m_view->dispLine(baseRow[0].D(), baseCol[0].D(), baseRow[1].D(), baseCol[1].D());
// 			}
// 			//			m_view->dispRect2(row[0].D(),col[0].D(),ang[0].D(),len1[0].D(),len2[0].D(),title[i],5);
// 			if (baseNG[0].I())
// 				return;
// 		}
// 		else
// 		{
// 			CsVariant NG;
// 			getDispOutData(i == 1 ? LT_LINE_NG : RT_LINE_NG, NG);
// 			if (NG[0].I())
// 			{
// 				m_view->setColor(255, 0, 0);
// 				m_view->dispRect2(row[0].D(), col[0].D(), ang[0].D(), len1[0].D(), len2[0].D(), title[i], 5);
// 				return;
// 			}
// 			else
// 			{
// 				for (int k = 0;k < 2;++k)
// 				{
// 					getDispOutData(i == 1 ? LT_LINE_ROW : RT_LINE_ROW, k, lineRow[i - 1][k]);
// 					getDispOutData(i == 1 ? LT_LINE_COL : RT_LINE_COL, k, lineCol[i - 1][k]);
// 					getDispOutData(i == 1 ? LT_LINE_ANG : RT_LINE_ANG, k, lineAng[i - 1][k]);
// 
// 					double a = lineAng[i - 1][k][0].D();
// 					if (a<m_param->minAng[i - 1] || a>m_param->maxAng[i - 1])
// 						m_view->setColor(255, 0, 0);
// 					else
// 						m_view->setColor(0, 255, 0);
// 					if (lineRow[i - 1][k].Num() >= 2)
// 					{
// 						char ts[64];
// 						sprintf(ts, "%d", k + 1);
// 						m_view->dispLine(
// 							lineRow[i - 1][k][0].D(), lineCol[i - 1][k][0].D(),
// 							lineRow[i - 1][k][1].D(), lineCol[i - 1][k][1].D(), 0, ts);
// 					}
// 				}
// 				//				m_view->dispRect2(row[0].D(),col[0].D(),ang[0].D(),len1[0].D(),len2[0].D(),title[i],5);
// 			}
// 		}
// 	}
}

void angleFilterMod::textResult(ResultText *text, const QString &funName)
{
	if (p_im->empty())
	{
		text->setTextColor(Qt::red);
		text->append(QObject::tr("Empty image!"));
		return;
	}
	if (m_param->isMatrixRefer&&p_homMat2d->size() != cv::Size(3, 2))
	{
		text->setTextColor(Qt::red);
		text->append(QObject::tr("Input matrix's size != 6!"));
		return;
	}

	for (size_t i = 0;i < 3;++i)
	{
		if (i == 0)
		{
			if (m_baseLineOk == false)
			{
				text->setTextColor(Qt::red);
				text->append(QObject::tr("base line detect failed!"));
				return;
			}
		}
		else
		{
			if (m_lineOK[i - 1] == false)
			{
				text->setTextColor(Qt::red);
				text->append(QObject::tr("%1 line detect failed!").arg(i == 1 ? QObject::tr("left") : QObject::tr("right")));
				return;
			}
			else
			{
				for (int k = 0;k < 2;++k)
				{
					QString info;
					if (m_ang[i - 1][k] < m_param->minAng[i - 1] ||
						m_ang[i - 1][k] > m_param->maxAng[i - 1])
					{
						text->setTextColor(Qt::red);
						info = QObject::tr("failed");
					}
					else
					{
						text->setTextColor(Qt::blue);
						info = QObject::tr("passed");
					}
					text->append(
						QObject::tr("%1 line%2 & base line: %3 [%4,%5] %6")
						.arg(i == 1 ? QObject::tr("left") : QObject::tr("right")).arg(k + 1).arg(m_ang[i - 1][k], 0, 'f', 3)
						.arg(m_param->minAng[i - 1]).arg(m_param->maxAng[i - 1]).arg(info));
				}
			}
		}
	}

// 	CsVariant isEmpty;
// 	getDispOutData(EMPTY_IMG, isEmpty);
// 	if (isEmpty[0].I())
// 		return;
// 
// 	HTuple lineAng[2][2];
// 	for (size_t i = 0;i < 3;++i)
// 	{
// 		if (i == 0)
// 		{
// 			CsVariant baseNG;
// 			getDispOutData(BASE_LINE_NG, baseNG);
// 			if (baseNG[0].I())
// 			{
// 				m_text->setTextColor(Qt::red);
// 				m_text->append(tr("base line detect failed!"));
// 				return;
// 			}
// 		}
// 		else
// 		{
// 			CsVariant NG;
// 			getDispOutData(i == 1 ? LT_LINE_NG : RT_LINE_NG, NG);
// 			if (NG[0].I())
// 			{
// 				m_text->setTextColor(Qt::red);
// 				m_text->append(tr("%1 line detect failed!").arg(i == 1 ? tr("left") : tr("right")));
// 				return;
// 			}
// 			else
// 			{
// 				for (int k = 0;k < 2;++k)
// 				{
// 					getDispOutData(i == 1 ? LT_LINE_ANG : RT_LINE_ANG, k, lineAng[i - 1][k]);
// 					double a = lineAng[i - 1][k][0].D();
// 					QString info;
// 					if (a<m_param->minAng[i - 1] || a>m_param->maxAng[i - 1])
// 					{
// 						m_text->setTextColor(Qt::red);
// 						info = tr("failed");
// 					}
// 					else
// 					{
// 						m_text->setTextColor(Qt::blue);
// 						info = tr("passed");
// 					}
// 					m_text->append(tr("%1 line%2 & base line: %3 [%4,%5] %6").arg(i == 1 ? tr("left") : tr("right"))
// 						.arg(k + 1).arg(a, 0, 'f', 3).arg(m_param->minAng[i - 1]).arg(m_param->maxAng[i - 1]).arg(info));
// 				}
// 			}
// 		}
// 	}
}

void angleFilterMod::iniData()
{
	m_param = new angleFilterParam;
	for (auto i = 0;i < 3;++i)
	{
		m_detector[i] = nullptr;

		m_curRow[i] = m_param->crow[i];
		m_curCol[i] = m_param->ccol[i];
		m_curAng[i] = m_param->ang[i];
		m_curLen1[i] = m_param->len1[i];
		m_curLen2[i] = m_param->len2[i];

		if (i < 2)
		{
			m_baseRow[i] = 0;
			m_baseCol[i] = 0;

			m_lineOK[i] = 0;
			for (auto j = 0;j < 2;++j)
			{
				m_ang[i][j] = 0;
				for (auto k = 0;k < 2;++k)
				{
					m_lineRow[i][j][k] = 0;
					m_lineCol[i][j][k] = 0;
				}
			}
		}
	}
	m_allok = 0;
	m_baseLineOk = true;
	createPins();
}

void angleFilterMod::createPins()
{
	addPin(&p_im, "im");
	addPin(&p_homMat2d, "hm2d");
}

void angleFilterMod::setNewDetector(int index, double row, double col, double ang, double L1, double L2)
{
	if (m_detector[index])
		delete m_detector[index];

	float vx[2] = { 0, 0 };
	float vy[2] = { L2, -L2 };

	CvPoint2D32f p[2];
	float cosa = cos(ang);
	float sina = sin(ang);

	for (size_t i = 0;i < 2;i++)
	{
		p[i].y = row + cosa*vy[i] - sina*vx[i];
		p[i].x = col + sina*vy[i] + cosa*vx[i];
	}
	m_detector[index] = new LineDetect(p[0], p[1], 2 * L1);
}

