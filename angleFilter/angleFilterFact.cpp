#include "angleFilterFact.h"
#include "angleFilterWdg.h"
#include "angleFilterMod.h"

angleFilterFact :: angleFilterFact()
	: ModuleFactory(FactoryName, 0)
{

	getModSetWidgetStyles();
}

angleFilterFact :: ~angleFilterFact()
{

}

qzcv::UnitModule* angleFilterFact::createModule()
{
	return new angleFilterMod;
}

qzcv::UnitModule* angleFilterFact::createModule(const QString& dirPath)
{
	return new angleFilterMod(dirPath);
}

QStringList angleFilterFact::getModSetWidgetStyles()
{
	m_styleList.clear();

	return m_styleList;
}

qzcv::ModSetWidget* angleFilterFact::createModSetWidget()
{
	return new angleFilterWdg;
}

qzcv::ModSetWidget* angleFilterFact::createModSetWidget(const QString &style)
{
	for(int i=0;i<m_styleList.size();i++)
	{
		if(style==m_styleList.at(i))
			return new angleFilterWdg(i);
	}
	return new angleFilterWdg;
}

extern "C" Q_DECL_EXPORT int getFactoryCnt()
{
	return 1;
}

extern "C" Q_DECL_EXPORT void* createModuleFactory(int index)
{
	if(0==index)
	{
		return new angleFilterFact;
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

