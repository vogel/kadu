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

#include <fcntl.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>

#include "config_dialog.h"
#include "dcc.h"
#include "debug.h"
#include "file_transfer.h"
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
	dcc_manager = new DccManager(0, "dcc_manager");
	file_transfer_manager = new FileTransferManager(dcc_manager, "file_transfer_manager");
	return 0;
}

extern "C" void dcc_close()
{
	kdebugf();
	delete file_transfer_manager;
	file_transfer_manager = NULL;
	delete dcc_manager;
	dcc_manager = NULL;
}

int DccSocket::Count = 0;

DccSocket::DccSocket(struct gg_dcc* dcc_sock) : QObject(0, 0),
	readSocketNotifier(0), writeSocketNotifier(0), dccsock(dcc_sock),
	dccevent(0), in_watchDcc(false), State(DCC_SOCKET_TRANSFERRING)
{
	kdebugf();
	++Count;
	kdebugmf(KDEBUG_FUNCTION_END|KDEBUG_INFO, "dcc sockets count = %d\n", Count);
}

DccSocket::~DccSocket()
{
	kdebugf();
	emit dcc_manager->socketDestroying(this);
	if (readSocketNotifier)
	{
		readSocketNotifier->setEnabled(false);
		delete readSocketNotifier;
		readSocketNotifier = NULL;
	}
	if (writeSocketNotifier)
	{
		writeSocketNotifier->setEnabled(false);
		delete writeSocketNotifier;
		writeSocketNotifier = NULL;
	}
	if (dccevent)
	{
		gadu->freeEvent(dccevent);
		dccevent = NULL;
	}
	if (dccsock)
	{
		if (dccsock->file_fd > 0)
		{
			close(dccsock->file_fd);
		}
		gadu->dccFree(dccsock);
		dccsock = NULL;
		--Count;
	}
	kdebugmf(KDEBUG_INFO|KDEBUG_FUNCTION_END, "end: dcc sockets count = %d\n", Count);
}

struct gg_dcc* DccSocket::ggDccStruct() const
{
	return dccsock;
}

struct gg_event* DccSocket::ggDccEvent() const
{
	return dccevent;
}

void DccSocket::initializeNotifiers()
{
	kdebugf();
	readSocketNotifier = new QSocketNotifier(dccsock->fd, QSocketNotifier::Read, this);
	QObject::connect(readSocketNotifier, SIGNAL(activated(int)), this, SLOT(dccDataReceived()));

	writeSocketNotifier = new QSocketNotifier(dccsock->fd, QSocketNotifier::Write, this);
	QObject::connect(writeSocketNotifier, SIGNAL(activated(int)), this, SLOT(dccDataSent()));
	kdebugf2();
}

void DccSocket::dccDataReceived()
{
	if (!in_watchDcc)
		watchDcc(GG_CHECK_READ);
}

void DccSocket::dccDataSent()
{
	kdebugf();
	writeSocketNotifier->setEnabled(false);
	if (dccsock->check & GG_CHECK_WRITE)
		watchDcc(GG_CHECK_WRITE);
	kdebugf2();
}

