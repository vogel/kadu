/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009, 2010 Tomasz Rostański (rozteck@interia.pl)
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

#include <QtCore/QCoreApplication>
#include <QtCore/QTimer>
#include <QtCrypto>

#ifdef Q_OS_WIN
#include <winsock2.h>
#else
#include <arpa/inet.h>
#endif

#include "accounts/account.h"
#include "accounts/account-manager.h"
#include "accounts/account-shared.h"
#include "avatars/avatar-manager.h"
#include "chat/chat.h"
#include "chat/message/formatted-message.h"
#include "chat/chat-manager.h"
#include "configuration/configuration-file.h"
#include "buddies/buddy-manager.h"
#include "contacts/contact-manager.h"
#include "gui/windows/message-dialog.h"
#include "gui/windows/password-window.h"
#include "protocols/protocols-manager.h"
#include "qt/long-validator.h"
#include "status/status.h"
#include "status/status-type.h"
#include "status/status-type-manager.h"
#include "url-handlers/url-handler-manager.h"

#include "debug.h"
#include "icons-manager.h"
#include "misc/misc.h"

#include "helpers/gadu-formatter.h"
#include "server/gadu-contact-list-handler.h"
#include "server/gadu-servers-manager.h"
#include "socket-notifiers/gadu-protocol-socket-notifiers.h"
#include "socket-notifiers/gadu-pubdir-socket-notifiers.h"

#include "helpers/gadu-importer.h"
#include "gadu-account-details.h"
#include "gadu-contact-details.h"
#include "gadu-id-validator.h"
#include "gadu-protocol-factory.h"
#include "gadu-resolver.h"
#include "gadu-url-handler.h"

#include "gadu-protocol.h"

#define GG8_DESCRIPTION_MASK 0x00ff

extern "C" KADU_EXPORT int gadu_protocol_init(bool firstLoad)
{
	Q_UNUSED(firstLoad)

	GaduServersManager::createInstance();

	if (ProtocolsManager::instance()->hasProtocolFactory("gadu"))
		return 0;

	// 8 bits for gadu debug
	gg_debug_level = debug_mask & 255;

	gg_proxy_host = 0;
	gg_proxy_username = 0;
	gg_proxy_password = 0;

#ifndef DEBUG_ENABLED
	gg_debug_level = 0;
#endif
	gg_global_set_custom_resolver(gadu_resolver_start, gadu_resolver_cleanup);

	GaduIdValidator::createInstance();

	GaduProtocolFactory::createInstance();

	ProtocolsManager::instance()->registerProtocolFactory(GaduProtocolFactory::instance());
	UrlHandlerManager::instance()->registerUrlHandler("Gadu", new GaduUrlHandler());

	GaduImporter::createInstance();

	if (AccountManager::instance()->allItems().isEmpty())
		GaduImporter::instance()->importAccounts();
	GaduImporter::instance()->importContacts();

	return 0;
}

extern "C" KADU_EXPORT void gadu_protocol_close()
{
	GaduImporter::destroyInstance();

	UrlHandlerManager::instance()->unregisterUrlHandler("Gadu");
	ProtocolsManager::instance()->unregisterProtocolFactory(GaduProtocolFactory::instance());

	GaduProtocolFactory::destroyInstance();

	GaduIdValidator::destroyInstance();
	GaduServersManager::destroyInstance();

	qRemovePostRoutine(QCA::deinit);
}

#define GG_STATUS_INVISIBLE2 0x0009
QString GaduProtocol::statusTypeFromGaduStatus(unsigned int index)
{
	switch (index & GG8_DESCRIPTION_MASK)
	{
		case GG_STATUS_FFC_DESCR:
		case GG_STATUS_FFC:
			return "FreeForChat";

		case GG_STATUS_AVAIL_DESCR:
		case GG_STATUS_AVAIL:
			return "Online";

		case GG_STATUS_BUSY_DESCR:
		case GG_STATUS_BUSY:
			return "Away";

		case GG_STATUS_DND_DESCR:
		case GG_STATUS_DND:
			return "DoNotDisturb";

		case GG_STATUS_INVISIBLE_DESCR:
		case GG_STATUS_INVISIBLE:
		case GG_STATUS_INVISIBLE2:
			return "Invisible";

		case GG_STATUS_BLOCKED:
		case GG_STATUS_NOT_AVAIL_DESCR:
		case GG_STATUS_NOT_AVAIL:

		default:
			return "Offline";
	}
}

