/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtCore/QLocale>
#include <QtGui/QApplication>
#include <QtGui/QClipboard>
#include <QtGui/QInputDialog>

#include "accounts/account.h"
#include "accounts/account_manager.h"
#include "chat/chat_manager.h"
#include "contacts/contact-account-data.h"
#include "contacts/contact-kadu-data.h"
#include "contacts/contact-manager.h"
#include "contacts/group-manager.h"
#include "core/core.h"
#include "gui/widgets/contacts-list-widget-menu-manager.h"
#include "gui/windows/contact-data-window.h"
#include "gui/windows/kadu-window.h"
#include "misc/misc.h"
#include "about.h"
#include "action.h"
#include "config_file.h"
#include "debug.h"
#include "expimp.h"
#include "html_document.h"
#include "ignore.h"
#include "kadu_parser.h"
#include "main_configuration_window.h"
#include "message_box.h"
#include "personal_info.h"
#include "search.h"

#include "../modules/gadu_protocol/gadu-contact-account-data.h"

#include "kadu-window-actions.h"

void disableNonIdUles(KaduAction *action)
{
	kdebugf();
	foreach(const Contact contact, action->contacts())
		if (contact.accountData(AccountManager::instance()->defaultAccount()) == 0)
		{
			action->setEnabled(false);
			return;
		}

	action->setEnabled(true);
	kdebugf2();
}

void disableContainsSelfUles(KaduAction *action)
{
	if (action->contacts().contains(Core::instance()->myself()))
	{
		action->setEnabled(false);
		return;
	}

	action->setEnabled(true);
}

void checkOfflineTo(KaduAction *action)
{
	kdebugf();
	Account *account = AccountManager::instance()->defaultAccount();
	bool on = true;
	foreach(const Contact contact, action->contacts())
		if (contact.accountData(account) == 0 || !contact.isOfflineTo(account))
		{
			on = false;
			break;
		}
	action->setChecked(on);
	kdebugf2();
}

void checkHideDescription(KaduAction *action)
{
	Account *account = AccountManager::instance()->defaultAccount();

	foreach(const Contact contact, action->contacts())
		if (contact.accountData(account) == 0)
		{
			action->setEnabled(false);
			return;
		}
	action->setEnabled(true);

	bool on = false;
	foreach(const Contact contact, action->contacts())
	{
		ContactKaduData *ckd = contact.moduleData<ContactKaduData>(true);
		if (!ckd)
			continue;

		if (ckd->hideDescription())
		{
			on = true;
			break;
		}
	}

	action->setChecked(on);
}

void disableNotOneUles(KaduAction *action)
{
	kdebugf();

	if (action->contact().isNull())
	{
		action->setEnabled(false);
		return;
	}

	action->setEnabled(true);
	kdebugf2();
}

void disableNoGaduUle(KaduAction *action)
{
	kdebugf();

	Contact contact = action->contact();

	if (contact.isNull())
	{
		action->setEnabled(false);
		return;
	}

	if (!contact.accountData(AccountManager::instance()->defaultAccount()))
	{
		action->setEnabled(false);
		return;
	}

	action->setEnabled(true);
	kdebugf2();
}

void disableNoGaduDescription(KaduAction *action)
{
	kdebugf();

	Contact contact = action->contact();
	Account *account = AccountManager::instance()->defaultAccount();

	if (contact.isNull())
	{
		action->setEnabled(false);
		return;
	}

	if (!contact.accountData(account))
	{
		action->setEnabled(false);
		return;
	}

	if (contact.accountData(account)->status().description().isEmpty())
	{
		action->setEnabled(false);
		return;
	}

	action->setEnabled(true);
	kdebugf2();
}

