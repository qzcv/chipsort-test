#pragma once
#include <moduleClass.h>
const char* const FactoryName = "edgeJudge";
using namespace qzcv;

class edgeJudgeFact : public ModuleFactory
{
public:
	explicit edgeJudgeFact();
	virtual ~edgeJudgeFact();
	virtual unsigned int classID()const { return 83651036; }
	virtual const QIcon* icon()const { return NULL; }
	virtual UnitModule* createModule();
	virtual UnitModule* createModule(const QString& dirPath);
	virtual QStringList getModSetWidgetStyles();
	virtual ModSetWidget* createModSetWidget();
	virtual ModSetWidget* createModSetWidget(const QString& style);
private:
	QStringList m_styleList;
};

