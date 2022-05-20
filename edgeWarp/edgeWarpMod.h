#pragma once
#include <moduleClass.h>
#include <imageModule.h>
#include "HalconCpp.h"
#include "edgeWarpParam.h"

using namespace qzcv;
using namespace Halcon;
namespace qzcv {
	class LineDetect;
}

class edgeWarpMod : public UnitModule
{
public:
	explicit edgeWarpMod();
	explicit edgeWarpMod(const QString&);
	~edgeWarpMod();
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
	edgeWarpParam* m_param;
	friend class edgeWarpWdg;
	LineDetect *m_detect[MAX_EDGE_LINE];
	bool m_roiFound[MAX_EDGE_LINE]; //各边界是否找1到

	double m_leadRow[MAX_EDGE_LINE - 1], m_leadCol[MAX_EDGE_LINE - 1];
	double m_begRow[MAX_EDGE_LINE], m_begCol[MAX_EDGE_LINE];//得到的边界线条的坐标
	double m_endRow[MAX_EDGE_LINE], m_endCol[MAX_EDGE_LINE];//得到的边界线条的坐标

	double m_warpVal[MAX_EDGE_LINE - 1];  //保存得出的两个翘曲值
	bool m_warpOk[MAX_EDGE_LINE - 1];
	bool m_warpDiffOk;
	double m_warpDiff;

	int m_allok;

private:
	UnitInputPin<cv::Mat> p_im;
	UnitInputPin<cv::Mat> p_homMat2d;
	UnitInputPin<QList<double>> p_pixelSize;
};

