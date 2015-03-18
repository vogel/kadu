/*
 * %kadu copyright begin%
 * Copyright 2011, 2012 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2012, 2013, 2014, 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "configuration/configuration-aware-object.h"

#include "services/gadu-avatar-service.h"
#include "services/gadu-buddy-list-serialization-service.h"
#include "services/gadu-chat-image-service.h"
#include "services/gadu-chat-service.h"
#include "services/gadu-chat-state-service.h"
#include "services/gadu-contact-personal-info-service.h"
#include "services/gadu-file-transfer-service.h"
#include "services/gadu-multilogon-service.h"
#include "services/gadu-personal-info-service.h"
#include "services/gadu-search-service.h"

#include "server/gadu-servers-manager.h"
#include "gadu-exports.h"
#include "gadu-search-record.h"
#include "gadu-search-result.h"

#include "protocols/protocol.h"

class GaduContactListHandler;
class GaduDriveService;
class GaduIMTokenService;
class GaduNotifyService;
class GaduProtocolSocketNotifiers;
class GaduUserDataService;
class ProtocolGaduConnection;

class GADUAPI GaduProtocol : public Protocol, public ConfigurationAwareObject
{
	Q_OBJECT
	Q_DISABLE_COPY(GaduProtocol)

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
	ProtocolGaduConnection *Connection;

	GaduAvatarService *CurrentAvatarService;
	GaduBuddyListSerializationService *CurrentBuddyListSerializationService;
	GaduChatImageService *CurrentChatImageService;
	GaduChatService *CurrentChatService;
	GaduContactPersonalInfoService *CurrentContactPersonalInfoService;
	GaduFileTransferService *CurrentFileTransferService;
	GaduPersonalInfoService *CurrentPersonalInfoService;
	GaduSearchService *CurrentSearchService;
	GaduMultilogonService *CurrentMultilogonService;
	GaduChatStateService *CurrentChatStateService;
	GaduNotifyService *CurrentNotifyService;
	GaduIMTokenService *CurrentImTokenService;
	GaduDriveService *CurrentDriveService;
	GaduUserDataService *CurrentUserDataService;

	GaduServersManager::GaduServer ActiveServer;

	struct gg_login_params GaduLoginParams;
	gg_session *GaduSession;

	friend class GaduProtocolSocketNotifiers;
	GaduProtocolSocketNotifiers *SocketNotifiers;

	QTimer *PingTimer;
	bool SecureConnection;

	void setupLoginParams();
	void cleanUpLoginParams();

	void connectSocketNotifiersToServices();

	void startFileTransferService();
	void stopFileTransferService();

	void socketContactStatusChanged(UinType uin, unsigned int status, const QString &description, unsigned int maxImageSize);
	void socketConnFailed(GaduError error);
	void connectedToServer();
	void disconnectedFromServer();

	void setStatusFlags();
	void configureServices();

private slots:
	void accountUpdated();

	void connectionTimeoutTimerSlot();
	void everyMinuteActions();

protected:
	virtual void login();
	virtual void afterLoggedIn();
	virtual void logout();
	virtual void sendStatusToServer();

	virtual void disconnectedCleanup();

	virtual void configurationUpdated();

public:
	GaduProtocol(Account account, ProtocolFactory *factory);
	virtual ~GaduProtocol();

	virtual AvatarService * avatarService() { return CurrentAvatarService; }
	virtual BuddyListSerializationService * buddyListSerializationService() { return CurrentBuddyListSerializationService; }
	virtual ChatImageService * chatImageService() { return CurrentChatImageService; }
	virtual ContactPersonalInfoService * contactPersonalInfoService() { return CurrentContactPersonalInfoService; }
	virtual FileTransferService * fileTransferService() { return CurrentFileTransferService; }
	virtual PersonalInfoService * personalInfoService() { return CurrentPersonalInfoService; }
	virtual SearchService * searchService() { return CurrentSearchService; }
	virtual MultilogonService * multilogonService() { return CurrentMultilogonService; }

	GaduIMTokenService * gaduIMTokenService() const { return CurrentImTokenService; }

	virtual bool contactsListReadOnly() { return false; }
	virtual bool supportsPrivateStatus() { return true; }

	virtual void changePrivateMode();

	virtual QString statusPixmapPath();

	virtual int maxDescriptionLength();

	bool secureConnection() const;

	void enableSocketNotifiers();
	void disableSocketNotifiers();
	gg_session * gaduSession() { return GaduSession; }

	GaduDriveService * driveService() const;
	GaduUserDataService * userDataService() const;

signals:
	/**
		Served sent information about status change for unknown user.
	**/
	void userStatusChangeIgnored(Buddy);

};

#endif // GADU_PROTOCOL_H
