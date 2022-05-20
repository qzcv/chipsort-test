#pragma once
#include <moduleClass.h>
const char* const FactoryName = "surfaceMult";
using namespace qzcv;

class surfaceMultFact : public ModuleFactory
{
public:
	explicit surfaceMultFact();
	virtual ~surfaceMultFact();
	virtual unsigned int classID()const { return 414650252; }
	virtual const QIcon* icon()const { return NULL; }
	virtual UnitModule* createModule();
	virtual UnitModule* createModule(const QString& dirPath);
	virtual QStringList getModSetWidgetStyles();
	virtual ModSetWidget* createModSetWidget();
	virtual ModSetWidget* createModSetWidget(const QString& style);
private:
	QStringList m_styleList;
};

