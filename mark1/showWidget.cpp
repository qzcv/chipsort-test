#include "showWidget.h"
#include "halOpenCV.h"

ShowWidget::ShowWidget(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	m_tempView = new ImageView(ui.widget_tmp);
}

ShowWidget::~ShowWidget()
{
}

void ShowWidget::setParam(mark1Param * p)
{
	m_param = p;
}

void ShowWidget::showTemp()
{
	if (!m_param->MultPatternEnable[m_param->NowPatternNum]) {
		m_tempView->clear();
		m_tempView->update();
		return;
	}
	if (&m_param->SegImg[m_param->NowPatternNum] == NULL || m_param->SegImg[m_param->NowPatternNum].Id() == H_EMPTY_REGION) {
		m_tempView->clear();
		m_tempView->update();
		return;
	}
	m_tempView->clear();
	cv::Mat showImg;
	HalOpenCV::hal2cvImg(m_param->SegImg[m_param->NowPatternNum], showImg, true);
	m_tempView->dispImage(showImg);
	int index = m_param->NowPatternNum;
	char buf[30][32];
	const char* title[30];
	for (int i = 0;i < m_param->charCounts[m_param->NowPatternNum];i++) {
		sprintf(buf[i], "%d", i + 1);
		title[i] = buf[i];
		m_tempView->dispRect1(
			m_param->segRegionVector[index][i].row1 - m_param->RegionRoi[index].row1,
			m_param->segRegionVector[index][i].col1 - m_param->RegionRoi[index].col1,
			m_param->segRegionVector[index][i].row2 - m_param->RegionRoi[index].row1,
			m_param->segRegionVector[index][i].col2 - m_param->RegionRoi[index].col1, title[i]);
	}
	m_tempView->update();
	m_tempView->fitImage();
}

void ShowWidget::closeEvent(QCloseEvent *event)
{
	this->hide();
	emit closeShowWidget();
}

int ShowWidget::exec()
{
	return QDialog::exec();
}
