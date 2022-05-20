#include "locationGuiFact.h"
#include "locationGuiWdg.h"
#include "locationGuiMod.h"

locationGuiFact :: locationGuiFact()
	: ModuleFactory(FactoryName, 0)
{

	getModSetWidgetStyles();
}

locationGuiFact :: ~locationGuiFact()
{

}

qzcv::UnitModule* locationGuiFact::createModule()
{
	return new locationGuiMod;
}

qzcv::UnitModule* locationGuiFact::createModule(const QString& dirPath)
{
	return new locationGuiMod(dirPath);
}

QStringList locationGuiFact::getModSetWidgetStyles()
{
	m_styleList.clear();

	return m_styleList;
}

qzcv::ModSetWidget* locationGuiFact::createModSetWidget()
{
	return new locationGuiWdg;
}

qzcv::ModSetWidget* locationGuiFact::createModSetWidget(const QString &style)
{
	for(int i=0;i<m_styleList.size();i++)
	{
		if(style==m_styleList.at(i))
			return new locationGuiWdg(i);
	}
	return new locationGuiWdg;
}

extern "C" Q_DECL_EXPORT int getFactoryCnt()
{
	return 1;
}

extern "C" Q_DECL_EXPORT void* createModuleFactory(int index)
{
	if(0==index)
	{
		return new locationGuiFact;
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

