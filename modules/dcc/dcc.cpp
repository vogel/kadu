/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qcheckbox.h>
#include <qfiledialog.h>
#include <qfileinfo.h>
#include <qlayout.h>
#include <qmessagebox.h>
#include <qpushbutton.h>
#include <qvgroupbox.h>
#include <qsocketnotifier.h>

#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>

#include "config_file.h"
#include "dcc.h"
#include "dcc_socket.h"
#include "debug.h"
#include "file_transfer_manager.h"
#include "hot_key.h"
#include "ignore.h"
#include "misc.h"
#include "kadu.h"
#include "message_box.h"
#include "userlist.h"

/**
 * @ingroup dcc
 * @{
 */
extern "C" int dcc_init()
{
	kdebugf();
	dcc_manager = new DccManager();
	file_transfer_manager = new FileTransferManager(dcc_manager, "file_transfer_manager");

	MainConfigurationWindow::registerUiFile(dataPath("kadu/modules/configuration/dcc.ui"), dcc_manager);

	return 0;
}

extern "C" void dcc_close()
{
	kdebugf();

	MainConfigurationWindow::unregisterUiFile(dataPath("kadu/modules/configuration/dcc.ui"), dcc_manager);

	delete file_transfer_manager;
	file_transfer_manager = 0;
	delete dcc_manager;
	dcc_manager = 0;
}

DccManager::DccManager()
	: MainSocket(0), TimeoutTimer(), requests(), DccEnabled(false)
{
	kdebugf();

	createDefaultConfiguration();

	connect(&TimeoutTimer, SIGNAL(timeout()), this, SLOT(timeout()));

	connect(gadu, SIGNAL(connecting()), this, SLOT(setupDcc()));
	connect(gadu, SIGNAL(disconnected()), this, SLOT(closeDcc()));
	connect(gadu, SIGNAL(dccConnectionReceived(const UserListElement&)),
		this, SLOT(dccConnectionReceived(const UserListElement&)));
	connect(gadu, SIGNAL(dcc7New(struct gg_dcc7 *)), this, SLOT(dcc7New(struct gg_dcc7 *)));

	kdebugf2();
}

DccManager::~DccManager()
{
	kdebugf();

	disconnect(gadu, SIGNAL(connecting()), this, SLOT(setupDcc()));
	disconnect(gadu, SIGNAL(disconnected()), this, SLOT(closeDcc()));
	disconnect(gadu, SIGNAL(dccConnectionReceived(const UserListElement&)),
		this, SLOT(dccConnectionReceived(const UserListElement&)));
	disconnect(gadu, SIGNAL(dcc7New(struct gg_dcc7 *)), this, SLOT(dcc7New(struct gg_dcc7 *)));

	closeDcc();

	kdebugf2();
}

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
}

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
}

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

void DccManager::timeout()
{
	// TODO: change into notification
	MessageBox::msg(tr("Direct connection timeout!\nThe receiver doesn't support direct connections or\nboth machines are behind routers with NAT."), true, "Warning");
}

void DccManager::startTimeout()
{
	TimeoutTimer.start(15000, TRUE);
}

void DccManager::cancelTimeout()
{
	TimeoutTimer.stop();
}

void DccManager::setupDcc()
{
	kdebugf();

	if (!config_file.readBoolEntry("Network", "AllowDCC"))
	{
		kdebugf2();
		return;
	}

	struct gg_dcc *socket = gg_dcc_socket_create(config_file.readNumEntry("General", "UIN"), config_file.readNumEntry("Network", "LocalPort"));

	if (!socket)
	{
		kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "Couldn't bind DCC socket.\n");

		MessageBox::msg(tr("Couldn't create DCC socket.\nDirect connections disabled."), true, "Warning");
		kdebugf2();
		return;
	}

	MainSocket = new DccSocket(socket);
	MainSocket->setHandler(this);

	QHostAddress DCCIP;
	short int DCCPort;

	if (config_file.readBoolEntry("Network", "DccIpDetect"))
		DCCIP.setAddress("255.255.255.255");
	else
		DCCIP.setAddress(config_file.readEntry("Network", "DccIP"));

	QHostAddress ext_ip;
	bool forwarding = config_file.readBoolEntry("Network", "DccForwarding") && ext_ip.setAddress(config_file.readEntry("Network", "ExternalIP"));

	if (forwarding)
	{
		gadu->setDccExternalIP(ext_ip);
		DCCPort = config_file.readNumEntry("Network", "ExternalPort");
	}
	else
	{
		gadu->setDccExternalIP(QHostAddress());
		DCCPort = socket->port;
	}

	gadu->setDccIpAndPort(htonl(DCCIP.ip4Addr()), DCCPort);

	kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "DCC_IP=%s DCC_PORT=%d\n", DCCIP.toString().local8Bit().data(), DCCPort);

	DccEnabled = true;

	kdebugf2();
}

void DccManager::closeDcc()
{
	kdebugf();

	if (MainSocket)
	{
		delete MainSocket;
		MainSocket = 0;
		gadu->setDccIpAndPort(0, 0);
	}

	DccEnabled = false;

	kdebugf2();
}

