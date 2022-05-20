#include "dimensionMeasureWdg.h"
#include "dimensionMeasureMod.h"
#include "dimensionMeasureParam.h"
#include "ui_dimensionMeasureWdg.h"
#include "ImageView.h"
#include "imageViewItem.h"
#include "customView.h"

dimensionMeasureWdg::dimensionMeasureWdg(int i) :
	ModSetWidget(),ui(new Ui::dimensionMeasureWdg),m_widgetType(i)
{
	m_hasConnect = false;
	ui->setupUi(this);
	iniUi();
	m_layerIdx = 0;
	m_param = nullptr;
	m_module = nullptr;
	m_rectList = nullptr;
}

dimensionMeasureWdg::~dimensionMeasureWdg()
{
	delete ui;
}

void dimensionMeasureWdg::setModule(UnitModule *module)
{
	m_module=dynamic_cast<dimensionMeasureMod*>(module);
	m_param = m_module->m_param;
	//currentView()->setLayer(m_layerIdx);
	//updateUi();
}

void dimensionMeasureWdg::showEvent(QShowEvent * event)
{
	updateUi();
}

void dimensionMeasureWdg::connectSlots(bool link)
{
	if(m_hasConnect==link)
		return;
	m_hasConnect=link;

	connectOrNot(link, ui->sp_regionNum, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_searchGap, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_threshold, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_noiseMax, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_noiseMin, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));
	connectOrNot(link, ui->sp_validPoints, SIGNAL(valueChanged(int)), SLOT(spinBox_valueChanged(int)));

	connectOrNot(link, ui->cb_useRefer, SIGNAL(toggled(bool)), SLOT(checkBox_toggled(bool)));
	connectOrNot(link, ui->cb_bestEdge, SIGNAL(toggled(bool)), SLOT(checkBox_toggled(bool)));
	connectOrNot(link, ui->cb_validPoints, SIGNAL(toggled(bool)), SLOT(checkBox_toggled(bool)));

	connectOrNot(link, ui->rb_white, SIGNAL(toggled(bool)), SLOT(radioButton_toggled(bool)));
	connectOrNot(link, ui->rb_black, SIGNAL(toggled(bool)), SLOT(radioButton_toggled(bool)));

	connectOrNot(link, ui->cb_showRegionAfterDet, SIGNAL(toggled(bool)), SLOT(checkBox_toggled(bool)));
	connectOrNot(link, ui->cob_regionIndex, SIGNAL(currentIndexChanged(int)), SLOT(comboBox_IndexChanged(int)));
	connectOrNot(link, ui->dsp_lengthMin, SIGNAL(valueChanged(double)), SLOT(doubleSpinBox_valueChanged(double)));
	connectOrNot(link, ui->dsp_lengthMax, SIGNAL(valueChanged(double)), SLOT(doubleSpinBox_valueChanged(double)));
}

void dimensionMeasureWdg::iniUi()
{

}

void dimensionMeasureWdg::setUiValue()
{
	if(!m_param)
		return;

	ui->sp_regionNum->setValue(m_param->regionCount);
	ui->sp_searchGap->setValue(m_param->searchGap);
	ui->sp_threshold->setValue(m_param->threshold);
	ui->sp_noiseMax->setValue(m_param->noiseMaxNum);
	ui->sp_noiseMin->setValue(m_param->noiseMin);
	ui->sp_validPoints->setValue(m_param->validPointsPer);

	ui->cb_useRefer->setChecked(m_param->IsReferSection);
	ui->cb_showRegionAfterDet->setChecked(m_param->IsShowAfterDet);
	ui->cb_bestEdge->setChecked(m_param->IsBestEdge);
	ui->cb_validPoints->setChecked(m_param->IsValidPoints);
	if (m_param->IsBlack) {
		ui->rb_black->setChecked(true);
	}
	else {
		ui->rb_white->setChecked(true);
	}
	ui->cob_regionIndex->clear();
	for (int i = 0;i < m_param->regionCount;i++) {
		ui->cob_regionIndex->addItem(tr("reg%1length").arg(i + 1));
	}
	ui->cob_regionIndex->setCurrentIndex(0);
	ui->dsp_lengthMin->setValue(m_param->lengthMinVec[0]);
	ui->dsp_lengthMax->setValue(m_param->lengthMaxVec[0]);
}

void dimensionMeasureWdg::updateUi()
{
	connectSlots(false);
	setUiValue();
	connectSlots(true);
}

