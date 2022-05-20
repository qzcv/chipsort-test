
#pragma once

//#ifndef PARAMTABLES_H
//#define PARAMTABLES_H

#include <QObject>
#include "QTableWidget"
#include "QCheckBox.h"
#include "qlabel.h"
#include "qmenu.h"
#include <QDoubleSpinBox>
#include <QMessageBox>
#include "inclu.h"

#include <vector>
using std::vector;
using namespace std;

class EXPORTS ParamTables : public QWidget
{
	Q_OBJECT

public:
	typedef enum{singleParam=0,groupParam,sameParam}ParamType; //���������ͣ�
	ParamTables(int Checknum,const vector<ParamType>& cbtype,bool labelOrCheck,bool withMinMax,int group0,int group1 ,QStringList cbName ,QWidget *parent=0);
	void getcheckBoxPointer(const QCheckBox* ptr);
	~ParamTables();
	QTableWidget *getTable();
	void setGroupLeadNumber(int Group0,int Group1);  //����ÿһ��Ĺܽ���Ŀ
	void setTemplateName(QString group0,QString group1); //����ÿ��ܽŵ�ģ������
	void setTableWidth(int Wid);
	void setTableHeight(int Height);
	void setCheckboxName(QStringList Cbname);
	void getCheckBoxVal(int checkIdx,int& checkVal); //��ȡcheckBox��ֵ
	void getCheckBoxName(int checkIdx,QString& checkName);
	void setHeaderName(QStringList Headername);
	void setHeadName(QString headname);
	void getValue(int checkIdx,int grpIdx, int leadIdx,bool minOrMax, double& val);
	void setValueRange(double minval,double maxval,int decimals);
	void setValue(int checkIdx,int grpIdx, int leadIdx,bool minOrMax, double val); //��ʼ������ֵ��ȥ
	void setParamType(int checkIdx,ParamType ptype);  //���һ��ܽŻ������йܽ�һ�����İ汾�����޸�
	//void getCheckBoxVal();
	
signals:
	void CellChanged(int checkIdx,int grpIdx, int leadIdx,bool minOrMax, double val); //�ڽ�������ı��ʱ�򴫳�ȥ���ź�
	void CopyItems(bool IsAll,int startCheck=0,int CheckRange=1,int starLead=0,int leadRange=1); //��������ʱ�򴫳����ź�
	void checkBoxToggled(int index,bool check);

public slots:
	void ContextMenuRequested(const QPoint & pos);
	void tableWidget_cellChanged(int row, int column);
	void CopyActionTriggered();

private slots:
	void Cb_Toggled(bool val);
	
private:
	void iniData();
	void iniTabHeaderLabels();
	void IniTableUI();
	void IniTableSlots();
	void resetLeadNumber(); //�ڹܽ���Ŀ�仯ʱ��ˢ�½���
	int m_checkboxNum;
	QMenu* m_cMenu;
	QAction* m_selAction[2];
	QTableWidget *m_ParentTable;
	//QCheckBox *m_cbBox[CheckBoxNum];
	//QTableWidget *m_childTable[ChildTableNum];
	vector<QCheckBox*> m_cbBox;
	vector<ParamType> m_cbParamType; //ÿһ��checkBox�Ĳ�������
	int m_parentWid;
	int m_parentHeight;
    QStringList m_cbName;
	QStringList m_listHeader;
	int m_groupNumber[2];  //��������Ĺܽ���Ŀ
	QString m_templateName[2]; //�ܽ�����ģ��
	QString m_headName;    //̧ͷ�ϵ����֣�������Χ��������Χ��
	bool m_loadingProject;
	double m_minRange,m_maxRange;  //ȡֵ��Χ
	int m_decimals; //С���㷶Χ
	bool m_labelOrCheck; //���������checkbox����label
	bool m_withMinMax;     //�������Ƿ����minmax
};

//#endif // TABLES_H
