#pragma once
#include <moduleClass.h>
const char* const FactoryName = "composeND_pre";
using namespace qzcv;

class composeND_preFactory : public ModuleFactory
{
public:
	explicit composeND_preFactory();
	virtual ~composeND_preFactory();
	virtual unsigned int classID()const { return 94039853; }
	virtual const QIcon* icon()const { return NULL; }
	virtual UnitModule* createModule();
	virtual UnitModule* createModule(const QString& dirPath);
	virtual QStringList getModSetWidgetStyles();
	virtual ModSetWidget* createModSetWidget();
	virtual ModSetWidget* createModSetWidget(const QString& style);
private:
	QStringList m_styleList;
};

