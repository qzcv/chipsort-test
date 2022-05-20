
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
	typedef enum{singleParam=0,groupParam,sameParam}ParamType; //参数的类型，
	ParamTables(int Checknum,const vector<ParamType>& cbtype,bool labelOrCheck,bool withMinMax,int group0,int group1 ,QStringList cbName ,QWidget *parent=0);
	void getcheckBoxPointer(const QCheckBox* ptr);
	~ParamTables();
	QTableWidget *getTable();
	void setGroupLeadNumber(int Group0,int Group1);  //设置每一组的管脚数目
	void setTemplateName(QString group0,QString group1); //设置每组管脚的模板名字
	void setTableWidth(int Wid);
	void setTableHeight(int Height);
	void setCheckboxName(QStringList Cbname);
	void getCheckBoxVal(int checkIdx,int& checkVal); //获取checkBox的值
	void getCheckBoxName(int checkIdx,QString& checkName);
	void setHeaderName(QStringList Headername);
	void setHeadName(QString headname);
	void getValue(int checkIdx,int grpIdx, int leadIdx,bool minOrMax, double& val);
	void setValueRange(double minval,double maxval,int decimals);
	void setValue(int checkIdx,int grpIdx, int leadIdx,bool minOrMax, double val); //初始化设置值进去
	void setParamType(int checkIdx,ParamType ptype);  //针对一组管脚或者所有管脚一个数的版本进行修改
	//void getCheckBoxVal();
	
signals:
	void CellChanged(int checkIdx,int grpIdx, int leadIdx,bool minOrMax, double val); //在结果发生改变的时候传出去的信号
	void CopyItems(bool IsAll,int startCheck=0,int CheckRange=1,int starLead=0,int leadRange=1); //发生复制时候传出的信号
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
	void resetLeadNumber(); //在管脚数目变化时候刷新界面
	int m_checkboxNum;
	QMenu* m_cMenu;
	QAction* m_selAction[2];
	QTableWidget *m_ParentTable;
	//QCheckBox *m_cbBox[CheckBoxNum];
	//QTableWidget *m_childTable[ChildTableNum];
	vector<QCheckBox*> m_cbBox;
	vector<ParamType> m_cbParamType; //每一个checkBox的参数类型
	int m_parentWid;
	int m_parentHeight;
    QStringList m_cbName;
	QStringList m_listHeader;
	int m_groupNumber[2];  //左右两组的管脚数目
	QString m_templateName[2]; //管脚名字模板
	QString m_headName;    //抬头上的名字，例如误差范围，补偿范围等
	bool m_loadingProject;
	double m_minRange,m_maxRange;  //取值范围
	int m_decimals; //小数点范围
	bool m_labelOrCheck; //决定左边是checkbox还是label
	bool m_withMinMax;     //数据项是否包含minmax
};

//#endif // TABLES_H
