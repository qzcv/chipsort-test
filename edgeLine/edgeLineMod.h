#pragma once
#include <moduleClass.h>
#include <imageModule.h>
#include "HalconCpp.h"

using namespace qzcv;
using namespace Halcon;
class edgeLineParam;

namespace qzcv {
	class LineDetect;
	class RlRegion;
}

class edgeLineMod : public UnitModule
{
public:
	explicit edgeLineMod();
	explicit edgeLineMod(const QString&);
	~edgeLineMod();
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
	edgeLineParam* m_param;
	friend class edgeLineWdg;
	LineDetect* m_detect[4];
	bool m_roiFound[4];

	double m_begRow[4], m_begCol[4];//得到的边界线条的坐标
	double m_endRow[4], m_endCol[4];//得到的边界线条的坐标

	double m_centerRow;
	double m_centerCol;

	int m_allok;

	bool m_errAngleOk;
	double m_maxAngle;

	bool m_errWidOk;
	double m_maxWid, m_minWid;

	bool m_errHeiOk;
	double m_maxHei, m_minHei;

	bool m_centerOk;
	double m_centerShift;

	RlRegion *m_dispReg;
private:
	UnitInputPin<cv::Mat> p_im;
	UnitInputPin<cv::Mat> p_homMat2d;

	UnitOutputPin<QList<cv::Point2d>> p_corners;
};

