#pragma once
#include <moduleClass.h>
#include "HalconCpp.h"
#include "inclu.h"

using namespace qzcv;
using namespace Halcon;

class EXPORTS ChipMod : public UnitModule
{
public:
	explicit ChipMod();
	~ChipMod();

protected:
	void toHtuple(const UnitInputPin<QList<double>> &p, HTuple &htuple);

	QList<QVariant> toQList(const HTuple &htuple);

	void setDetectOutData(const QString &key, const HTuple& val);
	void setDetectOutData(const QString &key, const HTuple& val, int index);
	void setDetectOutData(const QString &key, const HTuple& val, int index0, int index1);

	bool getDispOutData(const QString &key, HTuple& val);
	bool getDispOutData(const QString &key, int index, HTuple& val);
	bool getDispOutData(const QString &key, int index1, int index2, HTuple& val);

	void setDetectOutObject(const QString &key, const Hobject& object);
	void setDetectOutObject(const QString &key, const Hobject& object, int index);
	void setDetectOutObject(const QString &key, const Hobject& object, int index0, int index1);

	const Hobject* getDispObject(const QString &key);
	const Hobject* getDispObject(const QString &key, int index);
	const Hobject* getDispObject(const QString &key, int index0, int index1);

	bool checkCrossGray(const Hobject* image, double Angle, double edgeRow, double edgeCol, int crossGray, int step, const char* polar);

private:
	QMap<QString, HTuple> m_mapHtup;
	QMap<QString, Hobject> m_mapHobj;


};
