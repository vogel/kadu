#ifndef TABBAR_H
#define TABBAR_H

#include <qtabbar.h>
#include <qpainter.h>
#include <qrect.h>

class KaduTabBar : public QTabBar {
	Q_OBJECT
	public:
		KaduTabBar(QWidget *parent = 0, const char *name = 0);
		void layoutTabs();
		QSize sizeHint();

	protected:
		void paint(QPainter *p, QTab *t, bool selected) const;
		void paintLabel(QPainter *p, const QRect &r, QTab *t, bool selected) const;
};

#endif
