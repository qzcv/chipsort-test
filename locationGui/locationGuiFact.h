#pragma once
#include <moduleClass.h>
const char* const FactoryName = "locationGui";
using namespace qzcv;

class locationGuiFact : public ModuleFactory
{
public:
	explicit locationGuiFact();
	virtual ~locationGuiFact();
	virtual unsigned int classID()const { return 205019953; }
	virtual const QIcon* icon()const { return NULL; }
	virtual UnitModule* createModule();
	virtual UnitModule* createModule(const QString& dirPath);
	virtual QStringList getModSetWidgetStyles();
	virtual ModSetWidget* createModSetWidget();
	virtual ModSetWidget* createModSetWidget(const QString& style);
private:
	QStringList m_styleList;
};

