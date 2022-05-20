#include "dimensionMeasureFact.h"
#include "dimensionMeasureWdg.h"
#include "dimensionMeasureMod.h"

dimensionMeasureFact :: dimensionMeasureFact()
	: ModuleFactory(FactoryName, 0)
{

	getModSetWidgetStyles();
}

dimensionMeasureFact :: ~dimensionMeasureFact()
{

}

qzcv::UnitModule* dimensionMeasureFact::createModule()
{
	return new dimensionMeasureMod;
}

qzcv::UnitModule* dimensionMeasureFact::createModule(const QString& dirPath)
{
	return new dimensionMeasureMod(dirPath);
}

QStringList dimensionMeasureFact::getModSetWidgetStyles()
{
	m_styleList.clear();

	return m_styleList;
}

qzcv::ModSetWidget* dimensionMeasureFact::createModSetWidget()
{
	return new dimensionMeasureWdg;
}

qzcv::ModSetWidget* dimensionMeasureFact::createModSetWidget(const QString &style)
{
	for(int i=0;i<m_styleList.size();i++)
	{
		if(style==m_styleList.at(i))
			return new dimensionMeasureWdg(i);
	}
	return new dimensionMeasureWdg;
}

extern "C" Q_DECL_EXPORT int getFactoryCnt()
{
	return 1;
}

extern "C" Q_DECL_EXPORT void* createModuleFactory(int index)
{
	if(0==index)
	{
		return new dimensionMeasureFact;
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

