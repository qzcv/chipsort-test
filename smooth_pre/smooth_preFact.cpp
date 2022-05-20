#include "smooth_preFact.h"
#include "smooth_preWdg.h"
#include "smooth_preMod.h"

smooth_preFact :: smooth_preFact()
	: ModuleFactory(FactoryName, 0)
{

	getModSetWidgetStyles();
}

smooth_preFact :: ~smooth_preFact()
{

}

qzcv::UnitModule* smooth_preFact::createModule()
{
	return new smooth_preMod;
}

qzcv::UnitModule* smooth_preFact::createModule(const QString& dirPath)
{
	return new smooth_preMod(dirPath);
}

QStringList smooth_preFact::getModSetWidgetStyles()
{
	m_styleList.clear();

	return m_styleList;
}

qzcv::ModSetWidget* smooth_preFact::createModSetWidget()
{
	return new smooth_preWdg;
}

qzcv::ModSetWidget* smooth_preFact::createModSetWidget(const QString &style)
{
	for(int i=0;i<m_styleList.size();i++)
	{
		if(style==m_styleList.at(i))
			return new smooth_preWdg(i);
	}
	return new smooth_preWdg;
}

extern "C" Q_DECL_EXPORT int getFactoryCnt()
{
	return 1;
}

extern "C" Q_DECL_EXPORT void* createModuleFactory(int index)
{
	if(0==index)
	{
		return new smooth_preFact;
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

