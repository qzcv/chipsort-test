#include "edgeWarpFact.h"
#include "edgeWarpWdg.h"
#include "edgeWarpMod.h"

edgeWarpFact :: edgeWarpFact()
	: ModuleFactory(FactoryName, 0)
{

	getModSetWidgetStyles();
}

edgeWarpFact :: ~edgeWarpFact()
{

}

qzcv::UnitModule* edgeWarpFact::createModule()
{
	return new edgeWarpMod;
}

qzcv::UnitModule* edgeWarpFact::createModule(const QString& dirPath)
{
	return new edgeWarpMod(dirPath);
}

QStringList edgeWarpFact::getModSetWidgetStyles()
{
	m_styleList.clear();

	return m_styleList;
}

qzcv::ModSetWidget* edgeWarpFact::createModSetWidget()
{
	return new edgeWarpWdg;
}

qzcv::ModSetWidget* edgeWarpFact::createModSetWidget(const QString &style)
{
	for(int i=0;i<m_styleList.size();i++)
	{
		if(style==m_styleList.at(i))
			return new edgeWarpWdg(i);
	}
	return new edgeWarpWdg;
}

extern "C" Q_DECL_EXPORT int getFactoryCnt()
{
	return 1;
}

extern "C" Q_DECL_EXPORT void* createModuleFactory(int index)
{
	if(0==index)
	{
		return new edgeWarpFact;
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

