/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <arpa/inet.h>

#include "accounts/account.h"
#include "contacts/contact.h"
#include "contacts/contact-account-data.h"
#include "contacts/contact-manager.h"

#include "config_file.h"
#include "debug.h"
#include "message_box.h"
#include "misc/misc.h"

#include "dcc/dcc-manager.h"
#include "dcc/dcc-socket-notifiers.h"
#include "file-transfer/gadu-file-transfer.h"
#include "services/gadu-file-transfer-service.h"
#include "socket-notifiers/gadu-protocol-socket-notifiers.h"
#include "gadu-account.h"
#include "gadu-contact-account-data.h"

#include "gadu-protocol.h"

DccManager::DccManager(GaduProtocol *protocol) :
		QObject(protocol), Protocol(protocol), MainSocketNotifiers(0)
{
	kdebugf();

	createDefaultConfiguration();

	setUpDcc();

	kdebugf2();
}

DccManager::~DccManager()
{
	kdebugf();

	closeDcc();

	kdebugf2();
}

void DccManager::setUpExternalAddress(gg_login_params &loginParams)
{
	bool haveExternalDcc = !DccExternalIP.isNull() && DccExternalPort > 1023;
	loginParams.external_addr = haveExternalDcc ? htonl(DccExternalIP.toIPv4Address()) : 0;
	loginParams.external_port = haveExternalDcc ? DccExternalPort : 0;
}


/*
void DccManager::mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow)
{
	QWidget *allowDCC = mainConfigurationWindow->widgetById("dcc/AllowDCC");
	QWidget *fileTransfers = mainConfigurationWindow->widgetById("dcc/fileTransfers");
	QWidget *ip = mainConfigurationWindow->widgetById("dcc/ip");

	connect(allowDCC, SIGNAL(toggled(bool)), fileTransfers, SLOT(setEnabled(bool)));
	connect(allowDCC, SIGNAL(toggled(bool)), ip, SLOT(setEnabled(bool)));

	QWidget *ipAutotetect = mainConfigurationWindow->widgetById("dcc/ipAutodetect");
	ipAddress = mainConfigurationWindow->widgetById("dcc/ipAddress");
	forwarding = dynamic_cast<QCheckBox *>(mainConfigurationWindow->widgetById("dcc/forwarding"));
	forwardingExternalIp = mainConfigurationWindow->widgetById("dcc/forwardingExternalIp");
	forwardingExternalPort = mainConfigurationWindow->widgetById("dcc/forwardingExternalPort");
	forwardingLocalPort = mainConfigurationWindow->widgetById("dcc/forwardingLocalPort");

	connect(forwarding, SIGNAL(toggled(bool)), forwardingExternalIp, SLOT(setEnabled(bool)));
	connect(forwarding, SIGNAL(toggled(bool)), forwardingExternalPort, SLOT(setEnabled(bool)));
	connect(forwarding, SIGNAL(toggled(bool)), forwardingLocalPort, SLOT(setEnabled(bool)));

	connect(ipAutotetect, SIGNAL(toggled(bool)), ipAddress, SLOT(setDisabled(bool)));
	connect(ipAutotetect, SIGNAL(toggled(bool)), this, SLOT(onIpAutotetectToggled(bool)));
}*/

void DccManager::setUpDcc()
{
	kdebugf();

	WaitingFileTransfers.clear();

	GaduAccount *account = dynamic_cast<GaduAccount *>(Protocol->account());
	if (!account)
		return;

	struct gg_dcc *socket = gg_dcc_socket_create(account->uin(), config_file.readNumEntry("Network", "LocalPort"));
	if (!socket)
	{
		kdebugmf(KDEBUG_NETWORK | KDEBUG_INFO, "Couldn't bind DCC socket.\n");

		// TODO: 0.6.6
		// MessageBox::msg(tr("Couldn't create DCC socket.\nDirect connections disabled."), true, "Warning");
		kdebugf2();
		return;
	}

	MainSocketNotifiers = new DccSocketNotifiers(Protocol, this);
	SocketNotifiers = QList<DccSocketNotifiers *>();

	QHostAddress DCCIP;
	short int DCCPort;

	if (config_file.readBoolEntry("Network", "DccIpDetect"))
		DCCIP.setAddress("255.255.255.255");
	else
		DCCIP.setAddress(config_file.readEntry("Network", "DccIP"));

	QHostAddress ext_ip;

	bool forwarding = config_file.readBoolEntry("Network", "DccForwarding") &&
			ext_ip.setAddress(config_file.readEntry("Network", "ExternalIP"));

	DccExternalIP = forwarding ? ext_ip : QHostAddress();
	DccExternalPort = forwarding ? config_file.readNumEntry("Network", "ExternalPort") : 0;

	gg_dcc_ip = htonl(DCCIP.toIPv4Address());
	gg_dcc_port = socket->port;

	kdebugmf(KDEBUG_NETWORK | KDEBUG_INFO, "DCC_IP=%s DCC_PORT=%d\n", qPrintable(DCCIP.toString()), DCCPort);

	DccEnabled = true;

	connectSocketNotifiers(MainSocketNotifiers);
	MainSocketNotifiers->watchFor(socket);

	kdebugf2();
}
/*
void DccManager::onIpAutotetectToggled(bool toggled)
{
	forwarding->setEnabled(!toggled);

	if (toggled)
	{
		forwardingExternalIp->setEnabled(false);
		forwardingExternalPort->setEnabled(false);
		forwardingLocalPort->setEnabled(false);
	}
	else
	{
		forwardingExternalIp->setEnabled(forwarding->isChecked());
		forwardingExternalPort->setEnabled(forwarding->isChecked());
		forwardingLocalPort->setEnabled(forwarding->isChecked());
	}
}*/

