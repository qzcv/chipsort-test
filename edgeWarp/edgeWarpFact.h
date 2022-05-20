#pragma once
#include <moduleClass.h>
const char* const FactoryName = "edgeWarp";
using namespace qzcv;

class edgeWarpFact : public ModuleFactory
{
public:
	explicit edgeWarpFact();
	virtual ~edgeWarpFact();
	virtual unsigned int classID()const { return 43662185; }
	virtual const QIcon* icon()const { return NULL; }
	virtual UnitModule* createModule();
	virtual UnitModule* createModule(const QString& dirPath);
	virtual QStringList getModSetWidgetStyles();
	virtual ModSetWidget* createModSetWidget();
	virtual ModSetWidget* createModSetWidget(const QString& style);
private:
	QStringList m_styleList;
};

