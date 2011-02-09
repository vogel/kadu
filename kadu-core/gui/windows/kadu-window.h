/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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
class GroupTabBar;
class KaduWebView;
class KaduWindowActions;
class StatusButtons;

class KADUAPI KaduWindow : public MainWindow, private ConfigurationAwareObject, CompositingAwareObject
{
	Q_OBJECT

public:
	enum MenuType
	{
		MenuKadu,
		MenuContacts,
		MenuTools,
		MenuHelp
	};

private:
	typedef QPair<Action *, MenuType> MenuAction;
	QMap<ActionDescription *, MenuAction> MenuActions;

	bool Docked; // TODO: 0.7.1 it is a hack
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

	QWidget *MainWidget;
	QVBoxLayout *MainLayout;

	QMenu *StatusButtonMenu;
	StatusButtons *ChangeStatusButtons;
	QPoint LastPositionBeforeStatusMenuHide;

	QWidget *WindowParent;

	bool CompositingEnabled;

	void createGui();
	void createMenu();
	void createKaduMenu();
	void createContactsMenu();
	void createToolsMenu();
	void createHelpMenu();

	void storeConfiguration();

	virtual void compositingEnabled();
	virtual void compositingDisabled();

#ifdef Q_OS_MAC
	QMenuBar* menuBar() const;
#endif

private slots:
	void openChatWindow(Chat chat);
	void buddyActivated(const Buddy &buddy);

	void invalidateRecentChatsMenu();
	void updateRecentChatsMenu();
	void openRecentChats(QAction *action);

	void iconThemeChanged();

protected:
	virtual void closeEvent(QCloseEvent *);
	virtual void keyPressEvent(QKeyEvent *);
	virtual void changeEvent(QEvent *event);

	virtual bool supportsActionType(ActionDescription::ActionType type);

	virtual StatusContainer * statusContainer();

	virtual ContactSet contacts();
	virtual BuddySet buddies();
	virtual Chat chat();
	virtual bool hasContactSelected();

	virtual void configurationUpdated();

public:
	static void createDefaultToolbars(QDomElement parentConfig);

	explicit KaduWindow(QWidget *parent = 0);
	virtual ~KaduWindow();

	virtual BuddiesListView * buddiesListView();

	void insertMenuActionDescription(ActionDescription *actionDescription, MenuType Type, int pos = -1);
	void removeMenuActionDescription(ActionDescription *actionDescription);

	void setDocked(bool);
	bool docked() { return Docked; }

	ActionDataSource * actionSource();
	BuddyInfoPanel * infoPanel() { return InfoPanel; }

signals:
	void keyPressed(QKeyEvent *e);
	void parentChanged(QWidget *oldParent);

};

#endif // KADU_WINDOW_H
