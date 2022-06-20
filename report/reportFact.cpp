#include "reportFact.h"
#include "reportWdg.h"
#include "reportMod.h"

const QString FILE_NAME = "report";

GRRreportFact :: GRRreportFact()
	: ModuleFactory(FILE_NAME, 0)
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

calibReportFact::calibReportFact()
	: ModuleFactory(FILE_NAME, 1)
{
	getModSetWidgetStyles();
}

calibReportFact :: ~calibReportFact()
{

}

qzcv::UnitModule* calibReportFact::createModule()
{
	return new calibReportMod;
}

qzcv::UnitModule* calibReportFact::createModule(const QString& dirPath)
{
	return new calibReportMod(dirPath);
}

QStringList calibReportFact::getModSetWidgetStyles()
{
	m_styleList << "simple" << "normal";
	return m_styleList;
}

qzcv::ModSetWidget* calibReportFact::createModSetWidget()
{
	return new calibReportWdg;
}

qzcv::ModSetWidget* calibReportFact::createModSetWidget(const QString &style)
{
	for (int i = 0; i < m_styleList.size(); i++)
	{
		if (style == m_styleList.at(i))
			return new calibReportWdg(i);
	}
	return new calibReportWdg;
}

extern "C" Q_DECL_EXPORT int getFactoryCnt()
{
	return 2;
}

extern "C" Q_DECL_EXPORT void* createModuleFactory(int index)
{
	if (0 == index)
		return new GRRreportFact;
	else if (1 == index)
		return new calibReportFact;
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

