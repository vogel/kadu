/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qapplication.h>
#include <qcursor.h>
#include <q3dragobject.h>
#include <qinputdialog.h>
#include <qpainter.h>
#include <qstyle.h>
#include <qtoolbutton.h>
//Added by qt3to4:
#include <QDropEvent>
#include <QResizeEvent>
#include <Q3PtrList>
#include <Q3PopupMenu>
#include <QDragEnterEvent>
#include <QStyleOptionTab>

#include "debug.h"
#include "groups_manager.h"
#include "misc.h"
#include "tabbar.h"
#include "userbox.h"
#include "userinfo.h"

struct QTabPrivate;

KaduTabBar::KaduTabBar(QWidget *parent, const char *name)
	: QTabBar(parent), lstatic2(new Q3PtrList<QString>), vertscrolls(false), upB(0), downB(0)
{
	kdebugf();
	setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred));
	upB = new QToolButton(Qt::UpArrow, this, "qt_up_btn");
	connect(upB, SIGNAL(clicked()), this, SLOT(scrollTabsVert()));
	upB->hide();
	downB = new QToolButton(Qt::DownArrow, this, "qt_down_btn");
	connect(downB, SIGNAL(clicked()), this, SLOT(scrollTabsVert()));
	downB->hide();
	setAcceptDrops(true);
}

KaduTabBar::~KaduTabBar()
{
	kdebugf();
	delete lstatic2;
	lstatic2 = 0;
}

void KaduTabBar::layoutTabs()
{
	kdebugf();
	if (lstatic2->isEmpty())
		return;

	int hframe, vframe, overlap;
	hframe  = style()->pixelMetric(QStyle::PM_TabBarTabHSpace, 0, this);
	vframe  = style()->pixelMetric(QStyle::PM_TabBarTabVSpace, 0, this);
	overlap = style()->pixelMetric(QStyle::PM_TabBarTabOverlap, 0, this);

	QFontMetrics fm = fontMetrics();
	int x = 0;
	QRect r;
	QString *t;
	bool reverse = QApplication::reverseLayout();
	if (reverse)
		t = lstatic2->last();
	else
		t = lstatic2->first();
	while (t)
	{
		int lw = fm.width(*t);
// 		lw -= t->contains('&') * fm.width('&');
// 		lw += t->contains("&&") * fm.width('&');
		int iw = 0;
		int ih = 0;
// 		if (t->iconSet() != 0)
// 		{
// 			iw = t->iconSet()->pixmap(QIcon::Small, QIcon::Normal).width() + 4;
// 			ih = t->iconSet()->pixmap(QIcon::Small, QIcon::Normal).height();
// 		}
		int h = QMAX(fm.height(), ih);
		h = QMAX(h, QApplication::globalStrut().height());

		h += vframe;
//		t->setRect(QRect(x, 0, QMAX(lw + hframe + iw, QApplication::globalStrut().width()), h));
// 		t->setRect(QRect(0, x, h, QMAX(lw + hframe + iw, QApplication::globalStrut().width())));
//		x += t->rect().width() - overlap;
// 		x += t->rect().height() - overlap;
// 		r = r.unite(t->rect());
		if (reverse)
			t = lstatic2->prev();
		else
			t = lstatic2->next();
	}
// 	for (t = lstatic2->first(); t; t = lstatic2->next())
// 		t->setRect(QRect(t->rect().left(), t->rect().top(), r.width(), t->rect().height()));
}

QSize KaduTabBar::sizeHint() const
{
	kdebugf();
// 	QString *t = lstatic2->first();
// 	if (t)
// 	{
// 		QRect r(t->rect());
// 		while ((t = lstatic2->next()) != 0)
// 			r = r.unite(t->rect());
// 		return r.size().expandedTo(QApplication::globalStrut());
// 	}
// 	else
// 	{
		return QSize(0, 0).expandedTo(QApplication::globalStrut());
// 	}
}

