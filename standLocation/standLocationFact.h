#pragma once
#include <moduleClass.h>
const char* const FactoryName = "standLocation";
using namespace qzcv;

class standLocationFact : public ModuleFactory
{
public:
	explicit standLocationFact();
	virtual ~standLocationFact();
	virtual unsigned int classID()const { return 219087242; }
	virtual const QIcon* icon()const { return NULL; }
	virtual UnitModule* createModule();
	virtual UnitModule* createModule(const QString& dirPath);
	virtual QStringList getModSetWidgetStyles();
	virtual ModSetWidget* createModSetWidget();
	virtual ModSetWidget* createModSetWidget(const QString& style);
private:
	QStringList m_styleList;
};

