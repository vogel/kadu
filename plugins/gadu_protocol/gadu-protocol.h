/*
 * %kadu copyright begin%
 * Copyright 2008, 2009, 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010, 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009, 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2003, 2004, 2005 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2005 Paweł Płuciennik (pawel_p@kadu.net)
 * Copyright 2004 Tomasz Chiliński (chilek@chilan.com)
 * Copyright 2007, 2008, 2009, 2009, 2010, 2011, 2012, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011, 2012 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2006, 2007 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2004, 2005, 2006, 2007 Marcin Ślusarz (joi@kadu.net)
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
#include "gadu-search-record.h"
#include "gadu-search-result.h"

#include "protocols/protocol.h"

class DccManager;
class GaduContactDetails;
class GaduContactListHandler;
class GaduProtocolSocketNotifiers;
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
	GaduChatImageService *CurrentChatImageService;
	GaduChatService *CurrentChatService;
	GaduContactListService *CurrentContactListService;
	GaduContactPersonalInfoService *CurrentContactPersonalInfoService;
	GaduFileTransferService *CurrentFileTransferService;
	GaduPersonalInfoService *CurrentPersonalInfoService;
	GaduSearchService *CurrentSearchService;
	GaduMultilogonService *CurrentMultilogonService;
	GaduChatStateService *CurrentChatStateService;

	GaduServersManager::GaduServer ActiveServer;

	struct gg_login_params GaduLoginParams;
	gg_session *GaduSession;

	friend class GaduProtocolSocketNotifiers;
	GaduProtocolSocketNotifiers *SocketNotifiers;

	QTimer *PingTimer;

	void setupLoginParams();
	void cleanUpLoginParams();

	void connectSocketNotifiersToServices();

	void startFileTransferService();
	void stopFileTransferService();

	void setUpFileTransferService(bool forceClose = false);

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
	virtual ChatImageService * chatImageService() { return CurrentChatImageService; }
	virtual ContactListService * contactListService() { return CurrentContactListService; }
	virtual ContactPersonalInfoService * contactPersonalInfoService() { return CurrentContactPersonalInfoService; }
	virtual FileTransferService * fileTransferService() { return CurrentFileTransferService; }
	virtual PersonalInfoService * personalInfoService() { return CurrentPersonalInfoService; }
	virtual SearchService * searchService() { return CurrentSearchService; }
	virtual MultilogonService * multilogonService() { return CurrentMultilogonService; }

	virtual bool contactsListReadOnly() { return false; }
	virtual bool supportsPrivateStatus() { return true; }

	virtual void changePrivateMode();

	virtual QString statusPixmapPath();

	virtual int maxDescriptionLength();

	void enableSocketNotifiers();
	void disableSocketNotifiers();
	gg_session * gaduSession() { return GaduSession; }

signals:
	/**
		Served sent information about status change for unknown user.
	**/
	void userStatusChangeIgnored(Buddy);

};

#endif // GADU_PROTOCOL_H
