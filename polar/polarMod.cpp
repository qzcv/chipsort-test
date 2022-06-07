#include "polarMod.h"
#include "polarParam.h"
#include <QObject>
#include <ImageView.h>
#include <resultText.h>
#include <opencv2/imgproc.hpp>
#include "Measure1D.h"
#include "HalOpenCV.h"

polarMod::polarMod()
{
	iniData();
}

polarMod::polarMod(const QString& dirPath)
{
	iniData();
	load(dirPath, StructLevel);
}

polarMod::~polarMod()
{
	delete m_param;
}

int polarMod::command(int cmdID, void* dataIn, void* dataOut)
{
	return 0;
}

int polarMod::afterSetProperty(MetaProperty*)
{
	return 0;
}

void polarMod::save(const QString &dirPath)
{
	m_param->readWriteParam(false, dirPath, StructLevel);
	resetParamChanged();
}

void polarMod::load(const QString &dirPath, QvsParamLevel level)
{
	m_param->readWriteParam(true,dirPath,level);
}

int polarMod::run(const QString &funName)
{
	if (p_im->empty())
		return -1;
	if (m_param->isMatrixRefer&&p_homMat2d->size() != cv::Size(3, 2))
		return -1;

	if (m_param->isDefaultSig)
	{
		*p_lf = 1;
		*p_rt = 0;
		return 0;
	}

	cv::Mat im;
	if (p_im->channels() == 3)
		cv::cvtColor(**p_im, im, CV_BGR2GRAY);
	else
		im = *p_im;

	//重置标志位
	*p_lf = 0;
	*p_rt = 0;
	m_allok = 0;
	m_pol = 0;
	int detectFailIndex = -1;		//-1表示没有
	cv::Point2d lineLt[2][2];
	cv::Point2d lineRt[2][2];
	double angLt = 0;
	cv::Point2d interLt = cvPoint2D32f(0, 0);
	cv::Point2d angOtherPointLt[2];
	double angRt = 0;
	cv::Point2d interRt = cvPoint2D32f(0, 0);
	cv::Point2d angOtherPointRt[2];

	for (size_t i = 0;i < 2;++i)
	{
		for (size_t j = 0;j < 2;++j)
		{
			lineLt[i][j] = cvPoint2D32f(0, 0);
			lineRt[i][j] = cvPoint2D32f(0, 0);
		}
		angOtherPointLt[i] = cvPoint2D32f(0, 0);
		angOtherPointRt[i] = cvPoint2D32f(0, 0);
	}

	bool isLf = 0;
	bool isRt = 0;

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

	for (size_t i = 0;i < 4;++i)
	{
		HTuple row, col;
		HTuple ang = m_param->rectAng[i] + rotateAng;
		affine_trans_pixel(HomMat, m_param->rectRow[i], m_param->rectCol[i], &row, &col);

		m_curRow[i] = row[0].D();
		m_curCol[i] = col[0].D();
		m_curAng[i] = ang[0].D();
		m_curLen1[i] = m_param->rectLe1[i];
		m_curLen2[i] = m_param->rectLe2[i];

		setNewDetector(i, row[0].D(), col[0].D(), ang[0].D(), m_param->rectLe1[i], m_param->rectLe2[i]);
		m_detector[i]->setFirst(m_param->isFirst[i]);
		m_detector[i]->setGradStep(m_param->step[i]);
		m_detector[i]->setMaxIterator(m_param->iteratNum[i]);
		m_detector[i]->setRemoveDistance(m_param->rmDist[i]);
		m_detector[i]->setHoughFilter(true, m_param->degRange[i]);

		cv::Point2f line[2];
		bool isOK = m_detector[i]->calucalte(im, (LineDetect::Polarity)(m_param->pol[i]), m_param->gradient[i], line);

		if (isOK)
			for (size_t j = 0;j < 2;++j)
			{
				i < 2 ? (lineLt[i][j].x = line[j].x) : (lineRt[i - 2][j].x = line[j].x);
				i < 2 ? (lineLt[i][j].y = line[j].y) : (lineRt[i - 2][j].y = line[j].y);
			}
		else
		{
			m_allok = -1;
			detectFailIndex = i;
			break;
		}
	}
	m_failIdx = detectFailIndex;

	if ((m_allok == 0) || (!(m_allok == 0) && detectFailIndex > 1))
		getIntersection(lineLt, m_ang[0], m_inter[0], m_angOtherPoint[0]);
	if ((m_allok == 0))
		getIntersection(lineRt, m_ang[1], m_inter[1], m_angOtherPoint[1]);

	for (auto x = 0;x < 2;++x)
		for (auto y = 0;y < 2;++y)
			for (auto z = 0;z < 2;++z)
				m_linePoint[x][y][z] = x ? lineRt[y][z] : lineLt[y][z];

// 	if ((m_allok == 0) || (!(m_allok == 0) && detectFailIndex > 1))
// 		getIntersection(lineLt, angLt, interLt, angOtherPointLt);
// 	if ((m_allok == 0))
// 		getIntersection(lineRt, angRt, interRt, angOtherPointRt);

	int pol;
	if (m_allok == 0)
	{
		if (m_param->isLeft0)
			pol = angLt > angRt ? 0 : 180;
		else
			pol = angLt > angRt ? 180 : 0;
	}
	m_pol = pol;

	int z = angLt > angRt ? 0 : 1;    //大小角判定转为左右角判定
	int y = z;
	for (auto i = 0;i < 2;++i)
	{
		if (i == 1)
			y = !z;
		m_enable[i] = m_param->isRangeEnable[y];
		m_minAng[i] = m_param->minAng[y];
		m_maxAng[i] = m_param->maxAng[y];
	}

	if (m_allok == 0)
	{
		isLf = pol ? 1 : 0;
		isRt = pol ? 0 : 1;

		for (auto i = 0;i < 2;++i)
		{
			double ang = i ? angRt : angLt;
			if (m_enable[i] && (ang < m_minAng[i] || ang > m_maxAng[i]))
			{
				isLf = 0;
				isRt = 0;
				m_allok = -1 * (i + 1);
				return -1;
			}
		}
	}
	else
	{
		isLf = 0;
		isRt = 0;
	}
	*p_lf = isLf;
	*p_rt = isRt;

	m_allok = 0;

	return m_allok;
}

