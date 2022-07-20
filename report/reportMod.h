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

	QVector<QVector<QVector<QVector<double>>>> m_R;					//����һ������һ���ϵļ���
	QVector<QVector<QVector<QVector<double>>>> m_Rabar;				//����һ�����ֺ����ݵ�ƽ������
	QVector<QVector<QVector<double>>> m_Rbar;						//�����������ֺ����ݵ�ƽ������

	QVector<QVector<QVector<QVector<double>>>> m_Xabar;				//����һ�����ֺ����ݵ�ƽ��ֵ
	QVector<QVector<QVector<double>>> m_Xbardiff;					//��������Xabar����

	QVector<QVector<QVector<QVector<double>>>> m_cycle1Mean;		//ÿһ���ϵ�������ʵ���ƽ��ֵ
	QVector<QVector<QVector<QVector<double>>>> m_cycle3Mean;		//ÿһ������������ʵ���ƽ��ֵ
	QVector<QVector<QVector<double>>> m_Rp;							//ȫ���ϵ�cycle3Mean�ļ���

	QVector<QVector<QVector<QVector<double>>>> m_data;				//�洢����
	QVector<QVector<int>> m_size;									//ȷ��ÿ�����ݵĳ������һ�����ݳ�����ͬ������NG

	QVector<QVector<QVector<double>>> m_GRR;						//���ظ���&������
	QVector<QVector<QVector<double>>> m_perGRR;						//���ظ���&������
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