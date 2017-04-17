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
#include "avatars/aggregated-account-avatar-service.h"
#include "avatars/aggregated-contact-avatar-service.h"
#include "avatars/contact-avatar-id.h"
#include "buddies/buddy-manager.h"
#include "chat/chat-manager.h"
#include "chat/chat-service-repository.h"
#include "chat/chat-state-service-repository.h"
#include "chat/chat.h"
#include "configuration/configuration.h"
#include "configuration/deprecated-configuration-api.h"
#include "contacts/contact-manager.h"
#include "core/version-service.h"
#include "formatted-string/composite-formatted-string.h"
#include "network/proxy/network-proxy-manager.h"
#include "plugin/plugin-injected-factory.h"
#include "qt/long-validator.h"
#include "status/status-type-data.h"
#include "status/status-type-manager.h"
#include "status/status-type.h"
#include "status/status.h"
#include "windows/message-dialog.h"
#include "windows/open-chat-with/open-chat-with-runner-manager.h"

#include "core/core.h"
#include "icons/icons-manager.h"
#include "misc/misc.h"

#include "open-chat-with/gadu-open-chat-with-runner.h"
#include "server/gadu-servers-manager.h"
#include "server/protocol-gadu-connection.h"
#include "socket-notifiers/gadu-protocol-socket-notifiers.h"

#include "gadu-account-data.h"
#include "helpers/gadu-protocol-helper.h"
#include "helpers/gadu-proxy-helper.h"
#include "server/gadu-writable-session-token.h"
#include "services/drive/gadu-drive-service.h"
#include "services/gadu-contact-avatar-service.h"
#include "services/gadu-imtoken-service.h"
#include "services/gadu-notify-service.h"
#include "services/gadu-roster-service.h"
#include "services/user-data/gadu-user-data-service.h"

#include "gadu-protocol.h"
#include "gadu-protocol.moc"

GaduProtocol::GaduProtocol(
    GaduListHelper *gaduListHelper, GaduServersManager *gaduServersManager, Account account, ProtocolFactory *factory)
        : Protocol(account, factory), m_gaduServersManager{gaduServersManager}, ActiveServer(), GaduLoginParams(),
          GaduSession(0), SocketNotifiers(0), PingTimer(0), m_gaduListHelper{gaduListHelper}
{
}

GaduProtocol::~GaduProtocol()
{
    OpenChatWithRunnerManager::instance()->unregisterRunner(OpenChatRunner);
    delete OpenChatRunner;
    OpenChatRunner = 0;

    disconnect(account(), 0, this, 0);
}

void GaduProtocol::setAggregatedAccountAvatarService(AggregatedAccountAvatarService *aggregatedAccountAvatarService)
{
    m_aggregatedAccountAvatarService = aggregatedAccountAvatarService;
}

void GaduProtocol::setAggregatedContactAvatarService(AggregatedContactAvatarService *aggregatedContactAvatarService)
{
    m_aggregatedContactAvatarService = aggregatedContactAvatarService;
}

void GaduProtocol::setChatServiceRepository(ChatServiceRepository *chatServiceRepository)
{
    m_chatServiceRepository = chatServiceRepository;
}

void GaduProtocol::setChatStateServiceRepository(ChatStateServiceRepository *chatStateServiceRepository)
{
    m_chatStateServiceRepository = chatStateServiceRepository;
}

void GaduProtocol::setConfiguration(Configuration *configuration)
{
    m_configuration = configuration;
}

void GaduProtocol::setIconsManager(IconsManager *iconsManager)
{
    m_iconsManager = iconsManager;
}

void GaduProtocol::setNetworkProxyManager(NetworkProxyManager *networkProxyManager)
{
    m_networkProxyManager = networkProxyManager;
}

void GaduProtocol::setPluginInjectedFactory(PluginInjectedFactory *pluginInjectedFactory)
{
    m_pluginInjectedFactory = pluginInjectedFactory;
}

void GaduProtocol::setVersionService(VersionService *versionService)
{
    m_versionService = versionService;
}

