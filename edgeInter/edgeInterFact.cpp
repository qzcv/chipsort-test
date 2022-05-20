#include "edgeInterFact.h"
#include "edgeInterWdg.h"
#include "edgeInterMod.h"

edgeInterFact :: edgeInterFact()
	: ModuleFactory(FactoryName, 0)
{

	getModSetWidgetStyles();
}

edgeInterFact :: ~edgeInterFact()
{

}

qzcv::UnitModule* edgeInterFact::createModule()
{
	return new edgeInterMod;
}

qzcv::UnitModule* edgeInterFact::createModule(const QString& dirPath)
{
	return new edgeInterMod(dirPath);
}

QStringList edgeInterFact::getModSetWidgetStyles()
{
	m_styleList.clear();

	return m_styleList;
}

qzcv::ModSetWidget* edgeInterFact::createModSetWidget()
{
	return new edgeInterWdg;
}

qzcv::ModSetWidget* edgeInterFact::createModSetWidget(const QString &style)
{
	for(int i=0;i<m_styleList.size();i++)
	{
		if(style==m_styleList.at(i))
			return new edgeInterWdg(i);
	}
	return new edgeInterWdg;
}

extern "C" Q_DECL_EXPORT int getFactoryCnt()
{
	return 1;
}

extern "C" Q_DECL_EXPORT void* createModuleFactory(int index)
{
	if(0==index)
	{
		return new edgeInterFact;
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

