#pragma once
#include <moduleClass.h>
const char* const FactoryName = "nccMatch_fourDir";
using namespace qzcv;

class nccMatch_fourDirFact : public ModuleFactory
{
public:
	explicit nccMatch_fourDirFact();
	virtual ~nccMatch_fourDirFact();
	virtual unsigned int classID()const { return 408939324; }
	virtual const QIcon* icon()const { return NULL; }
	virtual UnitModule* createModule();
	virtual UnitModule* createModule(const QString& dirPath);
	virtual QStringList getModSetWidgetStyles();
	virtual ModSetWidget* createModSetWidget();
	virtual ModSetWidget* createModSetWidget(const QString& style);
private:
	QStringList m_styleList;
};

