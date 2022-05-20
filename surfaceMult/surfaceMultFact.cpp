#include "surfaceMultFact.h"
#include "surfaceMultWdg.h"
#include "surfaceMultMod.h"

surfaceMultFact :: surfaceMultFact()
	: ModuleFactory(FactoryName, 0)
{

	getModSetWidgetStyles();
}

surfaceMultFact :: ~surfaceMultFact()
{

}

qzcv::UnitModule* surfaceMultFact::createModule()
{
	return new surfaceMultMod;
}

qzcv::UnitModule* surfaceMultFact::createModule(const QString& dirPath)
{
	return new surfaceMultMod(dirPath);
}

QStringList surfaceMultFact::getModSetWidgetStyles()
{
	m_styleList.clear();

	return m_styleList;
}

qzcv::ModSetWidget* surfaceMultFact::createModSetWidget()
{
	return new surfaceMultWdg;
}

qzcv::ModSetWidget* surfaceMultFact::createModSetWidget(const QString &style)
{
	for(int i=0;i<m_styleList.size();i++)
	{
		if(style==m_styleList.at(i))
			return new surfaceMultWdg(i);
	}
	return new surfaceMultWdg;
}

extern "C" Q_DECL_EXPORT int getFactoryCnt()
{
	return 1;
}

extern "C" Q_DECL_EXPORT void* createModuleFactory(int index)
{
	if(0==index)
	{
		return new surfaceMultFact;
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

