#pragma once
#include <QString>
#include <QVector>

class GRRreportParam
{
public:
	explicit GRRreportParam();
	~GRRreportParam();
	virtual void readWriteParam(bool r, const QString& dirPath, int level);

	QString GrrLotName;

	int modNum;
	int InputUnits;
	bool IsOutAll;
	bool IsGrrByTol;

	QVector<int> itemNum;				//һ���ܽ����������Ŀ����
	QVector<QVector<bool>> enable;		//������Ŀ���Ƿ�д�뱨��
	QVector<QVector<QString>> itText;	//������Ŀ��д�뱨�������
	QVector<QVector<double>> USL;		//������Ŀ��������
	QVector<QVector<double>> LSL;		//������Ŀ��������

private:
	void iniData();
};

