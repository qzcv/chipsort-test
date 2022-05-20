#pragma once
#include <moduleClass.h>
const char* const FactoryName = "pinLength1";
using namespace qzcv;

class pinLength1Fact : public ModuleFactory
{
public:
	explicit pinLength1Fact();
	virtual ~pinLength1Fact();
	virtual unsigned int classID()const { return 258415379; }
	virtual const QIcon* icon()const { return NULL; }
	virtual UnitModule* createModule();
	virtual UnitModule* createModule(const QString& dirPath);
	virtual QStringList getModSetWidgetStyles();
	virtual ModSetWidget* createModSetWidget();
	virtual ModSetWidget* createModSetWidget(const QString& style);
private:
	QStringList m_styleList;
};

