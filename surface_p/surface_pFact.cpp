#include "surface_pFact.h"
#include "surface_pWdg.h"
#include "surface_pMod.h"

surface_pFact :: surface_pFact()
	: ModuleFactory(FactoryName, 0)
{

	getModSetWidgetStyles();
}

surface_pFact :: ~surface_pFact()
{

}

qzcv::UnitModule* surface_pFact::createModule()
{
	return new surface_pMod;
}

qzcv::UnitModule* surface_pFact::createModule(const QString& dirPath)
{
	return new surface_pMod(dirPath);
}

QStringList surface_pFact::getModSetWidgetStyles()
{
	m_styleList.clear();

	return m_styleList;
}

qzcv::ModSetWidget* surface_pFact::createModSetWidget()
{
	return new surface_pWdg;
}

qzcv::ModSetWidget* surface_pFact::createModSetWidget(const QString &style)
{
	for(int i=0;i<m_styleList.size();i++)
	{
		if(style==m_styleList.at(i))
			return new surface_pWdg(i);
	}
	return new surface_pWdg;
}

extern "C" Q_DECL_EXPORT int getFactoryCnt()
{
	return 1;
}

extern "C" Q_DECL_EXPORT void* createModuleFactory(int index)
{
	if(0==index)
	{
		return new surface_pFact;
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

