#include "calib3D5SFact.h"
#include "calib3D5SWdg.h"
#include "calib3D5SMod.h"

calib3D5SFact :: calib3D5SFact()
	: ModuleFactory(FactoryName, 0)
{

	getModSetWidgetStyles();
}

calib3D5SFact :: ~calib3D5SFact()
{

}

qzcv::UnitModule* calib3D5SFact::createModule()
{
	return new calib3D5SMod;
}

qzcv::UnitModule* calib3D5SFact::createModule(const QString& dirPath)
{
	return new calib3D5SMod(dirPath);
}

QStringList calib3D5SFact::getModSetWidgetStyles()
{
	m_styleList.clear();

	return m_styleList;
}

qzcv::ModSetWidget* calib3D5SFact::createModSetWidget()
{
	return new calib3D5SWdg;
}

qzcv::ModSetWidget* calib3D5SFact::createModSetWidget(const QString &style)
{
	for(int i=0;i<m_styleList.size();i++)
	{
		if(style==m_styleList.at(i))
			return new calib3D5SWdg(i);
	}
	return new calib3D5SWdg;
}

extern "C" Q_DECL_EXPORT int getFactoryCnt()
{
	return 1;
}

extern "C" Q_DECL_EXPORT void* createModuleFactory(int index)
{
	if(0==index)
	{
		return new calib3D5SFact;
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

