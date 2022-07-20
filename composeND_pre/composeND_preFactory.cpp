#include "composeND_preFactory.h"
#include "composeND_preWidget.h"
#include "composeND_preModule.h"

composeND_preFactory :: composeND_preFactory()
	: ModuleFactory(FactoryName, 0)
{

	getModSetWidgetStyles();
}

composeND_preFactory :: ~composeND_preFactory()
{

}

qzcv::UnitModule* composeND_preFactory::createModule()
{
	return new composeND_preModule;
}

qzcv::UnitModule* composeND_preFactory::createModule(const QString& dirPath)
{
	return new composeND_preModule(dirPath);
}

QStringList composeND_preFactory::getModSetWidgetStyles()
{
	m_styleList.clear();
	m_styleList << "normal" << "simple";
	return m_styleList;
}

qzcv::ModSetWidget* composeND_preFactory::createModSetWidget()
{
	return new composeND_preWidget;
}

qzcv::ModSetWidget* composeND_preFactory::createModSetWidget(const QString &style)
{
	for(int i=0;i<m_styleList.size();i++)
	{
		if(style==m_styleList.at(i))
			return new composeND_preWidget(i);
	}
	return new composeND_preWidget;
}

extern "C" Q_DECL_EXPORT int getFactoryCnt()
{
	return 1;
}

extern "C" Q_DECL_EXPORT void* createModuleFactory(int index)
{
	if(0==index)
	{
		return new composeND_preFactory;
	}
	else
		return NULL;
}

extern "C" Q_DECL_EXPORT void* getTranslator(QvsLanguage language)
{
	return ModuleFactory::getTranslator("composeND_pre", language);
}

extern "C" Q_DECL_EXPORT int isDebug() {
#if defined(_DEBUG)||defined(QT_DEBUG)
	return 1;
#else
	return 0;
#endif
}

