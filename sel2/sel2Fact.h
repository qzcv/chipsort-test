#pragma once
#include <moduleClass.h>
const char* const FactoryName = "sel2";
using namespace qzcv;

class sel2Fact : public ModuleFactory
{
public:
	explicit sel2Fact();
	virtual ~sel2Fact();
	virtual unsigned int classID()const { return 18540; }
	virtual const QIcon* icon()const { return NULL; }
	virtual UnitModule* createModule();
	virtual UnitModule* createModule(const QString& dirPath);
	virtual QStringList getModSetWidgetStyles();
	virtual ModSetWidget* createModSetWidget();
	virtual ModSetWidget* createModSetWidget(const QString& style);
private:
	QStringList m_styleList;
};

