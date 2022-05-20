#pragma once
#include <moduleClass.h>
#include <imageModule.h>
#include "HalconCpp.h"

using namespace qzcv;
using namespace Halcon;
class imageSegment_preParam;

class imageSegment_preMod : public UnitModule
{
public:
	explicit imageSegment_preMod();
	explicit imageSegment_preMod(const QString&);
	~imageSegment_preMod();
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
	void calReg(Hobject* segImg);
private:
	imageSegment_preParam* m_param;
	friend class imageSegment_preWdg;

	Hobject *m_image;
private:
	UnitInputPin<cv::Mat> p_iIm;

	UnitOutputPin<cv::Mat> p_oIm;
};

