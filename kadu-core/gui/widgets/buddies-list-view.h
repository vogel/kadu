/*
 * %kadu copyright begin%
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * %kadu copyright end%
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef BUDDIES_LIST_VIEW_H
#define BUDDIES_LIST_VIEW_H

#include <QtGui/QTreeView>
#include <QtCore/QDateTime>
#include <QtCore/QTimer>

#include "buddies/buddy.h"
#include "chat/chat.h"
#include "contacts/contact.h"
#include "gui/actions/action-data-source.h"
#include "gui/widgets/buddies-list-view-selection-item.h"

class QContextMenuEvent;
class QModelIndex;
class QTemporaryFile;

class AbstractBuddyFilter;
class AbstractBuddiesModel;
class Buddy;
class BuddySet;
class BuddiesListViewDelegate;
class BuddiesListViewDelegateConfiguration;
class BuddiesModelProxy;
class ContactNoUnloadedAccountFilter;
class ContactSet;
class MainWindow;

class BuddiesListView : public QTreeView, public ActionDataSource
{
	Q_OBJECT

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
	MainWindow *MyMainWindow;
	BuddiesListViewDelegate *Delegate;
	AbstractBuddiesModel *Model;
	BuddiesModelProxy *ProxyModel;

	ContactNoUnloadedAccountFilter *HideUnloadedFilter;

	Buddy buddyAt(const QModelIndex &index) const;
	Contact contactAt(const QModelIndex &index) const;
	void triggerActivate(const QModelIndex &index);

	Chat chatForIndex(const QModelIndex &index) const;

	// Tool tips
	Contact ToolTipContact;
	QTimer ToolTipTimeoutTimer;

	QString BackgroundColor;
	QString AlternateBackgroundColor;
	BackgroundMode BackgroundImageMode;
	QString BackgroundImageFile;
	QTemporaryFile *BackgroundTemporaryFile;

private slots:
	void simpleModeChanged();
	void doubleClickedSlot(const QModelIndex &index);

	// Tool tips
	void toolTipTimeout();
	void toolTipRestart(QPoint pos);
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

	BuddiesModelProxy * proxyModel() { return ProxyModel; }

	void setShowAccountName(bool show);
	void useConfigurationColors(bool use);

	void selectBuddy(Buddy buddy);

	Contact currentContact() const;
	ContactSet selectedContacts() const;

	Buddy currentBuddy() const;
	BuddySet selectedBuddies() const;

	Chat currentChat() const;

	BuddiesListViewDelegateConfiguration & delegateConfiguration();

	void setBackground(const QString& backgroundColor, const QString& alternateColor, const QString& file = QString::null, BuddiesListView::BackgroundMode mode = BackgroundNone);
	void updateBackground();
	void updateLayout() { updateGeometries(); }

	// ActionDataSource implementation
	virtual BuddySet buddies();
	virtual ContactSet contacts();
	virtual Chat chat();

signals:
	void chatActivated(Chat chat);
	void buddyActivated(Buddy buddy);

	void buddySelectionChanged();
	void currentBuddyChanged(Buddy buddy);
	void currentContactChanged(Contact contact);
	void currentChanged(BuddiesListViewSelectionItem selectionItem);

};

#endif // BUDDIES_LIST_VIEW_H
