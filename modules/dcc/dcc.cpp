/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "dcc.h"

#include <qfiledialog.h>
#include <qmessagebox.h>
#include <qpushbutton.h>
#include <qfileinfo.h>
#include <qlayout.h>

#include <fcntl.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>

#include "file_transfer.h"
#include "kadu.h"
#include "ignore.h"
#include "debug.h"
#include "message_box.h"
#include "config_dialog.h"
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
	snr = snw = NULL;
	State = DCC_SOCKET_TRANSFERRING;
	in_watchDcc = false;
	++Count;
	kdebugmf(KDEBUG_FUNCTION_END|KDEBUG_INFO, "dcc sockets count = %d\n", Count);
}

DccSocket::~DccSocket()
{
	kdebugf();
	emit dcc_manager->socketDestroying(this);
	if (snr)
	{
		snr->setEnabled(false);
		delete snr;
		snr = NULL;
	}
	if (snw)
	{
		snw->setEnabled(false);
		delete snw;
		snw = NULL;
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
	snr = new QSocketNotifier(dccsock->fd, QSocketNotifier::Read, this);
	QObject::connect(snr, SIGNAL(activated(int)), this, SLOT(dccDataReceived()));

	snw = new QSocketNotifier(dccsock->fd, QSocketNotifier::Write, this);
	QObject::connect(snw, SIGNAL(activated(int)), this, SLOT(dccDataSent()));
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
	snw->setEnabled(false);
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
			emit dcc_manager->callbackReceived(this);
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
		snw->setEnabled(true);

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
	snr->setEnabled(false);
	snw->setEnabled(false);
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
	DccSnr = NULL;
	DccSnw = NULL;

	if (!config_file.readBoolEntry("Network","DccIpDetect"))
		ConfigDccIp.setAddress(config_file.readEntry("Network","DccIP", ""));

	QHostAddress ext_ip;
	if (config_file.readBoolEntry("Network","DccForwarding"))
		if (ext_ip.setAddress(config_file.readEntry("Network","ExternalIP", "")))
			gadu->setDccExternalIP(ext_ip);

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
		delete DccSnr;
		DccSnr = NULL;
		delete DccSnw;
		DccSnw = NULL;
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
				DccSocket* dcc = new DccSocket(dcc_e->event.dcc_new);
				connect(dcc, SIGNAL(dccFinished(DccSocket *)), this, SLOT(dccFinished(DccSocket *)));
				dcc->initializeNotifiers();	
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
		DccSnw->setEnabled(true);

	gadu->freeEvent(dcc_e);
	kdebugf2();
}

void DccManager::dccFinished(DccSocket* dcc)
{
	kdebugf();
	delete dcc;
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
	DccSnw->setEnabled(false);
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

	if (ConfigDccIp.isNull())
		dccIp.setAddress("255.255.255.255");
	else
		dccIp = ConfigDccIp;

	DccSock = gadu->dccSocketCreate(config_file.readNumEntry("General", "UIN"), config_file.readNumEntry("Network", "LocalPort", 1550));

	if (!DccSock)
	{
		kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "Couldn't bind DCC socket.\n");
		gadu->dccFree(DccSock);

		MessageBox::wrn(tr("Couldn't create DCC socket.\nDirect connections disabled."));
		kdebugf2();
		return;
	}

	gadu->setDccIpAndPort(htonl(dccIp.ip4Addr()), DccSock->port);

	kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "DCC_IP=%s DCC_PORT=%d\n", dccIp.toString().latin1(), DccSock->port);

	DccSnr = new QSocketNotifier(DccSock->fd, QSocketNotifier::Read, kadu);
	connect(DccSnr, SIGNAL(activated(int)), this, SLOT(dccReceived()));

	DccSnw = new QSocketNotifier(DccSock->fd, QSocketNotifier::Write, kadu);
	connect(DccSnw, SIGNAL(activated(int)), this, SLOT(dccSent()));

	kdebugf2();
}

void DccManager::closeDcc()
{
	kdebugf();
	if (DccSnr)
	{
		delete DccSnr;
		DccSnr = NULL;
	}

	if (DccSnw)
	{
		delete DccSnw;
		DccSnw = NULL;
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
		dcc_new = gadu->dccGetFile(htonl(sender.ip().ip4Addr()), sender.port(), config_file.readNumEntry("General","UIN"), sender.uin());
		if (dcc_new)
		{
			DccSocket* dcc = new DccSocket(dcc_new);
			connect(dcc, SIGNAL(dccFinished(DccSocket*)), this, SLOT(dccFinished(DccSocket*)));
			dcc->initializeNotifiers();
		}
	}
	kdebugf2();
}

DccManager* dcc_manager = NULL;