void DccManager::configurationUpdated()
{
	QHostAddress host;

	if (!host.setAddress(config_file.readEntry("Network", "DccIP")))
		config_file.writeEntry("Network", "DccIP", "0.0.0.0");

	if (!host.setAddress(config_file.readEntry("Network", "ExternalIP")))
		config_file.writeEntry("Network", "ExternalIP", "0.0.0.0");

	// kadu->reconnect() ??
}

bool DccManager::dccEnabled() const
{
	return DccEnabled;
}

// void DccManager::timeout()
// {
	// TODO: change into notification
// 	MessageBox::msg(tr("Direct connection timeout!\nThe receiver doesn't support direct connections or\nboth machines are behind routers with NAT."), true, "Warning");
// }

void DccManager::closeDcc()
{
	kdebugf();

	gg_dcc_ip = 0;
	gg_dcc_port = 0;
	DccEnabled = false;

	kdebugf2();
}

void DccManager::connectSocketNotifiers(DccSocketNotifiers *notifiers)
{
	connect(notifiers, SIGNAL(destroyed(QObject *)),
			this, SLOT(socketNotifiersDestroyed(QObject *)));
	connect(notifiers, SIGNAL(incomingConnection(struct gg_dcc *)),
			this, SLOT(dccIncomingConnection(struct gg_dcc *)));
}

void DccManager::disconnectSocketNotifiers(DccSocketNotifiers *notifiers)
{
	disconnect(notifiers, SIGNAL(destroyed(QObject *)),
			this, SLOT(socketNotifiersDestroyed(QObject *)));
	disconnect(notifiers, SIGNAL(incomingConnection(struct gg_dcc *)),
			this, SLOT(dccIncomingConnection(struct gg_dcc *)));
}

void DccManager::socketNotifiersDestroyed(QObject *socketNotifiers)
{
	SocketNotifiers.removeAll(dynamic_cast<DccSocketNotifiers *>(socketNotifiers));
}

void DccManager::connectionRequestReceived(Contact contact)
{
	kdebugf();

	GaduContactAccountData *gcad = Protocol->gaduContactAccountData(contact);
	if (!gcad)
		return;
	GaduAccount *account = dynamic_cast<GaduAccount *>(Protocol->account());
	if (!account)
		return;

	struct gg_dcc *dcc = gg_dcc_get_file(htonl(gcad->ip().toIPv4Address()), gcad->port(), account->uin(), gcad->uin());
	if (!dcc)
		return;

	DccSocketNotifiers *dccSocketNotifiers = new DccSocketNotifiers(Protocol, this);
	SocketNotifiers << dccSocketNotifiers;
	connectSocketNotifiers(dccSocketNotifiers);
	dccSocketNotifiers->watchFor(dcc);

	kdebugf2();
}

