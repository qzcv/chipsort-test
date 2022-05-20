#include "colorPickerFact.h"
#include "colorPickerWdg.h"
#include "colorPickerMod.h"

ColorPickerFact :: ColorPickerFact()
	: ModuleFactory(FactoryName, 0)
{

	getModSetWidgetStyles();
}

ColorPickerFact :: ~ColorPickerFact()
{

}

qzcv::UnitModule* ColorPickerFact::createModule()
{
	return new ColorPickerMod;
}

qzcv::UnitModule* ColorPickerFact::createModule(const QString& dirPath)
{
	return new ColorPickerMod(dirPath);
}

QStringList ColorPickerFact::getModSetWidgetStyles()
{
	m_styleList.clear();

	return m_styleList;
}

qzcv::ModSetWidget* ColorPickerFact::createModSetWidget()
{
	return new ColorPickerWdg;
}

qzcv::ModSetWidget* ColorPickerFact::createModSetWidget(const QString &style)
{
	for(int i=0;i<m_styleList.size();i++)
	{
		if(style==m_styleList.at(i))
			return new ColorPickerWdg(i);
	}
	return new ColorPickerWdg;
}

extern "C" Q_DECL_EXPORT int getFactoryCnt()
{
	return 1;
}

extern "C" Q_DECL_EXPORT void* createModuleFactory(int index)
{
	if(0==index)
	{
		return new ColorPickerFact;
	}
	else
		return NULL;
}

extern "C" Q_DECL_EXPORT int isDebug() {
#if defined(_DEBUG)||defined(QT_DEBUG)
	return 1;
#else
	return 0;
#endif
}

