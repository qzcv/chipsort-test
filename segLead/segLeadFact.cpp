#include "segLeadFact.h"
#include "segLeadWdg.h"
#include "segLeadMod.h"

segLeadFact :: segLeadFact()
	: ModuleFactory(FactoryName, 0)
{

	getModSetWidgetStyles();
}

segLeadFact :: ~segLeadFact()
{

}

qzcv::UnitModule* segLeadFact::createModule()
{
	return new segLeadMod;
}

qzcv::UnitModule* segLeadFact::createModule(const QString& dirPath)
{
	return new segLeadMod(dirPath);
}

QStringList segLeadFact::getModSetWidgetStyles()
{
	m_styleList.clear();

	return m_styleList;
}

qzcv::ModSetWidget* segLeadFact::createModSetWidget()
{
	return new segLeadWdg;
}

qzcv::ModSetWidget* segLeadFact::createModSetWidget(const QString &style)
{
	for(int i=0;i<m_styleList.size();i++)
	{
		if(style==m_styleList.at(i))
			return new segLeadWdg(i);
	}
	return new segLeadWdg;
}

extern "C" Q_DECL_EXPORT int getFactoryCnt()
{
	return 1;
}

extern "C" Q_DECL_EXPORT void* createModuleFactory(int index)
{
	if(0==index)
	{
		return new segLeadFact;
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

