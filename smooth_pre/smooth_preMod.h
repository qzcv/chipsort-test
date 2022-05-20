#pragma once
#include <moduleClass.h>
#include <imageModule.h>
#include "HalconCpp.h"

using namespace Halcon;
using namespace qzcv;
class smooth_preParam;

class smooth_preMod : public UnitModule
{
public:
	explicit smooth_preMod();
	explicit smooth_preMod(const QString&);
	~smooth_preMod();
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
	smooth_preParam* m_param;
	friend class smooth_preWdg;

	Hobject *m_image;
private:
	UnitInputPin<cv::Mat> p_iIm;

	UnitOutputPin<cv::Mat> p_oIm;
};

