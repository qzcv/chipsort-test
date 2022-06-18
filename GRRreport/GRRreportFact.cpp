#include "GRRreportFact.h"
#include "GRRreportWdg.h"
#include "GRRreportMod.h"

GRRreportFact :: GRRreportFact()
	: ModuleFactory(FactoryName, 0)
{

	getModSetWidgetStyles();
}

GRRreportFact :: ~GRRreportFact()
{

}

qzcv::UnitModule* GRRreportFact::createModule()
{
	return new GRRreportMod;
}

qzcv::UnitModule* GRRreportFact::createModule(const QString& dirPath)
{
	return new GRRreportMod(dirPath);
}

QStringList GRRreportFact::getModSetWidgetStyles()
{
	//m_styleList.clear();
	m_styleList << "normal" << "advanced";
	return m_styleList;
}

qzcv::ModSetWidget* GRRreportFact::createModSetWidget()
{
	return new GRRreportWdg;
}

qzcv::ModSetWidget* GRRreportFact::createModSetWidget(const QString &style)
{
	for(int i=0;i<m_styleList.size();i++)
	{
		if(style==m_styleList.at(i))
			return new GRRreportWdg(i);
	}
	return new GRRreportWdg;
}

extern "C" Q_DECL_EXPORT int getFactoryCnt()
{
	return 1;
}

extern "C" Q_DECL_EXPORT void* createModuleFactory(int index)
{
	if(0==index)
	{
		return new GRRreportFact;
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

