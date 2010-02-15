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

class QMenu;
class QMenuBar;
class QPushButton;
class QVBoxLayout;

class Action;
class ActionDescription;
class BuddyInfoPanel;
class BuddiesListWidget;
class GroupTabBar;
class KaduTextBrowser;
class KaduWindowActions;
class StatusButtons;

class KADUAPI KaduWindow : public MainWindow, private ConfigurationAwareObject
{
	Q_OBJECT

public:
	enum MenuType
	{
		MenuKadu,
		MenuContacts,
		MenuHelp
	};

private:
	typedef QPair<Action *, MenuType> MenuAction;
	QMap<ActionDescription *, MenuAction> MenuActions;

	bool Docked; // TODO: 0.7.1 it is a hack

	KaduWindowActions *Actions;

	BuddyInfoPanel *InfoPanel;
	QMenuBar *MenuBar;
	QMenu *KaduMenu;
	QMenu *ContactsMenu;
	QMenu *HelpMenu;
	QMenu *RecentChatsMenu;
	QAction *RecentChatsMenuAction;
	GroupTabBar *GroupBar;

	BuddiesListWidget *ContactsWidget;

	QWidget *MainWidget;
	QVBoxLayout *MainLayout;

	QMenu *StatusButtonMenu;
	StatusButtons *ChangeStatusButtons;
	QPoint LastPositionBeforeStatusMenuHide;

	void createGui();
	void createMenu();
	void createKaduMenu();
	void createContactsMenu();
	void createHelpMenu();

	void storeConfiguration();

	void updateInformationPanel();
	void updateInformationPanel(Buddy buddy);

private slots:
	void openChatWindow(Chat chat);

	void createRecentChatsMenu();
	void openRecentChats(QAction *action);

protected:
	virtual void closeEvent(QCloseEvent *);
	virtual void customEvent(QEvent *);
	virtual void keyPressEvent(QKeyEvent *);

	virtual bool supportsActionType(ActionDescription::ActionType type);

	virtual BuddiesListView * contactsListView();
	virtual StatusContainer * statusContainer();

	virtual ContactSet contacts();
	virtual BuddySet buddies();
	virtual Chat chat();

	virtual void configurationUpdated();

public:
	static void createDefaultToolbars(QDomElement parentConfig);
	static void addAction(const QString &actionName, Qt::ToolButtonStyle style = Qt::ToolButtonIconOnly);

	explicit KaduWindow(QWidget *parent = 0);
	virtual ~KaduWindow();

	void insertMenuActionDescription(ActionDescription *actionDescription, MenuType Type, int pos = -1);
	void removeMenuActionDescription(ActionDescription *actionDescription);

	void setDocked(bool);
	bool docked() { return Docked; }

	BuddyInfoPanel * infoPanel() { return InfoPanel; }

signals:
	void keyPressed(QKeyEvent *e);

};

#endif // KADU_WINDOW_H
