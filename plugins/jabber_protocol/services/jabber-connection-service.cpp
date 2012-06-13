/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2009, 2010, 2011 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * %kadu copyright end%
 *
 * Copyright (C) 2006 Remko Troncon
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

#include <QtCore/QUrl>

#include <bsocket.h>
#include <xmpp_tasks.h>

#include "identities/identity.h"
#include "network/proxy/network-proxy-manager.h"

#include "certificates/certificate-helpers.h"
#include "jabber-account-details.h"
#include "jabber-protocol.h"

#include "jabber-connection-service.h"

namespace XMPP
{

JabberConnectionService::JabberConnectionService(JabberProtocol *protocol) :
		QObject(protocol), ParentProtocol(protocol),
		AccountDetails(dynamic_cast<JabberAccountDetails *>(protocol->account().details())),
		XmppClient(protocol->xmppClient()), Connector(0), TLSHandler(0)
{
}

JabberConnectionService::~JabberConnectionService()
{
}

bool JabberConnectionService::forceTLS() const
{
	return JabberAccountDetails::Encryption_No != AccountDetails->encryptionMode();
}

bool JabberConnectionService::useSSL() const
{
	return JabberAccountDetails::Encryption_Legacy == AccountDetails->encryptionMode();
}

AdvancedConnector::Proxy JabberConnectionService::createProxyConfiguration(NetworkProxy proxy) const
{
	XMPP::AdvancedConnector::Proxy proxySettings;

	if (proxy.type() == "http") // HTTP Connect
		proxySettings.setHttpConnect(proxy.address(), proxy.port());
	else if (proxy.type() == "socks") // SOCKS
		proxySettings.setSocks(proxy.address(), proxy.port());
	else if (proxy.type() == "poll") // HTTP Poll
	{
		QUrl pollingUrl = proxy.pollingUrl();
		if (pollingUrl.queryItems().isEmpty())
		{
			if (AccountDetails->useCustomHostPort())
			{
				QString host = AccountDetails->customHost().isEmpty() ? MyJid.domain() : AccountDetails->customHost();
				pollingUrl.addQueryItem("server", host + ':' + QString::number(AccountDetails->customPort()));
			}
			else
				pollingUrl.addQueryItem("server", MyJid.domain());
		}
		proxySettings.setHttpPoll(proxy.address(), proxy.port(), pollingUrl.toString());
		proxySettings.setPollInterval(2);
	}

	if (!proxy.user().isEmpty())
		proxySettings.setUserPass(proxy.user(), proxy.password());

	return proxySettings;
}

AdvancedConnector * JabberConnectionService::createConnector()
{
	AdvancedConnector *result = new AdvancedConnector(this);
	result->setOptSSL(useSSL());

	if (AccountDetails->useCustomHostPort())
		result->setOptHostPort(AccountDetails->customHost(), AccountDetails->customPort());

	NetworkProxy proxy = ParentProtocol->account().useDefaultProxy()
			? NetworkProxyManager::instance()->defaultProxy()
			: ParentProtocol->account().proxy();

	if (proxy && !proxy.address().isEmpty())
		result->setProxy(createProxyConfiguration(proxy));

	return result;
}

QCATLSHandler * JabberConnectionService::createTLSHandler()
{
	QCA::TLS *tls = new QCA::TLS(this);
	tls->setTrustedCertificates(CertificateHelpers::allCertificates(CertificateHelpers::getCertificateStoreDirs()));

	QCATLSHandler *result = new QCATLSHandler(tls);
	result->setXMPPCertCheck(true);

	return result;
}

ClientStream::AllowPlainType JabberConnectionService::plainAuthToXMPP(JabberAccountDetails::AllowPlainType type)
{
	if (type == JabberAccountDetails::NoAllowPlain)
		return XMPP::ClientStream::NoAllowPlain;
	if (type == JabberAccountDetails::AllowPlain)
		return XMPP::ClientStream::AllowPlain;
	else
		return XMPP::ClientStream::AllowPlainOverTLS;
}

ClientStream * JabberConnectionService::createClientStream(AdvancedConnector *connector, QCATLSHandler *tlsHandler) const
{
	ClientStream *result = new XMPP::ClientStream(connector, tlsHandler);
	result->setNoopTime(55000); // send noop every 55 seconds
	result->setAllowPlain(plainAuthToXMPP(AccountDetails->plainAuthMode()));

	return result;
}

void JabberConnectionService::tlsHandshaken()
{
	QString domain = AccountDetails->tlsOverrideDomain();
	QString host = AccountDetails->useCustomHostPort() ? AccountDetails->customHost() : XMPP::Jid(ParentProtocol->account().id()).domain();
	QByteArray cert = AccountDetails->tlsOverrideCert();

	// TODO: use signal for checking certificates
	if (CertificateHelpers::checkCertificate(TLSHandler->tls(), TLSHandler, domain,
		QString("%1: ").arg(ParentProtocol->account().accountIdentity().name()) + tr("security problem"), host, ParentProtocol))
		TLSHandler->continueAfterHandshake();
	else
		emit connectionClosed(tr("TLS certificate not accepted"));
}

void JabberConnectionService::streamNeedAuthParams(bool user, bool pass, bool realm)
{
	if (user)
		Stream->setUsername(MyJid.node());

	if (pass)
		Stream->setPassword(Password);

	if (realm)
		Stream->setRealm(MyJid.domain());

	Stream->continueAfterParams();
}

void JabberConnectionService::streamAuthenticated()
{
	Connector->changePollInterval(10); // for http poll, slow down after login

	// Update our jid(if necessary)
	if (!Stream->jid().isEmpty())
		MyJid = Stream->jid();

	// get IP address
	ByteStream *bs = Connector->stream();
	if (!bs)
	{
		emit connectionError(tr("Unknown error"));
		return;
	}

	if (bs->inherits("BSocket") || bs->inherits("XMPP::BSocket"))
		LocalAddress = ((BSocket *)bs)->address().toString();

	// start the client operation
	XmppClient.data()->start(MyJid.domain(), MyJid.node(), Password, MyJid.resource());

	if (!Stream->old())
	{
		XMPP::JT_Session *j = new XMPP::JT_Session(XmppClient.data()->rootTask());
		QObject::connect(j, SIGNAL(finished()), this, SLOT(streamSessionStarted()));
		j->go(true);
	}
	else
		emit connected();
}

void JabberConnectionService::streamSessionStarted()
{
	XMPP::JT_Session *j = static_cast<XMPP::JT_Session *>(sender());
	if (j->success())
		emit connected();
	else
		emit connectionError(tr("Unknown error"));
}

void JabberConnectionService::streamWarning(int warning)
{
	if ((warning == ClientStream::WarnNoTLS) && forceTLS())
		emit connectionError(tr("The server does not support TLS encryption."));
	else
		Stream->continueAfterWarning();
}

void JabberConnectionService::streamError(int error)
{
	if ((error == XMPP::ClientStream::ErrAuth) && (Stream->errorCondition() == XMPP::ClientStream::NotAuthorized))
	{
		emit invalidPassword();
		return;
	}

	QString errorText;
	bool reconn;

	XMPP::JabberClient::getErrorInfo(error, Connector, Stream, TLSHandler, &errorText, &reconn);

	if (reconn)
		emit connectionError(tr("There was an error communicating with the server.\nDetails: %1").arg(errorText));
	else
		emit connectionClosed(tr("There was an error communicating with the server.\nDetails: %1").arg(errorText));
}

void JabberConnectionService::connectToServer()
{
	if (!XmppClient)
		return;

	if ((forceTLS() || useSSL()) && !QCA::isSupported("tls"))
	{
		emit connectionError(tr("SSL support could not be initialized for account %1. This is most likely because the QCA TLS plugin is not installed on your system."));
		return;
	}

	MyJid = XMPP::Jid(ParentProtocol->account().id()).withResource(AccountDetails->resource());
	Password = ParentProtocol->account().password();

	Connector = createConnector();

	if (forceTLS() || useSSL())
	{
		TLSHandler = createTLSHandler();
		connect(TLSHandler, SIGNAL(tlsHandshaken()), SLOT(tlsHandshaken()));

		QString host = AccountDetails->useCustomHostPort() ? AccountDetails->customHost() : XMPP::Jid(ParentProtocol->account().id()).domain();
		TLSHandler->startClient(host);
	}

	Stream = createClientStream(Connector, TLSHandler);
	connect(Stream, SIGNAL(needAuthParams(bool, bool, bool)), this, SLOT(streamNeedAuthParams(bool, bool, bool)));
	connect(Stream, SIGNAL(authenticated()), this, SLOT(streamAuthenticated()));
	connect(Stream, SIGNAL(connectionClosed()), this, SIGNAL(connectionClosed()));
	connect(Stream, SIGNAL(delayedCloseFinished()), this, SIGNAL(connectionClosed()));
	connect(Stream, SIGNAL(warning(int)), this, SLOT(streamWarning(int)));
	connect(Stream, SIGNAL(error(int)), this, SLOT(streamError(int)));

	XmppClient.data()->connectToServer(Stream, MyJid, true);
}

}
