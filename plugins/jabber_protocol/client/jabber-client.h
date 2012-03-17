/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2009, 2010, 2011 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#ifndef JABBER_CLIENT_H
#define JABBER_CLIENT_H

#include <QtCore/QObject>

#include <accounts/account.h>

// include these because of namespace reasons
#include <iris/im.h>
#include <iris/xmpp.h>

#include <QtCrypto>

namespace XMPP
{
	class AdvancedConnector;
}

class JabberProtocol;
class PEPManager;
class ServerInfoManager;

/**
 * This class provides an interface to the Iris subsystem. The goal is to
 * abstract the Iris layer and manage it via a single, simple to use class.
 * By default, @ref JabberClient will attempt to establish a connection
 * using XMPP 1.0. This means that apart from the JID and password, no
 * further details are necessary to connect. The server and port will be
 * determined using a SRV lookup. If TLS is possible (meaning, the TLS
 * plugin is available and the server supports TLS) it will automatically
 * be used. Otherwise, a non-encrypted connection will be established.
 * If XMPP 1.0 is not possible, the connection will fall back to the old
 * protocol. By default, this connection is not encrypted. You can, however,
 * use @ref setUseSSL to immediately attempt an SSL connection. This is
 * most useful if you want to establish an SSL connection to a non-standard
 * port, in which case you will also have to use @ref setOverrideHost. In case
 * XMPP 1.0 does not work, an automatic attempt to connect to the standard port
 * 5223 with SSL can be made with @ref setProbeSSL. If the attempt is not
 * sucessful, the connection will fall back to an unencrypted attempt
 * at port 5222.
 * @brief Provides a Jabber client
 * @author Till Gerken
 */

namespace XMPP
{
	class JabberClient : public QObject
	{
		Q_OBJECT

		// connection details
		XMPP::Jid MyJid;
		QString Password;

		// XMPP backend
		XMPP::Client *Client;
		XMPP::ClientStream *JabberClientStream;
		XMPP::AdvancedConnector *JabberClientConnector;
		QCA::TLS *JabberTLS;
		XMPP::QCATLSHandler *JabberTLSHandler;

		JabberProtocol *Protocol;

		ServerInfoManager *serverInfoManager;
		PEPManager *PepManager;

		// ignore TLS warnings
		bool IgnoreTLSWarnings;

		// local IP address
		QString LocalAddress;

		// whether TLS (or direct SSL in case of the old protocol) should be used
		bool ForceTLS;

		// whether direct SSL connections should be used
		bool UseSSL;

		// use XMPP 1.0 or the older protocol version
		bool UseXMPP09;

		// whether SSL support should be probed in case the old protocol is used
		bool ProbeSSL;

		// override the default server name and port (only pre-XMPP 1.0)
		bool OverrideHost;
		QString Server;
		int Port;

		// allow transmission of plaintext passwords
		XMPP::ClientStream::AllowPlainType AllowPlainTextPassword;

		// current penalty time
		int CurrentPenaltyTime;

		// client information
		QString ClientName, ClientVersion, OsName;

		// Caps(JEP-0115: Entity Capabilities) information
		QString CapsNode, CapsVersion;
		DiscoItem::Identity DiscoIdentity;

		bool PepAvailable;

		void setPEPAvailable(bool b);

		// TODO 0.10: move this to proper place:
		QString calculateCapsVersion(const DiscoItem::Identity &identity, const QStringList &features);
		void updateClientInfo();

	private slots:
                /**
                 * Delete all member classes and reset the class to a predefined state.
                 */
                void cleanUp();

		/* update the penalty timer */
		void slotUpdatePenaltyTime();

		/* Login if the connection was OK. */
		void slotCSNeedAuthParams(bool user, bool pass, bool realm);

		/* Called from Psi: tells us when we're logged in OK. */
		void slotCSAuthenticated();

		/* Called from Psi: tells us when we've been disconnected from the server. */
		void slotCSDisconnected();

		/* Called from Psi: alerts us to a protocol warning. */
		void slotCSWarning(int);

		/* Called from Psi: alerts us to a protocol error. */
		void slotCSError(int);

		/* Called from Psi: report certificate status */
		void slotTLSHandshaken();

		/* Someone on our contact list had(another) resource come online. */
		void slotResourceAvailable(const Jid &, const Resource &);

		/* Someone on our contact list had a resource go offline. */
		void slotResourceUnavailable(const Jid &, const Resource &);

		/* Called from Psi: debug messages from the backend. */
		void slotPsiDebug(const QString &msg);
		void slotIncomingXML(const QString &msg);
		void slotOutgoingXML(const QString &msg);

		/* Incoming subscription request. */
		void slotSubscription(const Jid &jid, const QString &type, const QString &nick);

		void serverFeaturesChanged();

		void sessionStart_finished();

