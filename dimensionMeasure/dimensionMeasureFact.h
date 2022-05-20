#pragma once
#include <moduleClass.h>
const char* const FactoryName = "dimensionMeasure";
using namespace qzcv;

class dimensionMeasureFact : public ModuleFactory
{
public:
	explicit dimensionMeasureFact();
	virtual ~dimensionMeasureFact();
	virtual unsigned int classID()const { return 181675068; }
	virtual const QIcon* icon()const { return NULL; }
	virtual UnitModule* createModule();
	virtual UnitModule* createModule(const QString& dirPath);
	virtual QStringList getModSetWidgetStyles();
	virtual ModSetWidget* createModSetWidget();
	virtual ModSetWidget* createModSetWidget(const QString& style);
private:
	QStringList m_styleList;
};

