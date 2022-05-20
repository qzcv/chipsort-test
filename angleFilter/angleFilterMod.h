#pragma once
#include <moduleClass.h>
#include <imageModule.h>
#include "HalconCpp.h"

using namespace qzcv;
using namespace Halcon;
class angleFilterParam;
namespace qzcv
{
	class LineDetect;
}

class angleFilterMod : public UnitModule
{
public:
	explicit angleFilterMod();
	explicit angleFilterMod(const QString&);
	~angleFilterMod();
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
	void setNewDetector(int index, double row, double col, double ang, double L1, double L2);
private:
	angleFilterParam* m_param;
	friend class angleFilterWdg;
	LineDetect *m_detector[3];

	double m_curRow[3];
	double m_curCol[3];
	double m_curAng[3];
	double m_curLen1[3];
	double m_curLen2[3];
	
	int m_allok;

	bool m_baseLineOk;
	double m_baseRow[2];
	double m_baseCol[2];

	double m_ang[2][2];
	bool m_lineOK[2];
	double m_lineRow[2][2][2];
	double m_lineCol[2][2][2];

private:
	UnitInputPin<cv::Mat> p_im;
	UnitInputPin<cv::Mat> p_homMat2d;
};

