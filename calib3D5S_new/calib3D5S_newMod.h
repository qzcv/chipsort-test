#pragma once
#include <moduleClass.h>
#include <imageModule.h>
#include "HalconCpp.h"
#include "chipMod.h"

#define ENT_CALIB "calib"
#define ENT_PRE "preprocess"

using namespace qzcv;
using namespace Halcon;
class calib3D5S_newParam;

class calib3D5S_newMod : public ChipMod
{
public:
	explicit calib3D5S_newMod();
	explicit calib3D5S_newMod(const QString&);
	~calib3D5S_newMod();
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
	void detectStand();
private:
	calib3D5S_newParam* m_param;
	friend class calib3D5S_newWdg;
	Hobject *m_image;
	HTuple m_outTuple;  //用于传出标定信息的变量

	bool m_allok;

	bool m_lineOK[4];
	bool m_standLineOK[4];
	double m_lineR[4][2], m_lineC[4][2];
	double m_depthRow[4], m_depthCol[4]; //槽底的坐标
	double m_depLineR[4][2], m_depLineC[4][2]; //槽底的坐标
	double m_standLineR[4][2], m_standLineC[4][2]; //站高台阶的边线,顺序高，低
	double m_standAngle[4];
private:
	UnitInputPin<cv::Mat> p_im;

	UnitOutputPin<QList<double>> p_pixSize;
	UnitOutputPin<QList<double>> p_lAng;
	UnitOutputPin<QList<double>> p_ratio;
	UnitOutputPin<QList<double>> p_aDepth;		//actual depth
	UnitOutputPin<QList<double>> p_info;		//calib information
};

