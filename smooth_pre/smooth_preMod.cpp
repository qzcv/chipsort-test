#include "smooth_preMod.h"
#include "smooth_preParam.h"
#include <QObject>
#include <ImageView.h>
#include <resultText.h>
#include "halOpenCV.h"

smooth_preMod::smooth_preMod()
{
	iniData();
}

smooth_preMod::smooth_preMod(const QString& dirPath)
{
	iniData();
	load(dirPath, StructLevel);
}

smooth_preMod::~smooth_preMod()
{
	delete m_param;
	delete m_image;
}

int smooth_preMod::command(int cmdID, void* dataIn, void* dataOut)
{
	return 0;
}

int smooth_preMod::afterSetProperty(MetaProperty*)
{
	return 0;
}

void smooth_preMod::save(const QString &dirPath)
{
	m_param->readWriteParam(false, dirPath, StructLevel);

	resetParamChanged();
}

void smooth_preMod::load(const QString &dirPath, QvsParamLevel level)
{
	m_param->readWriteParam(true,dirPath,level);
}

int smooth_preMod::run(const QString &funName)
{
	if (p_iIm->empty())
	{
		*p_oIm = cv::Mat(0, 0, CV_8UC1);
		return -1;
	}
	HalOpenCV::cv2halImg(*p_iIm, m_image, false);

	Hobject* smImage = new Hobject;
	//if (m_image) {
		copy_image(*m_image, smImage);
		int cvType;
		switch (m_param->smoothType) {
		case 0:
			//GAUSSIAN
			gauss_image(*smImage, smImage, m_param->smoothParam1);
			break;
		case 1:
			//mean
			mean_image(*smImage, smImage, m_param->smoothParam1, m_param->smoothParam2);
			break;
		case 2:
			//MEDIAN
			median_image(*smImage, smImage, "circle", m_param->smoothParam1, "mirrored");
			break;
		case 3:
			//sigma
			sigma_image(*smImage, smImage, m_param->smoothParam1, m_param->smoothParam2, m_param->smoothParam3);
			break;
		case 4:
			//contrast
			emphasize(*smImage, smImage, m_param->smoothParam1, m_param->smoothParam2, m_param->smoothParam4);
			break;
		default:
			gauss_image(*smImage, smImage, m_param->smoothParam1);
		}
		p_oIm->release();
		HalOpenCV::hal2cvImg(*smImage, *p_oIm, true);
		return 0;
	//}
	//else
	//	return smImage;
	//return smImage;
}

void smooth_preMod::viewResult(ImageView *iv, const QString &funName, int)
{

}

void smooth_preMod::textResult(ResultText *text, const QString &funName)
{
	if (p_iIm->empty())
	{
		text->setTextColor(Qt::red);
		text->append(QObject::tr("Empty image!"));
		return;
	}
	text->setTextColor(Qt::green);
	text->append(QObject::tr("Smooth succeed!"));
	return;
}

void smooth_preMod::iniData()
{
	m_param = new smooth_preParam;
	m_image = new Hobject;
	createPins();
}

void smooth_preMod::createPins()
{
	addPin(&p_iIm, "iIm");

	addPin(&p_oIm, "oIm");
}

