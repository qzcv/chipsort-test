#include "edgeLineFact.h"
#include "edgeLineWdg.h"
#include "edgeLineMod.h"

edgeLineFact :: edgeLineFact()
	: ModuleFactory(FactoryName, 0)
{

	getModSetWidgetStyles();
}

edgeLineFact :: ~edgeLineFact()
{

}

qzcv::UnitModule* edgeLineFact::createModule()
{
	return new edgeLineMod;
}

qzcv::UnitModule* edgeLineFact::createModule(const QString& dirPath)
{
	return new edgeLineMod(dirPath);
}

QStringList edgeLineFact::getModSetWidgetStyles()
{
	m_styleList.clear();

	return m_styleList;
}

qzcv::ModSetWidget* edgeLineFact::createModSetWidget()
{
	return new edgeLineWdg;
}

qzcv::ModSetWidget* edgeLineFact::createModSetWidget(const QString &style)
{
	for(int i=0;i<m_styleList.size();i++)
	{
		if(style==m_styleList.at(i))
			return new edgeLineWdg(i);
	}
	return new edgeLineWdg;
}

extern "C" Q_DECL_EXPORT int getFactoryCnt()
{
	return 1;
}

extern "C" Q_DECL_EXPORT void* createModuleFactory(int index)
{
	if(0==index)
	{
		return new edgeLineFact;
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