bool GaduProtocol::isBlockingStatus(unsigned int index)
{
	return GG_STATUS_BLOCKED == index;
}

unsigned int GaduProtocol::gaduStatusFromStatus(const Status &status)
{
	bool hasDescription = !status.description().isEmpty();
	const QString &type = status.type();

	if ("FreeForChat" == type)
		return hasDescription ? GG_STATUS_FFC_DESCR : GG_STATUS_FFC;

	if ("Online" == type)
		return hasDescription ? GG_STATUS_AVAIL_DESCR : GG_STATUS_AVAIL;

	if ("Away" == type || "NotAvailable" == type)
		return hasDescription ? GG_STATUS_BUSY_DESCR : GG_STATUS_BUSY;

	if ("DoNotDisturb" == type)
		return hasDescription ? GG_STATUS_DND_DESCR : GG_STATUS_DND;

	if ("Invisible" == type)
		return hasDescription ? GG_STATUS_INVISIBLE_DESCR : GG_STATUS_INVISIBLE;

	return hasDescription ? GG_STATUS_NOT_AVAIL_DESCR : GG_STATUS_NOT_AVAIL;
}

Buddy GaduProtocol::searchResultToBuddy(gg_pubdir50_t res, int number)
{
	Buddy result = Buddy::create();

	Contact contact = Contact::create();
	contact.setContactAccount(account());
	contact.setOwnerBuddy(result);
	contact.setId(gg_pubdir50_get(res, number, GG_PUBDIR50_UIN));
	contact.setDetails(new GaduContactDetails(contact));

	const char *pubdirStatus = gg_pubdir50_get(res, number, GG_PUBDIR50_STATUS);
	if (pubdirStatus)
	{
		Status status;
		status.setType(statusTypeFromGaduStatus(atoi(pubdirStatus) & 127));
		contact.setCurrentStatus(status);
	}

	result.setFirstName(QString::fromUtf8(gg_pubdir50_get(res, number, GG_PUBDIR50_FIRSTNAME)));
	result.setLastName(QString::fromUtf8(gg_pubdir50_get(res, number, GG_PUBDIR50_LASTNAME)));
	result.setNickName(QString::fromUtf8(gg_pubdir50_get(res, number, GG_PUBDIR50_NICKNAME)));
	result.setBirthYear(QString::fromUtf8(gg_pubdir50_get(res, number, GG_PUBDIR50_BIRTHYEAR)).toUShort());
	result.setCity(QString::fromUtf8(gg_pubdir50_get(res, number, GG_PUBDIR50_CITY)));
	result.setFamilyName(QString::fromUtf8(gg_pubdir50_get(res, number, GG_PUBDIR50_FAMILYNAME)));
	result.setFamilyCity(QString::fromUtf8(gg_pubdir50_get(res, number, GG_PUBDIR50_FAMILYCITY)));
	result.setGender((BuddyGender)QString::fromUtf8(gg_pubdir50_get(res, number, GG_PUBDIR50_GENDER)).toUShort());

	return result;
}

