#pragma once
#include <moduleClass.h>
const char* const FactoryName = "GRRreport";
using namespace qzcv;

class GRRreportFact : public ModuleFactory
{
public:
	explicit GRRreportFact();
	virtual ~GRRreportFact();
	virtual unsigned int classID()const { return 268875657; }
	virtual const QIcon* icon()const { return NULL; }
	virtual UnitModule* createModule();
	virtual UnitModule* createModule(const QString& dirPath);
	virtual QStringList getModSetWidgetStyles();
	virtual ModSetWidget* createModSetWidget();
	virtual ModSetWidget* createModSetWidget(const QString& style);
private:
	QStringList m_styleList;
};

