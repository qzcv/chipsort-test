#pragma once
#include <moduleClass.h>
#include <imageModule.h>
#include "edgeInterParam.h"
#include "HalconCpp.h"

using namespace qzcv;
using namespace Halcon;
namespace qzcv {
	class LineDetect;
}

class edgeInterMod : public UnitModule
{
public:
	explicit edgeInterMod();
	explicit edgeInterMod(const QString&);
	~edgeInterMod();
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
	edgeInterParam* m_param;
	friend class edgeInterWdg;
	double m_centerX, m_centerY;

	LineDetect *m_detect[MAX_EDGE_LINE];
	bool m_roiFound[MAX_EDGE_LINE];		//各边界是否找到

	double m_begRow[MAX_EDGE_LINE], m_begCol[MAX_EDGE_LINE];//得到的边界线条的坐标
	double m_endRow[MAX_EDGE_LINE], m_endCol[MAX_EDGE_LINE];//得到的边界线条的坐标

	bool m_errInterOK;
	double m_errInterDist;

	bool m_errXok, m_errYok;
	double m_errX, m_errY;

	int m_allok;
private:
	UnitInputPin<cv::Mat> p_im;
	UnitInputPin<cv::Mat> p_homMat2d;

	UnitOutputPin<QList<cv::Point2d>> p_inters;
};

