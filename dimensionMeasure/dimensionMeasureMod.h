#pragma once
#include <moduleClass.h>
#include <imageModule.h>
#include "HalconCpp.h"
#include "dimensionMeasureParam.h"

using namespace Halcon;
using namespace qzcv;

class dimensionMeasureMod : public UnitModule
{
public:
	explicit dimensionMeasureMod();
	explicit dimensionMeasureMod(const QString&);
	~dimensionMeasureMod();
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
private:
	dimensionMeasureParam* m_param;
	friend class dimensionMeasureWdg;
	Hobject *m_image;
	int m_allok;

	bool m_foundOk[Max_Roi_Num];
	double m_usePer[Max_Roi_Num];

	HTuple m_errLength[Max_Roi_Num];
	int m_errUltraIndex[Max_Roi_Num];
	bool m_regionOk[Max_Roi_Num];

	double m_lengthmin[Max_Roi_Num];
	double m_lengthmax[Max_Roi_Num];

	HTuple m_StartRow[Max_Roi_Num];
	HTuple m_StartCol[Max_Roi_Num];
	HTuple m_EndRow[Max_Roi_Num];
	HTuple m_EndCol[Max_Roi_Num];

	HTuple m_errStartRow[Max_Roi_Num];
	HTuple m_errStartCol[Max_Roi_Num];
	HTuple m_errEndRow[Max_Roi_Num];
	HTuple m_errEndCol[Max_Roi_Num];
private:
	UnitInputPin<cv::Mat> p_im;
	UnitInputPin<cv::Mat> p_homMat2d;
};

