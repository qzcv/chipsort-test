#pragma once
#include <moduleClass.h>
#include <imageModule.h>
#include "HalconCpp.h"
#include "chipMod.h"

using namespace qzcv;
using namespace Halcon;
class edgeJudgeParam;

#define CenterRow "centerRow"     //中心点坐1标
#define CenterCol "centerCol" 

class edgeJudgeMod : public ChipMod
{
public:
	explicit edgeJudgeMod();
	explicit edgeJudgeMod(const QString&);
	~edgeJudgeMod();
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
	edgeJudgeParam* m_param;
	friend class edgeJudgeWdg;
	Hobject *m_image;
private:
	UnitInputPin<cv::Mat> p_im;
	UnitInputPin<cv::Mat> p_homMat2d;
	UnitInputPin<double> p_pixSize;
	UnitInputPin<QList<cv::Point2d>> p_inCorners;
	UnitInputPin<RlRegion> p_inReferRegion;

	UnitOutputPin<QList<cv::Point2d>> p_corners;
};

