#pragma once
#include <moduleClass.h>
const char* const FactoryName = "surface_p";
using namespace qzcv;

class surface_pFact : public ModuleFactory
{
public:
	explicit surface_pFact();
	virtual ~surface_pFact();
	virtual unsigned int classID()const { return 351750267; }
	virtual const QIcon* icon()const { return NULL; }
	virtual UnitModule* createModule();
	virtual UnitModule* createModule(const QString& dirPath);
	virtual QStringList getModSetWidgetStyles();
	virtual ModSetWidget* createModSetWidget();
	virtual ModSetWidget* createModSetWidget(const QString& style);
private:
	QStringList m_styleList;
};

