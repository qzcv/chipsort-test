#include "pinLength1Fact.h"
#include "pinLength1Wdg.h"
#include "pinLength1Mod.h"

pinLength1Fact :: pinLength1Fact()
	: ModuleFactory(FactoryName, 0)
{

	getModSetWidgetStyles();
}

pinLength1Fact :: ~pinLength1Fact()
{

}

qzcv::UnitModule* pinLength1Fact::createModule()
{
	return new pinLength1Mod;
}

qzcv::UnitModule* pinLength1Fact::createModule(const QString& dirPath)
{
	return new pinLength1Mod(dirPath);
}

QStringList pinLength1Fact::getModSetWidgetStyles()
{
	m_styleList.clear();

	return m_styleList;
}

qzcv::ModSetWidget* pinLength1Fact::createModSetWidget()
{
	return new pinLength1Wdg;
}

qzcv::ModSetWidget* pinLength1Fact::createModSetWidget(const QString &style)
{
	for(int i=0;i<m_styleList.size();i++)
	{
		if(style==m_styleList.at(i))
			return new pinLength1Wdg(i);
	}
	return new pinLength1Wdg;
}

extern "C" Q_DECL_EXPORT int getFactoryCnt()
{
	return 1;
}

extern "C" Q_DECL_EXPORT void* createModuleFactory(int index)
{
	if(0==index)
	{
		return new pinLength1Fact;
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

