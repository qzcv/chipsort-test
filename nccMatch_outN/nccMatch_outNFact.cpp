#include "nccMatch_outNFact.h"
#include "nccMatch_outNWdg.h"
#include "nccMatch_outNMod.h"

nccMatch_outNFact :: nccMatch_outNFact()
	: ModuleFactory(FactoryName, 0)
{

	getModSetWidgetStyles();
}

nccMatch_outNFact :: ~nccMatch_outNFact()
{

}

qzcv::UnitModule* nccMatch_outNFact::createModule()
{
	return new nccMatch_outNMod;
}

qzcv::UnitModule* nccMatch_outNFact::createModule(const QString& dirPath)
{
	return new nccMatch_outNMod(dirPath);
}

QStringList nccMatch_outNFact::getModSetWidgetStyles()
{
	m_styleList.clear();

	return m_styleList;
}

qzcv::ModSetWidget* nccMatch_outNFact::createModSetWidget()
{
	return new nccMatch_outNWdg;
}

qzcv::ModSetWidget* nccMatch_outNFact::createModSetWidget(const QString &style)
{
	for(int i=0;i<m_styleList.size();i++)
	{
		if(style==m_styleList.at(i))
			return new nccMatch_outNWdg(i);
	}
	return new nccMatch_outNWdg;
}

extern "C" Q_DECL_EXPORT int getFactoryCnt()
{
	return 1;
}

extern "C" Q_DECL_EXPORT void* createModuleFactory(int index)
{
	if(0==index)
	{
		return new nccMatch_outNFact;
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

