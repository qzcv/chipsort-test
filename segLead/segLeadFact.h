#pragma once
#include <moduleClass.h>
const char* const FactoryName = "segLead";
using namespace qzcv;

class segLeadFact : public ModuleFactory
{
public:
	explicit segLeadFact();
	virtual ~segLeadFact();
	virtual unsigned int classID()const { return 18471403; }
	virtual const QIcon* icon()const { return NULL; }
	virtual UnitModule* createModule();
	virtual UnitModule* createModule(const QString& dirPath);
	virtual QStringList getModSetWidgetStyles();
	virtual ModSetWidget* createModSetWidget();
	virtual ModSetWidget* createModSetWidget(const QString& style);
private:
	QStringList m_styleList;
};

