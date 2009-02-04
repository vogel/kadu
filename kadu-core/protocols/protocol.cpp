/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtGui/QIcon>
#include <QtGui/QTextDocument>

#include "contacts/contact-account-data.h"
#include "contacts/contact-manager.h"
#include "icons_manager.h"
#include "message.h"
#include "protocols/protocol_factory.h"
#include "protocols/status.h"

#include "protocol.h"

Protocol::Protocol(Account *account, ProtocolFactory *factory)
	: Factory(factory), CurrentAccount(account)
{
}

Protocol::~Protocol()
{
}

const QDateTime &Protocol::connectionTime() const
{
	return ConnectionTime;
}

bool Protocol::sendMessage(Contact user, const QString &messageContent)
{
	ContactList users;
	users.append(user);
	QTextDocument document(messageContent);
	Message message = Message::parse(&document);
	return sendMessage(users, message);
}

bool Protocol::sendMessage(ContactList users, const QString &messageContent)
{
	QTextDocument document(messageContent);
	Message message = Message::parse(&document);
	return sendMessage(users, message);
}

bool Protocol::sendMessage(Contact user, Message &message)
{
	ContactList users;
	users.append(user);
	return sendMessage(users, message);
}

QIcon Protocol::icon()
{
	QString iconName = Factory->iconName();
	return iconName.isEmpty()
		? QIcon()
		: icons_manager->loadIcon(iconName);
}


void Protocol::setAllOffline()
{
	Status status(Status::Offline);
	Status oldStatus;
	ContactAccountData *data;

	foreach (Contact contact, ContactManager::instance()->contacts(CurrentAccount, true))
	{
		data = contact.accountData(CurrentAccount);
		oldStatus = data->status();
		data->setStatus(status);
		emit contactStatusChanged(CurrentAccount, contact, oldStatus);
	}
}

void Protocol::setAccount(Account* account)
{
	CurrentAccount = account;
}

void Protocol::setStatus(Status status)
{
	NextStatus = status;
	changeStatus(status);
}

void Protocol::statusChanged(Status status)
{
	CurrentStatus = status;
	emit statusChanged(CurrentAccount, CurrentStatus);
}
