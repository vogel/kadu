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
#include "kadu.h"
#include "message_box.h"
#include "userlist.h"

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

DccSocket::DccSocket(struct gg_dcc* dcc_sock)
{
	kdebugf();
	dccsock = dcc_sock;
	dccevent = NULL;
	readSocketNotifier = writeSocketNotifier = NULL;
	State = DCC_SOCKET_TRANSFERRING;
	in_watchDcc = false;
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

struct gg_dcc* DccSocket::ggDccStruct()
{
	return dccsock;
}

struct gg_event* DccSocket::ggDccEvent()
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
	UinsList uins;

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
			uins.append(dccsock->peer_uin);
			if (dccsock->uin != (UinType)config_file.readNumEntry("General", "UIN")
				|| !userlist.containsUin(dccsock->peer_uin) || isIgnored(uins))
			{
				setState(DCC_SOCKET_TRANSFER_DISCARDED);
				//emit dcc_manager->tranferDiscarded(this);
			}
			break;
		case GG_EVENT_NONE:
			emit dcc_manager->noneEvent(this);
			break;
		case GG_EVENT_DCC_CALLBACK:
			kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "GG_EVENT_DCC_CALLBACK! uin:%d peer_uin:%d\n",
				dccsock->uin, dccsock->peer_uin);
			dcc_manager->cancelTimeout();
			dcc_manager->callbackReceived(this);
			break;
		case GG_EVENT_DCC_NEED_FILE_ACK:
			kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "GG_EVENT_DCC_NEED_FILE_ACK! uin:%d peer_uin:%d\n",
				dccsock->uin, dccsock->peer_uin);
			emit dcc_manager->needFileAccept(this);
			break;
		case GG_EVENT_DCC_NEED_FILE_INFO:
			kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "GG_EVENT_DCC_NEED_FILE_INFO! uin:%d peer_uin:%d\n",
				dccsock->uin, dccsock->peer_uin);
			emit dcc_manager->needFileInfo(this);
			break;
		case GG_EVENT_DCC_ERROR:
			kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "GG_EVENT_DCC_ERROR\n");
			emit dcc_manager->dccError(this);
			return;
		case GG_EVENT_DCC_DONE:
			kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "GG_EVENT_DCC_DONE\n");
			setState(DCC_SOCKET_TRANSFER_FINISHED);
			emit dcc_manager->dccDone(this);
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