void dimensionMeasureWdg::drawRects2(int num, double * row, double * col, double * angle, double * len1, double * len2, const QStringList & titles, int arrowSize)
{
	QList<ImageViewRotRectList::RotRectListItem> list;
	for (auto i = 0;i < num;++i)
	{
		ImageViewRotRectList::RotRectListItem item;
		item.center = QPointF(col[i], row[i]);
		item.angle = angle[i];
		item.len[0] = len1[i];
		item.len[1] = len2[i];
		item.text = titles[i];

		list.append(item);
	}

	if (m_rectList)
		delete m_rectList;
	m_rectList = new cusViewRotRectList(list, arrowSize);
	currentView()->drawItem(m_rectList, true);
}

void dimensionMeasureWdg::dispRoi()
{
	//currentView()->clear();
	currentView()->clearLayer(m_layerIdx);
	currentView()->setDrawColor(0, 255, 0);
	double row[Max_Roi_Num], col[Max_Roi_Num], angle[Max_Roi_Num], len1[Max_Roi_Num], len2[Max_Roi_Num];
	QStringList title;
	for (int i = 0;i < m_param->regionCount;i++) {
		row[i] = m_param->RoiRegionVector[i].row;
		col[i] = m_param->RoiRegionVector[i].col;
		angle[i] = m_param->RoiRegionVector[i].angle;
		len1[i] = m_param->RoiRegionVector[i].length1;
		len2[i] = m_param->RoiRegionVector[i].length2;
		title << QString("roi%1").arg(i);
	}
	drawRects2(m_param->regionCount, row, col, angle, len1, len2, title, 10);
	currentView()->update();
}

void dimensionMeasureWdg::getRoi()
{
	currentView()->finishDraw();
	//currentView()->clear();
	currentView()->clearLayer(m_layerIdx);
	currentView()->update();
	double row[Max_Roi_Num], col[Max_Roi_Num], angle[Max_Roi_Num], len1[Max_Roi_Num], len2[Max_Roi_Num];
	currentView()->getDrawRects2(m_param->regionCount, row, col, angle, len1, len2);
	for (int i = 0;i < m_param->regionCount;i++) {
		m_param->RoiRegionVector[i].row = row[i];
		m_param->RoiRegionVector[i].col = col[i];
		m_param->RoiRegionVector[i].angle = angle[i];
		m_param->RoiRegionVector[i].length1 = len1[i];
		m_param->RoiRegionVector[i].length2 = len2[i];
	}
}

void dimensionMeasureWdg::on_bt_selectROI_toggled(bool check)
{
	if (check) {
		dispRoi();
		ui->bt_selectROI->setText(tr("Region Ok"));
	}
	else {
		//log(LogL1, ui.bt_selectROI->text());
		getRoi();
		ui->bt_selectROI->setText(tr("Select Region"));
		m_module->setParamChanged(ProductLevel);
	}
}

void dimensionMeasureWdg::spinBox_valueChanged(int val)
{
	QSpinBox* sp = qobject_cast<QSpinBox*>(sender());
	QString str0 = QString("");
	QString str1 = QString("");
	if (sp == ui->sp_regionNum) {
		if (ui->bt_selectROI->isChecked()) {
			getRoi();
		}
		dimensionMeasureParam::segRegion seg;
		seg.row = seg.col = 100;
		seg.length1 = seg.length2 = 30;  //给新添加的roi固定的开始位1置
		seg.angle = 0;
		if (val >= m_param->regionCount) {
			for (int i = m_param->regionCount;i < val;i++) {
				seg.row += 20 * i;
				seg.col += 20 * i;
				m_param->RoiRegionVector.push_back(seg);
				m_param->lengthMinVec.push_back(0);
				m_param->lengthMaxVec.push_back(100);
			}
		}
		else if (val < m_param->regionCount) {
			vector<dimensionMeasureParam::segRegion>::iterator itbegin = m_param->RoiRegionVector.begin() + val;
			vector<dimensionMeasureParam::segRegion>::iterator itEnd = m_param->RoiRegionVector.end();
			vector<double>::iterator minbegin = m_param->lengthMinVec.begin() + val;
			vector<double>::iterator minend = m_param->lengthMinVec.end();
			vector<double>::iterator maxbegin = m_param->lengthMaxVec.begin() + val;
			vector<double>::iterator maxend = m_param->lengthMaxVec.end();
			m_param->RoiRegionVector.erase(itbegin, itEnd);
			m_param->lengthMinVec.erase(minbegin, minend);
			m_param->lengthMaxVec.erase(maxbegin, maxend);
		}
		str0 = ui->label->text();
		str1 = tr(" change from %1 to %2").arg(m_param->regionCount).arg(val);
		m_param->regionCount = val;
		if (ui->bt_selectROI->isChecked()) {
			dispRoi();
		}
		connectSlots(false);
		ui->cob_regionIndex->clear();
		for (int i = 0;i < m_param->regionCount;i++) {
			ui->cob_regionIndex->addItem(tr("reg%1length").arg(i + 1));
		}
		ui->cob_regionIndex->setCurrentIndex(0);
		connectSlots(true);

	}
	else if (sp == ui->sp_searchGap) {
		str0 = ui->label_2->text();
		str1 = tr(" change from %1 to %2").arg(m_param->searchGap).arg(val);
		m_param->searchGap = val;
	}
	else if (sp == ui->sp_threshold) {
		str0 = ui->label_3->text();
		str1 = tr(" change from %1 to %2").arg(m_param->threshold).arg(val);
		m_param->threshold = val;
	}
	else if (sp == ui->sp_noiseMax) {
		str0 = ui->label_4->text();
		str1 = tr(" change from %1 to %2").arg(m_param->noiseMaxNum).arg(val);
		m_param->noiseMaxNum = val;
	}
	else if (sp == ui->sp_noiseMin) {
		str0 = ui->label_5->text();
		str1 = tr(" change from %1 to %2").arg(m_param->noiseMin).arg(val);
		m_param->noiseMin = val;
	}
	else if (sp == ui->sp_validPoints) {
		str0 = ui->cb_validPoints->text();
		str1 = tr(" change from %1 to %2").arg(m_param->validPointsPer).arg(val);
		m_param->validPointsPer = val;
	}
	m_module->setParamChanged(ProductLevel);
	//log(LogL1, str0 + str1);
}

