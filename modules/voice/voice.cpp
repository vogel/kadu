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
	voice_manager = new VoiceManager();
	voice_dsp = new VoiceDsp();
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
		kdebugm(KDEBUG_INFO, "PlayThread::run(): rsem = %d\n", rsem.available());
		if (!rsem.available())
			break;
		wsem++;
		mutex.lock();
		kdebugm(KDEBUG_INFO, "PlayThread::run(): wokenUp\n");
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
	while (true)
	{
		if (!rsem.available())
			break;
		emit recordSample(data, length);
	}
	kdebugf2();
}

VoiceManager::VoiceManager()
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
			kdebugm(KDEBUG_ERROR, "VoiceManager::playGsmSampleReceived(): gsm_decode() error\n");
			return;
		}
		pos += 33;
		emit playSample((char *) output, outlen);
		if (gsm_decode(voice_dec, (gsm_byte *) pos, output))
		{
			kdebugm(KDEBUG_ERROR, "VoiceManager::playGsmSampleReceived(): gsm_decode() error\n");
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
	emit gsmSampleRecorded(data - 1, length + 1);
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
			if (user.port >= 10)
			{
				if ((dcc_new = gadu->dccVoiceChat(htonl(user.ip.ip4Addr()), user.port,
					config_file.readNumEntry("General", "UIN"), user.uin)) != NULL) {
					VoiceSocket* dcc = new VoiceSocket(dcc_new);
					connect(dcc, SIGNAL(dccFinished(DccSocket *)), this,
						SLOT(dccFinished(DccSocket *)));
					dcc->initializeNotifiers();
				}
			}
			else
				gadu->dccRequest(user.uin);
		}
	kdebugf2();
}

void VoiceManager::dccFinished(DccSocket* dcc)
{
	kdebugf();
	delete dcc;
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
		(user.status->isOnline() || user.status->isBusy())
	   )

		UserBox::userboxmenu->setItemEnabled(voicechat, true);
	else
		UserBox::userboxmenu->setItemEnabled(voicechat, false);
	kdebugf2();
}

DccVoiceDialog::DccVoiceDialog(QDialog *parent, const char *name)
	: QDialog (parent, name, FALSE, Qt::WDestructiveClose)
{
	kdebugf();
	setCaption(tr("Voice chat"));
	resize(200, 100);

	QPushButton *b_stop = new QPushButton(tr("&Stop"), this);

	QGridLayout *grid = new QGridLayout(this, 1, 1, 3, 3);
	grid->addWidget(b_stop, 0, 0, Qt::AlignCenter);

	connect(b_stop, SIGNAL(clicked()), this, SLOT(close()));
	show();
	kdebugf2();
}

void DccVoiceDialog::closeEvent(QCloseEvent *e)
{
	kdebugf();
	emit cancelVoiceChat();
	QDialog::closeEvent(e);
	kdebugf2();
}

VoiceSocket::VoiceSocket(struct gg_dcc* dcc_sock)
	: DccSocket(dcc_sock)
{
	voicedialog = new DccVoiceDialog();
	connect(voicedialog, SIGNAL(cancelVoiceChat()), this, SLOT(cancelVoiceChatReceived()));
}

VoiceSocket::~VoiceSocket()
{
	if (voicedialog)
	{
		voicedialog->close();
		voicedialog = NULL;
	}
}

void VoiceSocket::connectionBroken()
{
	kdebugf();
	DccSocket::connectionBroken();
	voice_manager->free();
	if (voicedialog)
		disconnect(voicedialog, SIGNAL(cancelVoiceChat()), this, SLOT(cancelVoiceChatReceived()));
	setState(DCC_SOCKET_VOICECHAT_DISCARDED);
	kdebugf2();
}

void VoiceSocket::dccError()
{
	kdebugf();
	DccSocket::dccError();
	voice_manager->free();
	if (voicedialog)
		disconnect(voicedialog, SIGNAL(cancelVoiceChat()), this, SLOT(cancelVoiceChatReceived()));
	setState(DCC_SOCKET_VOICECHAT_DISCARDED);
	kdebugf2();
}

void VoiceSocket::dccEvent()
{
	DccSocket::dccEvent();
	char* voice_buf;
	switch(dccevent->type)
	{
		case GG_EVENT_DCC_NEED_VOICE_ACK:
			kdebugm(KDEBUG_INFO, "VoiceSocket::dccEvent():  GG_EVENT_DCC_NEED_VOICE_ACK! %d %d\n",
				dccsock->uin, dccsock->peer_uin);
			askAcceptVoiceChat();
			break;
		case GG_EVENT_DCC_ACK:
			voice_manager->setup();
			break;
		case GG_EVENT_DCC_VOICE_DATA:
			voice_buf = new char[dccevent->event.dcc_voice_data.length];
			memcpy(voice_buf, dccevent->event.dcc_voice_data.data,
				dccevent->event.dcc_voice_data.length);
			voice_manager->addGsmSample(voice_buf,
				dccevent->event.dcc_voice_data.length);
			break;
	}
}

void VoiceSocket::askAcceptVoiceChat()
{
	kdebugf();
	QString str=tr("User %1 wants to talk with you. Do you accept it?").arg(userlist.byUin(dccsock->peer_uin).altnick);

	switch (QMessageBox::information(0, tr("Incoming voice chat"), str, tr("Yes"), tr("No"),
		QString::null, 0, 1))
	{
		case 0: // Yes?
			kdebugm(KDEBUG_INFO, "VoiceSocket::askAcceptVoiceChat(): accepted\n");
			voicedialog = new DccVoiceDialog();
			connect(voicedialog, SIGNAL(cancelVoiceChat()), this, SLOT(cancelVoiceChatReceived()));
			voice_manager->setup();
			break;
		case 1:
			kdebugm(KDEBUG_INFO, "VoiceSocket::::askAcceptVoiceChat(): discarded\n");
			setState(DCC_SOCKET_VOICECHAT_DISCARDED);
			break;
	}
	kdebugf2();
}

void VoiceSocket::initializeNotifiers()
{
	DccSocket::initializeNotifiers();
	connect(voice_manager, SIGNAL(gsmSampleRecorded(char *, int)), this, SLOT(voiceDataRecorded(char *, int)));
}

void VoiceSocket::cancelVoiceChatReceived()
{
	kdebugf();
	voicedialog = NULL;
	voice_manager->free();
	deleteLater();
	kdebugf2();
}

void VoiceSocket::voiceDataRecorded(char *data, int length)
{
	kdebugf();
	gadu->dccVoiceSend(dccsock, data, length);
}

VoiceManager* voice_manager = NULL;
