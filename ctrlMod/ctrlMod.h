#pragma once
#include <moduleClass.h>
#include <imageModule.h>
#include "HalconCpp.h"

using namespace qzcv;
using namespace Halcon;
class CtrlParam;

#define ENT_CLEAR "clear"

class CtrlMod : public UnitModule
{
	Q_OBJECT
public:
	explicit CtrlMod();
	explicit CtrlMod(const QString&);
	~CtrlMod();
	int command(int cmdID, void* dataIn, void* dataOut);
	virtual int afterSetProperty(MetaProperty*);

signals:
	void test();
protected:
	virtual void save(const QString& dirPath);
	virtual void load(const QString& dirPath, QvsParamLevel level);
	virtual int run(const QString& funName);
	virtual void viewResult(ImageView* iv, const QString& funName, int);
	virtual void textResult(ResultText* text, const QString& funName);
private:
	void iniData();
	void createEvents();

	void setStationNum();

private:
	CtrlParam* m_param;
	friend class CtrlWdg;
private:
	QList<UnitInputPin<QList<double>> *> p_calibInfo;
};

class CtrlParam
{
public:
	explicit CtrlParam();
	~CtrlParam();
	virtual void readWriteParam(bool r, const QString& dirPath, int level);

	int stNum;
	int stIdx;
	QStringList stName;
};