GaduProtocol::GaduProtocol(Account account, ProtocolFactory *factory) :
		Protocol(account, factory),
		ActiveServer(), GaduLoginParams(), GaduSession(0), PingTimer(0)
{
	kdebugf();

	SocketNotifiers = new GaduProtocolSocketNotifiers(account, this);

	CurrentAvatarService = new GaduAvatarService(account, this);
	CurrentChatImageService = new GaduChatImageService(this);
	CurrentChatService = new GaduChatService(this);
	CurrentContactListService = new GaduContactListService(this);
	CurrentContactPersonalInfoService = new GaduContactPersonalInfoService(this);
	CurrentPersonalInfoService = new GaduPersonalInfoService(this);
	CurrentSearchService = new GaduSearchService(this);
#ifdef GADU_HAVE_MULTILOGON
	CurrentMultilogonService = new GaduMultilogonService(account, this);
#endif // GADU_HAVE_MULTILOGON
#ifdef GADU_HAVE_TYPING_NOTIFY
	CurrentChatStateService = new GaduChatStateService(this);
#endif // GADU_HAVE_TYPING_NOTIFY
	ContactListHandler = 0;

	connect(BuddyManager::instance(), SIGNAL(buddySubscriptionChanged(Buddy &)),
			this, SLOT(buddySubscriptionChanged(Buddy &)));
	connect(ContactManager::instance(), SIGNAL(contactAttached(Contact)),
			this, SLOT(contactAttached(Contact)));
	connect(ContactManager::instance(), SIGNAL(contactReattached(Contact)),
			this, SLOT(contactAttached(Contact)));
	connect(ContactManager::instance(), SIGNAL(contactAboutToBeDetached(Contact)),
			this, SLOT(contactAboutToBeDetached(Contact)));
	connect(ContactManager::instance(), SIGNAL(contactIdChanged(Contact, const QString &)),
			this, SLOT(contactIdChanged(Contact, const QString &)));
	connect(account, SIGNAL(updated()), this, SLOT(accountUpdated()));

	kdebugf2();
}

GaduProtocol::~GaduProtocol()
{
	kdebugf();

	disconnect(BuddyManager::instance(), SIGNAL(buddySubscriptionChanged(Buddy &)),
			this, SLOT(buddySubscriptionChanged(Buddy &)));
	disconnect(ContactManager::instance(), SIGNAL(contactAttached(Contact)),
			this, SLOT(contactAttached(Contact)));
	disconnect(ContactManager::instance(), SIGNAL(contactReattached(Contact)),
			this, SLOT(contactAttached(Contact)));
	disconnect(ContactManager::instance(), SIGNAL(contactAboutToBeDetached(Contact)),
			this, SLOT(contactAboutToBeDetached(Contact)));
	disconnect(ContactManager::instance(), SIGNAL(contactIdChanged(Contact, const QString &)),
			this, SLOT(contactIdChanged(Contact, const QString &)));
	disconnect(account(), SIGNAL(updated()), this, SLOT(accountUpdated()));

	networkDisconnected(false, false);

	kdebugf2();
}

int GaduProtocol::maxDescriptionLength()
{
	return GG_STATUS_DESCR_MAXSIZE;
}

void GaduProtocol::changeStatus()
{
	changeStatus(false);
}

void GaduProtocol::changeStatus(bool force)
{
	Status newStatus = nextStatus();
	if (newStatus == status() && !force)
		return; // don't reset password

	if (newStatus.isDisconnected() && status().isDisconnected())
	{
		if (newStatus.description() != status().description())
			statusChanged(newStatus);

		if (NetworkConnecting == state())
			networkDisconnected(false, false);
		return;
	}

	if (NetworkConnecting == state())
		return;

	if (status().isDisconnected())
	{
		login();
		return;
	}

// TODO 0.10.0: workaround. Find general solution
	if (newStatus.type() == "NotAvailable" && status().type() == "Away")
		return;

	int friends = (!newStatus.isDisconnected() && account().privateStatus() ? GG_STATUS_FRIENDS_MASK : 0);

	int type = gaduStatusFromStatus(newStatus);
	bool hasDescription = !newStatus.description().isEmpty();

	if (hasDescription)
		gg_change_status_descr(GaduSession, type | friends, newStatus.description().toUtf8());
	else
		gg_change_status(GaduSession, type | friends);

	if (newStatus.isDisconnected())
		networkDisconnected(false, false);

	statusChanged(newStatus);
}

void GaduProtocol::changePrivateMode()
{
	changeStatus(true);
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

	gg_ping(GaduSession);
	CurrentChatImageService->resetSendImageRequests();
}

void GaduProtocol::login(const QString &password, bool permanent)
{
	if (password.isEmpty()) // user did not give us password, so prevent from further reconnecting
	{
		Status newstat = status();
		newstat.setType("Offline");
		setStatus(newstat);
		statusChanged(newstat);
		return;
	}

	account().setPassword(password);
	account().setRememberPassword(permanent);
	account().setHasPassword(!password.isEmpty());

	login();
}