	public:
		/**
		 * Error codes indicating problems during operation.
		 */
		enum ErrorCode
		{
			Ok,					/** No error. */
			InvalidPassword,	/** Password used to connect to the server was incorrect. */
			AlreadyConnected,	/** A new connection was attempted while the previous one hasn't been closed. */
			NoTLS,				/** Use of TLS has been forced (see @ref forceTLS) but TLS is not available, either server- or client-side. */
			InvalidPasswordForMUC = 401,	/** A password is require to enter on this Multi-User Chat /
			NicknameConflict = 409,		** There is already someone with that nick connected to the Multi-User Chat /
			BannedFromThisMUC = 403,	** You can't join this Multi-User Chat because you were bannished */
			MaxUsersReachedForThisMuc = 503	/** You can't join this Multi-User Chat because it is full */
									};

		explicit JabberClient(JabberProtocol *protocol, QObject *parent = 0);
		virtual ~JabberClient();

		/**
		 * Connect to a Jabber server.
		 * @param jid JID to connect to.
		 * @param password Password to authenticate with.
		 * @param auth True if authentication should be done, false if not.
		 */
		void connect(const XMPP::Jid &jid, const QString &password, bool auth = true);

		/**
		 * Disconnect from Jabber server.
		 */
		void disconnect();

		/**
		 * Disconnect from Jabber server with reason
		 * @param reason The reason for disconnecting
		 */
		void disconnect(XMPP::Status &reason);

		/**
		 * Returns if this instance is connected to a server.
		 */
		bool isConnected() const;

		void setPresence(const XMPP::Status &status);

		/**
		 * Returns the JID associated with this instance.
		 */
		const XMPP::Jid & jid() const { return MyJid; }

		/**
		 * Set flag to ignore TLS warnings. If TLS
		 * warnings are not ignored, the class will emit
		 * @ref tlsWarning and wait for the user to
		 * call @ref continueAfterTLSWarning or
		 * @ref disconnect. Default is false.
		 */
		void setIgnoreTLSWarnings(bool flag) { IgnoreTLSWarnings = flag; }
		/**
		 * Return if TLS warnings are being ignored.
		 */
		bool ignoreTLSWarnings() { return IgnoreTLSWarnings; }

		/**
		 * Force the use of TLS. If TLS connections are forced,
		 * unencrypted connections will not be established.
		 * Default is false.
		 */
		void setForceTLS(bool flag) { ForceTLS = flag; }

		/**
		 * Returns if TLS connections are forced.
		 */
		bool forceTLS() const { return ForceTLS; }

		/**
		 * Force direct SSL connection, also for the
		 * handshake. This is only useful if you know
		 * the server supports it or you want to use
		 * a non-standard port, in which case @ref setOverrideHost
		 * will be useful. Default is false.
		 */
		void setUseSSL(bool flag) { UseSSL = flag; }
		/**
		 * Returns if an SSL connection attempt should be made.
		 */
		bool useSSL() const { return UseSSL; }

		/**
		 * Use only the old protocol (pre-XMPP 1.0). This should only
		 * be used with servers not supporting XMPP 1.0 or with servers
		 * that have a broken login procedure. Default is false. If
		 * a connection attempt is not possible, Iris will automatically
		 * fall back to the old protocol.
		 */
		void setUseXMPP09(bool flag) { UseXMPP09 = flag; }

		/**
		 * Returns if the old protocol should be used.
		 */
		bool useXMPP09() const { return UseXMPP09; }

		/**
		 * Probe port 5223 if an SSL connection is possible. If
		 * a connection is not possible, an unencrypted connection
		 * will be attempted at port 5222. This is only meaningful
		 * if @ref useXMPP09 is true. Default is false.
		 */
		void setProbeSSL(bool flag) { ProbeSSL = flag; }

		/**
		 * Returns if SSL support will be probed.
		 */
		bool probeSSL() const { return ProbeSSL; }

		/**
		 * Override the name and port of the server to connect to.
		 * This only has an effect if the old protocol (@ref useXMPP09)
		 * has been enabled. Default is false.
		 */
		void setOverrideHost(bool flag, const QString &server = QString(), int port = 5222);

		/**
		 * Returns if the server name and port are overridden.
		 */
		bool overrideHost() const { return OverrideHost; }

		/**
		 * Allow the transmission of a plain text password. If digested
		 * passwords are supported by the server, they will still be preferred.
		 * Defaults to true.
		 */
		void setAllowPlainTextPassword(XMPP::ClientStream::AllowPlainType flag) { AllowPlainTextPassword = flag; }

		/**
		* Returns if plain text passwords are allowed.
		 */
		XMPP::ClientStream::AllowPlainType allowPlainTextPassword() const { return AllowPlainTextPassword; }

		/**
		 * Returns the address of the local interface.
		 */
		const QString & localAddress() const { return LocalAddress; }

		/**
		 * Set client name.
		 */
		void setClientName(const QString &clientName) { ClientName = clientName; }
		/**
		 * Return client name.
		 */
		const QString & clientName() const { return ClientName; }

		/**
		 * Set client version.
		 */
		void setClientVersion(const QString &clientVersion) { ClientVersion = clientVersion; }
		/**
		 * Return client version.
		 */
		const QString & clientVersion() const { return ClientVersion; }

		/**
		 * Set operating system name.
		 */
		void setOSName(const QString &osName) { OsName = osName; }
		/**
		 * Return operating system name.
		 */
		const QString & osName() const { return OsName; }

