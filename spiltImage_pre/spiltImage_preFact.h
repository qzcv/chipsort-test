#pragma once
#include <moduleClass.h>
const char* const FactoryName = "spiltImage_pre";
using namespace qzcv;

class spiltImage_preFact : public ModuleFactory
{
public:
	explicit spiltImage_preFact();
	virtual ~spiltImage_preFact();
	virtual unsigned int classID()const { return 262797320; }
	virtual const QIcon* icon()const { return NULL; }
	virtual UnitModule* createModule();
	virtual UnitModule* createModule(const QString& dirPath);
	virtual QStringList getModSetWidgetStyles();
	virtual ModSetWidget* createModSetWidget();
	virtual ModSetWidget* createModSetWidget(const QString& style);
private:
	QStringList m_styleList;
};

