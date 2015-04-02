/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2011, 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtCore/QTimer>

#include <libgadu.h>

#ifdef Q_OS_WIN
#include <winsock2.h>
#else
#include <arpa/inet.h>
#endif

#include "accounts/account-manager.h"
#include "accounts/account.h"
#include "avatars/avatar-manager.h"
#include "buddies/buddy-manager.h"
#include "chat/chat-manager.h"
#include "chat/chat.h"
#include "configuration/configuration.h"
#include "configuration/deprecated-configuration-api.h"
#include "contacts/contact-manager.h"
#include "core/core.h"
#include "formatted-string/composite-formatted-string.h"
#include "gui/windows/message-dialog.h"
#include "network/proxy/network-proxy-manager.h"
#include "qt/long-validator.h"
#include "status/status-type-manager.h"
#include "status/status-type.h"
#include "status/status.h"

#include "core/application.h"
#include "icons/icons-manager.h"
#include "misc/misc.h"
#include "debug.h"

#include "server/gadu-servers-manager.h"
#include "server/protocol-gadu-connection.h"
#include "socket-notifiers/gadu-protocol-socket-notifiers.h"

#include "helpers/gadu-importer.h"
#include "helpers/gadu-protocol-helper.h"
#include "helpers/gadu-proxy-helper.h"
#include "server/gadu-writable-session-token.h"
#include "services/drive/gadu-drive-service.h"
#include "services/gadu-imtoken-service.h"
#include "services/gadu-notify-service.h"
#include "services/gadu-roster-service.h"
#include "services/user-data/gadu-user-data-service.h"
#include "gadu-account-details.h"

#include "gadu-protocol.h"

GaduProtocol::GaduProtocol(Account account, ProtocolFactory *factory) :
		Protocol(account, factory),
		ActiveServer(), GaduLoginParams(), GaduSession(0), SocketNotifiers(0), PingTimer(0),
		SecureConnection{false}
{
	Connection = new ProtocolGaduConnection(this);
	Connection->setConnectionProtocol(this);

	CurrentAvatarService = new GaduAvatarService(account, this);

	CurrentBuddyListSerializationService = new GaduBuddyListSerializationService{account, this};

	CurrentChatImageService = new GaduChatImageService(account, this);
	CurrentChatImageService->setConnection(Connection);

	CurrentImTokenService = new GaduIMTokenService{this};

	CurrentFileTransferService = new GaduFileTransferService{this};
	CurrentFileTransferService->setGaduIMTokenService(CurrentImTokenService);

	CurrentChatService = new GaduChatService(account, this);
	CurrentChatService->setConnection(Connection);
	CurrentChatService->setFormattedStringFactory(Core::instance()->formattedStringFactory());
	CurrentChatService->setGaduChatImageService(CurrentChatImageService);
	CurrentChatService->setGaduFileTransferService(CurrentFileTransferService);
	CurrentChatService->setImageStorageService(Core::instance()->imageStorageService());
	CurrentChatService->setRawMessageTransformerService(Core::instance()->rawMessageTransformerService());
	CurrentChatImageService->setGaduChatService(CurrentChatService);

	CurrentContactPersonalInfoService = new GaduContactPersonalInfoService(account, this);
	CurrentContactPersonalInfoService->setConnection(Connection);

	CurrentPersonalInfoService = new GaduPersonalInfoService(account, this);
	CurrentPersonalInfoService->setConnection(Connection);

	CurrentSearchService = new GaduSearchService(account, this);
	CurrentSearchService->setConnection(Connection);

	CurrentMultilogonService = new GaduMultilogonService(account, this);
	CurrentMultilogonService->setConnection(Connection);

	CurrentChatStateService = new GaduChatStateService(account, this);
	CurrentChatStateService->setConnection(Connection);

	connect(CurrentChatService, SIGNAL(messageReceived(Message)),
	        CurrentChatStateService, SLOT(messageReceived(Message)));

	CurrentDriveService = new GaduDriveService{account, this};
	CurrentDriveService->setGaduIMTokenService(CurrentImTokenService);

	CurrentUserDataService = new GaduUserDataService{account, this};
	CurrentUserDataService->setAvatarManager(AvatarManager::instance());
	CurrentUserDataService->setContactManager(ContactManager::instance());

	auto contacts = ContactManager::instance()->contacts(account, ContactManager::ExcludeAnonymous);
	auto rosterService = new GaduRosterService{contacts, this};
	rosterService->setConnection(Connection);
	rosterService->setRosterNotifier(Core::instance()->rosterNotifier());
	rosterService->setRosterReplacer(Core::instance()->rosterReplacer());

	CurrentNotifyService = new GaduNotifyService{Connection, this};
	connect(rosterService, SIGNAL(contactAdded(Contact)), CurrentNotifyService, SLOT(contactAdded(Contact)));
	connect(rosterService, SIGNAL(contactRemoved(Contact)), CurrentNotifyService, SLOT(contactRemoved(Contact)));
	connect(rosterService, SIGNAL(contactUpdatedLocally(Contact)), CurrentNotifyService, SLOT(contactUpdatedLocally(Contact)));

	setChatService(CurrentChatService);
	setChatStateService(CurrentChatStateService);
	setRosterService(rosterService);

	configureServices();

	connect(account, SIGNAL(updated()), this, SLOT(accountUpdated()));

	kdebugf2();
}