void DccSocket::setState(int pstate)
{
	kdebugf();
	readSocketNotifier->setEnabled(false);
	writeSocketNotifier->setEnabled(false);
	State = pstate;

	switch (State)
	{
		case DCC_SOCKET_TRANSFER_FINISHED:
			MessageBox::msg(tr("File has been transferred sucessfully."));
			break;
		case DCC_SOCKET_TRANSFER_DISCARDED:
			kdebugm(KDEBUG_INFO, "state: DCC_SOCKET_TRANSFER_DISCARDED\n");
			break;
		case DCC_SOCKET_TRANSFER_ERROR:
			MessageBox::msg(tr("File transfer error!"));
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

int DccSocket::state()
{
	return State;
}

int DccSocket::count()
{
	return Count;
}


DccManager::DccManager(QObject *parent, const char *name) : QObject(parent, name)
{
	kdebugf();
	ConfigDialog::addHotKeyEdit("ShortCuts", "Define keys", QT_TRANSLATE_NOOP("@default", "Send file"), "kadu_sendfile", "F8");

	ConfigDialog::addCheckBox("Network", "Network", QT_TRANSLATE_NOOP("@default", "DCC enabled"), "AllowDCC", false);
	ConfigDialog::addCheckBox("Network", "Network", QT_TRANSLATE_NOOP("@default", "DCC IP autodetection"), "DccIpDetect", true);

	ConfigDialog::addVGroupBox("Network", "Network", QT_TRANSLATE_NOOP("@default", "DCC IP"));
	ConfigDialog::addLineEdit("Network", "DCC IP", QT_TRANSLATE_NOOP("@default", "IP address:"),"DccIP");
	ConfigDialog::addCheckBox("Network", "Network", QT_TRANSLATE_NOOP("@default", "DCC forwarding enabled"), "DccForwarding", false);

	ConfigDialog::addVGroupBox("Network", "Network", QT_TRANSLATE_NOOP("@default", "DCC forwarding properties"));
	ConfigDialog::addLineEdit("Network", "DCC forwarding properties", QT_TRANSLATE_NOOP("@default", "External IP address:"), "ExternalIP");
	ConfigDialog::addLineEdit("Network", "DCC forwarding properties", QT_TRANSLATE_NOOP("@default", "External TCP port:"), "ExternalPort", "0");
	ConfigDialog::addLineEdit("Network", "DCC forwarding properties", QT_TRANSLATE_NOOP("@default", "Local TCP port:"), "LocalPort", "1550");

	ConfigDialog::connectSlot("Network", "DCC enabled", SIGNAL(toggled(bool)), this, SLOT(ifDccEnabled(bool)));
	ConfigDialog::connectSlot("Network", "DCC IP autodetection", SIGNAL(toggled(bool)), this, SLOT(ifDccIpEnabled(bool)));

	ConfigDialog::registerSlotOnCreate(this, SLOT(configDialogCreated()));
	ConfigDialog::registerSlotOnApply(this, SLOT(configDialogApply()));

	DccSock = NULL;
	DCCReadSocketNotifier = NULL;
	DCCWriteSocketNotifier = NULL;

	if (!config_file.readBoolEntry("Network","DccIpDetect"))
		ConfigDccIp.setAddress(config_file.readEntry("Network","DccIP", ""));

	QHostAddress ext_ip;
	if (config_file.readBoolEntry("Network","DccForwarding"))
		if (ext_ip.setAddress(config_file.readEntry("Network","ExternalIP", "")))
		{
			gadu->setDccExternalIP(ext_ip);
			ConfigDccPort=config_file.readNumEntry("Network", "ExternalPort", 1550);
		}

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

	ConfigDialog::unregisterSlotOnCreate(this, SLOT(configDialogCreated()));
	ConfigDialog::unregisterSlotOnApply(this, SLOT(configDialogApply()));

	ConfigDialog::removeControl("Network", "DCC enabled");
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

QHostAddress DccManager::configDccIp()
{
	return ConfigDccIp;
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

	QHostAddress dccIp;
	short int dccPort;

	gadu->dccSocketCreate(config_file.readNumEntry("General", "UIN"), config_file.readNumEntry("Network", "LocalPort", 1550), DccSock);

	if (!DccSock)
	{
		kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "Couldn't bind DCC socket.\n");
		gadu->dccFree(DccSock);

		MessageBox::wrn(tr("Couldn't create DCC socket.\nDirect connections disabled."));
		kdebugf2();
		return;
	}

	if (ConfigDccIp == QHostAddress())
	{
		dccIp.setAddress("255.255.255.255");
		dccPort = DccSock->port;
	}
	else
	{
		dccIp = ConfigDccIp;
		dccPort = ConfigDccPort;
	}

	gadu->setDccIpAndPort(htonl(dccIp.ip4Addr()), dccPort);

	kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "DCC_IP=%s DCC_PORT=%d\n", dccIp.toString().latin1(), dccPort);

	DCCReadSocketNotifier = new QSocketNotifier(DccSock->fd, QSocketNotifier::Read, kadu);
	connect(DCCReadSocketNotifier, SIGNAL(activated(int)), this, SLOT(dccReceived()));

	DCCWriteSocketNotifier = new QSocketNotifier(DccSock->fd, QSocketNotifier::Write, kadu);
	connect(DCCWriteSocketNotifier, SIGNAL(activated(int)), this, SLOT(dccSent()));

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
	kdebugf2();
}


void DccManager::ifDccEnabled(bool value)
{
	kdebugf();

	QCheckBox *b_dccip= ConfigDialog::getCheckBox("Network", "DCC IP autodetection");
	QVGroupBox *g_dccip = ConfigDialog::getVGroupBox("Network", "DCC IP");
	QVGroupBox *g_fwdprop = ConfigDialog::getVGroupBox("Network", "DCC forwarding properties");
	QCheckBox *b_dccfwd = ConfigDialog::getCheckBox("Network", "DCC forwarding enabled");

	b_dccip->setEnabled(value);
	g_dccip->setEnabled(!b_dccip->isChecked() && value);
	b_dccfwd->setEnabled(value);
	g_fwdprop->setEnabled(b_dccfwd->isChecked() && value);
	kdebugf2();
}

void DccManager::ifDccIpEnabled(bool value)
{
	kdebugf();
	ConfigDialog::getVGroupBox("Network", "DCC IP")->setEnabled(!value);
	kdebugf2();
}

void DccManager::configDialogCreated()
{
	kdebugf();
	QCheckBox *b_dccenabled = ConfigDialog::getCheckBox("Network", "DCC enabled");
	QCheckBox *b_dccip= ConfigDialog::getCheckBox("Network", "DCC IP autodetection");
	QVGroupBox *g_dccip = ConfigDialog::getVGroupBox("Network", "DCC IP");
	QVGroupBox *g_fwdprop = ConfigDialog::getVGroupBox("Network", "DCC forwarding properties");
	QCheckBox *b_dccfwd = ConfigDialog::getCheckBox("Network", "DCC forwarding enabled");

	b_dccip->setEnabled(b_dccenabled->isChecked());
	g_dccip->setEnabled(!b_dccip->isChecked()&& b_dccenabled->isChecked());
	b_dccfwd->setEnabled(b_dccenabled->isChecked());
	g_fwdprop->setEnabled(b_dccenabled->isChecked() && b_dccfwd->isChecked());
	connect(b_dccfwd, SIGNAL(toggled(bool)), g_fwdprop, SLOT(setEnabled(bool)));
	kdebugf2();
}

void DccManager::configDialogApply()
{
	kdebugf();
	if (config_file.readBoolEntry("Network","DccIpDetect"))
		ConfigDccIp = QHostAddress();
	else
		if (!ConfigDccIp.setAddress(config_file.readEntry("Network", "DccIP")))
		{
			config_file.writeEntry("Network", "DccIP", "0.0.0.0");
			ConfigDccIp = QHostAddress();
		}
	QHostAddress ext_ip;
	if (!ext_ip.setAddress(config_file.readEntry("Network", "ExternalIP")))	
		config_file.writeEntry("Network", "ExternalIP", "0.0.0.0");
	if (!config_file.readBoolEntry("Network","DccForwarding"))
		ext_ip = QHostAddress();
	gadu->setDccExternalIP(ext_ip);
	if (config_file.readNumEntry("Network", "ExternalPort")<=1023)
		config_file.writeEntry("Network", "ExternalPort", 0);
	kdebugf2();
}

void DccManager::dccConnectionReceived(const UserListElement& sender)
{
	kdebugf();
	struct gg_dcc* dcc_new;
	if (DccSocket::count() < 8)
	{
		gadu->dccGetFile(htonl(sender.ip().ip4Addr()), sender.port(), config_file.readNumEntry("General","UIN"), sender.uin(), dcc_new);
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

void DccManager::initDCCConnection(uint32_t ip, uint16_t port, UinType my_uin, UinType peer_uin, const char *gadu_slot, int dcc_type)
{
	kdebugf();
	if (port>=10)
	{
		struct gg_dcc *sock=NULL;
		connect(this, SIGNAL(dccSig(uint32_t, uint16_t, UinType, UinType, struct gg_dcc *&)), gadu, gadu_slot);
		emit dccSig(htonl(ip), port, my_uin, peer_uin, sock);
		disconnect(this, SIGNAL(dccSig(uint32_t, uint16_t, UinType, UinType, struct gg_dcc *&)), gadu, gadu_slot);
		if (sock)
		{
			DccSocket* dcc_socket = new DccSocket(sock);
			connect(dcc_socket, SIGNAL(dccFinished(DccSocket*)),
					this, SLOT(dccFinished(DccSocket*)));
			dcc_socket->initializeNotifiers();
		}
		else
			kdebugm(KDEBUG_WARNING, "socket is null (ip:%d port:%d my:%d peer:%d type:%d)\n", ip, port, my_uin, peer_uin, dcc_type);
	}
	else
	{
		kdebugm(KDEBUG_INFO, "user.port()<10, asking for connection (uin: %d)\n", peer_uin);
		dcc_manager->startTimeout();
		requests.insert(peer_uin, dcc_type);
		gadu->dccRequest(peer_uin);
	}
	kdebugf2();
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
