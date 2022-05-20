#pragma once
#include <moduleClass.h>
#include <imageModule.h>
#include "HalconCpp.h"

using namespace Halcon;
using namespace qzcv;
class transform_preParam;

class transform_preMod : public UnitModule
{
public:
	explicit transform_preMod();
	explicit transform_preMod(const QString&);
	~transform_preMod();
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
	transform_preParam* m_param;
	friend class transform_preWdg;

	enum { Gray = 0, Red, Green, BLue, Hue, Saturation, Brightness, Lab_a, Lab_b };
	Hobject *m_image;
private:
	UnitInputPin<cv::Mat> p_iIm;

	UnitOutputPin<cv::Mat> p_oIm;
};