QSize KaduTabBar::minimumSizeHint() const
{
	kdebugf();
	return QSize(sizeHint().width(), downB->sizeHint().height() * 2 + 75);
}

void KaduTabBar::paint(QPainter *p, int index, bool selected) const
{
//	kdebugf();
	QStyle::State state = QStyle::State_None;

	if (isEnabled()/* && t->isEnabled()*/)
		state |= QStyle::State_Enabled;
	if ( selected )
		state |= QStyle::State_Selected;
//	else if(t == d->pressed)
//		flags |= QStyle::Style_Sunken;
	//selection flags
	if (tabRect(index).contains(mapFromGlobal(QCursor::pos())))
		state |= QStyle::State_MouseOver;

	QRect r(tabRect(index));
	p->setFont(font());
	QRect v(r.top(), r.left(), r.height(), r.width());

	p->save();
	p->setWindow(- r.width(), 0, p->window().width(), p->window().height());
	p->rotate(90.0);

	QStyleOptionTab styleOptionTab;
	styleOptionTab.rect = v;
	styleOptionTab.state = state;

	style()->drawControl(QStyle::CE_TabBarTab, &styleOptionTab, p, this);

	int iw = 0;
	int ih = 0;
	if (!tabIcon(index).isNull())
	{
		iw = tabIcon(index).pixmap(QIcon::Small, QIcon::Normal).width() + 4;
		ih = tabIcon(index).pixmap(QIcon::Small, QIcon::Normal).height();
	}
	QFontMetrics fm = p->fontMetrics();
	int fw = fm.width(tabText(index));
// 	fw -= tabText(index).contains('&') * fm.width('&');
// 	fw += tabText(index).contains("&&") * fm.width('&');
	int w = iw + fw + 4;
	int h = QMAX(fm.height() + 4, ih);
// 	paintLabel(p, QRect(v.left() + (v.width() - w) / 2 - 3,
// 		v.top() + (v.height() - h) / 2,
// 		w, h), t, t->identifier() == keyboardFocusTab());
	p->restore();
//	kdebugf2();
}
/*
int KaduTabBar::insertTab(QString tabName, int index)
{
	kdebugf();
	if (index < 0 || index > int(lstatic2->count()))
		lstatic2->append(newTab);
	else
		lstatic2->insert(index, newTab);
	int id = QTabBar::insertTab(newTab, index);
	updateArrowButtonsVert();
	makeVisibleVert(tab(currentTab()));

	return id;
}*/
/*
void KaduTabBar::removeTab(QTab *t)
{
	kdebugf();
	lstatic2->remove(t);
	QTabBar::removeTab(t);
	updateArrowButtonsVert();
	makeVisibleVert(tab(currentTab()));
	update();
}*/
/*
void KaduTabBar::setCurrentTab(QTab *tab)
{
	if (tab && lstatic2)
		makeVisibleVert(tab);
	QTabBar::setCurrentTab(tab);
}*/

void KaduTabBar::resizeEvent(QResizeEvent *e)
{
	kdebugf();
	const int arrowHeight = 16;
	downB->setGeometry(0, height() - arrowHeight, width(), arrowHeight);
	upB->setGeometry(0, height() - 2 * arrowHeight, width(), arrowHeight);
	QTabBar::resizeEvent(e);
	updateArrowButtonsVert();
// 	makeVisibleVert(tab(currentTab()));
}
/*
void KaduTabBar::makeVisibleVert(QString tab)
{
	kdebugf();
	bool tooFarUp = (tab && tab->rect().top() < 0);
	bool tooFarDown = (tab && tab->rect().bottom() >= upB->y());
	if (!vertscrolls || (!tooFarUp && !tooFarDown))
		return;

	layoutTabs();

	int offset = 0;
	if (tooFarUp)
		offset = tab == lstatic2->first() ? 0 : tab->rect().top() - 8;
	else
		if (tooFarDown)
			offset = tab->rect().bottom() - upB->y() + 1;
	for (QTab *t = lstatic2->first(); t; t = lstatic2->next())
	{
		QRect r = t->rect();
		r.moveBy(0, -offset);
		t->setRect(r);
	}

	upB->setEnabled(offset != 0);
	downB->setEnabled(lstatic2->last()->rect().bottom() >= upB->y());

	if (!upB->isEnabled() && upB->isDown())
		upB->setDown(FALSE);
	if (!downB->isEnabled() && downB->isDown())
		downB->setDown(FALSE);

	update();
}*/

