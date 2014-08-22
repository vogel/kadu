/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2009, 2010, 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2010, 2011, 2012, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011, 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

/*
 * miniclient.cpp
 * Copyright (C) 2001, 2002  Justin Karneges
 */

#include <QtCrypto>

#include "gui/windows/message-dialog.h"
#include "icons/kadu-icon.h"
#include "debug.h"

#include "certificates/certificate-helpers.h"
#include "jabber-error-helper.h"
#include "mini-client.h"
#include "xmpp_tasks.h"

using namespace XMPP;

MiniClient::MiniClient(QObject *parent)
:QObject(parent)
{
	_client = new Client;

		QObject::connect ( _client, SIGNAL ( xmlIncoming(const QString& ) ),
				   this, SLOT ( slotDebug(const QString &) ) );
		QObject::connect ( _client, SIGNAL ( xmlOutgoing(const QString& ) ),
				   this, SLOT ( slotDebug(const QString &) ) );
	conn = 0;
	tls = 0;
	tlsHandler = 0;
	stream = 0;
	auth = false;
	force_ssl = false;
	error_disconnect = true;
}

MiniClient::~MiniClient()
{
	delete _client;
	reset();
}

void MiniClient::reset()
{
	delete stream;
	stream = 0;

	delete tls;
	tls = 0;
	tlsHandler = 0;

	delete conn;
	conn = 0;
}

void MiniClient::connectToServer(const Jid &jid, bool legacy_ssl_probe, bool legacy_ssl, bool forcessl, const QString &_host, quint16 _port/*, ProxyManager *pm, QString proxy, QString *_pass*/)
{
	j = jid;

	QString host;
	quint16 port = -1;
	bool useHost = false;
	force_ssl = forcessl;
	if(!_host.isEmpty()) {
		useHost = true;
		host = _host;
		port = _port;
	}
/*
	AdvancedConnector::Proxy p;
	if(proxy != QString()) {
		const ProxyItem &pi = pm->getItem(proxy);
		if(pi.type == "http") // HTTP Connect
			p.setHttpConnect(pi.settings.host, pi.settings.port);
		else if(pi.type == "socks") // SOCKS
			p.setSocks(pi.settings.host, pi.settings.port);
		else if(pi.type == "poll") { // HTTP Poll
			QUrl u = pi.settings.url;
			if(u.queryItems().isEmpty()) {
				if (useHost)
					u.addQueryItem("server",host + ':' + QString::number(port));
				else
					u.addQueryItem("server",jid.host());
			}
			p.setHttpPoll(pi.settings., pi.settings.port, u.toString());
			p.setPollInterval(2);
		}

		if(pi.settings.useAuth)
			p.setUserPass(pi.settings.user, pi.settings.pass);
	}
*/

	conn = new AdvancedConnector;
	tls = new QCA::TLS;
	tls->setTrustedCertificates(CertificateHelpers::allCertificates(CertificateHelpers::getCertificateStoreDirs()));
	tlsHandler = new QCATLSHandler(tls);
	tlsHandler->setXMPPCertCheck(true);
	connect(tlsHandler, SIGNAL(tlsHandshaken()), SLOT(tls_handshaken()));
///	conn->setProxy(p);
	if (useHost) {
		conn->setOptHostPort(host, port);
		conn->setOptSSL(legacy_ssl);
	}
	else {
		conn->setOptProbe(legacy_ssl_probe);
	}

	stream = new ClientStream(conn, tlsHandler);
	connect(stream, SIGNAL(connected()), SLOT(cs_connected()));
	connect(stream, SIGNAL(securityLayerActivated(int)), SLOT(cs_securityLayerActivated(int)));
	connect(stream, SIGNAL(needAuthParams(bool, bool, bool)), SLOT(cs_needAuthParams(bool, bool, bool)));
	connect(stream, SIGNAL(authenticated()), SLOT(cs_authenticated()));
	connect(stream, SIGNAL(connectionClosed()), SLOT(cs_connectionClosed()));
	connect(stream, SIGNAL(delayedCloseFinished()), SLOT(cs_delayedCloseFinished()));
	connect(stream, SIGNAL(warning(int)), SLOT(cs_warning(int)));
	connect(stream, SIGNAL(error(int)), SLOT(cs_error(int)), Qt::QueuedConnection);
/*
	if(_pass) {
		auth = true;
		pass = *_pass;
		_client->connectToServer(stream, j);
	}
	else {
*/		auth = false;
		_client->connectToServer(stream, j, false);
//	}
}

void MiniClient::close()
{
	_client->close();
	reset();
}

Client *MiniClient::client()
{
	return _client;
}

void MiniClient::setErrorOnDisconnect(bool b)
{
	error_disconnect = b;
}

void MiniClient::tls_handshaken()
{
	// TODO: This (blocking=true) is probably not a very good idea. If the response from the user
	// is delayed, the connection will timeout and we will receive an error. JabberConnectionService
	// handles this by reporting error, stopping connection, and automatically redoing everything
	// again if the user decided to accept the certificate.
	if (CertificateHelpers::checkCertificate(tls, tlsHandler, TlsOverrideDomain, tr("Server Authentication"), j.domain(), true, 0, 0))
		tlsHandler->continueAfterHandshake();
	else {
		close();
		error();
	}
}

void MiniClient::cs_connected()
{
}

void MiniClient::cs_securityLayerActivated(int)
{
}

void MiniClient::cs_needAuthParams(bool user, bool password, bool realm)
{
	if(user)
		stream->setUsername(j.node());
	if(password)
		stream->setPassword(pass);
	if(realm)
		stream->setRealm(j.domain());
	stream->continueAfterParams();
}

void MiniClient::cs_authenticated()
{
	_client->start(j.domain(), j.node(), QString(), QString());

	if (!stream->old() && auth) {
		JT_Session *j = new JT_Session(_client->rootTask());
		connect(j,SIGNAL(finished()),SLOT(sessionStart_finished()));
		j->go(true);
	}
	else {
		handshaken();
	}
}

void MiniClient::sessionStart_finished()
{
	JT_Session *j = (JT_Session*)sender();
	if ( j->success() ) {
		handshaken();
	}
	else {
		cs_error(-1);
	}
}

void MiniClient::cs_connectionClosed()
{
	if (error_disconnect)
		cs_error(-1);
	else
		emit disconnected();
}

void MiniClient::cs_delayedCloseFinished()
{
}

void MiniClient::cs_warning(int err)
{
	if (err == ClientStream::WarnNoTLS && force_ssl) {
		close();
		MessageDialog::show(KaduIcon("dialog-error"), tr("Server Error"), tr("The server does not support TLS encryption."));
	}
	else {
		stream->continueAfterWarning();
	}
}

void MiniClient::cs_error(int err)
{
	QString str;
	bool reconn;
	JabberErrorHelper::getErrorInfo(err, conn, stream, tlsHandler, &str, &reconn);
	close();

	MessageDialog::show(KaduIcon("dialog-error"), tr("Server Error"), tr("There was an error communicating with the Jabber server.\nDetails: %1").arg(str));
	error();
}

void MiniClient::slotDebug(const QString &text)
{
	Q_UNUSED(text)

	kdebugm(KDEBUG_WARNING, "Jabber MiniClient debug:  %s\n", qPrintable(text));
}


#include "moc_mini-client.cpp"