void DccManager::dccConnectionReceived(const UserListElement& sender)
{
	kdebugf();

	struct gg_dcc *dcc_new = gg_dcc_get_file(htonl(sender.IP("Gadu").ip4Addr()), sender.port("Gadu"),
		config_file.readNumEntry("General","UIN"),
		sender.ID("Gadu").toUInt());

	if (dcc_new)
	{
		DccSocket* dcc_socket = new DccSocket(dcc_new);
		dcc_socket->setHandler(this);
	}

	kdebugf2();
}

void DccManager::dcc7New(struct gg_dcc7 *dcc)
{
	kdebugf();

	if (!acceptClient(dcc->uin, dcc->peer_uin, dcc->remote_addr))
	{
		gg_dcc7_reject(dcc, 0);
		gg_dcc7_free(dcc);
		return;
	}

	switch (dcc->dcc_type)
	{
		case GG_DCC7_TYPE_FILE:
			file_transfer_manager->dcc7IncomingFileTransfer(new DccSocket(dcc));
			return;

		default:
			gg_dcc7_reject(dcc, 0);
			gg_dcc7_free(dcc);
			return;
	}

	kdebugf2();
}

void DccManager::getFileTransferSocket(uint32_t ip, uint16_t port, UinType myUin, UinType peerUin, DccHandler *handler, bool request)
{
	kdebugf();

	if ((port >= 10) && !request)
	{
		struct gg_dcc *sock = gg_dcc_send_file(htonl(ip), port, myUin, peerUin);

		if (sock)
		{
			DccSocket *result = new DccSocket(sock);
			result->setHandler(handler);
			return;
		}
	}

	startTimeout();
	requests.insert(peerUin, handler);
	gadu->dccRequest(peerUin);

	kdebugf2();
}

void DccManager::getVoiceSocket(uint32_t ip, uint16_t port, UinType myUin, UinType peerUin, DccHandler *handler, bool request)
{
	kdebugf();

	if ((port >= 10) && !request)
	{
		struct gg_dcc *sock = gg_dcc_voice_chat(htonl(ip), port, myUin, peerUin);

		if (sock)
		{
			DccSocket *result = new DccSocket(sock);
			result->setHandler(handler);
			return;
		}
	}

	startTimeout();
	requests.insert(peerUin, handler);
	gadu->dccRequest(peerUin);

	kdebugf2();
}

void DccManager::callbackReceived(DccSocket *socket)
{
	kdebugf();

	cancelTimeout();

	UinType peerUin = socket->peerUin();
	if (requests.contains(peerUin))
	{
		DccHandler *handler = requests[peerUin];
		socket->setType(handler->dccType());
		requests.remove(peerUin);

		socket->setHandler(handler);
	}
	else
		delete socket;

	kdebugf2();
}

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

bool DccManager::addSocket(DccSocket *socket)
{
	kdebugf();

	UnhandledSockets.append(socket);
	return true;
}

void DccManager::removeSocket(DccSocket *socket)
{
	kdebugf();

	UnhandledSockets.remove(socket);
}

void DccManager::addHandler(DccHandler *handler)
{
	kdebugf();

	SocketHandlers.append(handler);
}

void DccManager::removeHandler(DccHandler *handler)
{
	kdebugf();

	SocketHandlers.remove(handler);
}

void DccManager::connectionError(DccSocket *socket)
{
	kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "Connection broken unexpectedly!\n");
}

bool DccManager::socketEvent(DccSocket *socket, bool &lock)
{
	kdebugf();

	DccSocket *dccSocket;

	switch (socket->ggDccEvent()->type) {
		case GG_EVENT_DCC_NEW:
			kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "GG_EVENT_DCC_NEW\n");

			dccSocket = new DccSocket(socket->ggDccEvent()->event.dcc_new);
			dccSocket->setHandler(this);
			return true;

		default:
			break;
	}

	FOREACH(handler, SocketHandlers)
		if ((*handler)->socketEvent(socket, lock))
			return true;

	return false;
}

bool DccManager::acceptClient(UinType uin, UinType peerUin, int remoteAddr)
{
	kdebugf();

	if (uin != (UinType)config_file.readNumEntry("General", "UIN") || !userlist->contains("Gadu", QString::number(peerUin)))
	{
		kdebugm(KDEBUG_WARNING, "insane values: uin:%d peer_uin:%d\n", uin, peerUin);
		return false;
	}

	UserListElement peer = userlist->byID("Gadu", QString::number(peerUin));
	UserListElements users;
	users.append(peer);

	if (peer.isAnonymous() || IgnoredManager::isIgnored(users))
	{
		kdebugm(KDEBUG_WARNING, "unbidden user: %d\n", peerUin);
		return false;
	}

	QHostAddress remoteAddress(ntohl(remoteAddr));
	if (remoteAddress == peer.IP("Gadu")) // TODO: make it async, make DccSocket no-ui-aware
		return true;

	kdebugm(KDEBUG_WARNING, "possible spoofing attempt from %s (uin:%d)\n", remoteAddress.toString().local8Bit().data(), peerUin);
	return MessageBox::ask(narg(
		tr("%1 is asking for direct connection but his/her\n"
			"IP address (%2) differs from what GG server returned\n"
			"as his/her IP address (%3). It may be spoofing\n"
			"or he/she has port forwarding. Continue connection?"),
			peer.altNick(),
			remoteAddress.toString(),
			peer.IP("Gadu").toString()));
}

DccManager* dcc_manager = NULL;

/** @} */