void KaduTabBar::updateArrowButtonsVert()
{
// 	kdebugf();
// 	bool b = lstatic2->last() && (lstatic2->last()->rect().bottom() > height());
// 	vertscrolls = b;
// 	if (vertscrolls)
// 	{
// 		upB->setEnabled(FALSE);
// 		downB->setEnabled(TRUE);
// 		upB->show();
// 		downB->show();
// 	}
// 	else
// 	{
// 		upB->hide();
// 		downB->hide();
// 	}
}

void KaduTabBar::scrollTabsVert()
{
// 	kdebugf();
// 	QTab *up = 0;
// 	QTab *down = 0;
// 	for (QTab *t = lstatic2->first(); t; t = lstatic2->next())
// 	{
// 		if (t->rect().top() < 0 && t->rect().bottom() > 0)
// 			up = t;
// 		if (t->rect().top() < upB->y() + 2)
// 			down = t;
// 	}
// 	if (sender() == upB)
// 		makeVisibleVert(up);
// 	else
// 		if (sender() == downB)
// 			makeVisibleVert(down);
}

void KaduTabBar::dragEnterEvent(QDragEnterEvent* e)
{
	kdebugf();
	e->accept(UlesDrag::canDecode(e) &&
		dynamic_cast<UserBox*>(e->source()));
	kdebugf2();
}

void KaduTabBar::dropEvent(QDropEvent* e)
{
// 	kdebugf();
// 
// 	QStringList ules;
// 	if (dynamic_cast<UserBox*>(e->source()) && UlesDrag::decode(e, ules))
// 	{
// 		QApplication::setOverrideCursor(QCursor(Qt::ArrowCursor));
// 		QString group;
// 		if (selectTab(e->pos()))
// 			group = selectTab(e->pos())->text();
// 		else
// 		{
// 			bool ok;
// 			QString text;
// 			do
// 			{
// 				text = QInputDialog::getText(tr("Add new group"), tr("Name of new group:"), QLineEdit::Normal, text, &ok);
// 				if (!ok)
// 				{
// 					QApplication::restoreOverrideCursor();
// 					return;
// 				}
// 				if (UserInfo::acceptableGroupName(text))
// 					group = text;
// 			}
// 			while (group.isEmpty());
// 		}
// 		if (group == GroupsManager::tr("All"))
// 			group = QString::null;
// 
// 		Q3PopupMenu menu(this);
// 		menu.insertItem(tr("Add to group %1").arg(group), 2);
// 
// 		if (tabText(currentIndex()) != GroupsManager::tr("All"))
// 			menu.insertItem(tr("Move to group %1").arg(group), 1);
// 
// 		int menuret = -1;
// 		if (group.isEmpty() || (menuret = showPopupMenu(&menu)) == 1)
// 		{
// 			QStringList groups;
// 			if (!group.isEmpty())
// 				groups.append(group);
// 			CONST_FOREACH(ule, ules)
// 				userlist->byAltNick(*ule).setData("Groups", groups);
// 		}
// 		else if (menuret == 2)
// 		{
// 			CONST_FOREACH(ule, ules)
// 			{
// 				UserListElement user = userlist->byAltNick(*ule);
// 				QStringList userGroups = user.data("Groups").toStringList();
// 				if (!userGroups.contains(group))
// 				{
// 					userGroups.append(group);
// 					user.setData("Groups", userGroups);
// 				}
// 			}
// 		}
// 
// 		// too slow, we need to do something about that
// 		userlist->writeToConfig();
// 
// 		QApplication::restoreOverrideCursor();
// 	}
// 	kdebugf2();
}
