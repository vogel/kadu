#ifndef KADU_TABBAR_H
#define KADU_TABBAR_H

#include <qtabbar.h>
#include <qpainter.h>

class QToolButton;

class KaduTabBar : public QTabBar {
	Q_OBJECT
	public:
		KaduTabBar(QWidget *parent = 0, const char *name = 0);
		~KaduTabBar();
		void layoutTabs();
		QSize sizeHint() const;
		QSize minimumSizeHint() const;
		int insertTab(QTab *newTab, int index = -1);
		void removeTab(QTab *t);

	public slots:
		void setCurrentTab(QTab *tab);

	private slots:
		void scrollTabsVert();

	protected:
		void paint(QPainter *p, QTab *t, bool selected) const;
		void updateArrowButtonsVert();
		void makeVisibleVert(QTab *t);
		void resizeEvent(QResizeEvent *);
		void dragEnterEvent(QDragEnterEvent* e);
		void dropEvent(QDropEvent* e);

		QPtrList<QTab> *lstatic2;
		bool vertscrolls;
		QToolButton *upB;
		QToolButton *downB;
};

#endif