void disableNoGaduDescriptionUrl(KaduAction *action)
{
	kdebugf();

	Account *account = AccountManager::instance()->defaultAccount();
	Contact contact = action->contact();

	if (contact.isNull())
	{
		action->setEnabled(false);
		return;
	}

	if (!contact.accountData(account))
	{
		action->setEnabled(false);
		return;
	}

	if (contact.accountData(account)->status().description().isEmpty())
	{
		action->setEnabled(false);
		return;
	}

	if (contact.accountData(account)->status().description().indexOf(HtmlDocument::urlRegExp()) < 0)
	{
		action->setEnabled(false);
		return;
	}

	action->setEnabled(true);
	kdebugf2();
}

void disableNoEMail(KaduAction *action)
{
	kdebugf();

	if (action->contacts().count() != 1)
	{
		action->setEnabled(false);
		return;
	}

	const ContactList contacts = action->contacts();

	if (contacts[0].email().isEmpty() || contacts[0].email().indexOf(HtmlDocument::mailRegExp()) < 0)
	{
		action->setEnabled(false);
		return;
	}

	action->setEnabled(true);
	kdebugf2();
}

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
	);

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

	WriteEmail = new ActionDescription(0,
		ActionDescription::TypeUser, "writeEmailAction",
		this, SLOT(writeEmailActionActivated(QAction *, bool)),
		"WriteEmail", tr("Write email message"), false, "",
		disableNoEMail
	);
	ContactsListWidgetMenuManager::instance()->addActionDescription(WriteEmail);

	CopyDescription = new ActionDescription(0,
		ActionDescription::TypeUser, "copyDescriptionAction",
		this, SLOT(copyDescriptionActionActivated(QAction *, bool)),
		"CopyDescription", tr("Copy description"), false, "",
		disableNoGaduDescription
	);
	ContactsListWidgetMenuManager::instance()->addActionDescription(CopyDescription);

	OpenDescriptionLink = new ActionDescription(0,
		ActionDescription::TypeUser, "openDescriptionLinkAction",
		this, SLOT(openDescriptionLinkActionActivated(QAction *, bool)),
		"OpenDescriptionLink", tr("Open description link in browser"), false, "",
		disableNoGaduDescriptionUrl
	);
	ContactsListWidgetMenuManager::instance()->addActionDescription(OpenDescriptionLink);

	CopyPersonalInfo = new ActionDescription(0,
		ActionDescription::TypeUser, "copyPersonalInfoAction",
		this, SLOT(copyPersonalInfoActionActivated(QAction *, bool)),
		"CopyPersonalInfo", tr("Copy personal info")
	);
	ContactsListWidgetMenuManager::instance()->addActionDescription(CopyPersonalInfo);

	LookupUserInfo = new ActionDescription(0,
		ActionDescription::TypeUser, "lookupUserInfoAction",
		this, SLOT(lookupInDirectoryActionActivated(QAction *, bool)),
		"LookupUserInfo", tr("Search in directory"), false, "",
		disableNoGaduUle
	);
	ContactsListWidgetMenuManager::instance()->addActionDescription(LookupUserInfo);

	ContactsListWidgetMenuManager::instance()->addSeparator();

	ContactsListWidgetMenuManager::instance()->addManagementActionDescription(chat_manager->ignoreUserActionDescription);
	ContactsListWidgetMenuManager::instance()->addManagementActionDescription(chat_manager->blockUserActionDescription);

	OfflineToUser = new ActionDescription(0,
		ActionDescription::TypeUser, "offlineToUserAction",
		this, SLOT(offlineToUserActionActivated(QAction *, bool)),
		"Offline", tr("Offline to user"), true, "",
		checkOfflineTo
	);
	ContactsListWidgetMenuManager::instance()->addManagementActionDescription(OfflineToUser);

	HideDescription = new ActionDescription(0,
		ActionDescription::TypeUser, "hideDescriptionAction",
		this, SLOT(hideDescriptionActionActivated(QAction *, bool)),
		"ShowDescription_off", tr("Hide description"), true, "",
		checkHideDescription
	);
	ContactsListWidgetMenuManager::instance()->addManagementActionDescription(HideDescription);

	ContactsListWidgetMenuManager::instance()->addManagementSeparator();

	DeleteUsers = new ActionDescription(0,
		ActionDescription::TypeUser, "deleteUsersAction",
		this, SLOT(deleteUsersActionActivated(QAction *, bool)),
		"RemoveFromUserlist", tr("Delete")
	);
	DeleteUsers->setShortcut("kadu_deleteuser");
	ContactsListWidgetMenuManager::instance()->addManagementActionDescription(DeleteUsers);
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
	bool ok;
	QString newGroupName = QInputDialog::getText(dynamic_cast<QWidget *>(sender->parent()), tr("New Group"),
				tr("Please enter the name for the new group:"), QLineEdit::Normal,
				QString::null, &ok);

	if (ok && !newGroupName.isEmpty() && GroupManager::instance()->acceptableGroupName(newGroupName))
		GroupManager::instance()->byName(newGroupName, true);
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
	(new ::About(Core::instance()->kaduWindow()))->show();
}