void DccSocket::watchDcc(int /*check*/)
{
	kdebugf();
	UserListElements users;
	bool spoofingAttempt, insane, unbidden;
	UserListElement peer;

	in_watchDcc = true;

	if (!(dccevent = gadu->dccWatchFd(dccsock)))
	{
		kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "Connection broken unexpectedly!\n");
		emit dcc_manager->connectionBroken(this);
		return;
	}

	switch (dccevent->type)
	{
		case GG_EVENT_DCC_CLIENT_ACCEPT:
			kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "GG_EVENT_DCC_CLIENT_ACCEPT! uin:%d peer_uin:%d\n",
				dccsock->uin, dccsock->peer_uin);

			insane = dccsock->uin != (UinType)config_file.readNumEntry("General", "UIN")
				|| !userlist->contains("Gadu", QString::number(dccsock->peer_uin));
			peer = userlist->byID("Gadu", QString::number(dccsock->peer_uin));
			users.append(peer);
			unbidden = peer.isAnonymous() || isIgnored(users);
			spoofingAttempt = !(QHostAddress(ntohl(dccsock->remote_addr)) == peer.IP("Gadu"));

			if (insane)
				kdebugm(KDEBUG_WARNING, "insane values: uin:%d peer_uin:%d\n", dccsock->uin, dccsock->peer_uin);
			if (!insane && unbidden)
				kdebugm(KDEBUG_WARNING, "unbidden user: %d\n", dccsock->peer_uin);

			if (!insane && !unbidden && spoofingAttempt)
			{
				kdebugm(KDEBUG_WARNING, "possible spoofing attempt from %s (uin:%d)\n",
						QHostAddress(ntohl(dccsock->remote_addr)).toString().local8Bit().data(), dccsock->peer_uin);
				if (!insane && !unbidden)
					spoofingAttempt = !MessageBox::ask(narg(
					tr("%1 is asking for direct connection but his/her\n"
					"IP address (%2) differs from what GG server returned\n"
					"as his/her IP address (%3). It may be spoofing\n"
					"or he/she has port forwarding. Continue connection?"),
					peer.altNick(),
					QHostAddress(ntohl(dccsock->remote_addr)).toString(),
					peer.IP("Gadu").toString()));
			}

			if (insane || unbidden || spoofingAttempt)
			{
				setState(DCC_SOCKET_TRANSFER_DISCARDED);
				//emit dcc_manager->tranferDiscarded(this);
			}
			break;
		case GG_EVENT_DCC_CALLBACK:
			kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "GG_EVENT_DCC_CALLBACK! uin:%d peer_uin:%d\n",
				dccsock->uin, dccsock->peer_uin);
			dcc_manager->cancelTimeout();
			dcc_manager->callbackReceived(this);
			break;
		case GG_EVENT_DCC_ERROR:
			kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "GG_EVENT_DCC_ERROR\n");
			emit dcc_manager->dccError(this);
			if (state() != DCC_SOCKET_VOICECHAT_DISCARDED &&
				state() != DCC_SOCKET_TRANSFER_ERROR)
				setState(DCC_SOCKET_CONNECTION_BROKEN);
			gadu->freeEvent(dccevent);
			dccevent = NULL;
			in_watchDcc = false;
			return;
		case GG_EVENT_DCC_DONE:
			kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "GG_EVENT_DCC_DONE\n");
			setState(DCC_SOCKET_TRANSFER_FINISHED);
			emit dcc_manager->dccDone(this);
			gadu->freeEvent(dccevent);
			dccevent = NULL;
			in_watchDcc = false;
			return;
		default:
			break;
	}

	emit dcc_manager->dccEvent(this);

	if (dccsock->check & GG_CHECK_WRITE)
		writeSocketNotifier->setEnabled(true);

	if (dccevent)
	{
		gadu->freeEvent(dccevent);
		dccevent = NULL;
	}

	in_watchDcc = false;
	kdebugf2();
}

void DccSocket::discard()
{
	kdebugf();

	setState(DCC_SOCKET_TRANSFER_DISCARDED);
}

void DccSocket::setState(int pstate)
{
	kdebugf();
	readSocketNotifier->setEnabled(false);
	writeSocketNotifier->setEnabled(false);
	State = pstate;

	switch (State)
	{
		case DCC_SOCKET_TRANSFER_DISCARDED:
			kdebugm(KDEBUG_INFO, "state: DCC_SOCKET_TRANSFER_DISCARDED\n");
			break;
		case DCC_SOCKET_CONNECTION_BROKEN:
			kdebugm(KDEBUG_INFO, "state: DCC_SOCKET_CONNECTION_BROKEN\n");
			break;
		case DCC_SOCKET_COULDNT_OPEN_FILE:
			MessageBox::msg(tr("Couldn't open file!"));
			break;
	}
	emit dcc_manager->setState(this);
	deleteLater();
	kdebugf2();
}

