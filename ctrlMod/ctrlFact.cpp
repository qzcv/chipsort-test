#include "CtrlFact.h"
#include "CtrlWdg.h"
#include "CtrlMod.h"

CtrlFact::CtrlFact()
	: ModuleFactory(FactoryName, 0)
{
	getModSetWidgetStyles();
}

CtrlFact :: ~CtrlFact()
{

}

qzcv::UnitModule* CtrlFact::createModule()
{
	return new CtrlMod;
}

qzcv::UnitModule* CtrlFact::createModule(const QString& dirPath)
{
	return new CtrlMod(dirPath);
}

QStringList CtrlFact::getModSetWidgetStyles()
{
	m_styleList.clear();
	m_styleList << STY_NORMAL << STY_SIMPLE;
	return m_styleList;
}

qzcv::ModSetWidget* CtrlFact::createModSetWidget()
{
	return new CtrlWdg;
}

qzcv::ModSetWidget* CtrlFact::createModSetWidget(const QString &style)
{
	for (int i = 0;i < m_styleList.size();i++)
	{
		if (style == m_styleList.at(i))
			return new CtrlWdg(i);
	}
	return new CtrlWdg;
}

extern "C" Q_DECL_EXPORT int getFactoryCnt()
{
	return 1;
}

extern "C" Q_DECL_EXPORT void* createModuleFactory(int index)
{
	if (0 == index)
	{
		return new CtrlFact;
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