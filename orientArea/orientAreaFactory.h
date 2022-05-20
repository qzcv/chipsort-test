#pragma once
#include <moduleClass.h>
const char* const FactoryName = "orientArea";
using namespace qzcv;

class orientAreaFactory : public ModuleFactory
{
public:
	explicit orientAreaFactory();
	virtual ~orientAreaFactory();
	virtual unsigned int classID()const { return 197854917; }
	virtual const QIcon* icon()const { return NULL; }
	virtual UnitModule* createModule();
	virtual UnitModule* createModule(const QString& dirPath);
	virtual QStringList getModSetWidgetStyles();
	virtual ModSetWidget* createModSetWidget();
	virtual ModSetWidget* createModSetWidget(const QString& style);
private:
	QStringList m_styleList;
};

