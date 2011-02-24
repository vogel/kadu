/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010 Tomasz Rostański (rozteck@interia.pl)
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

#ifndef GADU_PROTOCOL_H
#define GADU_PROTOCOL_H

#include <QtGui/QPixmap>
#include <QtNetwork/QHostAddress>

#include <libgadu.h>

#include "services/gadu-avatar-service.h"
#include "services/gadu-chat-image-service.h"
#include "services/gadu-chat-service.h"
#include "services/gadu-chat-state-service.h"
#include "services/gadu-contact-list-service.h"
#include "services/gadu-contact-personal-info-service.h"
#include "services/gadu-file-transfer-service.h"
#include "services/gadu-multilogon-service.h"
#include "services/gadu-personal-info-service.h"
#include "services/gadu-search-service.h"

#include "server/gadu-servers-manager.h"
#include "gadu-exports.h"
#include "gadu-features.h"
#include "gadu-search-record.h"
#include "gadu-search-result.h"

#include "protocols/protocol.h"

class DccManager;
class GaduContactDetails;
class GaduContactListHandler;
class GaduProtocolSocketNotifiers;

class GADUAPI GaduProtocol : public Protocol
{
	Q_OBJECT
	Q_DISABLE_COPY(GaduProtocol);

public:
	enum GaduError
	{
		ConnectionServerNotFound,
		ConnectionCannotConnect,
		ConnectionNeedEmail,
		ConnectionInvalidData,
		ConnectionCannotRead,
		ConnectionCannotWrite,
		ConnectionIncorrectPassword,
		ConnectionTlsError,
		ConnectionIntruderError,
		ConnectionUnavailableError,
		ConnectionUnknow,
		ConnectionTimeout,
		Disconnected
	};

	typedef unsigned int UinType;

private:
	GaduAvatarService *CurrentAvatarService;
	GaduChatImageService *CurrentChatImageService;
	GaduChatService *CurrentChatService;
	GaduContactListService *CurrentContactListService;
	GaduContactPersonalInfoService *CurrentContactPersonalInfoService;
	GaduFileTransferService *CurrentFileTransferService;
	GaduPersonalInfoService *CurrentPersonalInfoService;
	GaduSearchService *CurrentSearchService;
#ifdef GADU_HAVE_MULTILOGON
	GaduMultilogonService *CurrentMultilogonService;
#endif // GADU_HAVE_TYPING_NOTIFY
#ifdef GADU_HAVE_TYPING_NOTIFY
	GaduChatStateService *CurrentChatStateService;
#endif // GADU_HAVE_TYPING_NOTIFY

	GaduContactListHandler *ContactListHandler;

	GaduServersManager::GaduServer ActiveServer;

	struct gg_login_params GaduLoginParams;
	gg_session *GaduSession;

	friend class GaduProtocolSocketNotifiers;
	GaduProtocolSocketNotifiers *SocketNotifiers;

	QTimer *PingTimer;

	void setupProxy();
	void setupLoginParams();
	void cleanUpLoginParams();
	void cleanUpProxySettings();

	void setUpFileTransferService(bool forceClose = false);

	void networkConnected();
	void networkDisconnected(bool tryAgain, bool waitForPassword);

	void sendUserList();

	void socketContactStatusChanged(UinType uin, unsigned int status, const QString &description,
			const QHostAddress &ip, unsigned short port, unsigned int maxImageSize, unsigned int version);
	void socketConnFailed(GaduError error);
	void socketConnSuccess();
	void socketDisconnected();
	void changeStatus(bool force);

private slots:
	void accountUpdated();

	void login();

	void connectionTimeoutTimerSlot();
	void everyMinuteActions();

	void buddySubscriptionChanged(Buddy &buddy);
	void contactAttached(Contact contact);
	void contactAboutToBeDetached(Contact contact);
	void contactIdChanged(Contact contact, const QString &oldId);

protected:
	virtual void changeStatus();

public:
	static GADUAPI QString statusTypeFromGaduStatus(unsigned int index);
	static GADUAPI bool isBlockingStatus(unsigned int index);
	static GADUAPI unsigned int gaduStatusFromStatus(const Status &status);
	Buddy searchResultToBuddy(gg_pubdir50_t res, int number);

	GaduProtocol(Account account, ProtocolFactory *factory);
	virtual ~GaduProtocol();

	virtual AvatarService * avatarService() { return CurrentAvatarService; }
	virtual ChatImageService * chatImageService() { return CurrentChatImageService; }
	virtual ChatService * chatService() { return CurrentChatService; }
	virtual ContactListService * contactListService() { return CurrentContactListService; }
	virtual ContactPersonalInfoService * contactPersonalInfoService() { return CurrentContactPersonalInfoService; }
	virtual FileTransferService * fileTransferService() { return CurrentFileTransferService; }
	virtual PersonalInfoService * personalInfoService() { return CurrentPersonalInfoService; }
	virtual SearchService * searchService() { return CurrentSearchService; }
#ifdef GADU_HAVE_MULTILOGON
    virtual MultilogonService * multilogonService() { return CurrentMultilogonService; }
#endif // GADU_HAVE_MULTILOGON
#ifdef GADU_HAVE_TYPING_NOTIFY
	virtual GaduChatStateService * chatStateService()  { return CurrentChatStateService; }
#endif // GADU_HAVE_TYPING_NOTIFY
	virtual bool contactsListReadOnly() { return false; }
	virtual bool supportsPrivateStatus() { return true; }

	virtual void changePrivateMode();

	virtual QString statusPixmapPath();

	UinType uin(Contact contact) const;
	GaduContactDetails * gaduContactDetails(Contact contact) const;

	virtual int maxDescriptionLength();

	gg_session * gaduSession() { return GaduSession; }
	GaduProtocolSocketNotifiers * socketNotifiers() { return SocketNotifiers; }

public slots:
	virtual void login(const QString &password, bool permanent);

signals:
	/**
		Served sent information about status change for unknown user.
	**/
	void userStatusChangeIgnored(Buddy);

};

#endif // GADU_PROTOCOL_H
