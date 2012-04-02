/*
 * %kadu copyright begin%
 * Copyright 2011 Tomasz Rostanski (rozteck@interia.pl)
 * Copyright 2008, 2009, 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010, 2010, 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2008, 2009, 2010, 2010 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2004, 2005, 2008, 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009, 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2004 Roman Krzystyniak (Ron_K@tlen.pl)
 * Copyright 2003, 2004, 2005 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2004 Tomasz Chiliński (chilek@chilan.com)
 * Copyright 2004, 2005 Paweł Płuciennik (pawel_p@kadu.net)
 * Copyright 2007, 2008, 2009, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2006, 2007, 2008 Dawid Stawiarski (neeo@kadu.net)
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
#include "configuration/configuration-file.h"
#include "contacts/contact-manager.h"
#include "core/core.h"
#include "gui/windows/message-dialog.h"
#include "gui/windows/password-window.h"
#include "message/formatted-message.h"
#include "network/proxy/network-proxy-manager.h"
#include "qt/long-validator.h"
#include "status/status-type-manager.h"
#include "status/status-type.h"
#include "status/status.h"

#include "icons/icons-manager.h"
#include "misc/misc.h"
#include "debug.h"

#include "server/gadu-contact-list-handler.h"
#include "server/gadu-servers-manager.h"
#include "socket-notifiers/gadu-protocol-socket-notifiers.h"

#include "helpers/gadu-importer.h"
#include "helpers/gadu-protocol-helper.h"
#include "helpers/gadu-proxy-helper.h"
#include "gadu-account-details.h"
#include "gadu-contact-details.h"

#include "gadu-protocol.h"

GaduProtocol::GaduProtocol(Account account, ProtocolFactory *factory) :
		Protocol(account, factory), CurrentFileTransferService(0),
		ActiveServer(), GaduLoginParams(), GaduSession(0), SocketNotifiers(0), PingTimer(0)
{
	kdebugf();

	CurrentAvatarService = new GaduAvatarService(account, this);
	CurrentChatImageService = new GaduChatImageService(this);
	CurrentChatService = new GaduChatService(this);
	CurrentContactListService = new GaduContactListService(this);
	CurrentContactPersonalInfoService = new GaduContactPersonalInfoService(this);
	CurrentPersonalInfoService = new GaduPersonalInfoService(this);
	CurrentSearchService = new GaduSearchService(this);
	CurrentMultilogonService = new GaduMultilogonService(account, this);
	CurrentChatStateService = new GaduChatStateService(this);
	ContactListHandler = 0;

	connect(account, SIGNAL(updated()), this, SLOT(accountUpdated()));

	kdebugf2();
}

GaduProtocol::~GaduProtocol()
{
	kdebugf();

	disconnect(account(), SIGNAL(updated()), this, SLOT(accountUpdated()));

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
	if (details && details->receiveSpam())
		statusFlags = statusFlags | GG_STATUS_FLAG_SPAM;

	gg_change_status_flags(GaduSession, GG_STATUS_FLAG_UNKNOWN | statusFlags);
}

void GaduProtocol::sendStatusToServer()
{
	if (!isConnected())
		return;

	if (!GaduSession)
		return;

	Status newStatus = status();

	int friends = account().privateStatus() ? GG_STATUS_FRIENDS_MASK : 0;

	int type = GaduProtocolHelper::gaduStatusFromStatus(newStatus);
	bool hasDescription = !newStatus.description().isEmpty();

	setStatusFlags();

	disableSocketNotifiers();
	if (hasDescription)
		gg_change_status_descr(GaduSession, type | friends, newStatus.description().toUtf8().constData());
	else
		gg_change_status(GaduSession, type | friends);
	enableSocketNotifiers();

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

	disableSocketNotifiers();
	gg_ping(GaduSession);
	enableSocketNotifiers();
	CurrentChatImageService->resetSendImageRequests();
}

void GaduProtocol::accountUpdated()
{
	sendStatusToServer();
	setUpFileTransferService();
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

	ContactListHandler = new GaduContactListHandler(this);

	SocketNotifiers = new GaduProtocolSocketNotifiers(account(), this);
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

	// set up DCC if needed
	setUpFileTransferService();

	sendUserList();
}

void GaduProtocol::logout()
{
	kdebugf();

	// we need to change status manually in gadu
	// status is offline
	sendStatusToServer();
	gg_logoff(GaduSession);

	loggedOut();
}

void GaduProtocol::disconnectedCleanup()
{
	Protocol::disconnectedCleanup();

	if (ContactListHandler)
		ContactListHandler->reset();

	setUpFileTransferService(true);

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

		delete ContactListHandler;
		ContactListHandler = 0;
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
	GaduLoginParams.password = strdup(account().password().toAscii().constData());

	GaduLoginParams.async = 1;

	GaduLoginParams.status = (GaduProtocolHelper::gaduStatusFromStatus(loginStatus()) | (account().privateStatus() ? GG_STATUS_FRIENDS_MASK : 0));

	if (!loginStatus().description().isEmpty())
		GaduLoginParams.status_descr = strdup(loginStatus().description().toUtf8().constData());

	GaduLoginParams.tls = gaduAccountDetails->tlsEncryption() ? GG_SSL_ENABLED : GG_SSL_DISABLED;

	ActiveServer = GaduServersManager::instance()->getServer(1 == GaduLoginParams.tls);

	bool haveServer = !ActiveServer.first.isNull();
	GaduLoginParams.server_addr = haveServer ? htonl(ActiveServer.first.toIPv4Address()) : 0;
	GaduLoginParams.server_port = haveServer ? ActiveServer.second : 0;

	if (!gaduAccountDetails->externalIp().isEmpty())
	{
		QHostAddress externalIp(gaduAccountDetails->externalIp());
		if (!externalIp.isNull())
			GaduLoginParams.external_addr = htonl(externalIp.toIPv4Address());
	}

	GaduLoginParams.external_port = gaduAccountDetails->externalPort();

	GaduLoginParams.protocol_version = GG_DEFAULT_PROTOCOL_VERSION;
	GaduLoginParams.client_version = strdup(Core::nameWithVersion().toUtf8().constData());
	GaduLoginParams.protocol_features =
			GG_FEATURE_UNKNOWN_4 | // GG_FEATURE_STATUS80
			GG_FEATURE_DND_FFC |
			GG_FEATURE_IMAGE_DESCR |
			GG_FEATURE_UNKNOWN_40 |
			GG_FEATURE_UNKNOWN_100 |
			GG_FEATURE_USER_DATA |
			GG_FEATURE_MSG_ACK |
			GG_FEATURE_TYPING_NOTIFICATION |
			GG_FEATURE_MULTILOGON;

	GaduLoginParams.encoding = GG_ENCODING_UTF8;

	GaduLoginParams.has_audio = false;
	GaduLoginParams.last_sysmsg = config_file.readNumEntry("General", "SystemMsgIndex", 1389);

	GaduLoginParams.image_size = qMax(qMin(gaduAccountDetails->maximumImageSize(), 255), 0);

	setStatusFlags();
}

void GaduProtocol::cleanUpLoginParams()
{
	memset(GaduLoginParams.password, 0, strlen(GaduLoginParams.password));
	free(GaduLoginParams.password);
	GaduLoginParams.password = 0;

	free(GaduLoginParams.client_version);
	GaduLoginParams.client_version = 0;

	if (GaduLoginParams.status_descr)
	{
		free(GaduLoginParams.status_descr);
		GaduLoginParams.status_descr = 0;
	}
}

void GaduProtocol::startFileTransferService()
{
	if (!CurrentFileTransferService)
	{
		CurrentFileTransferService = new GaduFileTransferService(this);
		account().data()->fileTransferServiceChanged(CurrentFileTransferService);
	}
}

void GaduProtocol::stopFileTransferService()
{
	delete CurrentFileTransferService;
	CurrentFileTransferService = 0;
	account().data()->fileTransferServiceChanged(0);
}

void GaduProtocol::setUpFileTransferService(bool forceClose)
{
	bool close = forceClose;
	if (!close)
		close = !isConnected();
	if (!close)
	{
		GaduAccountDetails *gaduAccountDetails = dynamic_cast<GaduAccountDetails *>(account().details());
		if (!gaduAccountDetails)
			close = true;
		else
			close = !gaduAccountDetails->allowDcc();
	}

	if (close)
		stopFileTransferService();
	else
		startFileTransferService();
}

void GaduProtocol::sendUserList()
{
	QVector<Contact> contacts = ContactManager::instance()->contacts(account());
	QVector<Contact> contactsToSend;

	foreach (const Contact &contact, contacts)
		if (!contact.isAnonymous())
			contactsToSend.append(contact);

	ContactListHandler->setUpContactList(contactsToSend);
}

void GaduProtocol::socketContactStatusChanged(UinType uin, unsigned int status, const QString &description, unsigned int maxImageSize)
{
	Contact contact = ContactManager::instance()->byId(account(), QString::number(uin), ActionReturnNull);

	if (contact.isAnonymous())
	{
		kdebugmf(KDEBUG_INFO, "buddy %u not in list. Damned server!\n", uin);
		if (contact.ownerBuddy())
			emit userStatusChangeIgnored(contact.ownerBuddy());
		ContactListHandler->updateContactEntry(contact);
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
	GaduContactDetails *details = static_cast<GaduContactDetails *>(contact.details());
	if (details && details->ignoreNextStatusChange())
		details->setIgnoreNextStatusChange(false);
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

void GaduProtocol::configurationUpdated()
{
#ifdef DEBUG_ENABLED
	// 8 bits for gadu debug
	gg_debug_level = debug_mask & 255;
#endif
}
