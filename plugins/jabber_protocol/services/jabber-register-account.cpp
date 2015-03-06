/*
 * %kadu copyright begin%
 * Copyright 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "jabber-register-account.h"

#include "services/jabber-error-service.h"
#include "jid.h"

#include <qxmpp/QXmppClient.h>
#include <qxmpp/QXmppRegisterIq.h>

JabberRegisterAccount::JabberRegisterAccount(const QString &server, QObject *parent) :
		QObject{parent},
		m_server{server}
{
/*
	auto registerIq = QXmppRegisterIq{};
	registerIq.setPassword(newPassword);
	registerIq.setType(QXmppIq::Type::Set);
	registerIq.setUsername(Jid::parse(jid).node());

	m_id = registerIq.id();
	client->sendPacket(registerIq);
	connect(client, SIGNAL(iqReceived(QXmppIq)), this, SLOT(iqReceived(QXmppIq)));
*/
}

JabberRegisterAccount::~JabberRegisterAccount()
{
}

void JabberRegisterAccount::setErrorService(JabberErrorService *errorService)
{
	m_errorService = errorService;
}

void JabberRegisterAccount::start()
{
	if (m_client)
	{
		printf("already in progress\n");
	}

	auto configuration = QXmppConfiguration{};
	configuration.setAutoAcceptSubscriptions(false);
	configuration.setAutoReconnectionEnabled(false);
	configuration.setIgnoreSslErrors(true); // TODO: replace with setCaCertificated
	configuration.setDomain(m_server);
	configuration.setStreamSecurityMode(QXmppConfiguration::StreamSecurityMode::TLSRequired);
/*
	if (account().proxy())
	{
		auto proxy = QNetworkProxy{};
		if (account().proxy().type() == "socks")
			proxy.setType(QNetworkProxy::Socks5Proxy);
		else
			proxy.setType(QNetworkProxy::HttpProxy);

		proxy.setHostName(account().proxy().address());
		proxy.setPort(account().proxy().port());
		proxy.setUser(account().proxy().user());
		proxy.setPassword(account().proxy().password());
		configuration.setNetworkProxy(proxy);
	}

	if (details->useCustomHostPort())
	{
		configuration.setHost(details->customHost());
		configuration.setPort(details->customPort());
	}
*/
	m_client = new QXmppClient{this};
	m_client->logger()->setLoggingType(QXmppLogger::StdoutLogging);
	m_client->connectToServer(configuration);

	connect(m_client, SIGNAL(connected()), this, SLOT(askForRegistration()));
}

void JabberRegisterAccount::askForRegistration()
{
	auto registerIq = QXmppRegisterIq{};
	m_client->sendPacket(registerIq);
}

void JabberRegisterAccount::iqReceived(const QXmppIq &iq)
{
	Q_UNUSED(iq);
/*
	if (iq.id() != m_id)
		return;

	if (m_errorService->isErrorIq(iq))
	{
		auto conditionString = QString{};
		switch (iq.error().condition())
		{
			case QXmppStanza::Error::NotAuthorized:
				conditionString = tr("Current connection is not safe for password change. Use encrypted connection or change password on provider's site.");
				break;
			case QXmppStanza::Error::NotAllowed:
			case QXmppStanza::Error::FeatureNotImplemented:
				conditionString = tr("Password change is not allowed.");
			default:
				break;
		}
		emit error(m_errorService->errorMessage(iq, conditionString));
	}
	else if (iq.type() == QXmppIq::Type::Result)
		emit passwordChanged();

	deleteLater();
*/
}

#include "moc_jabber-register-account.cpp"