int DccSocket::state() const
{
	return State;
}

int DccSocket::count()
{
	return Count;
}


DccManager::DccManager(QObject *parent, const char *name) : QObject(parent, name),
	DccSock(0), DCCReadSocketNotifier(0), DCCWriteSocketNotifier(0), TimeoutTimer(),
	requests(), DccEnabled(false)
{
	kdebugf();
	ConfigDialog::addHotKeyEdit("ShortCuts", "Define keys", QT_TRANSLATE_NOOP("@default", "Send file"), "kadu_sendfile", "F8");

	ConfigDialog::addCheckBox("Network", "dcc", QT_TRANSLATE_NOOP("@default", "DCC enabled"), "AllowDCC", false);
	ConfigDialog::addCheckBox("Network", "dcc", QT_TRANSLATE_NOOP("@default", "Remove completed transfers from transfers list"), "RemoveCompletedTransfers", true);
	ConfigDialog::addCheckBox("Network", "dcc", QT_TRANSLATE_NOOP("@default", "DCC IP autodetection"), "DccIpDetect", true, 0, 0, Advanced);

	ConfigDialog::addVGroupBox("Network", "dcc", QT_TRANSLATE_NOOP("@default", "DCC IP"), 0, Advanced);
	ConfigDialog::addLineEdit("Network", "DCC IP", QT_TRANSLATE_NOOP("@default", "IP address:"), "DccIP");
	ConfigDialog::addCheckBox("Network", "dcc", QT_TRANSLATE_NOOP("@default", "DCC forwarding enabled"), "DccForwarding", false, 0, 0, Advanced);

	ConfigDialog::addVGroupBox("Network", "dcc", QT_TRANSLATE_NOOP("@default", "DCC forwarding properties"), 0, Advanced);
	ConfigDialog::addLineEdit("Network", "DCC forwarding properties", QT_TRANSLATE_NOOP("@default", "External IP address:"), "ExternalIP");
	ConfigDialog::addLineEdit("Network", "DCC forwarding properties", QT_TRANSLATE_NOOP("@default", "External TCP port:"), "ExternalPort", "1550");
	ConfigDialog::addLineEdit("Network", "DCC forwarding properties", QT_TRANSLATE_NOOP("@default", "Local TCP port:"), "LocalPort", "1550");

	ConfigDialog::connectSlot("Network", "DCC enabled", SIGNAL(toggled(bool)), this, SLOT(ifDccEnabled(bool)));
	ConfigDialog::connectSlot("Network", "DCC IP autodetection", SIGNAL(toggled(bool)), this, SLOT(ifDccIpEnabled(bool)));

	ConfigDialog::registerSlotOnCreateTab("Network", this, SLOT(configDialogCreated()));
	ConfigDialog::registerSlotOnApplyTab("Network", this, SLOT(configDialogApply()));

	connect(&TimeoutTimer, SIGNAL(timeout()), this, SLOT(timeout()));

	connect(gadu, SIGNAL(connecting()), this, SLOT(setupDcc()));
	connect(gadu, SIGNAL(disconnected()), this, SLOT(closeDcc()));
	connect(gadu, SIGNAL(dccConnectionReceived(const UserListElement&)),
		this, SLOT(dccConnectionReceived(const UserListElement&)));
	kdebugf2();
}

