#include "polarFact.h"
#include "polarWdg.h"
#include "polarMod.h"

polarFact :: polarFact()
	: ModuleFactory(FactoryName, 0)
{

	getModSetWidgetStyles();
}

polarFact :: ~polarFact()
{

}

qzcv::UnitModule* polarFact::createModule()
{
	return new polarMod;
}

qzcv::UnitModule* polarFact::createModule(const QString& dirPath)
{
	return new polarMod(dirPath);
}

QStringList polarFact::getModSetWidgetStyles()
{
	m_styleList.clear();

	return m_styleList;
}

qzcv::ModSetWidget* polarFact::createModSetWidget()
{
	return new polarWdg;
}

qzcv::ModSetWidget* polarFact::createModSetWidget(const QString &style)
{
	for(int i=0;i<m_styleList.size();i++)
	{
		if(style==m_styleList.at(i))
			return new polarWdg(i);
	}
	return new polarWdg;
}

extern "C" Q_DECL_EXPORT int getFactoryCnt()
{
	return 1;
}

extern "C" Q_DECL_EXPORT void* createModuleFactory(int index)
{
	if(0==index)
	{
		return new polarFact;
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