GaduProtocol::~GaduProtocol()
{
	kdebugf();

	disconnect(account(), 0, this, 0);

	kdebugf2();
}

int GaduProtocol::maxDescriptionLength()
{
	return GG_STATUS_DESCR_MAXSIZE;
}

void GaduProtocol::setStatusFlags()
{
	if (!GaduSession)
		return;

	GaduAccountDetails *details = static_cast<GaduAccountDetails *>(account().details());

	int statusFlags = GG_STATUS_FLAG_UNKNOWN;
	if (details && !details->receiveSpam())
		statusFlags = statusFlags | GG_STATUS_FLAG_SPAM;

	gg_change_status_flags(GaduSession, GG_STATUS_FLAG_UNKNOWN | statusFlags);
}

void GaduProtocol::sendStatusToServer()
{
	if (!isConnected() && !isDisconnecting())
		return;

	if (!GaduSession)
		return;

	// some services have per-status configuration
	configureServices();

	Status newStatus = status();

	int friends = account().privateStatus() ? GG_STATUS_FRIENDS_MASK : 0;

	int type = GaduProtocolHelper::gaduStatusFromStatus(newStatus);
	bool hasDescription = !newStatus.description().isEmpty();

	setStatusFlags();

	auto writableSessionToken = Connection->writableSessionToken();
	if (hasDescription)
		gg_change_status_descr(writableSessionToken.rawSession(), type | friends, newStatus.description().toUtf8().constData());
	else
		gg_change_status(writableSessionToken.rawSession(), type | friends);

	account().accountContact().setCurrentStatus(status());
}

void GaduProtocol::changePrivateMode()
{
	sendStatusToServer();
}

void GaduProtocol::connectionTimeoutTimerSlot()
{
	kdebugf();

	kdebugm(KDEBUG_INFO, "Timeout, breaking connection\n");
	socketConnFailed(ConnectionTimeout);

	kdebugf2();
}

void GaduProtocol::everyMinuteActions()
{
	kdebugf();

	auto writableSessionToken = Connection->writableSessionToken();
	gg_ping(writableSessionToken.rawSession());
}

void GaduProtocol::configureServices()
{
	GaduAccountDetails *gaduAccountDetails = dynamic_cast<GaduAccountDetails *>(account().details());
	if (!gaduAccountDetails)
		return;

	CurrentChatStateService->setSendTypingNotifications(gaduAccountDetails->sendTypingNotification());

	switch (status().group())
	{
		case StatusTypeGroupOffline:
			CurrentChatImageService->setReceiveImages(false);
			break;
		case StatusTypeGroupInvisible:
			CurrentChatImageService->setReceiveImages(gaduAccountDetails->receiveImagesDuringInvisibility());
			break;
		default:
			CurrentChatImageService->setReceiveImages(true);
	}
}

void GaduProtocol::accountUpdated()
{
	sendStatusToServer();
	configureServices();
}

void GaduProtocol::connectSocketNotifiersToServices()
{
	connect(SocketNotifiers, SIGNAL(msgEventReceived(gg_event*)),
	        CurrentChatService, SLOT(handleEventMsg(gg_event*)));
	connect(SocketNotifiers, SIGNAL(multilogonMsgEventReceived(gg_event*)),
	        CurrentChatService, SLOT(handleEventMultilogonMsg(gg_event*)));
	connect(SocketNotifiers, SIGNAL(ackEventReceived(gg_event*)),
	        CurrentChatService, SLOT(handleEventAck(gg_event*)));
	connect(SocketNotifiers, SIGNAL(typingNotificationEventReceived(gg_event*)),
	        CurrentChatStateService, SLOT(handleEventTypingNotify(gg_event*)));
}

