#include "standLocationNewFact.h"
#include "standLocationNewWdg.h"
#include "standLocationNewMod.h"

standLocationNewFact :: standLocationNewFact()
	: ModuleFactory(FactoryName, 0)
{

	getModSetWidgetStyles();
}

standLocationNewFact :: ~standLocationNewFact()
{

}

qzcv::UnitModule* standLocationNewFact::createModule()
{
	return new standLocationNewMod;
}

qzcv::UnitModule* standLocationNewFact::createModule(const QString& dirPath)
{
	return new standLocationNewMod(dirPath);
}

QStringList standLocationNewFact::getModSetWidgetStyles()
{
	m_styleList.clear();

	return m_styleList;
}

qzcv::ModSetWidget* standLocationNewFact::createModSetWidget()
{
	return new standLocationNewWdg;
}

qzcv::ModSetWidget* standLocationNewFact::createModSetWidget(const QString &style)
{
	for(int i=0;i<m_styleList.size();i++)
	{
		if(style==m_styleList.at(i))
			return new standLocationNewWdg(i);
	}
	return new standLocationNewWdg;
}

extern "C" Q_DECL_EXPORT int getFactoryCnt()
{
	return 1;
}

extern "C" Q_DECL_EXPORT void* createModuleFactory(int index)
{
	if(0==index)
	{
		return new standLocationNewFact;
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

