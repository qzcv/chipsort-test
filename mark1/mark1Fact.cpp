#include "mark1Fact.h"
#include "mark1Wdg.h"
#include "mark1Mod.h"

mark1Fact :: mark1Fact()
	: ModuleFactory(FactoryName, 0)
{

	getModSetWidgetStyles();
}

mark1Fact :: ~mark1Fact()
{

}

qzcv::UnitModule* mark1Fact::createModule()
{
	return new mark1Mod;
}

qzcv::UnitModule* mark1Fact::createModule(const QString& dirPath)
{
	return new mark1Mod(dirPath);
}

QStringList mark1Fact::getModSetWidgetStyles()
{
	m_styleList.clear();

	return m_styleList;
}

qzcv::ModSetWidget* mark1Fact::createModSetWidget()
{
	return new mark1Wdg;
}

qzcv::ModSetWidget* mark1Fact::createModSetWidget(const QString &style)
{
	for(int i=0;i<m_styleList.size();i++)
	{
		if(style==m_styleList.at(i))
			return new mark1Wdg(i);
	}
	return new mark1Wdg;
}

extern "C" Q_DECL_EXPORT int getFactoryCnt()
{
	return 1;
}

extern "C" Q_DECL_EXPORT void* createModuleFactory(int index)
{
	if(0==index)
	{
		return new mark1Fact;
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

