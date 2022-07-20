#pragma once
#include <QString>
#include <QVector>

#define ROI_MAX_NUM 2

class composeND_preParam
{
public:
	explicit composeND_preParam();
	~composeND_preParam();
	virtual void readWriteParam(bool r, const QString& dirPath, int level);

	int imNum;				//ͼ����
	QVector<int> roiNum;	//��ͼ��ROI��Ŀ
	QVector<QVector<double>> regRow0;
	QVector<QVector<double>> regCol0;
	QVector<QVector<double>> regRow1;
	QVector<QVector<double>> regCol1;

private:
	void iniData();
};