void polarMod::viewResult(ImageView *iv, const QString &funName, int)
{
	if (p_im->empty())
		return;
	if (m_param->isMatrixRefer&&p_homMat2d->size() != cv::Size(3, 2))
		return;

	iv->clear();
	int grayVal = 255;
	for (size_t i = 0;i < 4;++i)
	{
		if (m_failIdx== i)
			iv->setColor(255, 0, 0);
		else
			iv->setColor(grayVal, grayVal, grayVal);

		if (m_failIdx != -1 || (m_failIdx == -1 && m_param->isDispAfterDet))
			iv->dispRect2(m_curRow[i], m_curCol[i], m_curAng[i], m_curLen1[i], m_curLen2[i]);

		if (m_failIdx == i)
			break;
	}

	if (m_failIdx == -1)
	{
		for (auto i = 0;i < 2;++i)
		{
			if (m_enable[i] && (m_ang[i] < m_minAng[i] || m_ang[i] > m_maxAng[i]))
				iv->setColor(255, 0, 0);
			else
			{
				iv->setColor(0, 255, 0);
				iv->setTextSize(20);
				iv->dispText(200, 500, m_pol ? "180°" : "0°");
			}
			for (size_t j = 0;j < 2;++j)
				iv->dispLine(m_inter[i].y, m_inter[i].x, m_angOtherPoint[i][j].y, m_angOtherPoint[i][j].x);
		}
	}
	else if (m_failIdx == 0)
		return;
	else if (m_failIdx == 1)
		iv->dispLine(m_linePoint[0][0][0].y, m_linePoint[0][0][0].x, m_linePoint[0][0][1].y, m_linePoint[0][0][1].x);
	else if (m_failIdx == 2)
		for (size_t j = 0;j < 2;++j)
			iv->dispLine(m_inter[0].y, m_inter[0].x, m_angOtherPoint[0][j].y, m_angOtherPoint[0][j].x);
	else if (m_failIdx == 3)
	{
		iv->dispLine(m_linePoint[1][0][0].y, m_linePoint[1][0][0].x, m_linePoint[1][0][1].y, m_linePoint[1][0][1].x);
		for (size_t j = 0;j < 2;++j)
			iv->dispLine(m_inter[0].y, m_inter[0].x, m_angOtherPoint[0][j].y, m_angOtherPoint[0][j].x);
	}
	iv->update();
}

