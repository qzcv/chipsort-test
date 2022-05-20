#pragma once
#include <moduleClass.h>
const char* const FactoryName = "transform_pre";
using namespace qzcv;

class transform_preFact : public ModuleFactory
{
public:
	explicit transform_preFact();
	virtual ~transform_preFact();
	virtual unsigned int classID()const { return 355822657; }
	virtual const QIcon* icon()const { return NULL; }
	virtual UnitModule* createModule();
	virtual UnitModule* createModule(const QString& dirPath);
	virtual QStringList getModSetWidgetStyles();
	virtual ModSetWidget* createModSetWidget();
	virtual ModSetWidget* createModSetWidget(const QString& style);
private:
	QStringList m_styleList;
};

