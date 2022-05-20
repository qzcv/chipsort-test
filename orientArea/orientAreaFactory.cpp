#include "orientAreaFactory.h"
#include "orientAreaWidget.h"
#include "orientAreaModule.h"

orientAreaFactory :: orientAreaFactory()
	: ModuleFactory(FactoryName, 0)
{

	getModSetWidgetStyles();
}

orientAreaFactory :: ~orientAreaFactory()
{

}

qzcv::UnitModule* orientAreaFactory::createModule()
{
	return new orientAreaModule;
}

qzcv::UnitModule* orientAreaFactory::createModule(const QString& dirPath)
{
	return new orientAreaModule(dirPath);
}

QStringList orientAreaFactory::getModSetWidgetStyles()
{
	m_styleList.clear();

	return m_styleList;
}

qzcv::ModSetWidget* orientAreaFactory::createModSetWidget()
{
	return new orientAreaWidget;
}

qzcv::ModSetWidget* orientAreaFactory::createModSetWidget(const QString &style)
{
	for(int i=0;i<m_styleList.size();i++)
	{
		if(style==m_styleList.at(i))
			return new orientAreaWidget(i);
	}
	return new orientAreaWidget;
}

extern "C" Q_DECL_EXPORT int getFactoryCnt()
{
	return 1;
}

extern "C" Q_DECL_EXPORT void* createModuleFactory(int index)
{
	if(0==index)
	{
		return new orientAreaFactory;
	}
	else
		return NULL;
}

extern "C" Q_DECL_EXPORT void* getTranslator(QvsLanguage language)
{
	return ModuleFactory::getTranslator("orientArea", language);
}

extern "C" Q_DECL_EXPORT int isDebug() {
#if defined(_DEBUG)||defined(QT_DEBUG)
	return 1;
#else
	return 0;
#endif
}

