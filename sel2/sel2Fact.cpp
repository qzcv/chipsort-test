#include "sel2Fact.h"
#include "sel2Wdg.h"
#include "sel2Mod.h"

sel2Fact :: sel2Fact()
	: ModuleFactory(FactoryName, 0)
{

	getModSetWidgetStyles();
}

sel2Fact :: ~sel2Fact()
{

}

qzcv::UnitModule* sel2Fact::createModule()
{
	return new sel2Mod;
}

qzcv::UnitModule* sel2Fact::createModule(const QString& dirPath)
{
	return new sel2Mod(dirPath);
}

QStringList sel2Fact::getModSetWidgetStyles()
{
	m_styleList.clear();

	return m_styleList;
}

qzcv::ModSetWidget* sel2Fact::createModSetWidget()
{
	return new sel2Wdg;
}

qzcv::ModSetWidget* sel2Fact::createModSetWidget(const QString &style)
{
	for(int i=0;i<m_styleList.size();i++)
	{
		if(style==m_styleList.at(i))
			return new sel2Wdg(i);
	}
	return new sel2Wdg;
}

extern "C" Q_DECL_EXPORT int getFactoryCnt()
{
	return 1;
}

extern "C" Q_DECL_EXPORT void* createModuleFactory(int index)
{
	if(0==index)
	{
		return new sel2Fact;
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