void GaduProtocol::accountUpdated()
{
}

void GaduProtocol::login()
{
	kdebugf();

	if (GaduSession)
		return;

	GaduAccountDetails *gaduAccountDetails = dynamic_cast<GaduAccountDetails *>(account().details());

	if (!gaduAccountDetails)
		return;

	if (0 == gaduAccountDetails->uin())
	{
		MessageDialog::show("dialog-warning", tr("Kadu"), tr("UIN not set!"));
		setStatus(Status());
		statusChanged(Status());
		kdebugmf(KDEBUG_FUNCTION_END, "end: gadu UIN not set\n");
		return;
	}

	if (!account().hasPassword())
	{
		QString message = tr("Please provide password for %1 (%2) account")
				.arg(account().accountIdentity().name())
				.arg(account().id());
		PasswordWindow::getPassword(message, this, SLOT(login(const QString &, bool)));
		return;
	}

	networkStateChanged(NetworkConnecting);

	setupProxy();
	setupLoginParams();

	GaduSession = gg_login(&GaduLoginParams);
	ContactListHandler = new GaduContactListHandler(this);

	cleanUpLoginParams();

	if (GaduSession)
		SocketNotifiers->watchFor(GaduSession);
	else
		networkDisconnected(false, false);

	kdebugf2();
}

void GaduProtocol::cleanUpProxySettings()
{
	if (gg_proxy_host)
	{
		free(gg_proxy_host);
		gg_proxy_host = 0;
	}

	if (gg_proxy_username)
	{
		free(gg_proxy_username);
		free(gg_proxy_password);
		gg_proxy_username = gg_proxy_password = 0;
	}
}

void GaduProtocol::setupProxy()
{
	kdebugf();

	cleanUpProxySettings();

	AccountProxySettings proxySettings = account().proxySettings();
	gg_proxy_enabled = proxySettings.enabled();
	if (!gg_proxy_enabled)
		return;

	gg_proxy_host = strdup((char *)unicode2latin(proxySettings.address()).data());
	gg_proxy_port = proxySettings.port();

	kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "gg_proxy_host = %s\n", gg_proxy_host);
	kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "gg_proxy_port = %d\n", gg_proxy_port);

	if (proxySettings.requiresAuthentication() && !proxySettings.user().isEmpty())
	{
		gg_proxy_username = strdup((char *)unicode2latin(proxySettings.user()).data());
		gg_proxy_password = strdup((char *)unicode2latin(proxySettings.password()).data());
	}
}

void GaduProtocol::setupLoginParams()
{
	memset(&GaduLoginParams, 0, sizeof(GaduLoginParams));

	GaduAccountDetails *gaduAccountDetails = dynamic_cast<GaduAccountDetails *>(account().details());
	if (!gaduAccountDetails)
		return;

	GaduLoginParams.uin = account().id().toULong();
	GaduLoginParams.password = strdup(account().password().toAscii().data());

	GaduLoginParams.async = 1;

	GaduLoginParams.status = (gaduStatusFromStatus(nextStatus()) | (account().privateStatus() ? GG_STATUS_FRIENDS_MASK : 0));
	if (!nextStatus().description().isEmpty())
		GaduLoginParams.status_descr = strdup(nextStatus().description().toUtf8());

#ifdef GADU_HAVE_TLS
	GaduLoginParams.tls = gaduAccountDetails->tlsEncryption() ? 1 : 0;
#else
	GaduLoginParams.tls = 0;
#endif

	ActiveServer = GaduServersManager::instance()->getServer(1 == GaduLoginParams.tls);
	bool haveServer = !ActiveServer.first.isNull();
	GaduLoginParams.server_addr = haveServer ? htonl(ActiveServer.first.toIPv4Address()) : 0;
	GaduLoginParams.server_port = haveServer ? ActiveServer.second : 0;

	GaduLoginParams.protocol_version = GG_DEFAULT_PROTOCOL_VERSION;
	GaduLoginParams.client_version = (char *)GG_DEFAULT_CLIENT_VERSION;
	GaduLoginParams.protocol_features = GG_FEATURE_DND_FFC; // enable new statuses

#ifdef GADU_HAVE_MULTILOGON
	GaduLoginParams.protocol_features |= GG_FEATURE_MULTILOGON;
#endif // GADU_HAVE_MULTILOGON

#ifdef GADU_HAVE_TYPING_NOTIFY
	GaduLoginParams.protocol_features |=GG_FEATURE_TYPING_NOTIFICATION;
#endif // GADU_HAVE_TYPING_NOTIFY

	GaduLoginParams.encoding = GG_ENCODING_UTF8;

	GaduLoginParams.has_audio = false;
	GaduLoginParams.last_sysmsg = config_file.readNumEntry("General", "SystemMsgIndex", 1389);

	GaduLoginParams.image_size = gaduAccountDetails->maximumImageSize();
}

