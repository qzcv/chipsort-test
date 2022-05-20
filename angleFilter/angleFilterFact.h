#pragma once
#include <moduleClass.h>
const char* const FactoryName = "angleFilter";
using namespace qzcv;

class angleFilterFact : public ModuleFactory
{
public:
	explicit angleFilterFact();
	virtual ~angleFilterFact();
	virtual unsigned int classID()const { return 209414591; }
	virtual const QIcon* icon()const { return NULL; }
	virtual UnitModule* createModule();
	virtual UnitModule* createModule(const QString& dirPath);
	virtual QStringList getModSetWidgetStyles();
	virtual ModSetWidget* createModSetWidget();
	virtual ModSetWidget* createModSetWidget(const QString& style);
private:
	QStringList m_styleList;
};

