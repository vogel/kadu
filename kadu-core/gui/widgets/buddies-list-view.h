/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef BUDDIES_LIST_VIEW_H
#define BUDDIES_LIST_VIEW_H

#include <QtGui/QTreeView>
#include <QtCore/QDateTime>
#include <QtCore/QTimer>

#include "chat/chat.h"
#include "buddies/buddy.h"

class QContextMenuEvent;
class QModelIndex;
class QTemporaryFile;

class AbstractBuddyFilter;
class AbstractBuddiesModel;
class Buddy;
class BuddySet;
class BuddiesListViewDelegate;
class BuddiesModelProxy;
class MainWindow;

class BuddiesListView : public QTreeView
{
	Q_OBJECT

	MainWindow *MyMainWindow;
	BuddiesListViewDelegate *Delegate;
	BuddiesModelProxy *ProxyModel;

	Buddy buddyAt(const QModelIndex &index) const;
	void triggerActivate(const QModelIndex &index);

	Chat  chatForIndex(const QModelIndex &index) const;

	// Tool tips
	Buddy ToolTipContact;
	QTimer ToolTipTimeoutTimer;

public:
	enum BackgroundMode
	{
		BackgroundNone,
		BackgroundCentered,
		BackgroundTiled,
		BackgroundTiledAndCentered,
		BackgroundStretched
	};

private:
	QString BackgroundColor;
	BackgroundMode BackgroundImageMode;
	QString BackgroundImageFile;
	QTemporaryFile *BackgroundTemporaryFile;

private slots:
	void doubleClickedSlot(const QModelIndex &index);

	// Tool tips
	void toolTipTimeout();
	void toolTipRestart();
	void toolTipHide(bool waitForAnother = true);

protected:
	virtual void contextMenuEvent(QContextMenuEvent *event);
	virtual void keyPressEvent(QKeyEvent *event);

	virtual void wheelEvent(QWheelEvent *event);
	virtual void leaveEvent(QEvent *event);

	virtual void mousePressEvent(QMouseEvent *event);
	virtual void mouseReleaseEvent(QMouseEvent *event);
	virtual void mouseMoveEvent(QMouseEvent *event);

	virtual void resizeEvent(QResizeEvent *event);

protected slots:
	virtual void currentChanged(const QModelIndex &current, const QModelIndex &previous);
	virtual void selectionChanged( const QItemSelection &selected, const QItemSelection &deselected);

public:
	BuddiesListView(MainWindow *mainWindow, QWidget *parent = 0);
	virtual ~BuddiesListView();

	virtual void setModel(AbstractBuddiesModel *model);
	void addFilter(AbstractBuddyFilter *filter);
	void removeFilter(AbstractBuddyFilter *filter);

	Buddy currentBuddy() const;
	BuddySet selectedBuddies() const;

	Chat  currentChat() const;

	void setBackground(const QString &backgroundColor, const QString &file = QString::null, BackgroundMode mode = BackgroundNone);
	void updateBackground();

signals:
	void chatActivated(Chat chat);
	void buddySelectionChanged();

	void currentBuddyChanged(Buddy buddy);

};

#endif // BUDDIES_LIST_VIEW_H
