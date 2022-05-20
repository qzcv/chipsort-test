#include "standLocationFact.h"
#include "standLocationWdg.h"
#include "standLocationMod.h"

standLocationFact :: standLocationFact()
	: ModuleFactory(FactoryName, 0)
{

	getModSetWidgetStyles();
}

standLocationFact :: ~standLocationFact()
{

}

qzcv::UnitModule* standLocationFact::createModule()
{
	return new standLocationMod;
}

qzcv::UnitModule* standLocationFact::createModule(const QString& dirPath)
{
	return new standLocationMod(dirPath);
}

QStringList standLocationFact::getModSetWidgetStyles()
{
	m_styleList.clear();

	return m_styleList;
}

qzcv::ModSetWidget* standLocationFact::createModSetWidget()
{
	return new standLocationWdg;
}

qzcv::ModSetWidget* standLocationFact::createModSetWidget(const QString &style)
{
	for(int i=0;i<m_styleList.size();i++)
	{
		if(style==m_styleList.at(i))
			return new standLocationWdg(i);
	}
	return new standLocationWdg;
}

extern "C" Q_DECL_EXPORT int getFactoryCnt()
{
	return 1;
}

extern "C" Q_DECL_EXPORT void* createModuleFactory(int index)
{
	if(0==index)
	{
		return new standLocationFact;
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

