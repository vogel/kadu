/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "voice.h"

#include <qpushbutton.h>
#include <qlayout.h>
#include <qmessagebox.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "voice_dsp.h"
#include "userbox.h"
#include "userlist.h"
#include "config_file.h"
#include "debug.h"
#include "config_dialog.h"
#include "kadu.h"
#include "gadu.h"

extern "C" int voice_init()
{
	voice_manager = new VoiceManager(NULL, "voice_manager");
	voice_dsp = new VoiceDsp(NULL, "voice_dsp");
	return 0;
}

extern "C" void voice_close()
{
	delete voice_dsp;
	voice_dsp = NULL;
	delete voice_manager;
	voice_manager = NULL;
}

PlayThread::PlayThread(): wsem(32), rsem(1)
{
	wsem += 32;
}

void PlayThread::run()
{
	kdebugf();
	struct gsm_sample gsmsample;
	while (true)
	{
		kdebugmf(KDEBUG_INFO, "rsem = %d\n", rsem.available());
		if (!rsem.available())
			break;
		wsem++;
		mutex.lock();
		kdebugmf(KDEBUG_INFO, "wokenUp\n");
		if (queue.empty())
		{
			mutex.unlock();
			continue;
		}
		gsmsample = queue.front();
		queue.pop_front();
		mutex.unlock();
		emit playGsmSample(gsmsample.data, gsmsample.length);
		delete gsmsample.data;
	}
	kdebugf2();
}

RecordThread::RecordThread(): rsem(1)
{
}

void RecordThread::run()
{
	kdebugf();
	char data[GG_DCC_VOICE_FRAME_LENGTH_505];
	int length = GG_DCC_VOICE_FRAME_LENGTH_505;
	while (rsem.available())
		emit recordSample(data, length);
	kdebugf2();
}

VoiceChatDialog::VoiceChatDialog(DccSocket* socket)
	: QDialog (NULL, "voice_chat_dialog"), Socket(socket)
{
	kdebugf();
	setWFlags(Qt::WDestructiveClose);
	setCaption(tr("Voice chat"));
	resize(200, 100);

	QPushButton *b_stop = new QPushButton(tr("&Stop"), this);

	QGridLayout *grid = new QGridLayout(this, 1, 1, 3, 3);
	grid->addWidget(b_stop, 0, 0, Qt::AlignCenter);

	connect(b_stop, SIGNAL(clicked()), this, SLOT(close()));
	show();
	
	voice_manager->setup();
	Dialogs.insert(socket, this);
	kdebugf2();
}

VoiceChatDialog::~VoiceChatDialog()
{
	kdebugf();
	Dialogs.remove(Socket);
	voice_manager->free();
	delete Socket;
	kdebugf2();
}

VoiceChatDialog* VoiceChatDialog::bySocket(DccSocket* socket)
{
	if (Dialogs.contains(socket))
		return Dialogs[socket];
	else
		return NULL;
}

void VoiceChatDialog::destroyAll()
{
	kdebugf();
	while (!Dialogs.empty())
		delete Dialogs.begin().data();
	kdebugf2();
}

void VoiceChatDialog::sendDataToAll(char* data, int length)
{
	for (QMap<DccSocket*, VoiceChatDialog*>::const_iterator i = Dialogs.begin();
		i != Dialogs.end(); ++i)
	{
		gadu->dccVoiceSend(i.key()->ggDccStruct(), data, length);
	}
}

QMap<DccSocket*, VoiceChatDialog*> VoiceChatDialog::Dialogs;

