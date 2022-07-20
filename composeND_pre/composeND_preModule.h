#pragma once
#include <moduleClass.h>
#include <imageModule.h>
#include "composeND_preParam.h"

#define ENT_DISPIMG "dispImg"

using namespace qzcv;

class composeND_preModule : public UnitModule
{
public:
	explicit composeND_preModule();
	explicit composeND_preModule(const QString&);
	~composeND_preModule();
	int command(int cmdID, void* dataIn, void* dataOut);
	virtual int afterSetProperty(MetaProperty*);
protected:
	virtual void save(const QString& dirPath);
	virtual void load(const QString& dirPath);
	virtual void load(const QString& dirPath, QvsParamLevel level);
	virtual int run(const QString& funName);
	virtual void viewResult(ImageView* iv, const QString& funName,int);
	virtual void textResult(ResultText* text, const QString& funName);
private:
	void iniData();
	void createPins();
	void createEvents();
	void changePinsNum(int imNum);
private:
	composeND_preParam* m_param;
	friend class composeND_preWidget;
	typedef enum { noDet, emptyImg, wrongSize, composeSucceed }Res;
	Res m_res;
	int m_emptyIdx;
private:
	QList<UnitInputPin<cv::Mat>*> p_inIm;
	UnitOutputPin<cv::Mat> p_outIm;
};

