#pragma once
#include <moduleClass.h>
const char* const FactoryName = "ctrl";
using namespace qzcv;

#define STY_NORMAL "normal"
#define STY_SIMPLE "simple"

class CtrlFact : public ModuleFactory
{
public:
	explicit CtrlFact();
	virtual ~CtrlFact();
	virtual unsigned int classID()const { return 240980081; }
	virtual const QIcon* icon()const { return NULL; }
	virtual UnitModule* createModule();
	virtual UnitModule* createModule(const QString& dirPath);
	virtual QStringList getModSetWidgetStyles();
	virtual ModSetWidget* createModSetWidget();
	virtual ModSetWidget* createModSetWidget(const QString& style);
private:
	QStringList m_styleList;
};