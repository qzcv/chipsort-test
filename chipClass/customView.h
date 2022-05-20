#pragma once
#include "imageViewItem.h"
#include "inclu.h"

void drawArrow(const QPointF &center, double len1, double len2, double angle, ImageView *iv, QPainter *painter, int arrowSize);

class EXPORTS cusViewRotRect : public ImageViewRotRect
{
public:
	cusViewRotRect(const QPointF& center, double len1, double len2, double angle, const QString& text = "", int arrowSize = 5);
	virtual void draw();
	~cusViewRotRect() {};

private:
	int m_arrowSize;
};

class EXPORTS cusViewRotRectList : public ImageViewRotRectList
{
public:
	cusViewRotRectList(const QList<RotRectListItem>& rects, int arrowSize = 5);
	virtual void draw();
	~cusViewRotRectList() {};

private:
	void draw_arrow(ImageViewRotRect *rect);
	
	int m_arrowSize;
};

