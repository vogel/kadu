/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef KADU_WINDOW_H
#define KADU_WINDOW_H

#include <QtCore/QMap>
#include <QtCore/QPair>

#include "contacts/contact-list.h"

#include "kadu_main_window.h"

class QMenu;
class QMenuBar;
class QPushButton;
class QVBoxLayout;

class ActionDescription;
class ContactsListWidget;
class GroupTabBar;
class KaduAction;
class KaduTextBrowser;
class UserStatusChanger;

class KaduWindow : public KaduMainWindow
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
	typedef QPair<KaduAction *, MenuType> MenuAction;
	QMap<ActionDescription *, MenuAction> MenuActions;

	KaduTextBrowser *InfoPanel;
	QString InfoPanelSyntax;
	QMenuBar *MenuBar;
	QMenu *KaduMenu;
	QMenu *ContactsMenu;
	QMenu *HelpMenu;
	QMenu *RecentChatsMenu;
	GroupTabBar *GroupBar;

	ContactsListWidget *ContactsWidget;

	QWidget *MainWidget;
	QVBoxLayout *MainLayout;

	QMenu *StatusMenu;
	QPushButton *StatusButton;
	QPoint LastPositionBeforeStatusMenuHide;

	QString InfoPanelStyle;

	UserStatusChanger *StatusChanger;

	QActionGroup *ChangeStatusActionGroup;
	QAction *ChangeStatusToOnline;
	QAction *ChangeStatusToOnlineDesc;
	QAction *ChangeStatusToBusy;
	QAction *ChangeStatusToBusyDesc;
	QAction *ChangeStatusToInvisible;
	QAction *ChangeStatusToInvisibleDesc;
	QAction *ChangeStatusToOffline;
	QAction *ChangeStatusToOfflineDesc;
	QAction *ChangePrivateStatus;

	void createGui();
	void createMenu();
	void createStatusPopupMenu();

protected:
	virtual bool supportsActionType(ActionDescription::ActionType type) { return false; }
	virtual ContactsListWidget * contactsListWidget() { return 0; }
	virtual ContactList contacts() { return ContactList(); }

public:
	explicit KaduWindow(QWidget *parent = 0);
	virtual ~KaduWindow();

	void insertMenuActionDescription(ActionDescription *actionDescription, MenuType Type, int pos = -1);
	void removeMenuActionDescription(ActionDescription *actionDescription);

};

#endif // KADU_WINDOW_H
