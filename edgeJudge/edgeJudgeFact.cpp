#include "edgeJudgeFact.h"
#include "edgeJudgeWdg.h"
#include "edgeJudgeMod.h"

edgeJudgeFact :: edgeJudgeFact()
	: ModuleFactory(FactoryName, 0)
{

	getModSetWidgetStyles();
}

edgeJudgeFact :: ~edgeJudgeFact()
{

}

qzcv::UnitModule* edgeJudgeFact::createModule()
{
	return new edgeJudgeMod;
}

qzcv::UnitModule* edgeJudgeFact::createModule(const QString& dirPath)
{
	return new edgeJudgeMod(dirPath);
}

QStringList edgeJudgeFact::getModSetWidgetStyles()
{
	m_styleList.clear();

	return m_styleList;
}

qzcv::ModSetWidget* edgeJudgeFact::createModSetWidget()
{
	return new edgeJudgeWdg;
}

qzcv::ModSetWidget* edgeJudgeFact::createModSetWidget(const QString &style)
{
	for(int i=0;i<m_styleList.size();i++)
	{
		if(style==m_styleList.at(i))
			return new edgeJudgeWdg(i);
	}
	return new edgeJudgeWdg;
}

extern "C" Q_DECL_EXPORT int getFactoryCnt()
{
	return 1;
}

extern "C" Q_DECL_EXPORT void* createModuleFactory(int index)
{
	if(0==index)
	{
		return new edgeJudgeFact;
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