void KaduWindowActions::writeEmailActionActivated(QAction *sender, bool toggled)
{
	kdebugf();

	KaduMainWindow *window = dynamic_cast<KaduMainWindow *>(sender->parent());
	if (!window)
		return;

	ContactList contacts = window->contacts();
	if (contacts.count() < 1)
		return;

	Contact contact = contacts[0];
	if (!contact.email().isEmpty())
		openMailClient(contact.email());

	kdebugf2();
}

void KaduWindowActions::copyDescriptionActionActivated(QAction *sender, bool toggled)
{
	kdebugf();

	KaduMainWindow *window = dynamic_cast<KaduMainWindow *>(sender->parent());
	if (!window)
		return;

	Contact contact = window->contact();
	if (contact.isNull())
		return;

	Account *account = AccountManager::instance()->defaultAccount();
	ContactAccountData *data = contact.accountData(account);

	if (!data)
		return;

	QString description = data->status().description();
	if (description.isEmpty())
		return;

	QApplication::clipboard()->setText(description, QClipboard::Selection);
	QApplication::clipboard()->setText(description, QClipboard::Clipboard);

	kdebugf2();
}

void KaduWindowActions::openDescriptionLinkActionActivated(QAction *sender, bool toggled)
{
	kdebugf();

	KaduMainWindow *window = dynamic_cast<KaduMainWindow *>(sender->parent());
	if (!window)
		return;

	Contact contact = window->contact();
	if (contact.isNull())
		return;

	Account *account = AccountManager::instance()->defaultAccount();
	ContactAccountData *data = contact.accountData(account);

	if (!data)
		return;

	QString description = data->status().description();
	if (description.isEmpty())
		return;

	QRegExp url = HtmlDocument::urlRegExp();
	int idx_start = url.indexIn(description);
	if (idx_start >= 0)
		openWebBrowser(description.mid(idx_start, url.matchedLength()));

	kdebugf2();
}

void KaduWindowActions::copyPersonalInfoActionActivated(QAction *sender, bool toggled)
{
	kdebugf();

	KaduMainWindow *window = dynamic_cast<KaduMainWindow *>(sender->parent());
	if (!window)
		return;

	ContactList contacts = window->contacts();

	QStringList infoList;
	QString copyPersonalDataSyntax = config_file.readEntry("General", "CopyPersonalDataSyntax", tr("Contact: %a[ (%u)]\n[First name: %f\n][Last name: %r\n][Mobile: %m\n]"));
	foreach (Contact contact, contacts)
		infoList.append(KaduParser::parse(copyPersonalDataSyntax, contact.prefferedAccount(), contact, false));

	QString info = infoList.join("\n");
	if (info.isEmpty())
		return;

	QApplication::clipboard()->setText(info, QClipboard::Selection);
	QApplication::clipboard()->setText(info, QClipboard::Clipboard);

	kdebugf2();
}

