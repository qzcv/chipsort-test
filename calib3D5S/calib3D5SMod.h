#pragma once
#include <moduleClass.h>
#include <imageModule.h>
#include "HalconCpp.h"

#define ENT_CALIB "calib"

using namespace qzcv;
using namespace Halcon;

class calib3D5SParam;

class calib3D5SMod : public UnitModule
{
public:
	explicit calib3D5SMod();
	explicit calib3D5SMod(const QString&);
	~calib3D5SMod();
	int command(int cmdID, void* dataIn, void* dataOut);
	virtual int afterSetProperty(MetaProperty*);
protected:
	virtual void save(const QString& dirPath);
	virtual void load(const QString& dirPath, QvsParamLevel level);
	virtual int run(const QString& funName);
	virtual void viewResult(ImageView* iv, const QString& funName,int);
	virtual void textResult(ResultText* text, const QString& funName);
private:
	void iniData();
	void createPins();
	void createEvents();

	void detectSize();
	void detectLine();
	void detectPrj();
private:
	calib3D5SParam* m_param;
	friend class calib3D5SWdg;
	Hobject *m_image;

	bool m_sizeOK[5];
	double m_sizeDist[5];
	double m_sizeRow[5][2];
	double m_sizeCol[5][2];

	bool m_lineOK[4];
	double m_lineRow[4][2];
	double m_lineCol[4][2];

	bool m_prjOK[4];
	double m_prjRow[4];
	double m_prjCol[4];

	bool m_allok;
private:
	UnitInputPin<cv::Mat> p_im;

	UnitOutputPin<QList<double>> p_pixelSize;
	UnitOutputPin<QList<double>> p_lineAngle;
	UnitOutputPin<QList<double>> p_prjRatio;
	UnitOutputPin<QList<double>> p_actualDepth;
	UnitOutputPin<QList<double>> p_calibInfo;
};

