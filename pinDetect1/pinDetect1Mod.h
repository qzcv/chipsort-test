#pragma once
#include <moduleClass.h>
#include <imageModule.h>
#include "HalconCpp.h"
#include "pinDetect1Param.h"
#include <vector>
#include "chipMod.h"

struct stPoint {
	double x;
	double y;
};

using std::vector;
using namespace qzcv;
using namespace Halcon;

class pinDetect1Mod : public ChipMod
{
public:
	explicit pinDetect1Mod();
	explicit pinDetect1Mod(const QString&);
	~pinDetect1Mod();
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
	bool IsPointInRegion(const Hobject *InputReg, double row, double col, int Larger = 0);
	int RansacLinear(stPoint* pstData, unsigned int nDataCnt,
		unsigned int nMinCnt, unsigned int nMaxIterCnt,
		unsigned int nConsesusCntThreshold, double dMaxErrorThreshold,
		double& dAA, double& dBB, double& dCC, double& dModelMeanError);
private:
	pinDetect1Param* m_param;
	friend class pinDetect1Wdg;
	Hobject *m_image;

	vector<bool> m_testItemsStatus;
	QString m_tmpHomDefine[2] = { "tmpHomMat2d1","tmpHomMat2d2" };
private:
	UnitInputPin<cv::Mat> p_im;
	UnitInputPin<cv::Mat> p_homMat2d[GRP_NUM];
	UnitInputPin<double> p_pixelSize;
	UnitInputPin<QList<cv::Point2d>> p_corners;
	UnitInputPin<cv::Mat> p_SearchHomMat2D;
	UnitInputPin<QList<cv::Point2d>> p_SearchCorners;

	//UnitOutputPin<bool> p_outLenEn;
	//UnitOutputPin<QList<double>> p_outLen;
	UnitOutputPin<QMap<QString, QVariant>> p_item;
	UnitOutputPin<QList<cv::Point2d>> p_outLead;//传出管脚尖坐标

	//UnitOutputPin<bool> p_lenEn;
	//UnitOutputPin<QList<double>> p_outLen;
};

