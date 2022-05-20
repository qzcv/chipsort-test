#pragma once
#include <moduleClass.h>
const char* const FactoryName = "smooth_pre";
using namespace qzcv;

class smooth_preFact : public ModuleFactory
{
public:
	explicit smooth_preFact();
	virtual ~smooth_preFact();
	virtual unsigned int classID()const { return 367559985; }
	virtual const QIcon* icon()const { return NULL; }
	virtual UnitModule* createModule();
	virtual UnitModule* createModule(const QString& dirPath);
	virtual QStringList getModSetWidgetStyles();
	virtual ModSetWidget* createModSetWidget();
	virtual ModSetWidget* createModSetWidget(const QString& style);
private:
	QStringList m_styleList;
};