VoiceManager::VoiceManager(QObject *parent, const char *name) : QObject(parent, name)
{
	kdebugf();
	ConfigDialog::addHotKeyEdit("ShortCuts", "Define keys",
			QT_TRANSLATE_NOOP("@default", "Voice chat"), "kadu_voicechat", "F7");

	voice_enc = voice_dec = NULL;
	pt = new PlayThread();
	rt = new RecordThread();
	connect(pt, SIGNAL(playGsmSample(char *, int)), this, SLOT(playGsmSampleReceived(char *, int)));
	connect(rt, SIGNAL(recordSample(char *, int)), this, SLOT(recordSampleReceived(char *, int)));
	UserBox::userboxmenu->addItemAtPos(2,"VoiceChat", tr("Voice chat"), this,
		SLOT(makeVoiceChat()), HotKey::shortCutFromFile("ShortCuts", "kadu_voicechat"));
	connect(UserBox::userboxmenu,SIGNAL(popup()),this,SLOT(userBoxMenuPopup()));
	connect(kadu,SIGNAL(keyPressed(QKeyEvent*)),this,SLOT(mainDialogKeyPressed(QKeyEvent*)));
	connect(dcc_manager, SIGNAL(connectionBroken(DccSocket*)),
		this, SLOT(connectionBroken(DccSocket*)));
	connect(dcc_manager, SIGNAL(dccError(DccSocket*)),
		this, SLOT(dccError(DccSocket*)));
	connect(dcc_manager, SIGNAL(callbackReceived(DccSocket*)),
		this, SLOT(callbackReceived(DccSocket*)));
	connect(dcc_manager, SIGNAL(dccEvent(DccSocket*)),
		this, SLOT(dccEvent(DccSocket*)));
	connect(dcc_manager, SIGNAL(socketDestroying(DccSocket*)),
		this, SLOT(socketDestroying(DccSocket*)));
	kdebugf2();
}

VoiceManager::~VoiceManager()
{
	kdebugf();
	ConfigDialog::removeControl("ShortCuts", "Voice chat");
	int voice_chat_item = UserBox::userboxmenu->getItem(tr("Voice chat"));
	UserBox::userboxmenu->removeItem(voice_chat_item);
	disconnect(UserBox::userboxmenu,SIGNAL(popup()),this,SLOT(userBoxMenuPopup()));
	disconnect(kadu,SIGNAL(keyPressed(QKeyEvent*)),this,SLOT(mainDialogKeyPressed(QKeyEvent*)));
	disconnect(dcc_manager, SIGNAL(connectionBroken(DccSocket*)),
		this, SLOT(connectionBroken(DccSocket*)));
	disconnect(dcc_manager, SIGNAL(dccError(DccSocket*)),
		this, SLOT(dccError(DccSocket*)));
	disconnect(dcc_manager, SIGNAL(callbackReceived(DccSocket*)),
		this, SLOT(callbackReceived(DccSocket*)));
	disconnect(dcc_manager, SIGNAL(dccEvent(DccSocket*)),
		this, SLOT(dccEvent(DccSocket*)));
	disconnect(dcc_manager, SIGNAL(socketDestroying(DccSocket*)),
		this, SLOT(socketDestroying(DccSocket*)));
	VoiceChatDialog::destroyAll();
	kdebugf2();
}

void VoiceManager::setup()
{
	kdebugf();
	if (!pt->running())
	{
		emit setupSoundDevice();
		pt->rsem--;
		pt->start();
	}
	if (!rt->running())
	{
		rt->rsem--;
		rt->start();
	}	
	kdebugf2();
}

void VoiceManager::free()
{
	kdebugf();
	struct gsm_sample gsmsample;
	if (rt->running())
		rt->rsem++;
	if (pt->running()) {
		pt->wsem--;
		pt->rsem++;
		pt->mutex.lock();
		while (!pt->queue.empty())
		{
			gsmsample = pt->queue.front();
			pt->queue.pop_front();
			delete gsmsample.data;
		}
		pt->mutex.unlock();
		emit freeSoundDevice();
	}
	kdebugf2();
}

void VoiceManager::resetCodec()
{
	kdebugf();
	resetCoder();
	resetDecoder();
}

void VoiceManager::resetCoder()
{
	kdebugf();
	int value = 1;
	if (voice_enc)
		gsm_destroy(voice_enc);
	voice_enc = gsm_create();
	gsm_option(voice_enc, GSM_OPT_FAST, &value);
	gsm_option(voice_enc, GSM_OPT_WAV49, &value);
	kdebugf2();
}

