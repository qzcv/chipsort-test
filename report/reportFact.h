#pragma once
#include <moduleClass.h>

using namespace qzcv;

class GRRreportFact : public ModuleFactory
{
public:
	explicit GRRreportFact();
	virtual ~GRRreportFact();
	virtual unsigned int classID()const { return 268875657; }
	virtual const QIcon* icon()const { return NULL; }
	virtual UnitModule* createModule();
	virtual UnitModule* createModule(const QString& dirPath);
	virtual QStringList getModSetWidgetStyles();
	virtual ModSetWidget* createModSetWidget();
	virtual ModSetWidget* createModSetWidget(const QString& style);
private:
	QStringList m_styleList;
};

class calibReportFact : public ModuleFactory
{
public:
	explicit calibReportFact();
	virtual ~calibReportFact();
	virtual unsigned int classID()const { return 247266427; }
	virtual const QIcon* icon()const { return NULL; }
	virtual UnitModule* createModule();
	virtual UnitModule* createModule(const QString& dirPath);
	virtual QStringList getModSetWidgetStyles();
	virtual ModSetWidget* createModSetWidget();
	virtual ModSetWidget* createModSetWidget(const QString& style);
private:
	QStringList m_styleList;
};

