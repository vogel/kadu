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

#include "qxmpp/jabber-register-extension.h"
#include "qxmpp/jabber-ssl-handler.h"
#include "services/jabber-error-service.h"
#include "jid.h"

#include "misc/memory.h"

#include <qxmpp/QXmppClient.h>
#include <qxmpp/QXmppRegisterIq.h>

JabberRegisterAccount::JabberRegisterAccount(Jid jid, QString password, QString email, QObject *parent) :
		QObject{parent},
		m_jid{std::move(jid)},
		m_password{std::move(password)},
		m_email{std::move(email)},
		m_state{State::None}
{
}

JabberRegisterAccount::~JabberRegisterAccount()
{
	if (m_client)
	{
		disconnect(m_client, nullptr, this, nullptr);
		m_client->deleteLater();
	}
}

void JabberRegisterAccount::setErrorService(JabberErrorService *errorService)
{
	m_errorService = errorService;
}

Jid JabberRegisterAccount::jid() const
{
	return m_jid;
}

void JabberRegisterAccount::start()
{
	if (m_client || m_state != State::None)
	{
		return;
	}

	auto configuration = QXmppConfiguration{};
	configuration.setAutoAcceptSubscriptions(false);
	configuration.setAutoReconnectionEnabled(false);
	configuration.setDomain(m_jid.domain());
	configuration.setIgnoreSslErrors(false);
	configuration.setStreamSecurityMode(QXmppConfiguration::StreamSecurityMode::TLSEnabled);
	configuration.setUseNonSASLAuthentication(false);
	configuration.setUseSASLAuthentication(false);
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

	m_registerExtension = make_unique<JabberRegisterExtension>();
	m_client = new QXmppClient{this};
	m_client->addExtension(m_registerExtension.get());

	new JabberSslHandler{m_client};

	m_client->connectToServer(configuration);

	connect(m_client, SIGNAL(connected()), this, SLOT(askForRegistration()));
	connect(m_client, SIGNAL(error(QXmppClient::Error)), this, SLOT(clientError(QXmppClient::Error)));
	connect(m_registerExtension.get(), SIGNAL(registerIqReceived(QXmppRegisterIq)), this, SLOT(registerIqReceived(QXmppRegisterIq)));

	m_state = State::Connecting;
}

void JabberRegisterAccount::clientError(QXmppClient::Error error)
{
	handleError(m_errorService->errorMessage(m_client, error));
}

void JabberRegisterAccount::handleError(const QString &errorMessage)
{
	disconnect(m_client, nullptr, this, nullptr);
	emit error(errorMessage);
	deleteLater();
}

void JabberRegisterAccount::handleSuccess()
{
	disconnect(m_client, nullptr, this, nullptr);
	emit success();
	deleteLater();
}

void JabberRegisterAccount::askForRegistration()
{
	if (m_state != State::Connecting)
	{
		handleError(tr("Internal error: invalid state"));
		return;
	}

	emit statusMessage(tr("Connection established."));
	emit statusMessage(tr("Negotiating registration parameters."));

	auto registerIq = QXmppRegisterIq{};
	registerIq.setType(QXmppIq::Type::Get);

	m_id = registerIq.id();
	m_client->sendPacket(registerIq);

	m_state = State::WaitForRegistrationForm;
}

void JabberRegisterAccount::registerIqReceived(const QXmppRegisterIq &registerIq)
{
	if (m_id != registerIq.id())
		return;

	if (m_errorService->isErrorIq(registerIq))
	{
		auto conditionString = QString{};
		switch (registerIq.error().condition())
		{
			case QXmppStanza::Error::Conflict:
				conditionString = tr("User with this username is already registered.");
				break;
			default:
				break;
		}

		handleError(m_errorService->errorMessage(registerIq, conditionString));
		return;
	}

	switch (m_state)
	{
		case State::WaitForRegistrationForm:
			handleRegistrationForm(registerIq);
			return;
		case State::WaitForRegistrationConfirmation:
			handleRegistrationConfirmation(registerIq);
			return;
		default:
			handleError(tr("Internal error: invalid state"));
			return;
	}

	emit statusMessage(tr("Registration parameters received."));
}

void JabberRegisterAccount::handleRegistrationForm(const QXmppRegisterIq &registerIq)
{
	if (!registerIq.form().isNull())
	{
		auto errorMessage = tr("Registration at this server requires XMPP Data Forms support. Kadu currently does not support XMPP Data Forms.");
		if (!registerIq.instructions().isEmpty())
			errorMessage = tr("%1\n\nServer message: %2").arg(errorMessage).arg(registerIq.instructions());
		handleError(errorMessage);
		return;
	}

	sendFilledRegistrationForm();
	m_state = State::WaitForRegistrationConfirmation;
}

void JabberRegisterAccount::sendFilledRegistrationForm()
{
	auto registerIq = QXmppRegisterIq{};
	registerIq.setEmail(m_email);
	registerIq.setPassword(m_password);
	registerIq.setType(QXmppIq::Type::Set);
	registerIq.setUsername(m_jid.node());

	m_id = registerIq.id();
	m_client->sendPacket(registerIq);
}

void JabberRegisterAccount::handleRegistrationConfirmation(const QXmppRegisterIq &registerIq)
{
	if (registerIq.type() == QXmppIq::Type::Result)
		handleSuccess();
	else
		handleError(tr("Unknown error: received stanza type %1").arg(registerIq.type()));
}

#include "moc_jabber-register-account.cpp"