void VoiceManager::resetDecoder()
{
	kdebugf();
	int value = 1;
	if (voice_dec)
		gsm_destroy(voice_dec);
	voice_dec = gsm_create();
	gsm_option(voice_dec, GSM_OPT_FAST, &value);
	gsm_option(voice_dec, GSM_OPT_WAV49, &value);
	gsm_option(voice_dec, GSM_OPT_VERBOSE, &value);
	gsm_option(voice_dec, GSM_OPT_LTP_CUT, &value);
	kdebugf2();
}

void VoiceManager::playGsmSampleReceived(char *data, int length)
{
	kdebugf();
	const char *pos = data;
	int outlen = 320;
	gsm_signal output[160];
	resetDecoder();
	++data;
	++pos;
	--length;
	while (pos <= (data + length - 65))
	{
		if (gsm_decode(voice_dec, (gsm_byte *) pos, output))
		{
			kdebugmf(KDEBUG_ERROR, "gsm_decode() error\n");
			return;
		}
		pos += 33;
		emit playSample((char *) output, outlen);
		if (gsm_decode(voice_dec, (gsm_byte *) pos, output))
		{
			kdebugmf(KDEBUG_ERROR, "gsm_decode() error\n");
			return;
		}
		pos += 32;
		emit playSample((char *) output, outlen);
	}
	kdebugf2();
}

void VoiceManager::recordSampleReceived(char *data, int length)
{
	kdebugf();
	const char *pos = data;
	int inlen = 320;
	gsm_signal input[160];
	resetCoder();
	*data = 0;
	++data;
	++pos;
	--length;
	while (pos <= (data + length - 65)) {
		emit recordSample((char *) input, inlen);
		gsm_encode(voice_enc, input, (gsm_byte *) pos);
		pos += 32;
		emit recordSample((char *) input, inlen);
		gsm_encode(voice_enc, input, (gsm_byte *) pos);
		pos += 33;
	}
	VoiceChatDialog::sendDataToAll(data - 1, length + 1);
	kdebugf2();
}

void VoiceManager::addGsmSample(char *data, int length)
{
	kdebugf();
	struct gsm_sample gsmsample;
	gsmsample.data = data;
	gsmsample.length = length;
	pt->mutex.lock();
	pt->queue << gsmsample;
	pt->mutex.unlock();
	pt->wsem--;
	kdebugf2();
}

void VoiceManager::makeVoiceChat()
{
	kdebugf();
	if (config_file.readBoolEntry("Network", "AllowDCC"))
		if (dcc_manager->configDccIp().isIp4Addr())
		{
			struct gg_dcc *dcc_new;
			UserBox *activeUserBox=UserBox::getActiveUserBox();
			UserList users;
			if (activeUserBox==NULL)
				return;
			users = activeUserBox->getSelectedUsers();
			if (users.count() != 1)
				return;
			UserListElement user = (*users.begin());
			if (user.port() >= 10)
			{
				kdebugm(KDEBUG_INFO, "ip: %s, port: %d, uin: %d\n", user.ip().toString().local8Bit().data(), user.port(), user.uin());
				if ((dcc_new = gadu->dccVoiceChat(htonl(user.ip().ip4Addr()), user.port(),
					config_file.readNumEntry("General", "UIN"), user.uin())) != NULL) {
					DccSocket* socket = new DccSocket(dcc_new);
					connect(socket, SIGNAL(dccFinished(DccSocket *)), dcc_manager,
						SLOT(dccFinished(DccSocket *)));
					socket->initializeNotifiers();
				}
			}
			else
			{
				kdebugm(KDEBUG_INFO, "user.port()<10, asking for connection (uin: %d)\n", user.uin());
				dcc_manager->startTimeout();
				Requests.insert(user.uin(), true);
				gadu->dccRequest(user.uin());
			}
		}
	kdebugf2();
}