DccManager::~DccManager()
{
	kdebugf();
	ConfigDialog::disconnectSlot("Network", "DCC enabled", SIGNAL(toggled(bool)), this, SLOT(ifDccEnabled(bool)));
	ConfigDialog::disconnectSlot("Network", "DCC IP autodetection", SIGNAL(toggled(bool)), this, SLOT(ifDccIpEnabled(bool)));

	ConfigDialog::unregisterSlotOnCreateTab("Network", this, SLOT(configDialogCreated()));
	ConfigDialog::unregisterSlotOnApplyTab("Network", this, SLOT(configDialogApply()));

	ConfigDialog::removeControl("Network", "DCC enabled");
	ConfigDialog::removeControl("Network", "Remove completed transfers from transfers list");
	ConfigDialog::removeControl("Network", "DCC IP autodetection");

	ConfigDialog::removeControl("Network", "IP address:");
	ConfigDialog::removeControl("Network", "DCC forwarding enabled");
	ConfigDialog::removeControl("Network", "DCC IP");

	ConfigDialog::removeControl("Network", "External IP address:");
	ConfigDialog::removeControl("Network", "External TCP port:");
	ConfigDialog::removeControl("Network", "Local TCP port:");
	ConfigDialog::removeControl("Network", "DCC forwarding properties");

	ConfigDialog::removeControl("ShortCuts", "Send file");

	disconnect(gadu, SIGNAL(connecting()), this, SLOT(setupDcc()));
	disconnect(gadu, SIGNAL(disconnected()), this, SLOT(closeDcc()));
	disconnect(gadu, SIGNAL(dccConnectionReceived(const UserListElement&)),
		this, SLOT(dccConnectionReceived(const UserListElement&)));
	closeDcc();
	kdebugf2();
}

bool DccManager::dccEnabled() const
{
	return DccEnabled;
}

void DccManager::watchDcc()
{
	kdebugf();
	struct gg_event* dcc_e;
	if (!(dcc_e = gadu->dccWatchFd(DccSock)))
	{
		kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "Connection broken unexpectedly!\n");
		config_file.writeEntry("Network", "AllowDCC", false);
		delete DCCReadSocketNotifier;
		DCCReadSocketNotifier = NULL;
		delete DCCWriteSocketNotifier;
		DCCWriteSocketNotifier = NULL;
		return;
	}

	switch (dcc_e->type)
	{
		case GG_EVENT_NONE:
			break;
		case GG_EVENT_DCC_ERROR:
			kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "GG_EVENT_DCC_ERROR\n");
			break;
		case GG_EVENT_DCC_NEW:
			kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "GG_EVENT_DCC_NEW\n");

			if (DccSocket::count() < 8)
			{
				DccSocket* dcc_socket = new DccSocket(dcc_e->event.dcc_new);
				connect(dcc_socket, SIGNAL(dccFinished(DccSocket *)),
						this, SLOT(dccFinished(DccSocket *)));
				dcc_socket->initializeNotifiers();
			}
			else
			{
				if (dcc_e->event.dcc_new->file_fd > 0)
					close(dcc_e->event.dcc_new->file_fd);
				gadu->dccFree(dcc_e->event.dcc_new);
			}
			break;
		default:
			break;
	}

	if (DccSock->check == GG_CHECK_WRITE)
		DCCWriteSocketNotifier->setEnabled(true);

	gadu->freeEvent(dcc_e);
	kdebugf2();
}

void DccManager::dccFinished(DccSocket* dcc_socket)
{
	kdebugf();
	delete dcc_socket;
	kdebugf2();
}

void DccManager::dccReceived()
{
	kdebugf();
	watchDcc();
	kdebugf2();
}

void DccManager::dccSent()
{
	kdebugf();
	DCCWriteSocketNotifier->setEnabled(false);
	if (DccSock->check & GG_CHECK_WRITE)
		watchDcc();
	kdebugf2();
}

