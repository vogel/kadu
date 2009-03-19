/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtGui/QApplication>

#include "accounts/account.h"
#include "accounts/account_manager.h"
#include "gui/windows/add-group-window.h"
#include "gui/windows/contact-data-window.h"
#include "misc/misc.h"
#include "about.h"
#include "action.h"
#include "debug.h"
#include "expimp.h"
#include "ignore.h"
#include "main_configuration_window.h"
#include "personal_info.h"
#include "search.h"

#include "kadu-window-actions.h"

KaduWindowActions::KaduWindowActions(QObject *parent) : QObject(parent)
{
	Configuration = new ActionDescription(this,
		ActionDescription::TypeGlobal, "configurationAction",
		this, SLOT(configurationActionActivated(QAction *, bool)),
		"Configuration", tr("Configuration")
	);
	Configuration->setShortcut("kadu_configure", Qt::ApplicationShortcut);

	YourAccounts = new ActionDescription(this,
		ActionDescription::TypeMainMenu, "yourAccountsAction",
		this, SLOT(yourAccountsActionActivated(QAction *, bool)),
		"PersonalInfo", tr("Your accounts")
	);//TODO 0.6.6: implement

	HideKadu = new ActionDescription(this,
		ActionDescription::TypeMainMenu, "hideKaduAction",
		this, SLOT(hideKaduActionActivated(QAction *, bool)),
		"HideKadu", tr("&Hide")
	);

	ExitKadu = new ActionDescription(this,
		ActionDescription::TypeMainMenu, "exitKaduAction",
		this, SLOT(exitKaduActionActivated(QAction *, bool)),
		"Exit", tr("&Exit")
	);

	AddUser = new ActionDescription(this,
		ActionDescription::TypeGlobal, "addUserAction",
		this, SLOT(addUserActionActivated(QAction *, bool)),
		"AddUser", tr("Add user")
	);
	AddUser->setShortcut("kadu_adduser", Qt::ApplicationShortcut);

	AddGroup = new ActionDescription(this,
		ActionDescription::TypeMainMenu, "addGroupAction",
		this, SLOT(addGroupActionActivated(QAction *, bool)),
		"", tr("Add Group")
	);//TODO 0.6.6: implement and update icons

	OpenSearch = new ActionDescription(this,
		ActionDescription::TypeGlobal, "openSearchAction",
		this, SLOT(searchInDirectoryActionActivated(QAction *, bool)),
		"LookupUserInfo", tr("Search user in directory")
	);

	ManageIgnored = new ActionDescription(this,
		ActionDescription::TypeMainMenu, "manageIgnoredAction",
		this, SLOT(manageIgnoredActionActivated(QAction *, bool)),
		"Ignore", tr("&Ignored users")
	);

	ImportExportContacts = new ActionDescription(this,
		ActionDescription::TypeMainMenu, "importExportUserlisAction",
		this, SLOT(importExportContactsActionActivated(QAction *, bool)),
		"ImportExport", tr("I&mport / Export userlist")
	); //TODO 0.6.6: remove

	Help = new ActionDescription(this,
		ActionDescription::TypeMainMenu, "helpAction",
		this, SLOT(helpActionActivated(QAction *, bool)),
		"HelpMenuItem", tr("Getting H&elp")
	);

	Bugs = new ActionDescription(this,
		ActionDescription::TypeMainMenu, "bugsAction",
		this, SLOT(bugsActionActivated(QAction *, bool)),
		"", tr("Submitt Bug Report")
	);

	Support = new ActionDescription(this,
		ActionDescription::TypeMainMenu, "supportAction",
		this, SLOT(supportActionActivated(QAction *, bool)),
		"", tr("Support us")
	);

	GetInvolved = new ActionDescription(this,
		ActionDescription::TypeMainMenu, "getInvolvedAction",
		this, SLOT(getInvolvedActionActivated(QAction *, bool)),
		"", tr("Get Involved")
	);

	About = new ActionDescription(this,
		ActionDescription::TypeMainMenu, "aboutAction",
		this, SLOT(aboutActionActivated(QAction *, bool)),
		"AboutMenuItem", tr("A&bout Kadu")
	);
}