void GaduProtocol::cleanUpLoginParams()
{
	memset(GaduLoginParams.password, 0, strlen(GaduLoginParams.password));
	free(GaduLoginParams.password);
	GaduLoginParams.password = 0;

	if (GaduLoginParams.status_descr)
	{
		free(GaduLoginParams.status_descr);
		GaduLoginParams.status_descr = 0;
	}
}

void GaduProtocol::networkConnected()
{
	networkStateChanged(NetworkConnected);

	// fetch current avatar after connection
	AvatarManager::instance()->updateAvatar(account().accountContact(), true);
}

void GaduProtocol::networkDisconnected(bool tryAgain, bool waitForPassword)
{
	if (!tryAgain)
		networkStateChanged(NetworkDisconnected);

	if (PingTimer)
	{
		PingTimer->stop();
		delete PingTimer;
		PingTimer = 0;
	}

	SocketNotifiers->watchFor(0); // stop watching

	if (GaduSession)
	{
		gg_free_session(GaduSession);
		GaduSession = 0;

		delete ContactListHandler;
		ContactListHandler = 0;
	}

	setAllOffline();

#ifdef GADU_HAVE_MULTILOGON
	CurrentMultilogonService->removeAllSessions();
#endif // GADU_HAVE_MULTILOGON

	if (tryAgain && !nextStatus().isDisconnected()) // user still wants to login
	{
		networkStateChanged(NetworkConnecting);
		statusChanged(Status());

		QTimer::singleShot(1000, this, SLOT(login())); // try again after one second
	}
	else if (!nextStatus().isDisconnected())
		if (!waitForPassword)
		{
			setStatus(Status());
			statusChanged(Status());
		}
}

void GaduProtocol::sendUserList()
{
	ContactListHandler->setUpContactList(ContactManager::instance()->contacts(account()));
}

void GaduProtocol::socketContactStatusChanged(UinType uin, unsigned int status, const QString &description,
		const QHostAddress &ip, unsigned short port, unsigned int maxImageSize, unsigned int version)
{
	Contact contact = ContactManager::instance()->byId(account(), QString::number(uin));
	Buddy buddy = contact.ownerBuddy();

	if (buddy.isAnonymous())
	{
		kdebugmf(KDEBUG_INFO, "buddy %u not in list. Damned server!\n", uin);
		emit userStatusChangeIgnored(buddy);
		ContactListHandler->removeContactEntry(uin);
		return;
	}

	contact.setAddress(ip);
	contact.setMaximumImageSize(maxImageSize);
	contact.setPort(port);
	contact.setProtocolVersion(QString::number(version));

	GaduContactDetails *details = gaduContactDetails(contact);
	if (details)
		details->setGaduProtocolVersion(version);

	Status oldStatus = contact.currentStatus();
	Status newStatus;
	newStatus.setType(statusTypeFromGaduStatus(status));
	newStatus.setDescription(description);
	contact.setCurrentStatus(newStatus);
	contact.setBlocking(isBlockingStatus(status));

	emit contactStatusChanged(contact, oldStatus);
}

