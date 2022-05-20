#pragma once
#include <moduleClass.h>
const char* const FactoryName = "arithmetic_pre";
using namespace qzcv;

class arithmetic_preFactory : public ModuleFactory
{
public:
	explicit arithmetic_preFactory();
	virtual ~arithmetic_preFactory();
	virtual unsigned int classID()const { return 102865659; }
	virtual const QIcon* icon()const { return NULL; }
	virtual UnitModule* createModule();
	virtual UnitModule* createModule(const QString& dirPath);
	virtual QStringList getModSetWidgetStyles();
	virtual ModSetWidget* createModSetWidget();
	virtual ModSetWidget* createModSetWidget(const QString& style);
private:
	QStringList m_styleList;
};

