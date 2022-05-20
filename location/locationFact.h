#pragma once
#include <moduleClass.h>
const char* const FactoryName = "location";
using namespace qzcv;

class locationFact : public ModuleFactory
{
public:
	explicit locationFact();
	virtual ~locationFact();
	virtual unsigned int classID()const { return 124219953; }
	virtual const QIcon* icon()const { return NULL; }
	virtual UnitModule* createModule();
	virtual UnitModule* createModule(const QString& dirPath);
	virtual QStringList getModSetWidgetStyles();
	virtual ModSetWidget* createModSetWidget();
	virtual ModSetWidget* createModSetWidget(const QString& style);
private:
	QStringList m_styleList;
};

