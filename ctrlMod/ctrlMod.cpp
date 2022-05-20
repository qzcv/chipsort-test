#include "CtrlMod.h"
#include <QObject>
#include <ImageView.h>
#include <resultText.h>
#include <iniParam.h>
#include "qvglobal.h"

CtrlMod::CtrlMod()
{
	iniData();
}

CtrlMod::CtrlMod(const QString& dirPath)
{
	iniData();
	load(dirPath, StructLevel);
}

CtrlMod::~CtrlMod()
{
	delete m_param;
}

int CtrlMod::command(int cmdID, void* dataIn, void* dataOut)
{
	return 0;
}

int CtrlMod::afterSetProperty(MetaProperty*)
{
	return 0;
}

void CtrlMod::save(const QString &dirPath)
{
	m_param->readWriteParam(false, dirPath, StructLevel);
	resetParamChanged();
}

void CtrlMod::load(const QString &dirPath, QvsParamLevel level)
{
	m_param->readWriteParam(true, dirPath, level);
	if (level == StructLevel)
		setStationNum();
}

int CtrlMod::run(const QString &funName)
{
	return 0;
}

void CtrlMod::viewResult(ImageView *iv, const QString &funName, int)
{

}

void CtrlMod::textResult(ResultText *text, const QString &funName)
{

}

void CtrlMod::iniData()
{
	m_param = new CtrlParam;
	createEvents();
}

void CtrlMod::setStationNum()
{
	QString stName("Info");

	int sz = p_calibInfo.size();
	if (sz == m_param->stNum)
		return;
	else if (sz > m_param->stNum)
		for (auto i = sz - 1;i >= m_param->stNum;--i)
		{
			removePin(stName + QString("%1").arg(i), false);
			delete p_calibInfo[i];
			p_calibInfo.pop_back();
		}
	else if (sz < m_param->stNum)
		for (auto i = sz;i < m_param->stNum;++i)
		{
			UnitInputPin<QList<double>> *p = new UnitInputPin<QList<double>>;
			addPin(p, stName + QString("%1").arg(i));
			p_calibInfo.push_back(p);
		}
	resetWidgets();
}

void CtrlMod::createEvents()
{
	addEvent(ENT_CLEAR);
}


CtrlParam::CtrlParam()
{
	stNum = 0;
	stIdx = -1;
	stName.clear();
}

CtrlParam::~CtrlParam()
{

}

void CtrlParam::readWriteParam(bool r, const QString& dirPath, int level)
{
	SETTING_INIT(dirPath + "/CtrlParam.ini");

	if (!r || (r&&level == StructLevel))
	{
		RW_VALUE(r, stNum);
	}
	if (!r || (r&&level == ProductLevel))
	{
		if (!r)		//write
			for (auto i = 0;i < stNum;++i)
				RW_VALUE1(r, stName[i], i);
		else        //read
		{
			stName.clear();
			for (auto i = 0;i < stNum;++i)
			{
				stName << "";
				RW_VALUE1(r, stName[i], i);
			}
		}
	}
}
