/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <QtCore/QDateTime>
#include <QtCore/QObject>
#include <QtGui/QIcon>

#ifdef __sun__
#include <sys/types.h>
#endif /* __sun__ */

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
class ContactListService;
class ContactSet;
class FileTransferService;
class Message;
class PersonalInfoService;
class ProtocolFactory;
class SearchService;
class Status;

class KADUAPI Protocol : public QObject
{
	Q_OBJECT
	Q_DISABLE_COPY(Protocol)

public:
	enum NetworkState {
		NetworkDisconnected,
		NetworkConnecting,
		NetworkConnected,
		NetworkDisconnecting
	};

private:
	ProtocolFactory *Factory;

	Account CurrentAccount;

	NetworkState State;
	Status CurrentStatus;
	bool PrivateMode;

private slots:
	void statusChanged(StatusContainer *container, Status status);

protected:
	void setAllOffline();

	virtual void changeStatus() = 0;
	void statusChanged(Status newStatus);

	virtual void changePrivateMode() = 0;

	void networkStateChanged(NetworkState state);

public:
	Protocol(Account account, ProtocolFactory *factory);
	virtual ~Protocol();

	ProtocolFactory * protocolFactory() const { return Factory; }
	Account account() const { return CurrentAccount; }

	virtual AvatarService * avatarService() { return 0; }
	virtual ChatImageService * chatImageService() { return 0; }
	virtual ChatService * chatService() { return 0; }
	virtual ContactListService * contactListService() { return 0; }
	virtual FileTransferService * fileTransferService() { return 0; }
	virtual PersonalInfoService * personalInfoService() { return 0; }
	virtual SearchService * searchService() { return 0; }

	virtual bool validateUserID(const QString &uid) = 0;

	NetworkState state() { return State; }
	bool isConnected() { return (State == NetworkConnected); }

	void setStatus(Status status);
	Status status() const;
	Status nextStatus() const;
	virtual int maxDescriptionLength() { return -1; }

	void setPrivateMode(bool privateMode);
	bool privateMode() { return PrivateMode; }

	virtual QPixmap statusPixmap(Status status) = 0;
	virtual QPixmap statusPixmap(const QString &statusType) = 0;
	QPixmap statusPixmap() { return statusPixmap(CurrentStatus); }

	QIcon icon();

signals:
	void connecting(Account account);
	void connected(Account account);
	void disconnecting(Account account);
	void disconnected(Account account);

	void statusChanged(Account account, Status newStatus);
	void contactStatusChanged(Contact contact, Status oldStatus);

// TODO: REVIEW
	void connectionError(Account account, const QString &server, const QString &reason);

};

#endif // PROTOCOL_H
