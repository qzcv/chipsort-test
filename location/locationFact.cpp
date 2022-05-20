#include "locationFact.h"
#include "locationWdg.h"
#include "locationMod.h"

locationFact :: locationFact()
	: ModuleFactory(FactoryName, 0)
{

	getModSetWidgetStyles();
}

locationFact :: ~locationFact()
{

}

qzcv::UnitModule* locationFact::createModule()
{
	return new locationMod;
}

qzcv::UnitModule* locationFact::createModule(const QString& dirPath)
{
	return new locationMod(dirPath);
}

QStringList locationFact::getModSetWidgetStyles()
{
	m_styleList.clear();

	return m_styleList;
}

qzcv::ModSetWidget* locationFact::createModSetWidget()
{
	return new locationWdg;
}

qzcv::ModSetWidget* locationFact::createModSetWidget(const QString &style)
{
	for(int i=0;i<m_styleList.size();i++)
	{
		if(style==m_styleList.at(i))
			return new locationWdg(i);
	}
	return new locationWdg;
}

extern "C" Q_DECL_EXPORT int getFactoryCnt()
{
	return 1;
}

extern "C" Q_DECL_EXPORT void* createModuleFactory(int index)
{
	if(0==index)
	{
		return new locationFact;
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

