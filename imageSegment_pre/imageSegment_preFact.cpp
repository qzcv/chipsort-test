#include "imageSegment_preFact.h"
#include "imageSegment_preWdg.h"
#include "imageSegment_preMod.h"

imageSegment_preFact :: imageSegment_preFact()
	: ModuleFactory(FactoryName, 0)
{

	getModSetWidgetStyles();
}

imageSegment_preFact :: ~imageSegment_preFact()
{

}

qzcv::UnitModule* imageSegment_preFact::createModule()
{
	return new imageSegment_preMod;
}

qzcv::UnitModule* imageSegment_preFact::createModule(const QString& dirPath)
{
	return new imageSegment_preMod(dirPath);
}

QStringList imageSegment_preFact::getModSetWidgetStyles()
{
	m_styleList.clear();

	return m_styleList;
}

qzcv::ModSetWidget* imageSegment_preFact::createModSetWidget()
{
	return new imageSegment_preWdg;
}

qzcv::ModSetWidget* imageSegment_preFact::createModSetWidget(const QString &style)
{
	for(int i=0;i<m_styleList.size();i++)
	{
		if(style==m_styleList.at(i))
			return new imageSegment_preWdg(i);
	}
	return new imageSegment_preWdg;
}

extern "C" Q_DECL_EXPORT int getFactoryCnt()
{
	return 1;
}

extern "C" Q_DECL_EXPORT void* createModuleFactory(int index)
{
	if(0==index)
	{
		return new imageSegment_preFact;
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

