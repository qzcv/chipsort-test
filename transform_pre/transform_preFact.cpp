#include "transform_preFact.h"
#include "transform_preWdg.h"
#include "transform_preMod.h"

transform_preFact :: transform_preFact()
	: ModuleFactory(FactoryName, 0)
{

	getModSetWidgetStyles();
}

transform_preFact :: ~transform_preFact()
{

}

qzcv::UnitModule* transform_preFact::createModule()
{
	return new transform_preMod;
}

qzcv::UnitModule* transform_preFact::createModule(const QString& dirPath)
{
	return new transform_preMod(dirPath);
}

QStringList transform_preFact::getModSetWidgetStyles()
{
	m_styleList.clear();

	return m_styleList;
}

qzcv::ModSetWidget* transform_preFact::createModSetWidget()
{
	return new transform_preWdg;
}

qzcv::ModSetWidget* transform_preFact::createModSetWidget(const QString &style)
{
	for(int i=0;i<m_styleList.size();i++)
	{
		if(style==m_styleList.at(i))
			return new transform_preWdg(i);
	}
	return new transform_preWdg;
}

extern "C" Q_DECL_EXPORT int getFactoryCnt()
{
	return 1;
}

extern "C" Q_DECL_EXPORT void* createModuleFactory(int index)
{
	if(0==index)
	{
		return new transform_preFact;
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

