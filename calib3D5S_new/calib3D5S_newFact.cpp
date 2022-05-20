#include "calib3D5S_newFact.h"
#include "calib3D5S_newWdg.h"
#include "calib3D5S_newMod.h"

calib3D5S_newFact :: calib3D5S_newFact()
	: ModuleFactory(FactoryName, 0)
{

	getModSetWidgetStyles();
}

calib3D5S_newFact :: ~calib3D5S_newFact()
{

}

qzcv::UnitModule* calib3D5S_newFact::createModule()
{
	return new calib3D5S_newMod;
}

qzcv::UnitModule* calib3D5S_newFact::createModule(const QString& dirPath)
{
	return new calib3D5S_newMod(dirPath);
}

QStringList calib3D5S_newFact::getModSetWidgetStyles()
{
	m_styleList.clear();

	return m_styleList;
}

qzcv::ModSetWidget* calib3D5S_newFact::createModSetWidget()
{
	return new calib3D5S_newWdg;
}

qzcv::ModSetWidget* calib3D5S_newFact::createModSetWidget(const QString &style)
{
	for(int i=0;i<m_styleList.size();i++)
	{
		if(style==m_styleList.at(i))
			return new calib3D5S_newWdg(i);
	}
	return new calib3D5S_newWdg;
}

extern "C" Q_DECL_EXPORT int getFactoryCnt()
{
	return 1;
}

extern "C" Q_DECL_EXPORT void* createModuleFactory(int index)
{
	if(0==index)
	{
		return new calib3D5S_newFact;
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