bool DccManager::acceptConnection(unsigned int uin, unsigned int peerUin, unsigned int peerAddr)
{
	GaduAccount *account = dynamic_cast<GaduAccount *>(Protocol->account());
	if (!account)
		return false;

	Contact contact = ContactManager::instance()->byId(Protocol->account(), QString::number(peerUin));
	if (uin != account->uin() || contact.isAnonymous() || contact.isNull())
	{
		kdebugm(KDEBUG_WARNING, "insane values: uin:%d peer_uin:%d\n", uin, peerUin);
		return false;
	}

	GaduContactAccountData *gcad = Protocol->gaduContactAccountData(contact);
	if (!gcad)
		return false;

	ContactList contacts(contact);

	if (contact.isIgnored())
	{
		kdebugm(KDEBUG_WARNING, "unbidden user: %d\n", peerUin);
		return false;
	}

	QHostAddress remoteAddress(ntohl(peerAddr));

	if (remoteAddress == gcad->ip())
		return true;

	kdebugm(KDEBUG_WARNING, "possible spoofing attempt from %s (uin:%d)\n", qPrintable(remoteAddress.toString()), peerUin);

	return MessageBox::ask(narg(
			tr("%1 is asking for direct connection but his/her\n"
				"IP address (%2) differs from what GG server returned\n"
				"as his/her IP address (%3). It may be spoofing\n"
				"or he/she has port forwarding. Continue connection?"),
			contact.display(),
			remoteAddress.toString(),
			gcad->ip().toString()));
}

void DccManager::needIncomingFileTransferAccept(DccSocketNotifiers *socket)
{
	GaduFileTransfer *gft = new GaduFileTransfer(Protocol->account(),
			ContactManager::instance()->byId(Protocol->account(), QString::number(socket->peerUin())),
			FileTransfer::TypeReceive);

	gft->setFileTransferNotifiers(socket);
	socket->setGaduFileTransfer(gft);

	emit Protocol->CurrentFileTransferService->incomingFileTransfer(gft);
}

GaduFileTransfer * DccManager::findFileTransfer(DccSocketNotifiers *notifiers)
{
	foreach (GaduFileTransfer *gft, WaitingFileTransfers)
	{
		UinType uin = Protocol->uin(gft->contact());
		if (uin == notifiers->peerUin())
		{
			disconnectSocketNotifiers(notifiers);
			SocketNotifiers.removeAll(notifiers);
			return gft;
		}
	}

	return 0;
}

void DccManager::handleEventDccNew(struct gg_event *e) {
	kdebugmf(KDEBUG_NETWORK | KDEBUG_INFO, "GG_EVENT_DCC_NEW\n");

	DccSocketNotifiers *newSocketNotifiers = new DccSocketNotifiers(Protocol, this);
	SocketNotifiers << newSocketNotifiers;
	connectSocketNotifiers(newSocketNotifiers);
	newSocketNotifiers->watchFor(e->event.dcc_new);

	e->event.dcc_new = 0;
}

void DccManager::handleEventDcc7New(struct gg_event *e)
{
	kdebugf();

	struct gg_dcc7 *dcc = e->event.dcc7_new;
/*
	if (!acceptConnection(dcc->uin, dcc->peer_uin, dcc->remote_addr))
	{
		gg_dcc7_reject(dcc, 0);
		gg_dcc7_free(dcc);
		return;
	}
*/
	switch (dcc->dcc_type)
	{
		case GG_DCC7_TYPE_FILE:
		{
// 			TODO: ZARAZ
			DccSocketNotifiers *newSocketNotifiers = new DccSocketNotifiers(Protocol, this);
			SocketNotifiers << newSocketNotifiers;
			connectSocketNotifiers(newSocketNotifiers);
			newSocketNotifiers->watchFor(e->event.dcc7_new);
			needIncomingFileTransferAccept(newSocketNotifiers);
// 			file_transfer_manager->dcc7IncomingFileTransfer(new DccSocket(dcc));
			break;
		}

		default:
			gg_dcc7_reject(dcc, GG_DCC7_REJECT_USER);
			gg_dcc7_free(dcc);
			break;
	}

	kdebugf2();
}

void DccManager::handleEventDcc7Accept(struct gg_event *e)
{
	kdebugf();

	foreach (DccSocketNotifiers *socketNotifiers, SocketNotifiers)
	{
		if (socketNotifiers->Socket7 == e->event.dcc7_accept.dcc7)
		{
			socketNotifiers->handleEventDcc7Accept(e);
			return;
		}
	}
}

void DccManager::handleEventDcc7Reject(struct gg_event *e)
{
	kdebugf();

	foreach (DccSocketNotifiers *socketNotifiers, SocketNotifiers)
	{
		if (socketNotifiers->Socket7 == e->event.dcc7_accept.dcc7)
		{
			socketNotifiers->handleEventDcc7Reject(e);
			return;
		}
	}
}

void DccManager::handleEventDcc7Pending(struct gg_event *e)
{
	kdebugf();

	foreach (DccSocketNotifiers *socketNotifiers, SocketNotifiers)
	{
		socketNotifiers->handleEventDcc7Pending(e);
		return;
	}
}

