#ifndef THRESHOLDWIDGET_H
#define THRESHOLDWIDGET_H

#include <QDialog>
#include "ui_thresholdwidget.h"
#include <vector>
using std::vector;

class thresholdWidget : public QDialog
{
	Q_OBJECT

public:
	thresholdWidget(QWidget *parent = 0);
	void setMinArea(int val);
	void setCharCounts(int val);
	void setThreshold(int val);
	void setCharTOL(int val);
	void setCharDir(int horOrVer);
	void setCharGap(int gap);
	void setMinHoles(int val);
	void getThresholdAndArea(int& thre,int& area,int& Tol,int& horOrVer,int& charGap,int& minHoles);
	void IsLoading(bool loading);
	~thresholdWidget();
	int exec();

signals:
	void areaAndThreChanged(int thre,int minarea,int tol,int horOrVer,int charGap,int minHoles);

private slots:
	void hs_threshold_valueChanged(int val);
	void cob_charIndex_currentIndexChanged(int index);
	void radioButton_toggled(bool val);
	void spinBox_valueChanged(int val);

private:
	Ui::thresholdWidget ui;
	int m_minArea;
	int m_charCounts;
	int m_threshold;
	int m_tol;  //»±œ›»›»Ã∂»
	int m_horOrVer;
	int m_charGap;
	int m_minHoles;
	bool m_loadingProject;
};

#endif // THRESHOLDWIDGET_H
