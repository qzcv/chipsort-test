#pragma once
#include <moduleClass.h>
const char* const FactoryName = "polar";
using namespace qzcv;

class polarFact : public ModuleFactory
{
public:
	explicit polarFact();
	virtual ~polarFact();
	virtual unsigned int classID()const { return 165117; }
	virtual const QIcon* icon()const { return NULL; }
	virtual UnitModule* createModule();
	virtual UnitModule* createModule(const QString& dirPath);
	virtual QStringList getModSetWidgetStyles();
	virtual ModSetWidget* createModSetWidget();
	virtual ModSetWidget* createModSetWidget(const QString& style);
private:
	QStringList m_styleList;
};

