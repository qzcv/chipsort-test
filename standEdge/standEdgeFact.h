#pragma once
#include <moduleClass.h>
const char* const FactoryName = "standEdge";
using namespace qzcv;

class standEdgeFact : public ModuleFactory
{
public:
	explicit standEdgeFact();
	virtual ~standEdgeFact();
	virtual unsigned int classID()const { return 239133436; }
	virtual const QIcon* icon()const { return NULL; }
	virtual UnitModule* createModule();
	virtual UnitModule* createModule(const QString& dirPath);
	virtual QStringList getModSetWidgetStyles();
	virtual ModSetWidget* createModSetWidget();
	virtual ModSetWidget* createModSetWidget(const QString& style);
private:
	QStringList m_styleList;
};

