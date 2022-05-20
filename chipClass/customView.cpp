#include "customView.h"

cusViewRotRect::cusViewRotRect(const QPointF & center, double len1, double len2, double angle, const QString & text, int arrowSize)
	:ImageViewRotRect(center, len1, len2, angle, text), m_arrowSize(arrowSize) {}

void cusViewRotRect::draw()
{
	ImageViewRotRect::draw();

	QPointF center = this->center();
	double angle = this->angle();
	double len1 = this->len1();
	double len2 = this->len2();

	drawArrow(center, len1, len2, angle, m_iv, m_painter, m_arrowSize);
}

cusViewRotRectList::cusViewRotRectList(const QList<RotRectListItem>& rects, int arrowSize)
	:ImageViewRotRectList(rects), m_arrowSize(arrowSize) {}

void cusViewRotRectList::draw()
{
	ImageViewRotRectList::draw();
	for (int i = 0; i < m_rects.size(); i++)
	{
		if (i == m_currentIndex)
		{
			draw_arrow(m_rects[i]);
		}
	}
}

void cusViewRotRectList::draw_arrow(ImageViewRotRect * rect)
{
	QPointF center = rect->center();
	double angle = rect->angle();
	double len1 = rect->len1();
	double len2 = rect->len2();

	drawArrow(center, len1, len2, angle, m_iv, m_painter, m_arrowSize);
}

void drawArrow(const QPointF &center, double len1, double len2, double angle, ImageView *iv, QPainter *painter, int arrowSize)
{
	for (auto i = 0;i < 2;++i)
	{
		QPointF midPoint;
		double cosa = len2*cos(angle + (i == 0 ? 1 : -1)*CV_PI / 2);
		double sina = len2*sin(angle + (i == 0 ? 1 : -1)*CV_PI / 2);

		midPoint.setX(center.x() + cosa);
		midPoint.setY(center.y() - sina);

		QPointF arrow[2];
		for (auto j = 0;j < 2;++j)
		{
			double cosa = arrowSize*cos(angle + (j == 0 ? 1 : -1)*CV_PI / 12);
			double sina = arrowSize*sin(angle + (j == 0 ? 1 : -1)*CV_PI / 12);

			arrow[j].setX(midPoint.x() - cosa);
			arrow[j].setY(midPoint.y() + sina);

			ImageViewLine *viewLine = new ImageViewLine(midPoint, arrow[j]);
			viewLine->setViewPainter(iv, painter);
			viewLine->display();
			delete viewLine;
		}
	}
}