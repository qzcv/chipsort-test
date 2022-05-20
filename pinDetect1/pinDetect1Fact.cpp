#include "pinDetect1Fact.h"
#include "pinDetect1Wdg.h"
#include "pinDetect1Mod.h"

pinDetect1Fact :: pinDetect1Fact()
	: ModuleFactory(FactoryName, 0)
{

	getModSetWidgetStyles();
}

pinDetect1Fact :: ~pinDetect1Fact()
{

}

qzcv::UnitModule* pinDetect1Fact::createModule()
{
	return new pinDetect1Mod;
}

qzcv::UnitModule* pinDetect1Fact::createModule(const QString& dirPath)
{
	return new pinDetect1Mod(dirPath);
}

QStringList pinDetect1Fact::getModSetWidgetStyles()
{
	m_styleList.clear();

	return m_styleList;
}

qzcv::ModSetWidget* pinDetect1Fact::createModSetWidget()
{
	return new pinDetect1Wdg;
}

qzcv::ModSetWidget* pinDetect1Fact::createModSetWidget(const QString &style)
{
	for(int i=0;i<m_styleList.size();i++)
	{
		if(style==m_styleList.at(i))
			return new pinDetect1Wdg(i);
	}
	return new pinDetect1Wdg;
}

extern "C" Q_DECL_EXPORT int getFactoryCnt()
{
	return 1;
}

extern "C" Q_DECL_EXPORT void* createModuleFactory(int index)
{
	if(0==index)
	{
		return new pinDetect1Fact;
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

