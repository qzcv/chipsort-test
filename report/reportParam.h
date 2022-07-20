#pragma once
#include <QString>
#include <QVector>

class GRRreportParam
{
public:
	explicit GRRreportParam();
	~GRRreportParam();
	virtual void readWriteParam(bool r, const QString& dirPath, int level);
	bool isLoadFinished;

	QString GrrLotName;

	int modNum;
	int InputUnits;
	bool IsOutAll;
	bool IsGrrByTol;

	QVector<int> itemNum;				//一个管脚中输出的项目个数
	QVector<QString> topText;			//模块名称
	QVector<QVector<bool>> enable;		//单个项目的是否写入报告
	QVector<QVector<QString>> subText;	//单个项目的写入报告的名称
	QVector<QVector<double>> USL;		//单个项目数据上限
	QVector<QVector<double>> LSL;		//单个项目数据下限

private:
	void iniData();
};