KaduWindowActions::~KaduWindowActions()
{
}

void KaduWindowActions::configurationActionActivated(QAction *sender, bool toggled)
{
	MainConfigurationWindow::instance()->show();
}

void KaduWindowActions::yourAccountsActionActivated(QAction *sender, bool toggled)
{
	(new PersonalInfoDialog(AccountManager::instance()->defaultAccount()->protocol(), dynamic_cast<QWidget *>(parent())))->show();
}

void KaduWindowActions::hideKaduActionActivated(QAction *sender, bool toggled)
{
	// TODO: 0.6.6
	//if (Docked)
		//if (dontHideOnClose)
			dynamic_cast<QWidget *>(parent())->showMinimized();
		//else
			//dynamic_cast<QWidget *>(parent())->close();
}

void KaduWindowActions::exitKaduActionActivated(QAction *sender, bool toggled)
{
	kdebugf();

	// TODO: 0.6.6
	//if (measureTime)
	//{
	//	time_t sec;
	//	int msec;
	//	getTime(&sec, &msec);
	//	endingTime = (sec % 1000) * 1000 + msec;
	//}
	qApp->quit();
}

void KaduWindowActions::addUserActionActivated(QAction *sender, bool toggled)
{
	kdebugf();

	KaduMainWindow *window = dynamic_cast<KaduMainWindow *>(sender->parent());
	if (!window)
		return;

	Contact contact = window->contact();
	if (contact.isAnonymous() || contact.isNull())
		(new ContactDataWindow(contact, window))->show();

 	kdebugf2();
}

void KaduWindowActions::addGroupActionActivated(QAction *sender, bool toggled)
{
	(new AddGroupWindow(dynamic_cast<QWidget *>(sender->parent())))->show();
}

void KaduWindowActions::searchInDirectoryActionActivated(QAction *sender, bool toggled)
{
	(new SearchDialog(dynamic_cast<QWidget *>(sender->parent())))->show();
}

void KaduWindowActions::manageIgnoredActionActivated(QAction *sender, bool toggled)
{
	(new Ignored(dynamic_cast<QWidget *>(sender->parent())))->show();
}

void KaduWindowActions::importExportContactsActionActivated(QAction *sender, bool toggled)
{
	(new UserlistImportExport(dynamic_cast<QWidget *>(sender->parent())))->show();
}

void KaduWindowActions::helpActionActivated(QAction *sender, bool toggled)
{
	if (config_file.readEntry("General", "Language", QString(qApp->keyboardInputLocale().name()).mid(0,2)) == "pl")
		openWebBrowser("http://www.kadu.net/w/Pomoc_online");
	else
		openWebBrowser("http://www.kadu.net/w/English:Kadu:Help_online");
}

void KaduWindowActions::bugsActionActivated(QAction *sender, bool toggled)
{
	if (config_file.readEntry("General", "Language", QString(qApp->keyboardInputLocale().name()).mid(0,2)) == "pl")
		openWebBrowser("http://www.kadu.net/w/B%C5%82%C4%99dy");
	else
		openWebBrowser("http://www.kadu.net/w/English:Bugs");
}

void KaduWindowActions::supportActionActivated(QAction *sender, bool toggled)
{
	if (config_file.readEntry("General", "Language", QString(qApp->keyboardInputLocale().name()).mid(0,2)) == "pl")
		openWebBrowser("http://www.kadu.net/w/Kadu:Site_support");
	else
		openWebBrowser("http://www.kadu.net/w/English:Kadu:Site_support");
}

void KaduWindowActions::getInvolvedActionActivated(QAction *sender, bool toggled)
{
	if (config_file.readEntry("General", "Language", QString(qApp->keyboardInputLocale().name()).mid(0,2)) == "pl")
		openWebBrowser("http://www.kadu.net/w/Do%C5%82%C4%85cz");
	else
		openWebBrowser("http://www.kadu.net/w/English:GetInvolved");
}

void KaduWindowActions::aboutActionActivated(QAction *sender, bool toggled)
{
	(new ::About(kadu))->show();
}
