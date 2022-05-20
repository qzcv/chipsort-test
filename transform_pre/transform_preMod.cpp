#include "transform_preMod.h"
#include "transform_preParam.h"
#include <QObject>
#include <ImageView.h>
#include <resultText.h>
#include "halOpenCV.h"

transform_preMod::transform_preMod()
{
	iniData();
}

transform_preMod::transform_preMod(const QString& dirPath)
{
	iniData();
	load(dirPath, StructLevel);
}

transform_preMod::~transform_preMod()
{
	delete m_param;
	delete m_image;
}

int transform_preMod::command(int cmdID, void* dataIn, void* dataOut)
{
	return 0;
}

int transform_preMod::afterSetProperty(MetaProperty*)
{
	return 0;
}

void transform_preMod::save(const QString &dirPath)
{
	m_param->readWriteParam(false, dirPath, StructLevel);

	resetParamChanged();
}

void transform_preMod::load(const QString &dirPath, QvsParamLevel level)
{
	m_param->readWriteParam(true,dirPath,level);
}

int transform_preMod::run(const QString &funName)
{
	if (p_iIm->empty())
	{
		*p_oIm = cv::Mat(0, 0, CV_8UC1);
		return -1;
	}
	
	Hobject* image = new Hobject;
// 	if (!m_image) {
// 		return image;
// 	}
	HalOpenCV::cv2halImg(*p_iIm, m_image, false);
	Hlong nchans;
	count_channels(*m_image, &nchans);
	if (nchans == 1) {
		copy_image(*m_image, image);
	}
	else {
		gen_empty_obj(image);
		//image=cvCreateImage(cvGetSize(m_image), IPL_DEPTH_8U, 1);
		//IplImage* img3Ch=NULL;
		Hobject* img3Ch = NULL;
		Hobject h, s, v, l, la, lb;
		if (m_param->transformType >= Hue) {
			img3Ch = new Hobject;
			Hobject r, g, b;
			decompose3(*m_image, &r, &g, &b);
			if (m_param->transformType <= Brightness) {
				trans_from_rgb(r, g, b, &h, &s, &v, "hsv");
			}
			else {
				trans_from_rgb(r, g, b, &l, &la, &lb, "cielab");
			}
			//img3Ch=cvCreateImage(cvGetSize(m_image), IPL_DEPTH_8U, 3);
			//if(m_param->transformType<=Brightness)
			//	cvCvtColor(m_image, img3Ch, CV_BGR2HSV_FULL);
			//else
			//	cvCvtColor(m_image, img3Ch, CV_BGR2Lab);
		}
		switch (m_param->transformType) {
		case Gray:
			rgb1_to_gray(*m_image, image);
			//cvCvtColor(m_image, image, CV_BGR2GRAY);
			break;
		case Red:
			//cvSplit(m_image, NULL, NULL, image, NULL);
			access_channel(*m_image, image, 1);
			break;
		case Green:
			//cvSplit(m_image, NULL, image, NULL, NULL);
			access_channel(*m_image, image, 2);
			break;
		case BLue:
			//cvSplit(m_image, image, NULL, NULL, NULL);
			access_channel(*m_image, image, 3);
			break;
		case Hue:
			//cvSplit(img3Ch, image, NULL, NULL, NULL);
			copy_image(h, image);
			break;
		case Saturation:
			copy_image(s, image);
			break;
		case Brightness:
			copy_image(v, image);
			break;
		case Lab_a:
			//cvSplit(img3Ch, NULL, image, NULL, NULL);
			copy_image(la, image);
			break;
		case Lab_b:
			//cvSplit(img3Ch, NULL, NULL, image, NULL);
			copy_image(lb, image);
			break;
		default:
			;
		}
		//if(img3Ch)
		//	cvReleaseImage(&img3Ch);
		if (img3Ch) {
			delete img3Ch;
		}
	}

	HalOpenCV::hal2cvImg(*image, *p_oIm, true);
	delete image;
	return 0;
	//return image;
}

void transform_preMod::viewResult(ImageView *iv, const QString &funName, int)
{
	
}

void transform_preMod::textResult(ResultText *text, const QString &funName)
{
	if (p_iIm->empty())
	{
		text->setTextColor(Qt::red);
		text->append(QObject::tr("Empty image!"));
		return;
	}
	text->setTextColor(Qt::green);
	text->append(QObject::tr("Transfrom succeed!"));
	return;
}

void transform_preMod::iniData()
{
	m_param = new transform_preParam;
	m_image = new Hobject;
	createPins();
}

void transform_preMod::createPins()
{
	addPin(&p_iIm, "iIm");

	addPin(&p_oIm, "oIm");
}

