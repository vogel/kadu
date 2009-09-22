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

#include "chat/chat.h"
#include "contacts/contact-list.h"

#include "gui/windows/main-window.h"

class QMenu;
class QMenuBar;
class QPushButton;
class QVBoxLayout;

class Action;
class ActionDescription;
class ContactInfoPanel;
class ContactsListWidget;
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

	ContactInfoPanel *InfoPanel;
	QMenuBar *MenuBar;
	QMenu *KaduMenu;
	QMenu *ContactsMenu;
	QMenu *HelpMenu;
	QMenu *RecentChatsMenu;
	QAction *RecentChatsMenuAction;
	GroupTabBar *GroupBar;

	ContactsListWidget *ContactsWidget;

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
	void updateInformationPanel(Contact contact);

private slots:
	void openChatWindow(Chat *chat);

	void createRecentChatsMenu();
	void openRecentChats(QAction *action);

protected:
	virtual void closeEvent(QCloseEvent *);
	virtual void customEvent(QEvent *);
	virtual void keyPressEvent(QKeyEvent *);

	virtual bool supportsActionType(ActionDescription::ActionType type);
	virtual ContactsListWidget * contactsListWidget();
	virtual ContactSet contacts();
	virtual Chat * chat();

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

	ContactInfoPanel *infoPanel() { return InfoPanel; }

signals:
	void keyPressed(QKeyEvent *e);

};

#endif // KADU_WINDOW_H
