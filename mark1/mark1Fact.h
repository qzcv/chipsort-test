#pragma once
#include <moduleClass.h>
const char* const FactoryName = "mark1";
using namespace qzcv;

class mark1Fact : public ModuleFactory
{
public:
	explicit mark1Fact();
	virtual ~mark1Fact();
	virtual unsigned int classID()const { return 121829; }
	virtual const QIcon* icon()const { return NULL; }
	virtual UnitModule* createModule();
	virtual UnitModule* createModule(const QString& dirPath);
	virtual QStringList getModSetWidgetStyles();
	virtual ModSetWidget* createModSetWidget();
	virtual ModSetWidget* createModSetWidget(const QString& style);
private:
	QStringList m_styleList;
};

