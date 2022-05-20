#include "pinWarpFact.h"
#include "pinWarpWdg.h"
#include "pinWarpMod.h"

pinWarpFact :: pinWarpFact()
	: ModuleFactory(FactoryName, 0)
{

	getModSetWidgetStyles();
}

pinWarpFact :: ~pinWarpFact()
{

}

qzcv::UnitModule* pinWarpFact::createModule()
{
	return new pinWarpMod;
}

qzcv::UnitModule* pinWarpFact::createModule(const QString& dirPath)
{
	return new pinWarpMod(dirPath);
}

QStringList pinWarpFact::getModSetWidgetStyles()
{
	m_styleList.clear();

	return m_styleList;
}

qzcv::ModSetWidget* pinWarpFact::createModSetWidget()
{
	return new pinWarpWdg;
}

qzcv::ModSetWidget* pinWarpFact::createModSetWidget(const QString &style)
{
	for(int i=0;i<m_styleList.size();i++)
	{
		if(style==m_styleList.at(i))
			return new pinWarpWdg(i);
	}
	return new pinWarpWdg;
}

extern "C" Q_DECL_EXPORT int getFactoryCnt()
{
	return 1;
}

extern "C" Q_DECL_EXPORT void* createModuleFactory(int index)
{
	if(0==index)
	{
		return new pinWarpFact;
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

