#include "sel2Mod.h"
#include "sel2Param.h"
#include <QObject>
#include <ImageView.h>
#include <resultText.h>

sel2Mod::sel2Mod()
{
	iniData();
}

sel2Mod::sel2Mod(const QString& dirPath)
{
	iniData();
	load(dirPath, StructLevel);
}

sel2Mod::~sel2Mod()
{
	delete m_param;
}

int sel2Mod::command(int cmdID, void* dataIn, void* dataOut)
{
	return 0;
}

int sel2Mod::afterSetProperty(MetaProperty*)
{
	return 0;
}

void sel2Mod::save(const QString &dirPath)
{
	m_param->readWriteParam(false, dirPath, StructLevel);
}

void sel2Mod::load(const QString &dirPath, QvsParamLevel level)
{
	m_param->readWriteParam(true,dirPath,level);

	*p_int = m_param->outIdx;
}

int sel2Mod::run(const QString &funName)
{
	return 0;
}

void sel2Mod::viewResult(ImageView *iv, const QString &funName, int)
{

}

void sel2Mod::textResult(ResultText *text, const QString &funName)
{

}

void sel2Mod::iniData()
{
	m_param = new sel2Param;
	createPins();
}

void sel2Mod::createPins()
{
	addPin(&p_int, "o");
}

