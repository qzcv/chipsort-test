#pragma once
#include <moduleClass.h>
#include <imageModule.h>

using namespace qzcv;
class spiltImage_preParam;

class spiltImage_preMod : public UnitModule
{
public:
	explicit spiltImage_preMod();
	explicit spiltImage_preMod(const QString&);
	~spiltImage_preMod();
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
	spiltImage_preParam* m_param;
	friend class spiltImage_preWdg;
private:
	UnitInputPin<cv::Mat> p_iIm;

	UnitOutputPin<cv::Mat> p_oIm;
};