void GaduProtocol::init()
{
    Connection = new ProtocolGaduConnection(this);
    Connection->setConnectionProtocol(this);

    m_gaduAccountAvatarService = new GaduAccountAvatarService(account(), this);

    CurrentBuddyListSerializationService = new GaduBuddyListSerializationService{m_gaduListHelper, account(), this};

    CurrentChatImageService = new GaduChatImageService(account(), this);
    CurrentChatImageService->setConnection(Connection);

    CurrentImTokenService = new GaduIMTokenService{this};

    CurrentFileTransferService = pluginInjectedFactory()->makeInjected<GaduFileTransferService>(this);
    CurrentFileTransferService->setGaduIMTokenService(CurrentImTokenService);

    CurrentChatService = pluginInjectedFactory()->makeInjected<GaduChatService>(account(), this);
    CurrentChatService->setConnection(Connection);
    CurrentChatService->setGaduChatImageService(CurrentChatImageService);
    CurrentChatService->setGaduFileTransferService(CurrentFileTransferService);
    CurrentChatImageService->setGaduChatService(CurrentChatService);

    CurrentContactPersonalInfoService =
        pluginInjectedFactory()->makeInjected<GaduContactPersonalInfoService>(account(), this);
    CurrentContactPersonalInfoService->setConnection(Connection);

    CurrentPersonalInfoService = pluginInjectedFactory()->makeInjected<GaduPersonalInfoService>(account(), this);
    CurrentPersonalInfoService->setConnection(Connection);

    CurrentSearchService = pluginInjectedFactory()->makeInjected<GaduSearchService>(account(), this);
    CurrentSearchService->setConnection(Connection);

    CurrentMultilogonService = new GaduMultilogonService(account(), this);
    CurrentMultilogonService->setConnection(Connection);

    CurrentChatStateService = pluginInjectedFactory()->makeInjected<GaduChatStateService>(account(), this);
    CurrentChatStateService->setConnection(Connection);

    m_gaduContactAvatarService = pluginInjectedFactory()->makeInjected<GaduContactAvatarService>(account(), this);

    connect(
        CurrentChatService, SIGNAL(messageReceived(Message)), CurrentChatStateService, SLOT(messageReceived(Message)));

    CurrentDriveService = pluginInjectedFactory()->makeInjected<GaduDriveService>(account(), this);
    CurrentDriveService->setGaduIMTokenService(CurrentImTokenService);

    CurrentUserDataService =
        pluginInjectedFactory()->makeInjected<GaduUserDataService>(m_gaduContactAvatarService, account(), this);

    auto contacts = contactManager()->contacts(account(), ContactManager::ExcludeAnonymous);
    auto rosterService = pluginInjectedFactory()->makeInjected<GaduRosterService>(m_gaduListHelper, contacts, this);
    rosterService->setConnection(Connection);

    CurrentNotifyService = new GaduNotifyService{Connection, this};
    connect(rosterService, SIGNAL(contactAdded(Contact)), CurrentNotifyService, SLOT(contactAdded(Contact)));
    connect(rosterService, SIGNAL(contactRemoved(Contact)), CurrentNotifyService, SLOT(contactRemoved(Contact)));
    connect(
        rosterService, SIGNAL(contactUpdatedLocally(Contact)), CurrentNotifyService,
        SLOT(contactUpdatedLocally(Contact)));

    setRosterService(rosterService);

    configureServices();

    connect(account(), SIGNAL(updated()), this, SLOT(accountUpdated()));

    OpenChatRunner = m_pluginInjectedFactory->makeInjected<GaduOpenChatWithRunner>(account());
    OpenChatWithRunnerManager::instance()->registerRunner(OpenChatRunner);

    m_aggregatedAccountAvatarService->add(m_gaduAccountAvatarService);
    m_aggregatedContactAvatarService->add(m_gaduContactAvatarService);
    m_chatServiceRepository->addChatService(CurrentChatService);
    m_chatStateServiceRepository->addChatStateService(CurrentChatStateService);
}

void GaduProtocol::done()
{
    m_chatStateServiceRepository->removeChatStateService(CurrentChatStateService);
    m_chatServiceRepository->removeChatService(CurrentChatService);
    m_aggregatedContactAvatarService->remove(m_gaduContactAvatarService);
    m_aggregatedAccountAvatarService->remove(m_gaduAccountAvatarService);
}

int GaduProtocol::maxDescriptionLength()
{
    return GG_STATUS_DESCR_MAXSIZE;
}