		/**
		 * Set the caps(JEP-0115: Entity capabilities) node name.
		 * @param node Node name.
		 */
		void setCapsNode(const QString &capsNode) { CapsNode = capsNode; }

		/**
		 * Return the caps node name for this client.
		 * @return the caps node name.
		 */
		const QString & capsNode() const { return CapsNode;}

		/**
		 * Set the caps(JEP-0115: Entity capabilities) node version.
		 * @param capsVersion the node version.
		 */
		void setCapsVersion(const QString &capsVersion) { CapsVersion = capsVersion; }

		/**
		 * Return the caps version for this client.
		 * @return the caps version.
		 */
		const QString & capsVersion() const { return CapsVersion; }

		/**
		 * Return the caps extension list for this client.
		 * @return A string containing all extensions separated by space.
		 */
		QString capsExt() const { return Client ? Client->capsExt() : QString(); }

		/**
		 * Set the disco Identity information for this client.
		 * Create a Disco identity like this:
		 * @code
		 * DiscoItem::Identity identity;
		 * identity.category = "client";
		 * identity.type = "pc";
		 * identity.name = "Kopete";
		 * @endcode
		 *
		 * @param identity DiscoItem::Identity for the client.
		 */
		void setDiscoIdentity(const DiscoItem::Identity &identity) { DiscoIdentity = identity; }

		/**
		 * Get the disco Identity information for this client.
		 * @return the DiscoItem::Identity for this client.
		 */
		const DiscoItem::Identity & discoIdentity() const { return DiscoIdentity; }

		/**
		 * This method can be used to implement a penalty
		 * system when a lot of queries need to be sent to the
		 * server. Using the time returned by this method,
		 * the caller can determine a delay until the next
		 * operation in the queue can be carried out.
		 * @brief Return current penalty time in seconds.
		 */
		int getPenaltyTime();

		/**
		 * Return the XMPP client instance.
		 */
		XMPP::Client *client() const { return Client; }

		/**
		 * Return client stream instance.
		 */
		XMPP::ClientStream *clientStream() const { return JabberClientStream; }

		/**
		 * Return client connector instance.
		 */
		XMPP::AdvancedConnector *clientConnector() const { return JabberClientConnector; }

		/**
		 * Get the root task for this connection.
		 * You need this instance for every task
		 * you want to start.
		 */
		XMPP::Task *rootTask() const { return client() ? client()->rootTask() : 0; }

		/**
		 * Send raw packet to the server.
		 */
		void send(const QString &packet);

		PEPManager *pepManager() { return PepManager; }
		bool isPEPAvailable() { return PepAvailable; }

		static void getErrorInfo(int err, AdvancedConnector *conn, Stream *stream, QCATLSHandler *tlsHandler, QString *_str, bool *_reconn);

		void changeSubscription(const XMPP::Jid &jid, const QString &type);
		void requestSubscription(const XMPP::Jid &jid);
		void resendSubscription(const XMPP::Jid &jid);
		void rejectSubscription(const XMPP::Jid &jid);


	signals:
		/**
		 * Connected successfully.
		 */
		void connected();

		/**
		 * Client stream authenticated. This
		 * signal is emitted when the socket
		 * connection has been successfully
		 * established, before sending the login
		 * packet.
		 */
		void csAuthenticated();

		void connectionError(const QString &message);
		void invalidPassword();

		/**
		 * Client stream error.
		 */
		void csError(int error);

		/**
		 * Client stream was disconnected.
		 */
		void csDisconnected();

		/**
		 * Fatal error has been encountered,
		 * further operations are not possible.
		 */
		void error(XMPP::JabberClient::ErrorCode code);

		/**
		 * New resource is available for a contact.
		 */
		void resourceAvailable(const XMPP::Jid &jid, const XMPP::Resource &resource);

		/**
		 * An existing resource has been removed.
		 */
		void resourceUnavailable(const XMPP::Jid &jid, const XMPP::Resource &resource);

		/**
		 * Groupchat has been joined.
		 */
		void groupChatJoined(const XMPP::Jid &jid);

		/**
		 * Groupchat has been left.
		 */
		void groupChatLeft(const XMPP::Jid &jid);

		/**
		 * A presence to a groupchat has been signalled.
		 */
		void groupChatPresence(const XMPP::Jid &jid, const XMPP::Status &status);

		/**
		 * An error was encountered joining or processing a groupchat.
		 */
		void groupChatError(const XMPP::Jid &jid, int error, const QString &reason);

		/**
		 * New subscription request.
		 */
		void subscription(const XMPP::Jid &jid, const QString &type, const QString &nick);

		/**
		 * Dispatches a debug message. Debug messages
		 * include incoming and outgoing XML packets
		 * as well as internal status messages.
		 */
		void debugMessage(const QString &message);
		void incomingXML(const QString &msg);
		void outgoingXML(const QString &msg);

		void publishSuccess(const QString&, const XMPP::PubSubItem&);
		void publishError(const QString&, const XMPP::PubSubItem&);
	};
}

#endif // JABBER_CLIENT_H