void polarMod::textResult(ResultText *text, const QString &funName)
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
		text->append(QObject::tr("Input matrix's size != 2*3!"));
		return;
	}

	if (m_failIdx == -1)
	{
		text->setTextColor(Qt::blue);
		text->append(QString::fromLocal8Bit("左:%1° %2 右:%3°  %4°")
			.arg(m_ang[0], 0, 'f', 2)
			.arg(m_ang[0] > m_ang[1] ? ">" : "<")
			.arg(m_ang[1], 0, 'f', 2)
			.arg(m_pol));

		for (auto i = 0;i < 2;++i)
		{
			if (m_enable[i] && (m_ang[i] < m_minAng[i] || m_ang[i] > m_maxAng[i]))
			{
				text->setTextColor(Qt::red);
				text->append(QString::fromLocal8Bit("%4角角度超出范围！%1° (%2°,%3°)")
					.arg(m_ang[i], 0, 'f', 2)
					.arg(m_minAng[i])
					.arg(m_maxAng[i]).arg(i == 0 ? QObject::tr("left") : QObject::tr("right")));
			}
		}
	}
	else if (m_failIdx < 2)
	{
		text->setTextColor(Qt::red);
		text->append(QString::fromLocal8Bit("左%1直线识别失败！")
			.arg(QString::number(m_failIdx + 1)));
	}
	else
	{
		text->setTextColor(Qt::red);
		text->append(QString::fromLocal8Bit("左:%1° 右%2直线识别失败！")
			.arg(QString::number(m_ang[0], 'f', 2))
			.arg(QString::number(m_failIdx - 1)));
	}	
}

void polarMod::iniData()
{
	m_param = new polarParam;
	for (size_t i = 0;i < 4;++i)
	{
		m_detector[i] = NULL;

		m_curRow[i] = m_param->rectRow[i];
		m_curCol[i] = m_param->rectCol[i];
		m_curAng[i] = m_param->rectAng[i];
		m_curLen1[i] = m_param->rectLe1[i];
		m_curLen2[i] = m_param->rectLe2[i];
	}
	m_failIdx = -1;
	m_allok = 0;
	m_pol = 0;
	createPins();
}

void polarMod::createPins()
{
	addPin(&p_im, "im");
	addPin(&p_homMat2d, "hm2d");

	addPin(&p_lf, "lf");
	addPin(&p_rt, "tr");
}

void polarMod::setNewDetector(int index, double row, double col, double ang, double L1, double L2)
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

void polarMod::getIntersection(cv::Point2d linePoint[2][2], double & angle, cv::Point2d & inter, cv::Point2d otherPoints[2])
{
	double lineDeg0 = cvFastArctan(linePoint[0][0].y - linePoint[0][1].y, linePoint[0][0].x - linePoint[0][1].x);
	double lineDeg1 = cvFastArctan(linePoint[1][0].y - linePoint[1][1].y, linePoint[1][0].x - linePoint[1][1].x);
	while (lineDeg0 > 90)
		lineDeg0 -= 180;
	while (lineDeg1 > 90)
		lineDeg1 -= 180;

	double angle0 = lineDeg0*CV_PI / 180;
	double angle1 = lineDeg1*CV_PI / 180;

	//交点
	double a = linePoint[0][0].y*cos(angle0) - linePoint[0][0].x*sin(angle0);
	double b = linePoint[1][0].y*cos(angle1) - linePoint[1][0].x*sin(angle1);

	inter.x = (b*cos(angle0) - a*cos(angle1)) / sin(angle0 - angle1);
	inter.y = (b*sin(angle0) - a*sin(angle1)) / sin(angle0 - angle1);

	//夹角
	angle = fabs(lineDeg0 - lineDeg1);
	while (angle > 180)
		angle -= 360;

	//组成夹角的另外两个点
	int minDistIndex = 0;
	double minDist = 0;
	for (size_t i = 0;i < 2;++i)
	{
		double L1 = sqrt(pow(inter.x - linePoint[i][0].x, 2) + pow(inter.y - linePoint[i][0].y, 2));
		double L2 = sqrt(pow(inter.x - linePoint[i][1].x, 2) + pow(inter.y - linePoint[i][1].y, 2));

		int index = L1 > L2 ? 0 : 1;

		otherPoints[i].x = linePoint[i][index].x;
		otherPoints[i].y = linePoint[i][index].y;
	}
}