void DccManager::timeout()
{
	MessageBox::wrn(tr("Direct connection timeout!\nThe receiver doesn't support direct connections or\nboth machines are behind routers with NAT."));
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

	gadu->dccSocketCreate(config_file.readNumEntry("General", "UIN"),
							config_file.readNumEntry("Network", "LocalPort"),
							&DccSock);

	if (!DccSock)
	{
		kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "Couldn't bind DCC socket.\n");

		MessageBox::wrn(tr("Couldn't create DCC socket.\nDirect connections disabled."));
		kdebugf2();
		return;
	}

	QHostAddress DCCIP;
	short int DCCPort;

	if (config_file.readBoolEntry("Network", "DccIpDetect"))
		DCCIP.setAddress("255.255.255.255");
	else
		DCCIP.setAddress(config_file.readEntry("Network", "DccIP"));

	QHostAddress ext_ip;
	bool forwarding = config_file.readBoolEntry("Network", "DccForwarding") &&
					ext_ip.setAddress(config_file.readEntry("Network", "ExternalIP"));
	if (forwarding)
	{
		gadu->setDccExternalIP(ext_ip);
		DCCPort = config_file.readNumEntry("Network", "ExternalPort");
	}
	else
	{
		gadu->setDccExternalIP(QHostAddress());
		DCCPort = DccSock->port;
	}

	gadu->setDccIpAndPort(htonl(DCCIP.ip4Addr()), DCCPort);

	kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "DCC_IP=%s DCC_PORT=%d\n", DCCIP.toString().local8Bit().data(), DCCPort);

	DCCReadSocketNotifier = new QSocketNotifier(DccSock->fd, QSocketNotifier::Read, this, "dcc_read_socket_notifier");
	connect(DCCReadSocketNotifier, SIGNAL(activated(int)), this, SLOT(dccReceived()));

	DCCWriteSocketNotifier = new QSocketNotifier(DccSock->fd, QSocketNotifier::Write, this, "dcc_write_socket_notifier");
	connect(DCCWriteSocketNotifier, SIGNAL(activated(int)), this, SLOT(dccSent()));

	DccEnabled=true;

	kdebugf2();
}

void DccManager::closeDcc()
{
	kdebugf();
	if (DCCReadSocketNotifier)
	{
		delete DCCReadSocketNotifier;
		DCCReadSocketNotifier = NULL;
	}

	if (DCCWriteSocketNotifier)
	{
		delete DCCWriteSocketNotifier;
		DCCWriteSocketNotifier = NULL;
	}

	if (DccSock)
	{
		gadu->dccFree(DccSock);
		DccSock = NULL;
		gadu->setDccIpAndPort(0, 0);
	}
	DccEnabled=false;
	kdebugf2();
}


void DccManager::ifDccEnabled(bool dccEnabled)
{
	kdebugf();

	QCheckBox *autodetectIP = ConfigDialog::getCheckBox("Network", "DCC IP autodetection");
	QCheckBox *removeCompleted = ConfigDialog::getCheckBox("Network", "Remove completed transfers from transfers list");
	QVGroupBox *dccIP = ConfigDialog::getVGroupBox("Network", "DCC IP");
	QCheckBox *forwarding = ConfigDialog::getCheckBox("Network", "DCC forwarding enabled");
	QVGroupBox *forwardingProperties = ConfigDialog::getVGroupBox("Network", "DCC forwarding properties");

	autodetectIP->setEnabled(dccEnabled);
	removeCompleted->setEnabled(dccEnabled);
	dccIP->setEnabled(!autodetectIP->isChecked() && dccEnabled);
	forwarding->setEnabled(dccEnabled);
	forwardingProperties->setEnabled(forwarding->isChecked() && dccEnabled);

	kdebugf2();
}

void DccManager::ifDccIpEnabled(bool autodetect)
{
	kdebugf();
	ConfigDialog::getVGroupBox("Network", "DCC IP")->setEnabled(!autodetect);
	kdebugf2();
}

