#ifndef TABBAR_H
#define TABBAR_H

#include <qtabbar.h>
#include <qpainter.h>
#include <qrect.h>

class KaduTabBar : public QTabBar {
	Q_OBJECT
	public:
		KaduTabBar(QWidget *parent = 0, const char *name = 0);
		~KaduTabBar();
		void layoutTabs();
		QSize sizeHint();
		int insertTab(QTab *newTab, int index = -1);
		void removeTab(QTab *t);

	protected:
		void paint(QPainter *p, QTab *t, bool selected) const;

		QPtrList<QTab> *lstatic2;
};

#endif
