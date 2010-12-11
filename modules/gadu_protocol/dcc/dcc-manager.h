/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2004, 2006 Marcin Ślusarz (joi@kadu.net)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2005 Paweł Płuciennik (pawel_p@kadu.net)
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

#ifndef DCC_MANAGER_H
#define DCC_MANAGER_H

#include <libgadu.h>

#include <QtCore/QObject>
#include <QtCore/QTimer>
#include <QtNetwork/QHostAddress>

#include "protocols/protocol.h"

class DccSocketNotifiers;
class GaduContact;
class GaduFileTransferHandler;
class GaduProtocol;

enum DccVersion
{
	DccUnknown,
	Dcc6,
	Dcc7
};

class DccManager : public QObject
{
	Q_OBJECT

	GaduProtocol *Protocol;

	DccSocketNotifiers *MainSocketNotifiers;
	QList<DccSocketNotifiers *> SocketNotifiers;
	QList<GaduFileTransferHandler *> WaitingFileTransfers;

	bool DccEnabled;
	QHostAddress DccExternalIP;
	unsigned int DccExternalPort;
/*
	QWidget *ipAddress;
	QCheckBox *forwarding;
	QWidget *forwardingExternalIp;
	QWidget *forwardingExternalPort;
	QWidget *forwardingLocalPort;*/

	void setUpDcc();
	void closeDcc();

	void connectSocketNotifiers(DccSocketNotifiers *notifiers);
	void disconnectSocketNotifiers(DccSocketNotifiers *notifiers);

	GaduFileTransferHandler * findFileTransferHandler(DccSocketNotifiers *notifiers);

	friend class DccSocketNotifiers;
	void handleEventDccNew(struct gg_event *e);

	void connectionRequestReceived(Contact contact);
	bool acceptConnection(UinType uin, UinType peerUin, unsigned int peerAddr);
	void needIncomingFileTransferAccept(DccSocketNotifiers *socket);

	friend class GaduProtocolSocketNotifiers;
	void handleEventDcc7New(struct gg_event *e);
	void handleEventDcc7Accept(struct gg_event *e);
	void handleEventDcc7Reject(struct gg_event *e);
	void handleEventDcc7Pending(struct gg_event *e);
	void handleEventDcc7Error(struct gg_event *e);

// 	void onIpAutotetectToggled(bool toggled);

	void attachSendFileTransferSocket6(UinType uin, Contact contact, GaduFileTransferHandler *handler);
	void attachSendFileTransferSocket7(Contact contact, GaduFileTransferHandler *handler);

private slots:
	void socketNotifiersDestroyed(QObject *socketNotifiers);
	void fileTransferHandlerDestroyed(QObject *object);

protected:
	virtual void configurationUpdated();

public:
	DccManager(GaduProtocol *protocol);
	virtual ~DccManager();

	void setUpExternalAddress(gg_login_params &loginParams);

	void attachSendFileTransferSocket(GaduFileTransferHandler *handler);

// 	void getFileTransferSocket(uint32_t ip, uint16_t port, UinType myUin, UinType peerUin, DccHandler *handler, bool request = false);
// 	void getVoiceSocket(uint32_t ip, uint16_t port, UinType myUin, UinType peerUin, DccHandler *handler, bool request = false);

	bool dccEnabled() const;

// 	virtual void mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow);

// signals:
// 	void socketDestroying(DccSocket* socket);
// 	void dcc7IncomingFileTransfer(DccSocket *socket);

};

#endif // DCC_MANAGER_H

// kate: indent-mode cstyle; replace-tabs off; tab-width 4;