void VoiceManager::askAcceptVoiceChat(DccSocket* socket)
{
	kdebugf();
	QString text=tr("User %1 wants to talk with you. Do you accept it?");
	if (userlist.containsUin(socket->ggDccStruct()->peer_uin))
		text=text.arg(userlist.byUin(socket->ggDccStruct()->peer_uin).altNick());
	else
		text=text.arg(socket->ggDccStruct()->peer_uin);

	switch (QMessageBox::information(0, tr("Incoming voice chat"), text, tr("Yes"), tr("No"),
		QString::null, 0, 1))
	{
		case 0: // Yes?
			kdebugmf(KDEBUG_INFO, "accepted\n");
			new VoiceChatDialog(socket);
			break;
		case 1:
			kdebugmf(KDEBUG_INFO, "discarded\n");
			socket->setState(DCC_SOCKET_VOICECHAT_DISCARDED);
			break;
	}
	kdebugf2();
}

void VoiceManager::mainDialogKeyPressed(QKeyEvent* e)
{
	if (HotKey::shortCut(e,"ShortCuts", "kadu_voicechat"))
		makeVoiceChat();
}

void VoiceManager::userBoxMenuPopup()
{
	kdebugf();
	UserBox* activeUserBox=UserBox::getActiveUserBox();
	if (activeUserBox==NULL) //to siê zdarza...
		return;
	UserList users = activeUserBox->getSelectedUsers();
	UserListElement user = (*users.begin());

	bool isOurUin=users.containsUin(config_file.readNumEntry("General", "UIN"));
	int voicechat = UserBox::userboxmenu->getItem(tr("Voice chat"));

	if (DccSocket::count() < 8 &&
		users.count() == 1 &&
		!isOurUin &&
		config_file.readBoolEntry("Network", "AllowDCC") &&
		(user.status().isOnline() || user.status().isBusy())
	   )

		UserBox::userboxmenu->setItemEnabled(voicechat, true);
	else
		UserBox::userboxmenu->setItemEnabled(voicechat, false);
	kdebugf2();
}

void VoiceManager::connectionBroken(DccSocket* socket)
{
	kdebugf();
	if (VoiceChatDialog::bySocket(socket) != NULL)
		socket->setState(DCC_SOCKET_VOICECHAT_DISCARDED);
	kdebugf2();
}

void VoiceManager::callbackReceived(DccSocket* socket)
{
	kdebugf();
	if (Requests.contains(socket->ggDccStruct()->peer_uin))
	{
		gadu->dccSetType(socket->ggDccStruct(), GG_SESSION_DCC_VOICE);
		Requests.remove(socket->ggDccStruct()->peer_uin);
	}
	kdebugf2();
}

void VoiceManager::dccError(DccSocket* socket)
{
	kdebugf();
	if (VoiceChatDialog::bySocket(socket) != NULL)
		socket->setState(DCC_SOCKET_VOICECHAT_DISCARDED);
	kdebugf2();
}

void VoiceManager::dccEvent(DccSocket* socket)
{
	char* voice_buf;
	switch (socket->ggDccEvent()->type)
	{
		case GG_EVENT_DCC_NEED_VOICE_ACK:
			kdebugmf(KDEBUG_INFO, "GG_EVENT_DCC_NEED_VOICE_ACK! %d %d\n",
				socket->ggDccStruct()->uin, socket->ggDccStruct()->peer_uin);
			askAcceptVoiceChat(socket);
			break;
		case GG_EVENT_DCC_ACK:
			kdebugmf(KDEBUG_INFO, "GG_EVENT_DCC_ACK\n");
			if (socket->ggDccStruct()->type == GG_SESSION_DCC_VOICE)
				new VoiceChatDialog(socket);
			break;
		case GG_EVENT_DCC_VOICE_DATA:
			kdebugmf(KDEBUG_INFO, "GG_EVENT_DCC_VOICE_DATA\n");
			voice_buf = new char[socket->ggDccEvent()->event.dcc_voice_data.length];
			memcpy(voice_buf, socket->ggDccEvent()->event.dcc_voice_data.data,
				socket->ggDccEvent()->event.dcc_voice_data.length);
			voice_manager->addGsmSample(voice_buf,
				socket->ggDccEvent()->event.dcc_voice_data.length);
			break;
	}
}

void VoiceManager::socketDestroying(DccSocket* socket)
{
	kdebugf();
	VoiceChatDialog *dialog=VoiceChatDialog::bySocket(socket);
	if (dialog)
		delete dialog;
	kdebugf2();
}

VoiceManager* voice_manager = NULL;