void GaduProtocol::login()
{
	// TODO: create some kind of cleanup method
	if (GaduSession)
	{
		gg_free_session(GaduSession);
		GaduSession = 0;

		// here was return... do not re-add it ;)
	}

	if (SocketNotifiers)
	{
		SocketNotifiers->deleteLater();
		SocketNotifiers = 0;
	}

	GaduAccountDetails *gaduAccountDetails = dynamic_cast<GaduAccountDetails *>(account().details());
	if (!gaduAccountDetails || 0 == gaduAccountDetails->uin())
	{
		connectionClosed();
		return;
	}

	GaduProxyHelper::setupProxy(account().useDefaultProxy()
			? NetworkProxyManager::instance()->defaultProxy()
			: account().proxy());

	setupLoginParams();

	GaduSession = gg_login(&GaduLoginParams);

	cleanUpLoginParams();

	if (!GaduSession)
	{
		// gadu session can be null if DNS failed, we can try IP after that
		connectionError();
		return;
	}

	SocketNotifiers = new GaduProtocolSocketNotifiers(account(), this);
	SocketNotifiers->setGaduIMTokenService(CurrentImTokenService);
	SocketNotifiers->setGaduUserDataService(CurrentUserDataService);
	connectSocketNotifiersToServices();
	SocketNotifiers->watchFor(GaduSession);
}

void GaduProtocol::connectedToServer()
{
	kdebugf();

	GaduServersManager::instance()->markServerAsGood(ActiveServer);

	PingTimer = new QTimer(0);
	connect(PingTimer, SIGNAL(timeout()), this, SLOT(everyMinuteActions()));
	PingTimer->start(60000);

	loggedIn();

	// workaround about servers errors
	if (StatusTypeInvisible == status().type())
		sendStatusToServer();

	kdebugf2();
}

void GaduProtocol::afterLoggedIn()
{
	// fetch current avatar after connection
	AvatarManager::instance()->updateAvatar(account().accountContact(), true);

	auto contacts = ContactManager::instance()->contacts(account(), ContactManager::ExcludeAnonymous);
	CurrentNotifyService->sendInitialData(contacts);

	static_cast<GaduRosterService *>(rosterService())->prepareRoster();

	sendStatusToServer();
}

void GaduProtocol::logout()
{
	// we need to changestatus manually in gadu
	// status is offline
	sendStatusToServer();
	// Kadu bug #2542
	// gg_logoff(GaduSession);

	// TODO: it never gets called when unloading gadu_protocol and causes memory and resource leak
	QTimer::singleShot(0, this, SLOT(loggedOut()));
}

void GaduProtocol::disconnectedCleanup()
{
	Protocol::disconnectedCleanup();

	if (PingTimer)
	{
		PingTimer->stop();
		delete PingTimer;
		PingTimer = 0;
	}

	if (SocketNotifiers)
	{
		SocketNotifiers->watchFor(0); // stop watching
		SocketNotifiers->deleteLater();
		SocketNotifiers = 0;
	}

	if (GaduSession)
	{
		gg_free_session(GaduSession);
		GaduSession = 0;
	}

	CurrentMultilogonService->removeAllSessions();
}

void GaduProtocol::setupLoginParams()
{
	memset(&GaduLoginParams, 0, sizeof(GaduLoginParams));

	GaduAccountDetails *gaduAccountDetails = dynamic_cast<GaduAccountDetails *>(account().details());
	if (!gaduAccountDetails)
		return;

	GaduLoginParams.uin = account().id().toULong();
	GaduLoginParams.password = qstrdup(account().password().toUtf8().constData());

	GaduLoginParams.async = 1;

	// always start with inivisible, after sending notify data new status is resent again
	auto gaduStatus = loginStatus().description().isEmpty()
		? GG_STATUS_INVISIBLE
		: GG_STATUS_INVISIBLE_DESCR;
	GaduLoginParams.status = gaduStatus | (account().privateStatus() ? GG_STATUS_FRIENDS_MASK : 0);

	if (!loginStatus().description().isEmpty())
		GaduLoginParams.status_descr = qstrdup(loginStatus().description().toUtf8().constData());

	SecureConnection = gaduAccountDetails->tlsEncryption();
	GaduLoginParams.tls = gaduAccountDetails->tlsEncryption() ? GG_SSL_ENABLED : GG_SSL_DISABLED;

	ActiveServer = GaduServersManager::instance()->getServer(1 == GaduLoginParams.tls);

	bool haveServer = !ActiveServer.first.isNull();
	GaduLoginParams.server_addr = haveServer ? htonl(ActiveServer.first.toIPv4Address()) : 0;
	GaduLoginParams.server_port = haveServer ? ActiveServer.second : 0;

	GaduLoginParams.protocol_version = GG_PROTOCOL_VERSION_110;
	GaduLoginParams.compatibility = GG_COMPAT_LEGACY; // TODO: #2961
	GaduLoginParams.client_version = qstrdup(Core::nameWithVersion().toUtf8().constData());
	GaduLoginParams.protocol_features =
		GG_FEATURE_DND_FFC |
		GG_FEATURE_TYPING_NOTIFICATION | GG_FEATURE_MULTILOGON |
		GG_FEATURE_USER_DATA;

	GaduLoginParams.encoding = GG_ENCODING_UTF8;

	GaduLoginParams.has_audio = false;
	GaduLoginParams.last_sysmsg = Application::instance()->configuration()->deprecatedApi()->readNumEntry("General", "SystemMsgIndex", 1389);

	GaduLoginParams.image_size = qMax(qMin(Application::instance()->configuration()->deprecatedApi()->readNumEntry("Chat", "MaximumImageSizeInKiloBytes", 255), 255), 0);

	GaduLoginParams.struct_size = sizeof(struct gg_login_params);

	setStatusFlags();
}

