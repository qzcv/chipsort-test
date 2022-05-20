#pragma once
#include <moduleClass.h>
const char* const FactoryName = "pinDetect1";
using namespace qzcv;

class pinDetect1Fact : public ModuleFactory
{
public:
	explicit pinDetect1Fact();
	virtual ~pinDetect1Fact();
	virtual unsigned int classID()const { return 378335942; }
	virtual const QIcon* icon()const { return NULL; }
	virtual UnitModule* createModule();
	virtual UnitModule* createModule(const QString& dirPath);
	virtual QStringList getModSetWidgetStyles();
	virtual ModSetWidget* createModSetWidget();
	virtual ModSetWidget* createModSetWidget(const QString& style);
private:
	QStringList m_styleList;
};