void DccManager::handleEventDcc7Error(struct gg_event *e)
{
	kdebugf();

	foreach (DccSocketNotifiers *socketNotifiers, SocketNotifiers)
	{
// 		if (socketNotifiers->Socket7 == e->event.dcc7_error)
// 		{
// 			socketNotifiers->handleEventDcc7Error(e);
// 			return;
// 		}
	}
}

void DccManager::attachSendFileTransferSocket6(unsigned int uin, GaduContactAccountData *gcad, GaduFileTransfer *gft)
{
	kdebugf();

	int port = gcad->port();
	if (port >= 10)
	{
		struct gg_dcc *socket = gg_dcc_send_file(htonl(gcad->ip().toIPv4Address()), port, uin, gcad->uin());
		if (socket)
		{
			DccSocketNotifiers *fileTransferNotifiers = new DccSocketNotifiers(Protocol, this);
			gft->setFileTransferNotifiers(fileTransferNotifiers);
			fileTransferNotifiers->watchFor(socket);
			return;
		}
	}

	kdebugmf(KDEBUG_INFO | KDEBUG_NETWORK, "needs callback\n");

// startTimeOut
	WaitingFileTransfers << gft;
	gg_dcc_request(Protocol->gaduSession(), gcad->uin());
}

void DccManager::attachSendFileTransferSocket7(unsigned int uin, GaduContactAccountData *gcad, GaduFileTransfer *gft)
{
	kdebugf();

	gg_dcc7 *dcc = gg_dcc7_send_file(Protocol->gaduSession(), gcad->uin(),
			qPrintable(gft->localFileName()), unicode2cp(gft->localFileName()).data(), 0);

	if (dcc)
	{
		DccSocketNotifiers *fileTransferNotifiers = new DccSocketNotifiers(Protocol, this);
		gft->setFileTransferNotifiers(fileTransferNotifiers);
		gft->changeFileTransferStatus(FileTransfer::StatusWaitingForAccept);
		fileTransferNotifiers->watchFor(dcc);

		SocketNotifiers << fileTransferNotifiers;
	}
	else
		gft->socketNotAvailable();
}

void DccManager::attachSendFileTransferSocket(GaduFileTransfer *gft)
{
	Contact peer = gft->contact();
	GaduContactAccountData *gcad = Protocol->gaduContactAccountData(peer);
	if (!gcad)
		return;

	GaduAccount *account = dynamic_cast<GaduAccount *>(Protocol->account());
	if (!account)
		return;

	DccVersion version = (gcad->gaduProtocolVersion() & 0x0000ffff) >= 0x29
		? Dcc7
		: Dcc6;

	switch (version)
	{
		case Dcc6:
			attachSendFileTransferSocket6(account->uin(), gcad, gft);
			break;

		case Dcc7:
			attachSendFileTransferSocket7(account->uin(), gcad, gft);
			break;
	}
}

/*void DccManager::getVoiceSocket(uint32_t ip, uint16_t port, UinType myUin, UinType peerUin, DccHandler *handler, bool request)
{
	kdebugf();

	if ((port >= 10) && !request)
	{

		struct gg_dcc *sock = gg_dcc_voice_chat(htonl(ip), port, myUin, peerUin);

		if (sock)
		{
			DccSocket *result = new DccSocket(this, sock);
			result->setHandler(handler);
			return;
		}
	}

	startTimeout();
	requests.insert(peerUin, handler);
	Protocol->dccRequest(peerUin);

	kdebugf2();
}*/

void DccManager::createDefaultConfiguration()
{
	config_file.addVariable("Network", "AllowDCC", true);
	config_file.addVariable("Network", "DccIP", "0.0.0.0");
	config_file.addVariable("Network", "DccIpDetect", true);
	config_file.addVariable("Network", "ExternalIP", "0.0.0.0");
	config_file.addVariable("Network", "ExternalPort", 0);
	config_file.addVariable("Network", "DccForwarding", false);
	config_file.addVariable("Network", "LastDownloadDirectory", QString(getenv("HOME")) + '/');
	config_file.addVariable("Network", "LastUploadDirectory", QString(getenv("HOME")) + '/');
	config_file.addVariable("Network", "LocalPort", 0);
	config_file.addVariable("Network", "RemoveCompletedTransfers", false);

	config_file.addVariable("ShortCuts", "kadu_sendfile", "F8");
	config_file.addVariable("ShortCuts", "kadu_voicechat", "F7");
}

// kate: indent-mode cstyle; replace-tabs off; tab-width 4; 