void DccManager::configDialogCreated()
{
	kdebugf();
	QCheckBox *dccEnabled = ConfigDialog::getCheckBox("Network", "DCC enabled");
	QCheckBox *removeCompleted = ConfigDialog::getCheckBox("Network", "Remove completed transfers from transfers list");
	QCheckBox *autodetectIP = ConfigDialog::getCheckBox("Network", "DCC IP autodetection");
	QVGroupBox *dccIP = ConfigDialog::getVGroupBox("Network", "DCC IP");
	QVGroupBox *forwardingProperties = ConfigDialog::getVGroupBox("Network", "DCC forwarding properties");
	QCheckBox *forwarding = ConfigDialog::getCheckBox("Network", "DCC forwarding enabled");

	autodetectIP->setEnabled(dccEnabled->isChecked());
	removeCompleted->setEnabled(dccEnabled->isChecked());
	dccIP->setEnabled(!autodetectIP->isChecked() && dccEnabled->isChecked());
	forwarding->setEnabled(dccEnabled->isChecked());
	forwardingProperties->setEnabled(dccEnabled->isChecked() && forwarding->isChecked());
	connect(forwarding, SIGNAL(toggled(bool)), forwardingProperties, SLOT(setEnabled(bool)));
	kdebugf2();
}

void DccManager::configDialogApply()
{
	kdebugf();

	QHostAddress host;
	if (!host.setAddress(config_file.readEntry("Network", "DccIP")))
		config_file.writeEntry("Network", "DccIP", "0.0.0.0");
	if (!host.setAddress(config_file.readEntry("Network", "ExternalIP")))
		config_file.writeEntry("Network", "ExternalIP", "0.0.0.0");

	kdebugf2();
}

void DccManager::dccConnectionReceived(const UserListElement& sender)
{
	kdebugf();
	struct gg_dcc* dcc_new;
	if (DccSocket::count() < 8 && sender.usesProtocol("Gadu"))
	{
		gadu->dccGetFile(htonl(sender.IP("Gadu").ip4Addr()),
						sender.port("Gadu"),
						config_file.readNumEntry("General","UIN"),
						sender.ID("Gadu").toUInt(),
						&dcc_new);
		if (dcc_new)
		{
			DccSocket* dcc_socket = new DccSocket(dcc_new);
			connect(dcc_socket, SIGNAL(dccFinished(DccSocket*)),
					this, SLOT(dccFinished(DccSocket*)));
			dcc_socket->initializeNotifiers();
		}
	}
	kdebugf2();
}

DccManager::TryType DccManager::initDCCConnection(uint32_t ip, uint16_t port,
								UinType my_uin, UinType peer_uin,
								const char *gadu_slot, int dcc_type,
								bool force_request)
{
	kdebugf();
	if (port>=10 && !force_request)
	{
		struct gg_dcc *sock=NULL;

		connect(this, SIGNAL(dccSig(uint32_t, uint16_t, UinType, UinType, struct gg_dcc **)),
				gadu, gadu_slot);

		emit dccSig(htonl(ip), port, my_uin, peer_uin, &sock);

		disconnect(this, SIGNAL(dccSig(uint32_t, uint16_t, UinType, UinType, struct gg_dcc **)),
				gadu, gadu_slot);

		if (sock)
		{
			DccSocket* dcc_socket = new DccSocket(sock);
			connect(dcc_socket, SIGNAL(dccFinished(DccSocket*)),
					this, SLOT(dccFinished(DccSocket*)));
			dcc_socket->initializeNotifiers();
		}
		else
			kdebugm(KDEBUG_WARNING, "socket is null (ip:%d port:%d my:%d peer:%d type:%d)\n", ip, port, my_uin, peer_uin, dcc_type);
		kdebugf2();
		return DIRECT;
	}
	else
	{
		kdebugm(KDEBUG_INFO, "user.port()<10, asking for connection (uin: %d)\n", peer_uin);
		dcc_manager->startTimeout();
		requests.insert(peer_uin, dcc_type);
		gadu->dccRequest(peer_uin);
		kdebugf2();
		return REQUEST;
	}
}

void DccManager::callbackReceived(DccSocket *sock)
{
	kdebugf();
	UinType peer_uin=sock->ggDccStruct()->peer_uin;
	if (requests.contains(peer_uin))
	{
		gadu->dccSetType(sock->ggDccStruct(), requests[peer_uin]);
		requests.remove(sock->ggDccStruct()->peer_uin);
	}
	kdebugf2();
}

DccManager* dcc_manager = NULL;

/** @} */

