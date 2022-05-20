#pragma once
#include <moduleClass.h>
const char* const FactoryName = "ColorPicker";
using namespace qzcv;

class ColorPickerFact : public ModuleFactory
{
public:
	explicit ColorPickerFact();
	virtual ~ColorPickerFact();
	virtual unsigned int classID()const { return 140958582; }
	virtual const QIcon* icon()const { return NULL; }
	virtual UnitModule* createModule();
	virtual UnitModule* createModule(const QString& dirPath);
	virtual QStringList getModSetWidgetStyles();
	virtual ModSetWidget* createModSetWidget();
	virtual ModSetWidget* createModSetWidget(const QString& style);
private:
	QStringList m_styleList;
};

