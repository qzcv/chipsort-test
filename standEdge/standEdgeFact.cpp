#include "standEdgeFact.h"
#include "standEdgeWdg.h"
#include "standEdgeMod.h"

standEdgeFact :: standEdgeFact()
	: ModuleFactory(FactoryName, 0)
{

	getModSetWidgetStyles();
}

standEdgeFact :: ~standEdgeFact()
{

}

qzcv::UnitModule* standEdgeFact::createModule()
{
	return new standEdgeMod;
}

qzcv::UnitModule* standEdgeFact::createModule(const QString& dirPath)
{
	return new standEdgeMod(dirPath);
}

QStringList standEdgeFact::getModSetWidgetStyles()
{
	m_styleList.clear();

	return m_styleList;
}

qzcv::ModSetWidget* standEdgeFact::createModSetWidget()
{
	return new standEdgeWdg;
}

qzcv::ModSetWidget* standEdgeFact::createModSetWidget(const QString &style)
{
	for(int i=0;i<m_styleList.size();i++)
	{
		if(style==m_styleList.at(i))
			return new standEdgeWdg(i);
	}
	return new standEdgeWdg;
}

extern "C" Q_DECL_EXPORT int getFactoryCnt()
{
	return 1;
}

extern "C" Q_DECL_EXPORT void* createModuleFactory(int index)
{
	if(0==index)
	{
		return new standEdgeFact;
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

