#include "nccMatch_fourDirFact.h"
#include "nccMatch_fourDirWdg.h"
#include "nccMatch_fourDirMod.h"

nccMatch_fourDirFact :: nccMatch_fourDirFact()
	: ModuleFactory(FactoryName, 0)
{

	getModSetWidgetStyles();
}

nccMatch_fourDirFact :: ~nccMatch_fourDirFact()
{

}

qzcv::UnitModule* nccMatch_fourDirFact::createModule()
{
	return new nccMatch_fourDirMod;
}

qzcv::UnitModule* nccMatch_fourDirFact::createModule(const QString& dirPath)
{
	return new nccMatch_fourDirMod(dirPath);
}

QStringList nccMatch_fourDirFact::getModSetWidgetStyles()
{
	m_styleList.clear();

	return m_styleList;
}

qzcv::ModSetWidget* nccMatch_fourDirFact::createModSetWidget()
{
	return new nccMatch_fourDirWdg;
}

qzcv::ModSetWidget* nccMatch_fourDirFact::createModSetWidget(const QString &style)
{
	for(int i=0;i<m_styleList.size();i++)
	{
		if(style==m_styleList.at(i))
			return new nccMatch_fourDirWdg(i);
	}
	return new nccMatch_fourDirWdg;
}

extern "C" Q_DECL_EXPORT int getFactoryCnt()
{
	return 1;
}

extern "C" Q_DECL_EXPORT void* createModuleFactory(int index)
{
	if(0==index)
	{
		return new nccMatch_fourDirFact;
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

