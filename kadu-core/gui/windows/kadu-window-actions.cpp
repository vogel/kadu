/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "action.h"

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
		this, SLOT(yourAccounts(QAction *, bool)),
		"PersonalInfo", tr("Your accounts")
	);//TODO 0.6.6: implement

	HideKadu = new ActionDescription(this,
		ActionDescription::TypeMainMenu, "hideKaduAction",
		this, SLOT(hideKadu(QAction *, bool)),
		"HideKadu", tr("&Hide")
	);

	ExitKadu = new ActionDescription(this,
		ActionDescription::TypeMainMenu, "exitKaduAction",
		this, SLOT(quit()),
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
		this, SLOT(manageIgnored(QAction *, bool)),
		"Ignore", tr("&Ignored users")
	);

	ImportExportContacts = new ActionDescription(this,
		ActionDescription::TypeMainMenu, "importExportUserlisAction",
		this, SLOT(importExportUserlist(QAction *, bool)),
		"ImportExport", tr("I&mport / Export userlist")
	); //TODO 0.6.6: remove

	Help = new ActionDescription(this,
		ActionDescription::TypeMainMenu, "helpAction",
		this, SLOT(help(QAction *, bool)),
		"HelpMenuItem", tr("Getting H&elp")
	);

	Bugs = new ActionDescription(this,
		ActionDescription::TypeMainMenu, "bugsAction",
		this, SLOT(bugs(QAction *, bool)),
		"", tr("Submitt Bug Report")
	);

	Support = new ActionDescription(this,
		ActionDescription::TypeMainMenu, "supportAction",
		this, SLOT(support(QAction *, bool)),
		"", tr("Support us")
	);

	GetInvolved = new ActionDescription(this,
		ActionDescription::TypeMainMenu, "getInvolvedAction",
		this, SLOT(getInvolved(QAction *, bool)),
		"", tr("Get Involved")
	);

	About = new ActionDescription(this,
		ActionDescription::TypeMainMenu, "aboutAction",
		this, SLOT(about(QAction *, bool)),
		"AboutMenuItem", tr("A&bout Kadu")
	);
}

KaduWindowActions::~KaduWindowActions()
{

}
