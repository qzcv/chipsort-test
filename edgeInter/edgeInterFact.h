#pragma once
#include <moduleClass.h>
const char* const FactoryName = "edgeInter";
using namespace qzcv;

class edgeInterFact : public ModuleFactory
{
public:
	explicit edgeInterFact();
	virtual ~edgeInterFact();
	virtual unsigned int classID()const { return 213649494; }
	virtual const QIcon* icon()const { return NULL; }
	virtual UnitModule* createModule();
	virtual UnitModule* createModule(const QString& dirPath);
	virtual QStringList getModSetWidgetStyles();
	virtual ModSetWidget* createModSetWidget();
	virtual ModSetWidget* createModSetWidget(const QString& style);
private:
	QStringList m_styleList;
};

