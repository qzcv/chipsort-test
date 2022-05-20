#include "edgeLocationFact.h"
#include "edgeLocationWdg.h"
#include "edgeLocationMod.h"

edgeLocationFact :: edgeLocationFact()
	: ModuleFactory(FactoryName, 0)
{

	getModSetWidgetStyles();
}

edgeLocationFact :: ~edgeLocationFact()
{

}

qzcv::UnitModule* edgeLocationFact::createModule()
{
	return new edgeLocationMod;
}

qzcv::UnitModule* edgeLocationFact::createModule(const QString& dirPath)
{
	return new edgeLocationMod(dirPath);
}

QStringList edgeLocationFact::getModSetWidgetStyles()
{
	m_styleList.clear();

	return m_styleList;
}

qzcv::ModSetWidget* edgeLocationFact::createModSetWidget()
{
	return new edgeLocationWdg;
}

qzcv::ModSetWidget* edgeLocationFact::createModSetWidget(const QString &style)
{
	for(int i=0;i<m_styleList.size();i++)
	{
		if(style==m_styleList.at(i))
			return new edgeLocationWdg(i);
	}
	return new edgeLocationWdg;
}

extern "C" Q_DECL_EXPORT int getFactoryCnt()
{
	return 1;
}

extern "C" Q_DECL_EXPORT void* createModuleFactory(int index)
{
	if(0==index)
	{
		return new edgeLocationFact;
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

