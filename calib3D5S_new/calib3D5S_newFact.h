#pragma once
#include <moduleClass.h>
const char* const FactoryName = "calib3D5S_new";
using namespace qzcv;

class calib3D5S_newFact : public ModuleFactory
{
public:
	explicit calib3D5S_newFact();
	virtual ~calib3D5S_newFact();
	virtual unsigned int classID()const { return 229546163; }
	virtual const QIcon* icon()const { return NULL; }
	virtual UnitModule* createModule();
	virtual UnitModule* createModule(const QString& dirPath);
	virtual QStringList getModSetWidgetStyles();
	virtual ModSetWidget* createModSetWidget();
	virtual ModSetWidget* createModSetWidget(const QString& style);
private:
	QStringList m_styleList;
};

