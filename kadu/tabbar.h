#ifndef TABBAR_H
#define TABBAR_H

#include <qtabbar.h>

class KaduTabBar : public QTabBar {
	Q_OBJECT
	public:
		KaduTabBar(QWidget *parent = 0, const char *name = 0);
};

#endif