void GaduProtocol::setStatusFlags()
{
    if (!GaduSession)
        return;

    auto data = GaduAccountData{account()};

    int statusFlags = GG_STATUS_FLAG_UNKNOWN;
    if (!data.receiveSpam())
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

    m_lastSentStatus = newStatus;
    auto writableSessionToken = Connection->writableSessionToken();
    if (hasDescription)
        gg_change_status_descr(
            writableSessionToken.rawSession(), type | friends, newStatus.description().toUtf8().constData());
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
    socketConnFailed(ConnectionTimeout);
}

void GaduProtocol::everyMinuteActions()
{
    auto writableSessionToken = Connection->writableSessionToken();
    gg_ping(writableSessionToken.rawSession());
}

void GaduProtocol::configureServices()
{
    auto accountData = GaduAccountData{account()};

    CurrentChatStateService->setSendTypingNotifications(accountData.sendTypingNotification());

    switch (statusTypeManager()->statusTypeData(status().type()).typeGroup())
    {
    case StatusTypeGroup::Offline:
        CurrentChatImageService->setReceiveImages(false);
        break;
    case StatusTypeGroup::Invisible:
        CurrentChatImageService->setReceiveImages(accountData.receiveImagesDuringInvisibility());
        break;
    default:
        CurrentChatImageService->setReceiveImages(true);
        break;
    }
}

void GaduProtocol::accountUpdated()
{
    sendStatusToServer();
    configureServices();
}

void GaduProtocol::connectSocketNotifiersToServices()
{
    connect(
        SocketNotifiers, SIGNAL(msgEventReceived(gg_event *)), CurrentChatService, SLOT(handleEventMsg(gg_event *)));
    connect(
        SocketNotifiers, SIGNAL(multilogonMsgEventReceived(gg_event *)), CurrentChatService,
        SLOT(handleEventMultilogonMsg(gg_event *)));
    connect(
        SocketNotifiers, SIGNAL(ackEventReceived(gg_event *)), CurrentChatService, SLOT(handleEventAck(gg_event *)));
    connect(
        SocketNotifiers, SIGNAL(typingNotificationEventReceived(gg_event *)), CurrentChatStateService,
        SLOT(handleEventTypingNotify(gg_event *)));
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

    auto accountData = GaduAccountData{account()};
    if (0 == accountData.uin())
    {
        connectionClosed();
        return;
    }

    GaduProxyHelper::setupProxy(
        account().useDefaultProxy() ? m_networkProxyManager->defaultProxy() : account().proxy());

    setupLoginParams();

    m_lastSentStatus = loginStatus();
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
    PingTimer = new QTimer(0);
    connect(PingTimer, SIGNAL(timeout()), this, SLOT(everyMinuteActions()));
    PingTimer->start(60000);

    loggedIn();
}

void GaduProtocol::afterLoggedIn()
{
    m_gaduContactAvatarService->download(ContactAvatarId{{account().accountContact().id().toUtf8()}, {}});

    auto contacts = contactManager()->contacts(account(), ContactManager::ExcludeAnonymous);
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
        SocketNotifiers->watchFor(0);   // stop watching
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

    auto accountData = GaduAccountData{account()};

    GaduLoginParams.uin = account().id().toULong();
    GaduLoginParams.password = qstrdup(account().password().toUtf8().constData());

    GaduLoginParams.async = 1;

    auto gaduStatus = GaduProtocolHelper::gaduStatusFromStatus(loginStatus());
    GaduLoginParams.status = gaduStatus | (account().privateStatus() ? GG_STATUS_FRIENDS_MASK : 0);

    if (!loginStatus().description().isEmpty())
        GaduLoginParams.status_descr = qstrdup(loginStatus().description().toUtf8().constData());

    GaduLoginParams.tls = GG_SSL_REQUIRED;

    ActiveServer = m_gaduServersManager->getServer();

    bool haveServer = !ActiveServer.first.isNull();
    GaduLoginParams.server_addr = haveServer ? htonl(ActiveServer.first.toIPv4Address()) : 0;
    GaduLoginParams.server_port = haveServer ? ActiveServer.second : 0;

    GaduLoginParams.protocol_version = GG_PROTOCOL_VERSION_110;
    GaduLoginParams.compatibility = GG_COMPAT_LEGACY;   // TODO: #2961
    GaduLoginParams.client_version = qstrdup(m_versionService->nameWithVersion().toUtf8().constData());
    GaduLoginParams.protocol_features =
        GG_FEATURE_DND_FFC | GG_FEATURE_TYPING_NOTIFICATION | GG_FEATURE_MULTILOGON | GG_FEATURE_USER_DATA;

    GaduLoginParams.encoding = GG_ENCODING_UTF8;

    GaduLoginParams.has_audio = false;
    GaduLoginParams.last_sysmsg = m_configuration->deprecatedApi()->readNumEntry("General", "SystemMsgIndex", 1389);

    GaduLoginParams.image_size =
        qMax(qMin(m_configuration->deprecatedApi()->readNumEntry("Chat", "MaximumImageSizeInKiloBytes", 255), 255), 0);

    GaduLoginParams.struct_size = sizeof(struct gg_login_params);

    setStatusFlags();
}

