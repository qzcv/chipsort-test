#pragma once
#include <moduleClass.h>
#include <imageModule.h>
#include "HalconCpp.h"
#include "pinLength1Param.h"
#include "chipMod.h"

#define MAX_TESTITEMS_NUM 16

using namespace qzcv;
using namespace Halcon;

class pinLength1Mod : public ChipMod
{
public:
	explicit pinLength1Mod();
	explicit pinLength1Mod(const QString&);
	~pinLength1Mod();
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
	bool IsPointInRegion(const Hobject *InputReg, double row, double col);
private:
	pinLength1Param* m_param;
	friend class pinLength1Wdg;
	Hobject *m_image;
	double m_row[5], m_col[5];  //前面模块传进来的定位框，左上角顺时1针
	HTuple m_homeMat2D;  //转换矩1阵

	bool m_testItemsStatus[MAX_TESTITEMS_NUM];

private:
	UnitInputPin<cv::Mat> p_im;
	UnitInputPin<cv::Mat> p_homMat2D;
	UnitInputPin<QList<cv::Point2d>> p_corners;
	UnitInputPin<double> p_pixelSize;
	UnitInputPin<cv::Mat> p_searchHomMat2D;
	UnitInputPin<int> p_inputIdx;
	UnitInputPin<QList<cv::Point2d>> p_searchCorners;

	UnitOutputPin<QList<double>> p_outLen;
	UnitOutputPin<QMap<QString, QVariant>> p_item;

// 	UnitOutputPin<bool> p_resultOk;
// 	UnitOutputPin<bool> p_lenEnable;
// 	UnitOutputPin<QList<double>> p_outLen;
// 	UnitOutputPin<bool> p_widEnable;
// 	UnitOutputPin<QList<double>> p_outWid;
// 	UnitOutputPin<bool> p_pitchEnable;
// 	UnitOutputPin<QList<double>> p_outPitch;
// 	UnitOutputPin<bool> p_lenDiffEnable;
// 	UnitOutputPin<QList<double>> p_outLenDiff;
// 	UnitOutputPin<QList<int>> p_outCount;
};

