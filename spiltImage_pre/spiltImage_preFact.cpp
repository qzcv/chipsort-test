#include "spiltImage_preFact.h"
#include "spiltImage_preWdg.h"
#include "spiltImage_preMod.h"

spiltImage_preFact :: spiltImage_preFact()
	: ModuleFactory(FactoryName, 0)
{

	getModSetWidgetStyles();
}

spiltImage_preFact :: ~spiltImage_preFact()
{

}

qzcv::UnitModule* spiltImage_preFact::createModule()
{
	return new spiltImage_preMod;
}

qzcv::UnitModule* spiltImage_preFact::createModule(const QString& dirPath)
{
	return new spiltImage_preMod(dirPath);
}

QStringList spiltImage_preFact::getModSetWidgetStyles()
{
	m_styleList.clear();

	return m_styleList;
}

qzcv::ModSetWidget* spiltImage_preFact::createModSetWidget()
{
	return new spiltImage_preWdg;
}

qzcv::ModSetWidget* spiltImage_preFact::createModSetWidget(const QString &style)
{
	for(int i=0;i<m_styleList.size();i++)
	{
		if(style==m_styleList.at(i))
			return new spiltImage_preWdg(i);
	}
	return new spiltImage_preWdg;
}

extern "C" Q_DECL_EXPORT int getFactoryCnt()
{
	return 1;
}

extern "C" Q_DECL_EXPORT void* createModuleFactory(int index)
{
	if(0==index)
	{
		return new spiltImage_preFact;
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

