#pragma once
#include <moduleClass.h>
const char* const FactoryName = "pinWarp";
using namespace qzcv;

class pinWarpFact : public ModuleFactory
{
public:
	explicit pinWarpFact();
	virtual ~pinWarpFact();
	virtual unsigned int classID()const { return 15952185; }
	virtual const QIcon* icon()const { return NULL; }
	virtual UnitModule* createModule();
	virtual UnitModule* createModule(const QString& dirPath);
	virtual QStringList getModSetWidgetStyles();
	virtual ModSetWidget* createModSetWidget();
	virtual ModSetWidget* createModSetWidget(const QString& style);
private:
	QStringList m_styleList;
};