void GaduProtocol::socketConnFailed(GaduError error)
{
	kdebugf();
	QString msg;

	bool tryAgain = true;
	bool waitForPassword = false;

	switch (error)
	{
		case ConnectionServerNotFound:
			msg = tr("Unable to connect, server has not been found");
			break;

		case ConnectionCannotConnect:
			msg = tr("Unable to connect");
			break;

		case ConnectionNeedEmail:
			msg = tr("Please change your email in \"Change password / email\" window. "
				"Leave new password field blank.");
			tryAgain = false;
			MessageDialog::show("dialog-warning", tr("Kadu"), msg);
			break;

		case ConnectionInvalidData:
			msg = tr("Unable to connect, server has returned unknown data");
			break;

		case ConnectionCannotRead:
			msg = tr("Unable to connect, connection break during reading");
			break;

		case ConnectionCannotWrite:
			msg = tr("Unable to connect, connection break during writing");
			break;

		case ConnectionIncorrectPassword:
			emit invalidPassword(account());
			waitForPassword = true;
			tryAgain = false;
			break;

		case ConnectionTlsError:
			msg = tr("Unable to connect, error of negotiation TLS");
			break;

		case ConnectionIntruderError:
			msg = tr("Too many connection attempts with bad password!");
			tryAgain = false;
			break;

		case ConnectionUnavailableError:
			msg = tr("Unable to connect, servers are down");
			break;

		case ConnectionUnknow:
			msg = tr("Connection broken");
			kdebugm(KDEBUG_INFO, "Connection broken unexpectedly!\nUnscheduled connection termination\n");
			break;

		case ConnectionTimeout:
			msg = tr("Connection timeout!");
			break;

		case Disconnected:
			msg = tr("Disconnection has occurred");
			break;

		default:
			kdebugm(KDEBUG_ERROR, "Unhandled error? (%d)\n", int(error));
			msg = tr("Connection broken");
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

	if (tryAgain)
		GaduServersManager::instance()->markServerAsBad(ActiveServer);
	networkDisconnected(tryAgain, waitForPassword);

	kdebugf2();
}

void GaduProtocol::socketConnSuccess()
{
	kdebugf();

	GaduServersManager::instance()->markServerAsGood(ActiveServer);

	PingTimer = new QTimer(0);
	connect(PingTimer, SIGNAL(timeout()), this, SLOT(everyMinuteActions()));
	PingTimer->start(60000);

	statusChanged(nextStatus());
	networkConnected();

	sendUserList();

	GaduAccountDetails *details = dynamic_cast<GaduAccountDetails *>(account().details());

	if (details && CurrentContactListService && details->initialRosterImport())
	{
		details->setState(StorableObject::StateNew);
		details->setInitialRosterImport(false);

		CurrentContactListService->importContactList();
	}

	// workaround about servers errors
	if ("Invisible" == status().type())
		setStatus(status());

	kdebugf2();
}

void GaduProtocol::socketDisconnected()
{
	kdebugf();

	networkDisconnected(false, false);

	kdebugf2();
}

unsigned int GaduProtocol::uin(Contact contact) const
{
	GaduContactDetails *data = gaduContactDetails(contact);
	return data
			? data->uin()
			: 0;
}

GaduContactDetails * GaduProtocol::gaduContactDetails(Contact contact) const
{
	if (contact.isNull())
		return 0;
	return dynamic_cast<GaduContactDetails *>(contact.details());
}

QString GaduProtocol::statusPixmapPath()
{
	return QLatin1String("gadu-gadu");
}

void GaduProtocol::buddySubscriptionChanged(Buddy &buddy)
{
	// update offline to and other data
	if (ContactListHandler)
		foreach (const Contact &contact, buddy.contacts(account()))
			ContactListHandler->updateContactEntry(contact);
}

void GaduProtocol::contactAttached(Contact contact)
{
	if (!ContactListHandler)
		return;

	if (contact.contactAccount() != account())
		return;

	ContactListHandler->addContactEntry(contact);
}

void GaduProtocol::contactAboutToBeDetached(Contact contact)
{
	if (!ContactListHandler)
		return;

	if (contact.contactAccount() != account())
		return;

	ContactListHandler->removeContactEntry(contact);
}

void GaduProtocol::contactIdChanged(Contact contact, const QString &oldId)
{
	if (!ContactListHandler)
		return;

	if (contact.contactAccount() != account())
		return;

	bool ok;
	UinType oldUin = oldId.toUInt(&ok);
	if (ok)
		ContactListHandler->removeContactEntry(oldUin);

	ContactListHandler->addContactEntry(contact);
}
