#include "arithmetic_preFactory.h"
#include "arithmetic_preWidget.h"
#include "arithmetic_preModule.h"

arithmetic_preFactory :: arithmetic_preFactory()
	: ModuleFactory(FactoryName, 0)
{

	getModSetWidgetStyles();
}

arithmetic_preFactory :: ~arithmetic_preFactory()
{

}

qzcv::UnitModule* arithmetic_preFactory::createModule()
{
	return new arithmetic_preModule;
}

qzcv::UnitModule* arithmetic_preFactory::createModule(const QString& dirPath)
{
	return new arithmetic_preModule(dirPath);
}

QStringList arithmetic_preFactory::getModSetWidgetStyles()
{
	m_styleList.clear();

	return m_styleList;
}

qzcv::ModSetWidget* arithmetic_preFactory::createModSetWidget()
{
	return new arithmetic_preWidget;
}

qzcv::ModSetWidget* arithmetic_preFactory::createModSetWidget(const QString &style)
{
	for(int i=0;i<m_styleList.size();i++)
	{
		if(style==m_styleList.at(i))
			return new arithmetic_preWidget(i);
	}
	return new arithmetic_preWidget;
}

extern "C" Q_DECL_EXPORT int getFactoryCnt()
{
	return 1;
}

extern "C" Q_DECL_EXPORT void* createModuleFactory(int index)
{
	if(0==index)
	{
		return new arithmetic_preFactory;
	}
	else
		return NULL;
}

extern "C" Q_DECL_EXPORT void* getTranslator(QvsLanguage language)
{
	return ModuleFactory::getTranslator("arithmetic_pre", language);
}

extern "C" Q_DECL_EXPORT int isDebug() {
#if defined(_DEBUG)||defined(QT_DEBUG)
	return 1;
#else
	return 0;
#endif
}

