/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qtabbar.h>
#include <qrect.h>
#include <qaccel.h>
#include <qbitmap.h>
#include <qtoolbutton.h>
#include <qtooltip.h>
#include <qapplication.h>
#include <qpainter.h>
#include <qstyle.h>
#include <qiconset.h>
#include <qcursor.h>
#include <qsizepolicy.h>

#include "tabbar.h"

struct QTabPrivate;

KaduTabBar::KaduTabBar(QWidget *parent, const char *name)
	: QTabBar(parent, name) {
	fprintf(stderr, "KK KaduTabBar::KaduTabBar()\n");
	setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred));
	lstatic2 = new QPtrList<QTab>;
}

KaduTabBar::~KaduTabBar() {
	fprintf(stderr, "KK KaduTabBar::~KaduTabBar()\n");
	delete lstatic2;
	lstatic2 = 0;
}

void KaduTabBar::layoutTabs() {
	fprintf(stderr, "KK KaduTabBar::layoutTabs()\n");
	if (lstatic2->isEmpty())
		return;

	int hframe, vframe, overlap;
	hframe  = style().pixelMetric(QStyle::PM_TabBarTabHSpace, this);
	vframe  = style().pixelMetric(QStyle::PM_TabBarTabVSpace, this);
	overlap = style().pixelMetric(QStyle::PM_TabBarTabOverlap, this);

	QFontMetrics fm = fontMetrics();
	int x = 0;
	QRect r;
	QTab *t;
	bool reverse = QApplication::reverseLayout();
	if (reverse)
		t = lstatic2->last();
	else
		t = lstatic2->first();
	while (t) {
		int lw = fm.width(t->text());
		lw -= t->text().contains('&') * fm.width('&');
		lw += t->text().contains("&&") * fm.width('&');
		int iw = 0;
		int ih = 0;
		if (t->iconSet() != 0) {
			iw = t->iconSet()->pixmap(QIconSet::Small, QIconSet::Normal).width() + 4;
			ih = t->iconSet()->pixmap(QIconSet::Small, QIconSet::Normal).height();
			}
		int h = QMAX(fm.height(), ih);
		h = QMAX(h, QApplication::globalStrut().height());

		h += vframe;
//		t->setRect(QRect(x, 0, QMAX(lw + hframe + iw, QApplication::globalStrut().width()), h));
		t->setRect(QRect(0, x, h, QMAX(lw + hframe + iw, QApplication::globalStrut().width())));
//		x += t->rect().width() - overlap;
		x += t->rect().height() - overlap;
		r = r.unite(t->rect());
		if (reverse)
			t = lstatic2->prev();
		else
			t = lstatic2->next();
		}
	for (t = lstatic2->first(); t; t = lstatic2->next())
		t->setRect(QRect(t->rect().left(), t->rect().top(), r.width(), t->rect().height()));
}

QSize KaduTabBar::sizeHint() {
	fprintf(stderr, "KK KaduTabBar::sizeHint()\n");
	QTab *t = lstatic2->first();
	if (t) {
		QRect r(t->rect());
		while ((t = lstatic2->next()) != 0)
			r = r.unite(t->rect());
		return r.size().expandedTo(QApplication::globalStrut());
		}
	else {
		return QSize(0, 0).expandedTo(QApplication::globalStrut());
		}
}

void KaduTabBar::paint(QPainter *p, QTab *t, bool selected) const {
	fprintf(stderr, "KK KaduTabBar::paint()\n");
	QStyle::SFlags flags = QStyle::Style_Default;

	if (isEnabled() && t->isEnabled())
		flags |= QStyle::Style_Enabled;
	if ( selected )
		flags |= QStyle::Style_Selected;
//    else if(t == d->pressed)
//        flags |= QStyle::Style_Sunken;
    //selection flags
	if (t->rect().contains(mapFromGlobal(QCursor::pos())))
		flags |= QStyle::Style_MouseOver;

	QRect r(t->rect());
	p->setFont(font());
	QRect v(t->rect().top(), t->rect().left(), t->rect().height(), t->rect().width());
	
	p->save();
	p->setWindow(- r.width(), 0, p->window().width(), p->window().height());
	p->rotate(90.0);
	style().drawControl(QStyle::CE_TabBarTab, p, this, v,
		colorGroup(), flags, QStyleOption(t));

	int iw = 0;
	int ih = 0;
	if (t->iconSet() != 0) {
		iw = t->iconSet()->pixmap(QIconSet::Small, QIconSet::Normal).width() + 4;
		ih = t->iconSet()->pixmap(QIconSet::Small, QIconSet::Normal).height();
		}
	QFontMetrics fm = p->fontMetrics();
	int fw = fm.width(t->text());
	fw -= t->text().contains('&') * fm.width('&');
	fw += t->text().contains("&&") * fm.width('&');
	int w = iw + fw + 4;
	int h = QMAX(fm.height() + 4, ih);
	paintLabel(p, QRect(v.left() + (v.width() - w) / 2 - 3,
		v.top() + (v.height() - h) / 2,
		w, h), t, t->identifier() == keyboardFocusTab());
	p->restore();
}

int KaduTabBar::insertTab(QTab *newTab, int index)
{
	fprintf(stderr, "KK KaduTabBar::insertTab()\n");
	if (index < 0 || index > int(lstatic2->count()))
		lstatic2->append(newTab);
	else
		lstatic2->insert(index, newTab);
	int id = QTabBar::insertTab(newTab, index);

	return id;
}

void KaduTabBar::removeTab(QTab *t)
{
	fprintf(stderr, "KK KaduTabBar::removeTab()\n");
	lstatic2->remove(t);
	QTabBar::removeTab(t);
}