void GaduProtocol::cleanUpLoginParams()
{
    if (GaduLoginParams.password)
    {
        memset(GaduLoginParams.password, 0, qstrlen(GaduLoginParams.password));
        delete[] GaduLoginParams.password;
        GaduLoginParams.password = 0;
    }

    delete[] GaduLoginParams.client_version;
    GaduLoginParams.client_version = 0;

    delete[] GaduLoginParams.status_descr;
    GaduLoginParams.status_descr = 0;
}

void GaduProtocol::socketContactStatusChanged(
    UinType uin, unsigned int ggStatusId, const QString &description, unsigned int maxImageSize)
{
    auto newStatus = Status{};
    newStatus.setType(GaduProtocolHelper::statusTypeFromGaduStatus(ggStatusId));
    newStatus.setDescription(description);

    if (uin == GaduLoginParams.uin)
    {
        if ((!m_lastRemoteStatusRequest.isValid() || m_lastRemoteStatusRequest.elapsed() > 10) &&
            newStatus != m_lastSentStatus)
        {
            emit remoteStatusChangeRequest(account(), newStatus);
            if (m_lastRemoteStatusRequest.isValid())
                m_lastRemoteStatusRequest.restart();
            else
                m_lastRemoteStatusRequest.start();
        }
        return;
    }

    auto contact = contactManager()->byId(account(), QString::number(uin), ActionReturnNull);
    contact.setMaximumImageSize(maxImageSize);

    auto oldStatus = contact.currentStatus();
    contact.setCurrentStatus(newStatus);
    contact.setBlocking(GaduProtocolHelper::isBlockingStatus(ggStatusId));

    if (contact.isAnonymous())
    {
        if (contact.ownerBuddy())
            emit userStatusChangeIgnored(contact.ownerBuddy());
        rosterService()->removeContact(contact);
        return;
    }

    // see issue #2159 - we need a way to ignore first status of given contact
    if (contact.ignoreNextStatusChange())
        contact.setIgnoreNextStatusChange(false);
    else
        emit contactStatusChanged(contact, oldStatus);
}

void GaduProtocol::socketConnFailed(GaduError error)
{
    QString msg = GaduProtocolHelper::connectionErrorMessage(error);

    switch (error)
    {
    case ConnectionNeedEmail:
        MessageDialog::show(m_iconsManager->iconByPath(KaduIcon("dialog-warning")), tr("Kadu"), msg);
        break;
    case ConnectionIncorrectPassword:
        passwordRequired();

        // do not call connectionClosed here
        // we dont need to do that, because no connection is open here
        // and we should not do that, because closing connection causes changing of state to offline
        // and it break our state machine, so we would be out of password-required state
        return;
    default:   // we need special code only for 2 cases
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
        emit connectionError(account(), host, msg);
    }

    if (!GaduProtocolHelper::isConnectionErrorFatal(error))
    {
        m_gaduServersManager->markServerAsBad(ActiveServer);
        logout();
        connectionError();
    }
    else
    {
        logout();
        connectionClosed();
    }
}

void GaduProtocol::disconnectedFromServer()
{
    connectionClosed();
}

QString GaduProtocol::statusPixmapPath()
{
    return QStringLiteral("gadu-gadu");
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

GaduDriveService *GaduProtocol::driveService() const
{
    return CurrentDriveService;
}

GaduUserDataService *GaduProtocol::userDataService() const
{
    return CurrentUserDataService;
}
