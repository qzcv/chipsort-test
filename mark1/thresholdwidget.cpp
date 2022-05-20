#include "thresholdwidget.h"

thresholdWidget::thresholdWidget(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	m_minArea=30;
	m_charCounts=1;
	m_threshold=70;
	m_horOrVer=0;
	m_charGap=50;
	m_minHoles=0;
	m_loadingProject=false;
	connect(ui.sp_minArea, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connect(ui.sp_threshold, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connect(ui.sp_TOL, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connect(ui.sp_charGap, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connect(ui.sp_minHoles, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));

	connect(ui.hs_threshold, SIGNAL(valueChanged(int)), SLOT(hs_threshold_valueChanged(int)));
	connect(ui.rb_hor, SIGNAL(toggled(bool)), SLOT(radioButton_toggled(bool)));
	connect(ui.rb_ver, SIGNAL(toggled(bool)), SLOT(radioButton_toggled(bool)));

	connect(ui.cob_charIndex, SIGNAL(currentIndexChanged(int)), SLOT(cob_charIndex_currentIndexChanged(int)));
}

thresholdWidget::~thresholdWidget()
{

}

void thresholdWidget::radioButton_toggled(bool val)
{
	if (m_loadingProject)
		return;
	QRadioButton* rb=qobject_cast<QRadioButton*>(sender());
	if (val){
		if (rb==ui.rb_hor){
			m_horOrVer=0;
		}
		else if(rb==ui.rb_ver){
			m_horOrVer=1;
		}
		emit areaAndThreChanged(m_threshold,m_minArea,m_tol,m_horOrVer,m_charGap,m_minHoles);
	}
}

void thresholdWidget::setMinArea(int val)
{
	m_minArea=val;
	ui.sp_minArea->setValue(val);
}

void thresholdWidget::setCharCounts(int val)
{
	if (val==m_charCounts || m_loadingProject){
		return;
	}
	m_charCounts=val;
	ui.lb_charCounts->setText(tr("%1").arg(val));
	disconnect(ui.cob_charIndex, SIGNAL(currentIndexChanged(int)),this, SLOT(cob_charIndex_currentIndexChanged(int)));
	ui.cob_charIndex->clear();
	for (int i=0;i<m_charCounts;i++){
		ui.cob_charIndex->addItem(QString::fromLocal8Bit("×Ö·û%1").arg(i+1));
	}
	connect(ui.cob_charIndex, SIGNAL(currentIndexChanged(int)),this, SLOT(cob_charIndex_currentIndexChanged(int)));

}

void thresholdWidget::setThreshold(int val)
{
	m_threshold=val;
	ui.sp_threshold->setValue(val);
	ui.hs_threshold->setValue(val);
}

void thresholdWidget::setCharTOL(int val)
{
	m_tol=val;
	ui.sp_TOL->setValue(val);
}

void thresholdWidget::setCharDir(int horOrVer)
{
	m_horOrVer=horOrVer;
	if (horOrVer==0){
		ui.rb_hor->setChecked(1);
	}
	else if(horOrVer==1){
		ui.rb_ver->setChecked(1);
	}
}

void thresholdWidget::setCharGap(int gap)
{
	m_charGap=gap;
	ui.sp_charGap->setValue(gap);
}

void thresholdWidget::setMinHoles(int val)
{
	m_minHoles=val;
	ui.sp_minHoles->setValue(val);
}

void thresholdWidget::getThresholdAndArea(int& thre,int& area,int& tol,int& horOrVer,int& charGap,int& minHoles)
{
	thre=ui.sp_threshold->value();
	area=ui.sp_minArea->value();
	tol=ui.sp_TOL->value();
	if (ui.rb_hor->isChecked()){
		horOrVer=0;
	}
	else{
		horOrVer=1;
	}
	charGap=ui.sp_charGap->value();
	minHoles=ui.sp_minHoles->value();
}

void thresholdWidget::IsLoading(bool val)
{
	m_loadingProject=val;
}

void thresholdWidget::spinBox_valueChanged(int val)
{
	if (m_loadingProject)
		return;
	QSpinBox* sp=qobject_cast<QSpinBox*>(sender());
	if (sp==ui.sp_minArea){
		m_minArea=val;
	}
	else if(sp==ui.sp_threshold){
		m_threshold=val;
		m_loadingProject=true;
		ui.hs_threshold->setValue(val);
		m_loadingProject=false;
	}
	else if(sp==ui.sp_TOL){
		m_tol=val;
	}
	else if(sp==ui.sp_charGap){
		m_charGap=val;
	}
	else if(sp==ui.sp_minHoles){
		m_minHoles=val;
	}
	emit areaAndThreChanged(m_threshold,m_minArea,m_tol,m_horOrVer,m_charGap,m_minHoles);
}

void thresholdWidget::hs_threshold_valueChanged(int val)
{
	if (m_loadingProject)
		return;
	ui.sp_threshold->setValue(val);
}

void thresholdWidget::cob_charIndex_currentIndexChanged(int index)
{
	
}

int thresholdWidget::exec()
{
	return QDialog::exec();
}