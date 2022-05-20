#pragma once
#include <moduleClass.h>
const char* const FactoryName = "edgeLocation";
using namespace qzcv;

class edgeLocationFact : public ModuleFactory
{
public:
	explicit edgeLocationFact();
	virtual ~edgeLocationFact();
	virtual unsigned int classID()const { return 243182420; }
	virtual const QIcon* icon()const { return NULL; }
	virtual UnitModule* createModule();
	virtual UnitModule* createModule(const QString& dirPath);
	virtual QStringList getModSetWidgetStyles();
	virtual ModSetWidget* createModSetWidget();
	virtual ModSetWidget* createModSetWidget(const QString& style);
private:
	QStringList m_styleList;
};

