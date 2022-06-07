#pragma once
#include <moduleClass.h>
#include <imageModule.h>

using namespace qzcv;
class sel2Param;

class sel2Mod : public UnitModule
{
public:
	explicit sel2Mod();
	explicit sel2Mod(const QString&);
	~sel2Mod();
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
	sel2Param* m_param;
	friend class sel2Wdg;
private:
	UnitOutputPin<int> p_int;
};

