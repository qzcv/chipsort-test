#pragma once
#include <moduleClass.h>
#include <imageModule.h>
#include "reportParam.h"

using namespace qzcv;

class GRRreportMod : public UnitModule
{
public:
	explicit GRRreportMod();
	explicit GRRreportMod(const QString&);
	~GRRreportMod();
	int command(int cmdID, void* dataIn, void* dataOut);
	virtual int afterSetProperty(MetaProperty*);
protected:
	virtual void save(const QString& dirPath);
	virtual void load(const QString& dirPath, QvsParamLevel level);
	virtual int run(const QString& funName);
	virtual void viewResult(ImageView* iv, const QString& funName,int);
	virtual void textResult(ResultText* text, const QString& funName);
private:
	void iniData();
	void createPins();
	void reset();
	bool isFinish();

	void resetGrrData();
	void calGrrData();
	void minMaxOf3(double a, double b, double c, double &min, double &max, double &tol);

	void genGrrReport_xlsx();
	void genGrrReport_html();

	void updatePin();
	QString pinName(QString prefix, int idx);
	QList<QVariant> transToList(QVariant);

private:
	GRRreportParam* m_param;
	friend class GRRreportWdg;
	bool m_ready;
	int m_count;
	typedef enum { idle = 0, logging, completed }Status;
	Status m_status;

	typedef enum { NoRun = 0, OkFail, SizeWrong }DetRes;
	DetRes m_detRes;

	QVector<QVector<QVector<QVector<double>>>> m_R;					//代表一人三轮一颗料的极差
	QVector<QVector<QVector<QVector<double>>>> m_Rabar;				//代表一人三轮后数据的平均极差
	QVector<QVector<QVector<double>>> m_Rbar;						//代表三人三轮后数据的平均极差

	QVector<QVector<QVector<QVector<double>>>> m_Xabar;				//代表一人三轮后数据的平均值
	QVector<QVector<QVector<double>>> m_Xbardiff;					//代表三人Xabar极差

	QVector<QVector<QVector<QVector<double>>>> m_cycle1Mean;		//每一个料单人三次实验的平均值
	QVector<QVector<QVector<QVector<double>>>> m_cycle3Mean;		//每一个料三人三次实验的平均值
	QVector<QVector<QVector<double>>> m_Rp;							//全部料的cycle3Mean的极差

	QVector<QVector<QVector<QVector<double>>>> m_data;				//存储数据
	QVector<QVector<int>> m_size;									//确保每次数据的长度与第一次数据长度相同，否则NG

	QVector<QVector<QVector<double>>> m_GRR;						//可重复性&再现性
	QVector<QVector<QVector<double>>> m_perGRR;						//可重复性&再现性
private:
	UnitInputPin<bool> p_auto;
	QList<UnitInputPin<int> *> p_ok;
	QList<UnitInputPin<QMap<QString, QVariant>> *> p_it;
};

class calibReportMod :public UnitModule
{
public:
	explicit calibReportMod();
	explicit calibReportMod(const QString&);
	~calibReportMod();
	int command(int cmdID, void* dataIn, void* dataOut);
	virtual int afterSetProperty(MetaProperty*);
protected:
	virtual void save(const QString& dirPath);
	virtual void load(const QString& dirPath, QvsParamLevel level);
	virtual int run(const QString& funName) { return 0; };
	virtual void viewResult(ImageView* iv, const QString& funName, int) {};
	virtual void textResult(ResultText* text, const QString& funName) {};
private:
	void iniData();
	void createPins();
	void RWParam(bool r, QString dirPath, int level);

	void genCalibReport_html(int ispix);
	void genCalibReport_xlsx(int ispix);
private:
	friend class calibReportWdg;

	bool m_is3D;
	QString m_name;
private:
	UnitInputPin<QList<double>> p_calibInfo;
};