void KaduWindowActions::lookupInDirectoryActionActivated(QAction *sender, bool toggled)
{
	kdebugf();

	KaduMainWindow *window = dynamic_cast<KaduMainWindow *>(sender->parent());
	if (!window)
		return;

	ContactList contacts = window->contacts();

	if (contacts.count() != 1)
	{
//		searchInDirectoryActionActivated(0, false); TODO: 0.6.6
		return;
	}

	Contact contact = contacts[0];
	GaduContactAccountData *cad = dynamic_cast<GaduContactAccountData *>(AccountManager::instance()->defaultAccount());
	if (!cad)
		return;

	SearchDialog *sd = new SearchDialog(Core::instance()->kaduWindow()/*, cad->uin()*/);
	sd->show();
	sd->firstSearch();

	kdebugf2();
}

void KaduWindowActions::offlineToUserActionActivated(QAction *sender, bool toggled)
{
	kdebugf();
	Account *account = AccountManager::instance()->defaultAccount();
	if (toggled && !config_file.readBoolEntry("General", "PrivateStatus"))
	{
// TODO: 0.6.6
// 		if (MessageBox::ask("You need to have private status to do it, would you like to set private status now?"))
// 			changePrivateStatus->setChecked(true);
// 		else
// 		{
// 			sender->setChecked(!toggled);
// 			return;
// 		}
	}

	KaduMainWindow *window = dynamic_cast<KaduMainWindow *>(sender->parent());
	if (!window)
		return;

	ContactList contacts = window->contacts();
	bool on = true;
	foreach (const Contact contact, contacts)
		if (contact.accountData(account) == 0 || !contact.isOfflineTo(account))
		{
			on = false;
			break;
		}
	/*
	foreach(const Contact contact, contacts)
		if (contact.accountData(account) != 0 || contact.isOfflineTo(account) == on)
			//TODO: 0.6.6
			user.setProtocolData("Gadu", "OfflineTo", !on); // TODO: here boolean
	*/
// TODO: 0.6.6
// 	userlist->writeToConfig();

	foreach (KaduAction *action, OfflineToUser->actions())
	{
		if (action->contacts() == contacts)
			action->setChecked(!on);
	}

	kdebugf2();
}

void KaduWindowActions::hideDescriptionActionActivated(QAction *sender, bool toggled)
{
	kdebugf();

	KaduMainWindow *window = dynamic_cast<KaduMainWindow *>(sender->parent());
	if (!window)
		return;

	ContactList contacts = window->contacts();

	foreach (const Contact &contact, contacts)
	{
		if (contact.isNull() || contact.isAnonymous())
			continue;

		ContactKaduData *ckd = contact.moduleData<ContactKaduData>(true);
		if (!ckd)
			continue;

		if (ckd->hideDescription() != toggled)
		{
			ckd->setHideDescription(toggled);
			ckd->storeConfiguration();
			delete ckd;
		}
	}

	foreach (KaduAction *action, HideDescription->actions())
	{
		if (action->contacts() == contacts)
			action->setChecked(toggled);
	}

	kdebugf2();
}

void KaduWindowActions::deleteUsersActionActivated(QAction *sender, bool toggled)
{
	kdebugf();

	KaduMainWindow *window = dynamic_cast<KaduMainWindow *>(sender->parent());
	if (!window)
		return;

	ContactList contacts = window->contacts();
	if (contacts.isEmpty())
		return;

	QStringList displays;
	foreach (Contact contact, contacts)
		displays.append(contact.display());
	if (MessageBox::ask(tr("Selected users:\n%0 will be deleted. Are you sure?").arg(displays.join(QString(', '))), "Warning", Core::instance()->kaduWindow()))
	{
		foreach (Contact contact, contacts)
			ContactManager::instance()->removeContact(contact);
		ContactManager::instance()->storeConfiguration(xml_config_file);
	}

	kdebugf2();
}
