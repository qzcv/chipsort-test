#include "calib3D5S_homFact.h"
#include "calib3D5S_homWdg.h"
#include "calib3D5S_homMod.h"

calib3D5S_homFact :: calib3D5S_homFact()
	: ModuleFactory(FactoryName, 0)
{

	getModSetWidgetStyles();
}

calib3D5S_homFact :: ~calib3D5S_homFact()
{

}

qzcv::UnitModule* calib3D5S_homFact::createModule()
{
	return new calib3D5S_homMod;
}

qzcv::UnitModule* calib3D5S_homFact::createModule(const QString& dirPath)
{
	return new calib3D5S_homMod(dirPath);
}

QStringList calib3D5S_homFact::getModSetWidgetStyles()
{
	m_styleList.clear();

	return m_styleList;
}

qzcv::ModSetWidget* calib3D5S_homFact::createModSetWidget()
{
	return new calib3D5S_homWdg;
}

qzcv::ModSetWidget* calib3D5S_homFact::createModSetWidget(const QString &style)
{
	for(int i=0;i<m_styleList.size();i++)
	{
		if(style==m_styleList.at(i))
			return new calib3D5S_homWdg(i);
	}
	return new calib3D5S_homWdg;
}

extern "C" Q_DECL_EXPORT int getFactoryCnt()
{
	return 1;
}

extern "C" Q_DECL_EXPORT void* createModuleFactory(int index)
{
	if(0==index)
	{
		return new calib3D5S_homFact;
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

