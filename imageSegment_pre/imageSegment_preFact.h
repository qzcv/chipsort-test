#pragma once
#include <moduleClass.h>
const char* const FactoryName = "imageSegment_pre";
using namespace qzcv;

class imageSegment_preFact : public ModuleFactory
{
public:
	explicit imageSegment_preFact();
	virtual ~imageSegment_preFact();
	virtual unsigned int classID()const { return 217584520; }
	virtual const QIcon* icon()const { return NULL; }
	virtual UnitModule* createModule();
	virtual UnitModule* createModule(const QString& dirPath);
	virtual QStringList getModSetWidgetStyles();
	virtual ModSetWidget* createModSetWidget();
	virtual ModSetWidget* createModSetWidget(const QString& style);
private:
	QStringList m_styleList;
};

