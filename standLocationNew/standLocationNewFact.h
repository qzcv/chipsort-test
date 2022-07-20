#pragma once
#include <moduleClass.h>
const char* const FactoryName = "standLocationNew";
using namespace qzcv;

class standLocationNewFact : public ModuleFactory
{
public:
	explicit standLocationNewFact();
	virtual ~standLocationNewFact();
	virtual unsigned int classID()const { return 219088604; }
	virtual const QIcon* icon()const { return NULL; }
	virtual UnitModule* createModule();
	virtual UnitModule* createModule(const QString& dirPath);
	virtual QStringList getModSetWidgetStyles();
	virtual ModSetWidget* createModSetWidget();
	virtual ModSetWidget* createModSetWidget(const QString& style);
private:
	QStringList m_styleList;
};

