#pragma once
#include <moduleClass.h>
#include <imageModule.h>
#include "HalconCpp.h"
#include "chipMod.h"

#define MAX_TESTITEMS_NUM 16

using namespace qzcv;
using namespace Halcon;
class pinDistanceParam;

class pinDistanceMod : public ChipMod
{
public:
	explicit pinDistanceMod();
	explicit pinDistanceMod(const QString&);
	~pinDistanceMod();
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
	pinDistanceParam* m_param;
	friend class pinDistanceWdg;
	Hobject *m_image;
	bool m_testItemsStatus[MAX_TESTITEMS_NUM];
private:
	UnitInputPin<cv::Mat> p_im;
	UnitInputPin<cv::Mat> p_homMat2d;
	UnitInputPin<QList<cv::Point2d>> p_corners;
};