void GaduProtocol::cleanUpLoginParams()
{
	if (GaduLoginParams.password)
	{
		memset(GaduLoginParams.password, 0, qstrlen(GaduLoginParams.password));
		delete [] GaduLoginParams.password;
		GaduLoginParams.password = 0;
	}

	delete [] GaduLoginParams.client_version;
	GaduLoginParams.client_version = 0;

	delete [] GaduLoginParams.status_descr;
	GaduLoginParams.status_descr = 0;
}

void GaduProtocol::socketContactStatusChanged(UinType uin, unsigned int status, const QString &description, unsigned int maxImageSize)
{
	Contact contact = ContactManager::instance()->byId(account(), QString::number(uin), ActionReturnNull);

	if (contact.isAnonymous())
	{
		kdebugmf(KDEBUG_INFO, "buddy %u not in list. Damned server!\n", uin);
		if (contact.ownerBuddy())
			emit userStatusChangeIgnored(contact.ownerBuddy());
		rosterService()->removeContact(contact);
		return;
	}

	contact.setMaximumImageSize(maxImageSize);

	Status oldStatus = contact.currentStatus();
	Status newStatus;
	newStatus.setType(GaduProtocolHelper::statusTypeFromGaduStatus(status));
	newStatus.setDescription(description);
	contact.setCurrentStatus(newStatus);
	contact.setBlocking(GaduProtocolHelper::isBlockingStatus(status));

	// see issue #2159 - we need a way to ignore first status of given contact
	if (contact.ignoreNextStatusChange())
		contact.setIgnoreNextStatusChange(false);
	else
		emit contactStatusChanged(contact, oldStatus);
}

void GaduProtocol::socketConnFailed(GaduError error)
{
	kdebugf();

	QString msg = GaduProtocolHelper::connectionErrorMessage(error);

	switch (error)
	{
		case ConnectionNeedEmail:
			MessageDialog::show(KaduIcon("dialog-warning"), tr("Kadu"), msg);
			break;
		case ConnectionIncorrectPassword:
			passwordRequired();

			// do not call connectionClosed here
			// we dont need to do that, because no connection is open here
			// and we should not do that, because closing connection causes changing of state to offline
			// and it break our state machine, so we would be out of password-required state
			return;
		default: // we need special code only for 2 cases
			break;
	}

	if (!msg.isEmpty())
	{
		QHostAddress server = ActiveServer.first;
		QString host;
		if (server.isNull() || server.toIPv4Address() == (quint32)0)
			host = "HUB";
		else
			host = QString("%1:%2").arg(server.toString()).arg(ActiveServer.second);
		kdebugm(KDEBUG_INFO, "%s %s\n", qPrintable(host), qPrintable(msg));
		emit connectionError(account(), host, msg);
	}

	if (!GaduProtocolHelper::isConnectionErrorFatal(error))
	{
		GaduServersManager::instance()->markServerAsBad(ActiveServer);
		logout();
		connectionError();
	}
	else
	{
		logout();
		connectionClosed();
	}

	kdebugf2();
}

void GaduProtocol::disconnectedFromServer()
{
	connectionClosed();
}

QString GaduProtocol::statusPixmapPath()
{
	return QLatin1String("gadu-gadu");
}

void GaduProtocol::disableSocketNotifiers()
{
	if (SocketNotifiers)
		SocketNotifiers->disable();
}

void GaduProtocol::enableSocketNotifiers()
{
	if (SocketNotifiers)
		SocketNotifiers->enable();
}

GaduDriveService * GaduProtocol::driveService() const
{
	return CurrentDriveService;
}

GaduUserDataService * GaduProtocol::userDataService() const
{
	return CurrentUserDataService;
}

void GaduProtocol::configurationUpdated()
{
#ifdef DEBUG_OUTPUT_ENABLED
	// 8 bits for gadu debug
	gg_debug_level = debug_mask & 255;
#endif
}

bool GaduProtocol::secureConnection() const
{
	return isConnected()
			? SecureConnection
			: false;
}

#include "moc_gadu-protocol.cpp"
