#pragma once
#include <moduleClass.h>
#include <imageModule.h>
#include "HalconCpp.h"

using namespace qzcv;
using namespace Halcon;
class polarParam;
namespace qzcv
{
	class LineDetect;
}

class polarMod : public UnitModule
{
public:
	explicit polarMod();
	explicit polarMod(const QString&);
	~polarMod();
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
	//input:两组可以组成直线的两个点
	//output:夹角,交点,组成这个角的其他两个点
	void getIntersection(cv::Point2d linePoints[2][2], double &angle, cv::Point2d &inter, cv::Point2d otherPoints[2]);
private:
	polarParam* m_param;
	friend class polarWdg;
	LineDetect *m_detector[4];

	double m_curRow[4];
	double m_curCol[4];
	double m_curAng[4];
	double m_curLen1[4];
	double m_curLen2[4];

	//0代表左，1代表右
	bool m_enable[2];
	double m_minAng[2];
	double m_maxAng[2];

	double m_ang[2];
	cv::Point2d m_inter[2];
	cv::Point2d m_angOtherPoint[2][2];
	cv::Point2d m_linePoint[2][2][2];

	int m_failIdx;
	int m_pol;
	int m_allok;

private:
	UnitInputPin<cv::Mat> p_im;
	UnitInputPin<cv::Mat> p_homMat2d;

	UnitOutputPin<bool> p_lf;
	UnitOutputPin<bool> p_rt;
};

