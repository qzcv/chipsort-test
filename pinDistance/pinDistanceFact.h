#pragma once
#include <moduleClass.h>
const char* const FactoryName = "pinDistance";
using namespace qzcv;

class pinDistanceFact : public ModuleFactory
{
public:
	explicit pinDistanceFact();
	virtual ~pinDistanceFact();
	virtual unsigned int classID()const { return 291739990; }
	virtual const QIcon* icon()const { return NULL; }
	virtual UnitModule* createModule();
	virtual UnitModule* createModule(const QString& dirPath);
	virtual QStringList getModSetWidgetStyles();
	virtual ModSetWidget* createModSetWidget();
	virtual ModSetWidget* createModSetWidget(const QString& style);
private:
	QStringList m_styleList;
};

