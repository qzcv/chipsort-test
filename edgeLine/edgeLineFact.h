#pragma once
#include <moduleClass.h>
const char* const FactoryName = "edgeLine";
using namespace qzcv;

class edgeLineFact : public ModuleFactory
{
public:
	explicit edgeLineFact();
	virtual ~edgeLineFact();
	virtual unsigned int classID()const { return 43651934; }
	virtual const QIcon* icon()const { return NULL; }
	virtual UnitModule* createModule();
	virtual UnitModule* createModule(const QString& dirPath);
	virtual QStringList getModSetWidgetStyles();
	virtual ModSetWidget* createModSetWidget();
	virtual ModSetWidget* createModSetWidget(const QString& style);
private:
	QStringList m_styleList;
};

