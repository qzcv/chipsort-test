#pragma once
#include <moduleClass.h>
const char* const FactoryName = "nccMatch_outN";
using namespace qzcv;

class nccMatch_outNFact : public ModuleFactory
{
public:
	explicit nccMatch_outNFact();
	virtual ~nccMatch_outNFact();
	virtual unsigned int classID()const { return 418524927; }
	virtual const QIcon* icon()const { return NULL; }
	virtual UnitModule* createModule();
	virtual UnitModule* createModule(const QString& dirPath);
	virtual QStringList getModSetWidgetStyles();
	virtual ModSetWidget* createModSetWidget();
	virtual ModSetWidget* createModSetWidget(const QString& style);
private:
	QStringList m_styleList;
};

