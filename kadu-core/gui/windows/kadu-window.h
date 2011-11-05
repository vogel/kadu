/*
 * %kadu copyright begin%
 * Copyright 2010 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010 Przemysław Rudy (prudy1@o2.pl)
 * Copyright 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2010 Tomasz Rostański (rozteck@interia.pl)
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

#ifndef KADU_WINDOW_H
#define KADU_WINDOW_H

#include <QtCore/QMap>
#include <QtCore/QPair>

#include "chat/chat.h"
#include "buddies/buddy-list.h"
#include "gui/windows/main-window.h"
#include "os/generic/compositing-aware-object.h"
#include "exports.h"

class QSplitter;
class QMenu;
class QMenuBar;
class QPushButton;
class QVBoxLayout;

class Action;
class ActionDescription;
class ActionSource;
class BuddyInfoPanel;
class BuddiesListWidget;
class ChatsTreeView;
class GroupTabBar;
class KaduWebView;
class KaduWindowActionDataSource;
class KaduWindowActions;
class StatusButtons;

class KADUAPI KaduWindow : public MainWindow, private ConfigurationAwareObject, CompositingAwareObject
{
	Q_OBJECT

public:
	enum MenuType
	{
		MenuKadu,
		MenuBuddies,
		MenuTools,
		MenuHelp
	};

private:
	typedef QPair<Action *, MenuType> MenuAction;
	QMap<ActionDescription *, MenuAction> MenuActions;

	bool Docked; // TODO: 0.11.0 it is a hack
	QSplitter *Split;

	KaduWindowActions *Actions;

	BuddyInfoPanel *InfoPanel;
#ifdef Q_OS_MAC
	QMenuBar *MenuBar;
#endif
	QMenu *KaduMenu;
	QMenu *ContactsMenu;
	QMenu *ToolsMenu;
	QMenu *HelpMenu;
	QMenu *RecentChatsMenu;
	QAction *RecentChatsMenuAction;
	bool RecentChatsMenuNeedsUpdate;
	GroupTabBar *GroupBar;

	BuddiesListWidget *ContactsWidget;
	BuddiesModelProxy *ProxyModel;
	ChatsTreeView *ChatsTree;

	QWidget *MainWidget;
	QVBoxLayout *MainLayout;

	QMenu *StatusButtonMenu;
	StatusButtons *ChangeStatusButtons;
	QPoint LastPositionBeforeStatusMenuHide;

	QWidget *WindowParent;

	bool CompositingEnabled;

	KaduWindowActionDataSource *ActionData;

	void createGui();
	QWidget *createBuddiesWidget(QWidget *parent);
	QWidget *createChatsWidget(QWidget *parent);

	void createMenu();
	void createKaduMenu();
	void createContactsMenu();
	void createToolsMenu();
	void createHelpMenu();

	void storeConfiguration();

	virtual void compositingEnabled();
	virtual void compositingDisabled();

private slots:
	void openChatWindow(Chat chat);
	void buddyActivatedSlot(const Buddy &buddy);

	void invalidateRecentChatsMenu();
	void updateRecentChatsMenu();
	void openRecentChats(QAction *action);

	void iconThemeChanged();

	void buddiesChatViewChanged(int index);

protected:
	virtual void closeEvent(QCloseEvent *);
	virtual void keyPressEvent(QKeyEvent *);
	virtual void changeEvent(QEvent *event);

	virtual bool supportsActionType(ActionDescription::ActionType type);

	virtual void configurationUpdated();

public:
	static void createDefaultToolbars(QDomElement parentConfig);

	explicit KaduWindow(QWidget *parent = 0);
	virtual ~KaduWindow();

	virtual BuddiesListView * buddiesListView();
	virtual BuddiesModelProxy * buddiesProxyModel();

	void insertMenuActionDescription(ActionDescription *actionDescription, MenuType Type, int pos = -1);
	void removeMenuActionDescription(ActionDescription *actionDescription);

	void setDocked(bool);
	bool docked() { return Docked; }

	ActionDataSource * actionSource();
	BuddyInfoPanel * infoPanel() { return InfoPanel; }

#ifdef Q_OS_MAC
	QMenuBar* menuBar() const;
#endif

signals:
	void keyPressed(QKeyEvent *e);
	void parentChanged(QWidget *oldParent);

	void buddyActivated(const Buddy &buddy);

};

#endif // KADU_WINDOW_H
