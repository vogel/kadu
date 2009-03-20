/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtGui/QApplication>

#include "accounts/account_manager.h"
#include "contacts/contact-manager.h"
#include "contacts/group-manager.h"
#include "gui/windows/kadu-window.h"
#include "protocols/protocol.h"
#include "protocols/protocol_factory.h"
#include "protocols/services/chat-service.h"

#include "config_file.h"
#include "debug.h"
#include "kadu.h"
#include "pending_msgs.h"
#include "status_changer.h"

#include "core.h"

Core * Core::Instance = 0;

Core * Core::instance()
{
	if (!Instance)
		Instance = new Core();

	return Instance;
}

Core::Core() : Window(0)
{
	QObject::connect(qApp, SIGNAL(aboutToQuit()), this, SLOT(quit()));

	Myself.setDisplay(config_file.readEntry("General", "Nick"));

	StatusChangerManager::initModule();
	connect(status_changer_manager, SIGNAL(statusChanged(Status)), this, SLOT(changeStatus(Status)));

	StatusChanger = new UserStatusChanger();
	status_changer_manager->registerStatusChanger(StatusChanger);

	triggerAllAccountsRegistered();
}

Core::~Core()
{
	status_changer_manager->unregisterStatusChanger(StatusChanger);
	delete StatusChanger;
	StatusChanger = 0;

	StatusChangerManager::closeModule();

	delete Window;
	Window = 0;

	storeConfiguration();
	triggerAllAccountsUnregistered();
}

void Core::loadConfiguration()
{
	pending.loadConfiguration(xml_config_file);
}

void Core::storeConfiguration()
{
// TODO: 0.6.6
// 	if (config_file.readEntry("General", "StartupStatus") == "LastStatus")
// 		config_file.writeEntry("General", "LastStatusIndex", userStatusChanger->status().index());

	if (config_file.readBoolEntry("General", "StartupLastDescription"))
		config_file.writeEntry("General", "LastStatusDescription", StatusChanger->status().description());

	pending.storeConfiguration(xml_config_file);
// 		IgnoredManager::writeToConfiguration();

	GroupManager::instance()->storeConfiguration();
	ContactManager::instance()->storeConfiguration(xml_config_file);
	AccountManager::instance()->storeConfiguration(xml_config_file);

	Protocol *gadu = AccountManager::instance()->defaultAccount()->protocol();
	if (gadu->isConnected())
		if (config_file.readBoolEntry("General", "DisconnectWithCurrentDescription"))
			setOffline(gadu->status().description());
		else
			setOffline(config_file.readEntry("General", "DisconnectDescription"));

	xml_config_file->makeBackup();
}

void Core::changeStatus(Status newStatus)
{
	kdebugf();

	if (NextStatus.isOffline())
	{
// TODO: 0.6.6
// 		changeStatusToOfflineDesc->setEnabled(false);
// 		changeStatusToOffline->setChecked(true);
	}

	Account *account = AccountManager::instance()->defaultAccount();
	if (!account)
		return;

	Protocol *gadu = account->protocol();
	if (!gadu)
		return;

	if (gadu->nextStatus() == newStatus)
		return;

	NextStatus = newStatus;
	gadu->setStatus(newStatus);
}

void Core::kaduWindowDestroyed()
{
	Window = 0;
}

void Core::accountRegistered(Account *account)
{
	Protocol *protocol = account->protocol();

	ChatService *chatService = protocol->chatService();
	if (chatService)
		connect(chatService, SIGNAL(messageReceived(Account *, Contact, ContactList, const QString &, time_t)),
			this, SIGNAL(messageReceived(Account *, Contact, ContactList, const QString &, time_t)));

	connect(protocol, SIGNAL(connecting(Account *)), this, SIGNAL(connecting()));
	connect(protocol, SIGNAL(connected(Account *)), this, SIGNAL(connected()));
	connect(protocol, SIGNAL(disconnected(Account *)), this, SIGNAL(disconnected()));
	connect(protocol, SIGNAL(statusChanged(Account *, Status)),
			this, SLOT(statusChanged(Account *, Status)));

	ContactAccountData *contactAccountData = protocol->protocolFactory()->
			newContactAccountData(Myself, account, account->id());
	Myself.addAccountData(contactAccountData);
}

void Core::accountUnregistered(Account *account)
{
	Protocol *protocol = account->protocol();

	ChatService *chatService = protocol->chatService();
	if (chatService)
		disconnect(chatService, SIGNAL(messageReceived(Account *, Contact, ContactList, const QString &, time_t)),
			this, SIGNAL(messageReceived(Account *, Contact, ContactList, const QString &, time_t)));

	disconnect(protocol, SIGNAL(connecting(Account *)), this, SIGNAL(connecting()));
	disconnect(protocol, SIGNAL(connected(Account *)), this, SIGNAL(connected()));
	disconnect(protocol, SIGNAL(disconnected(Account *)), this, SIGNAL(disconnected()));
	disconnect(protocol, SIGNAL(statusChanged(Account *, Status)),
			this, SLOT(statusChanged(Account *, Status)));

	Myself.removeAccountData(account);
}

void Core::createGui()
{
	new Kadu(0);

	Window = new KaduWindow(0);
	connect(Window, SIGNAL(destroyed(QObject *)), this, SLOT(kaduWindowDestroyed()));

	Window->show();
}

KaduWindow * Core::kaduWindow()
{
	return Window;
}

void Core::setStatus(const Status &status)
{
	StatusChanger->userStatusSet(status);
}

void Core::setOnline(const QString &description)
{
	StatusChanger->userStatusSet(Status(Status::Online, description));
}

void Core::setBusy(const QString &description)
{
	StatusChanger->userStatusSet(Status(Status::Busy, description));
}

void Core::setInvisible(const QString &description)
{
	StatusChanger->userStatusSet(Status(Status::Invisible, description));
}

void Core::setOffline(const QString &description)
{
	StatusChanger->userStatusSet(Status(Status::Offline, description));
}

void Core::quit()
{
	if (!Instance)
		return;

	delete Instance;
	Instance = 0;
}
