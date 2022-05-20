#include "pinDistanceFact.h"
#include "pinDistanceWdg.h"
#include "pinDistanceMod.h"

pinDistanceFact :: pinDistanceFact()
	: ModuleFactory(FactoryName, 0)
{

	getModSetWidgetStyles();
}

pinDistanceFact :: ~pinDistanceFact()
{

}

qzcv::UnitModule* pinDistanceFact::createModule()
{
	return new pinDistanceMod;
}

qzcv::UnitModule* pinDistanceFact::createModule(const QString& dirPath)
{
	return new pinDistanceMod(dirPath);
}

QStringList pinDistanceFact::getModSetWidgetStyles()
{
	m_styleList.clear();

	return m_styleList;
}

qzcv::ModSetWidget* pinDistanceFact::createModSetWidget()
{
	return new pinDistanceWdg;
}

qzcv::ModSetWidget* pinDistanceFact::createModSetWidget(const QString &style)
{
	for(int i=0;i<m_styleList.size();i++)
	{
		if(style==m_styleList.at(i))
			return new pinDistanceWdg(i);
	}
	return new pinDistanceWdg;
}

extern "C" Q_DECL_EXPORT int getFactoryCnt()
{
	return 1;
}

extern "C" Q_DECL_EXPORT void* createModuleFactory(int index)
{
	if(0==index)
	{
		return new pinDistanceFact;
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

