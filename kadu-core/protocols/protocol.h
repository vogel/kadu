/*
 * %kadu copyright begin%
 * Copyright 2007 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2008, 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2005, 2006, 2007 Marcin Ślusarz (joi@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2008, 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
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

#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <QtCore/QDateTime>
#include <QtCore/QObject>
#include <QtGui/QIcon>

#include "exports.h"

#include "buddies/buddy.h"
#include "buddies/buddy-list.h"
#include "chat/chat.h"
#include "status/status.h"

typedef quint32 UinType;

class QPixmap;

class AccountShared;
class AvatarService;
class ChatImageService;
class ChatService;
class ChatStateService;
class ContactListService;
class ContactPersonalInfoService;
class ContactSet;
class FileTransferService;
class Message;
class MultilogonService;
class PersonalInfoService;
class ProtocolFactory;
class ProtocolStateMachine;
class RosterService;
class SearchService;
class Status;

class KADUAPI Protocol : public QObject
{
	Q_OBJECT
	Q_DISABLE_COPY(Protocol)

	ProtocolFactory *Factory;
	ProtocolStateMachine *Machine;

	Account CurrentAccount;

	Status CurrentStatus;

	void setAllOffline();

private slots:
	void prepareStateMachine();
	void statusChanged(StatusContainer *container, Status status);
	void passwordRequired();

	void connectedSlot();
	void disconnectedSlot();

protected:
	void connectionError();
	void connectionClosed();

	virtual void disconnectedCleanup();
	void statusChanged(Status newStatus);

protected slots:
	virtual bool login();
	virtual void logout();
	virtual void wantToLogin();
	virtual void changeStatus() = 0;

public:
	Protocol(Account account, ProtocolFactory *factory);
	virtual ~Protocol();

	ProtocolFactory * protocolFactory() const { return Factory; }
	Account account() const { return CurrentAccount; }

	virtual AvatarService * avatarService() { return 0; }
	virtual ChatImageService * chatImageService() { return 0; }
	virtual ChatService * chatService() { return 0; }
	virtual ChatStateService * chatStateService() { return 0; }
	virtual ContactPersonalInfoService * contactPersonalInfoService() { return 0; }
	virtual ContactListService * contactListService() { return 0; }
	virtual FileTransferService * fileTransferService() { return 0; }
	virtual MultilogonService * multilogonService() { return 0; }
	virtual PersonalInfoService * personalInfoService() { return 0; }
	virtual RosterService * rosterService() { return 0; }
	virtual SearchService * searchService() { return 0; }
	virtual bool contactsListReadOnly() = 0;
	virtual bool supportsPrivateStatus() { return false; }

	bool isConnected();
	bool isConnecting();

	void setStatus(Status status);
	Status status() const;
	virtual int maxDescriptionLength() { return -1; }

	virtual void changePrivateMode() = 0;

	virtual QString statusPixmapPath() = 0;

	virtual QIcon statusIcon(Status status);
	virtual QString statusIconPath(const QString &statusType);
	virtual QString statusIconFullPath(const QString &statusType);
	virtual QIcon statusIcon(const QString &statusType);
	QIcon statusIcon() { return statusIcon(CurrentStatus); }

	QIcon icon();

	// TODO: workaround
	void emitContactStatusChanged(Contact contact, Status oldStatus)
	{
		emit contactStatusChanged(contact, oldStatus);
	}

public slots:
	void passwordProvided();

signals:
	void connecting(Account account);
	void connected(Account account);
	void disconnected(Account account);

	void statusChanged(Account account, Status newStatus);
	void contactStatusChanged(Contact contact, Status oldStatus);

// TODO: REVIEW
	void connectionError(Account account, const QString &server, const QString &reason);
	void invalidPassword(Account account);

// state machine signals
	void stateMachineLoggedIn();
	void stateMachineLoggedOut();

	void stateMachineChangeStatusToNotOffline();
	void stateMachineChangeStatusToOffline();

	void stateMachinePasswordRequired();
	void stateMachinePasswordAvailable();
	void stateMachinePasswordNotAvailable();

	void stateMachineConnectionError();
	void stateMachineConnectionClosed();

};

#endif // PROTOCOL_H
