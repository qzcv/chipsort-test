#include "chipMod.h"

ChipMod::ChipMod()
{
	m_mapHtup.clear();
	m_mapHobj.clear();
}

ChipMod::~ChipMod()
{
}

void ChipMod::setDetectOutData(const QString & key, const HTuple & val)
{
	m_mapHtup.insert(key, val);
}

void ChipMod::setDetectOutData(const QString & key, const HTuple & val, int index)
{
	QString k = key + QString("_%1").arg(index);
	setDetectOutData(k, val);
}

void ChipMod::setDetectOutData(const QString & key, const HTuple & val, int index0, int index1)
{
	QString k = key + QString("_%1_%2").arg(index0).arg(index1);
	setDetectOutData(k, val);
}

bool ChipMod::getDispOutData(const QString &key, HTuple & val)
{
	if (m_mapHtup.find(key) == m_mapHtup.cend())
		return false;
	val = *m_mapHtup.find(key);
	return true;
}

bool ChipMod::getDispOutData(const QString &key, int index, HTuple & val)
{
	QString k = key + QString("_%1").arg(index);
	return getDispOutData(k, val);
}

bool ChipMod::getDispOutData(const QString & key, int index1, int index2, HTuple & val)
{
	QString k = key + QString("_%1_%2").arg(index1).arg(index2);
	return getDispOutData(k, val);
}

void ChipMod::setDetectOutObject(const QString & key, const Hobject & object)
{
	m_mapHobj.insert(key, object);
}

void ChipMod::setDetectOutObject(const QString & key, const Hobject & object, int index)
{
	QString k = key + QString("_%1").arg(index);
	setDetectOutObject(k, object);
}

void ChipMod::setDetectOutObject(const QString & key, const Hobject & object, int index0, int index1)
{
	QString k = key + QString("_%1_%2").arg(index0).arg(index1);
	setDetectOutObject(k, object);
}

const Hobject * ChipMod::getDispObject(const QString & key)
{
	if (m_mapHobj.find(key) == m_mapHobj.cend())
		return nullptr;
	return &(*m_mapHobj.find(key));
}

const Hobject * ChipMod::getDispObject(const QString & key, int index)
{
	QString k = key + QString("_%1").arg(index);
	return getDispObject(k);
}

const Hobject * ChipMod::getDispObject(const QString & key, int index0, int index1)
{
	QString k = key + QString("_%1_%2").arg(index0).arg(index1);
	return getDispObject(k);
}

bool ChipMod::checkCrossGray(const Hobject * image, double Angle, double edgeRow, double edgeCol, int crossGray, int step, const char * polar)
{
	if (image == NULL || image->Id() == H_EMPTY_REGION)
	{
		return false;
	}
	Hlong imgWidth, imgHeight;
	get_image_size(*image, &imgWidth, &imgHeight);

	double stepRow, stepCol;
	if (step <= 0)
	{
		return false;
	}
	bool negOK[2] = { false,false }, posOk[2] = { false,false };
	for (int i = 0;i < step;i++)
	{
		int polVal[2] = { 1,-1 };
		for (int j = 0;j < 2;j++)  //分别对应正负两个方向
		{
			stepRow = edgeRow - polVal[j] * (i + 1)*sin(Angle);
			stepCol = edgeCol + polVal[j] * (i + 1)*cos(Angle);
			int tmpRow, tmpCol;
			tmpRow = stepRow + 0.5;
			tmpCol = stepCol + 0.5;
			if (tmpRow<0 || tmpRow>(imgHeight - 1) || tmpCol<0 || tmpCol>(imgWidth - 1))
			{
				return false;
			}
			HTuple grayval;
			get_grayval(*image, tmpRow, tmpCol, &grayval);
			int grayValue = grayval[0].I();
			if (strcmp(polar, "positive") == 0 || strcmp(polar, "all") == 0)
			{
				if (j == 0) {
					if (!posOk[j])
						posOk[j] = grayValue >= crossGray;
				}
				else {
					if (!posOk[j])
						posOk[j] = grayValue <= crossGray;
				}
			}
			if (strcmp(polar, "negative") == 0 || strcmp(polar, "all") == 0)
			{
				if (j == 0) {
					if (!negOK[j])
						negOK[j] = grayValue <= crossGray;
				}
				else {
					if (!negOK[j])
						negOK[j] = grayValue >= crossGray;
				}
			}
		}
	}
	bool allok = true;
	if (strcmp(polar, "positive") == 0)
		allok = (posOk[0] && posOk[1]);
	else if (strcmp(polar, "negative") == 0)
		allok = (negOK[0] && negOK[1]);
	else {
		allok = (posOk[0] && posOk[1]) || (negOK[0] && negOK[1]);
	}//polar=="all"
	return allok;
}

void ChipMod::toHtuple(const UnitInputPin<QList<double>>& p, HTuple &htuple)
{
	htuple.Reset();
	for (auto i = 0; i < p->size(); ++i)
		htuple[i] = p->at(i);
}

QList<QVariant> ChipMod::toQList(const HTuple & htuple)
{
	QList<QVariant> p;
	for (auto i = 0; i < htuple.Num(); ++i)
		p.push_back(htuple[i].D());
	return p;
}