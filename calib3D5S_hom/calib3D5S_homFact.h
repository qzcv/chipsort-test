#pragma once
#include <moduleClass.h>
const char* const FactoryName = "calib3D5S_hom";
using namespace qzcv;

class calib3D5S_homFact : public ModuleFactory
{
public:
	explicit calib3D5S_homFact();
	virtual ~calib3D5S_homFact();
	virtual unsigned int classID()const { return 229036163; }
	virtual const QIcon* icon()const { return NULL; }
	virtual UnitModule* createModule();
	virtual UnitModule* createModule(const QString& dirPath);
	virtual QStringList getModSetWidgetStyles();
	virtual ModSetWidget* createModSetWidget();
	virtual ModSetWidget* createModSetWidget(const QString& style);
private:
	QStringList m_styleList;
};

