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
class ContactInfoPanel;
class ContactsListWidget;
class GroupTabBar;
class KaduAction;
class KaduTextBrowser;
class KaduWindowActions;

class KaduWindow : public KaduMainWindow, private ConfigurationAwareObject
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

	bool Docked; // TODO: 0.7.1 it is a hack

	KaduWindowActions *Actions;

	ContactInfoPanel *InfoPanel;
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

	QMenu *StatusButtonMenu;
	QPushButton *StatusButton;
	QPoint LastPositionBeforeStatusMenuHide;

	QString InfoPanelStyle;

	void createGui();
	void createMenu();
	void createKaduMenu();
	void createContactsMenu();
	void createHelpMenu();
	void createStatusPopupMenu();

	void storeConfiguration();

	void updateInformationPanel();
	void updateInformationPanel(Contact contact);

private slots:
	void createRecentChatsMenu();
	void openRecentChats(QAction *action);

protected:
	virtual void closeEvent(QCloseEvent *);
	virtual void keyPressEvent(QKeyEvent *);

	virtual bool supportsActionType(ActionDescription::ActionType type);
	virtual ContactsListWidget * contactsListWidget();
	virtual ContactList contacts();

	virtual void configurationUpdated();

public:
	static void createDefaultToolbars(QDomElement parentConfig);

	explicit KaduWindow(QWidget *parent = 0);
	virtual ~KaduWindow();

	void insertMenuActionDescription(ActionDescription *actionDescription, MenuType Type, int pos = -1);
	void removeMenuActionDescription(ActionDescription *actionDescription);

	void setDocked(bool);
	bool docked() { return Docked; }

signals:
	void keyPressed(QKeyEvent *e);

};

#endif // KADU_WINDOW_H