void dimensionMeasureWdg::checkBox_toggled(bool check)
{
	QCheckBox* cb = qobject_cast<QCheckBox*>(sender());
	QString str0 = QString("");
	if (check)
		str0 = tr(" Check");
	else
		str0 = tr(" Uncheck");
	if (cb == ui->cb_useRefer) {
		m_param->IsReferSection = check;
	}
	else if (cb == ui->cb_showRegionAfterDet) {
		m_param->IsShowAfterDet = check;
	}
	else if (cb == ui->cb_bestEdge) {
		m_param->IsBestEdge = check;
	}
	else if (cb == ui->cb_validPoints) {
		m_param->IsValidPoints = check;
	}
	m_module->setParamChanged(ProductLevel);
	//log(LogL1, cb->text() + str0);
}

void dimensionMeasureWdg::comboBox_IndexChanged(int index)
{
	QComboBox* cob = qobject_cast<QComboBox*>(sender());
	if (cob == ui->cob_regionIndex) {
		ui->dsp_lengthMin->setValue(m_param->lengthMinVec[index]);
		ui->dsp_lengthMax->setValue(m_param->lengthMaxVec[index]);
	}
}

void dimensionMeasureWdg::doubleSpinBox_valueChanged(double val)
{
	QDoubleSpinBox* dsp = qobject_cast<QDoubleSpinBox*>(sender());
	QString str0 = QString("");
	QString str1 = QString("");
	if (dsp == ui->dsp_lengthMin) {
		str0 = ui->cob_regionIndex->currentText() + tr(" min val");
		str1 = tr(" change from %1 to %2")
			.arg(QString::number(m_param->lengthMinVec[ui->cob_regionIndex->currentIndex()], 'f', 2))
			.arg(QString::number(val, 'f', 2));
		m_param->lengthMinVec[ui->cob_regionIndex->currentIndex()] = val;
	}
	else if (dsp == ui->dsp_lengthMax) {
		str0 = ui->cob_regionIndex->currentText() + tr(" max val");
		str1 = tr(" change from %1 to %2")
			.arg(QString::number(m_param->lengthMaxVec[ui->cob_regionIndex->currentIndex()], 'f', 2))
			.arg(QString::number(val, 'f', 2));
		m_param->lengthMaxVec[ui->cob_regionIndex->currentIndex()] = val;
	}
	m_module->setParamChanged(ProductLevel);
	//log(LogL1, str0 + str1);
}

void dimensionMeasureWdg::radioButton_toggled(bool val)
{
	QRadioButton* rb = qobject_cast<QRadioButton*>(sender());
	QString str0 = QString("");
	QString str1 = QString("");
	if (val) {
		if (rb == ui->rb_white) {
			str0 = ui->groupBox_3->title();
			str1 = tr(" change from %1 to %2").arg(ui->rb_black->text()).arg(ui->rb_white->text());
			m_param->IsBlack = 0;
		}
		else if (rb == ui->rb_black) {
			str0 = ui->groupBox_3->title();
			str1 = tr(" change from %1 to %2").arg(ui->rb_white->text()).arg(ui->rb_black->text());
			m_param->IsBlack = 1;
		}
		m_module->setParamChanged(ProductLevel);
		//log(LogL1, str0 + str1);
	}
}
