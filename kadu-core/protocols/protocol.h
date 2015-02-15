/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2008, 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2010, 2011, 2012, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2008, 2009, 2010, 2011, 2012, 2013, 2014, 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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
#include <QtCore/QPointer>

#include "exports.h"

#include "accounts/account.h"
#include "chat/chat.h"
#include "status/status-change-source.h"
#include "status/status.h"

typedef quint32 UinType;

class QPixmap;

class AccountShared;
class AvatarService;
class BuddyListSerializationService;
class ChatImageService;
class ChatService;
class ChatStateService;
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
class SubscriptionService;
class KaduIcon;

class KADUAPI Protocol : public QObject
{
	Q_OBJECT
	Q_DISABLE_COPY(Protocol)

	ProtocolFactory *Factory;
	ProtocolStateMachine *Machine;

	Account CurrentAccount;

	// services
	QPointer<ChatService> CurrentChatService;
	QPointer<ChatStateService> CurrentChatStateService;
	QPointer<RosterService> CurrentRosterService;

	// real status, can be offline after connection error
	Status CurrentStatus;
	// status used by user to login, after connection error its value does not change
	// it can only by changed by user or status changer
	Status LoginStatus;

	void setAllOffline();

private slots:
	// state machine slots
	void prepareStateMachine();

	void loggingInStateEntered();
	void loggedInStateEntered();
	void loggingOutStateEntered();
	void loggedOutAnyStateEntered();
	void wantToLogInStateEntered();
	void passwordRequiredStateEntered();

protected:
	Status loginStatus() const;

	virtual void login() = 0;
	virtual void afterLoggedIn() {}
	virtual void logout() = 0;
	virtual void sendStatusToServer() = 0;

	virtual void disconnectedCleanup();
	void statusChanged(Status newStatus);

	void doSetStatus(Status status);

	// services
	void setChatService(ChatService * const chatService);
	void setChatStateService(ChatStateService * const chatStateService);
	void setRosterService(RosterService * const rosterService);

protected slots:
	void loggedIn();
	void loggedOut();
	void passwordRequired();
	void connectionError();
	void connectionClosed();
	void reconnect();

public:
	Protocol(Account account, ProtocolFactory *factory);
	virtual ~Protocol();

	ProtocolFactory * protocolFactory() const { return Factory; }
	Account account() const { return CurrentAccount; }

	virtual AvatarService * avatarService() { return 0; }
	virtual BuddyListSerializationService * buddyListSerializationService() { return nullptr; }
	virtual ChatImageService * chatImageService() { return 0; }
	virtual ChatService * chatService();
	virtual ChatStateService * chatStateService();
	virtual ContactPersonalInfoService * contactPersonalInfoService() { return 0; }
	virtual FileTransferService * fileTransferService() { return 0; }
	virtual MultilogonService * multilogonService() { return 0; }
	virtual PersonalInfoService * personalInfoService() { return 0; }
	virtual RosterService * rosterService() const;
	virtual SearchService * searchService() { return 0; }
	virtual SubscriptionService * subscriptionService() { return 0; }

	virtual bool contactsListReadOnly() = 0;
	virtual bool supportsPrivateStatus() { return false; }

	bool isConnected() const;
	bool isConnecting() const;
	bool isDisconnecting() const;

	// method called by user
	void setStatus(Status status, StatusChangeSource source);
	Status status() const;
	virtual int maxDescriptionLength() { return -1; }

	virtual void changePrivateMode() = 0;

	virtual QString statusPixmapPath() = 0;

	KaduIcon statusIcon();
	KaduIcon statusIcon(const Status &status);

	KaduIcon icon();

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

	void stateMachineChangeStatus();
	void stateMachineLogout();

	void stateMachinePasswordRequired();
	void stateMachinePasswordAvailable();
	void stateMachinePasswordNotAvailable();

	void stateMachineConnectionError();
	void stateMachineConnectionClosed();

};

#endif // PROTOCOL_H
