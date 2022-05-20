#pragma once
#include <moduleClass.h>
#include <imageModule.h>
#include "HalconCpp.h"
#include "segLeadParam.h"

using namespace qzcv;
using namespace Halcon;

class segLeadMod : public UnitModule
{
public:
	explicit segLeadMod();
	explicit segLeadMod(const QString&);
	~segLeadMod();
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
	segLeadParam* m_param;
	friend class segLeadWdg;
	Hobject *m_image;

	int m_allok;

	bool m_foundok[Max_LeadNum];

	double m_leadWid[Max_LeadNum];
	bool m_leadWidOk[Max_LeadNum];

	double m_leadLen[Max_LeadNum];
	bool m_leadLenOk[Max_LeadNum];

	double m_leadInter[Max_LeadNum - 1];
	bool m_leadInterOk[Max_LeadNum - 1];

	double m_leadRow[Max_LeadNum];
	double m_leadCol[Max_LeadNum];
	double m_leadAng[Max_LeadNum];
	double m_leadLen1[Max_LeadNum];
	double m_leadLen2[Max_LeadNum];
	bool m_leadOk[Max_LeadNum];

	RlRegion m_dispReg;
private:
	UnitInputPin<cv::Mat> p_im;
	UnitInputPin<QList<double>> p_row;
	UnitInputPin<QList<double>> p_col;
	UnitInputPin<cv::Mat> p_homMat2d;
	UnitInputPin<QList<double>> p_pixelSize;

	HTuple m_row;
	HTuple m_col;
	HTuple m_homMat2d;
	HTuple m_pixelSize;
};

