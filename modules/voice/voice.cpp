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

#include "userbox.h"
#include "userlist.h"
#include "config_file.h"
#include "debug.h"
#include "config_dialog.h"
#include "kadu.h"
#include "gadu.h"
#include "message_box.h"

extern "C" int voice_init()
{
	voice_manager = new VoiceManager(NULL, "voice_manager");
	return 0;
}

extern "C" void voice_close()
{
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
		delete [] gsmsample.data;
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
	FOREACH(i, Dialogs)
		gadu->dccVoiceSend(i.key()->ggDccStruct(), data, length);
}

QMap<DccSocket*, VoiceChatDialog*> VoiceChatDialog::Dialogs;

VoiceManager::VoiceManager(QObject *parent, const char *name) : QObject(parent, name)
{
	kdebugf();
	ConfigDialog::addHotKeyEdit("ShortCuts", "Define keys",
			QT_TRANSLATE_NOOP("@default", "Voice chat"), "kadu_voicechat", "F7");
	ConfigDialog::addHGroupBox("Sounds", "Sounds", QT_TRANSLATE_NOOP("@default","Voice chat"));
	ConfigDialog::addPushButton("Sounds", "Voice chat", QT_TRANSLATE_NOOP("@default","Test GSM Encoding"));
	ConfigDialog::connectSlot("Sounds", "Test GSM Encoding", SIGNAL(clicked()), this, SLOT(testGsmEncoding()));

	GsmEncodingTestMsgBox = NULL;
	GsmEncodingTestDevice = NULL;
	GsmEncodingTestSample = NULL;

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
	connect(dcc_manager, SIGNAL(dccEvent(DccSocket*)),
		this, SLOT(dccEvent(DccSocket*)));
	connect(dcc_manager, SIGNAL(socketDestroying(DccSocket*)),
		this, SLOT(socketDestroying(DccSocket*)));
	kdebugf2();
}

VoiceManager::~VoiceManager()
{
	kdebugf();
	ConfigDialog::disconnectSlot("Sounds", "Test GSM Encoding", SIGNAL(clicked()), this, SLOT(testGsmEncoding()));
	ConfigDialog::removeControl("Sounds", "Test GSM Encoding");
	ConfigDialog::removeControl("Sounds", "Voice chat");
	ConfigDialog::removeControl("ShortCuts", "Voice chat");
	int voice_chat_item = UserBox::userboxmenu->getItem(tr("Voice chat"));
	UserBox::userboxmenu->removeItem(voice_chat_item);
	disconnect(UserBox::userboxmenu,SIGNAL(popup()),this,SLOT(userBoxMenuPopup()));
	disconnect(kadu,SIGNAL(keyPressed(QKeyEvent*)),this,SLOT(mainDialogKeyPressed(QKeyEvent*)));
	disconnect(dcc_manager, SIGNAL(connectionBroken(DccSocket*)),
		this, SLOT(connectionBroken(DccSocket*)));
	disconnect(dcc_manager, SIGNAL(dccError(DccSocket*)),
		this, SLOT(dccError(DccSocket*)));
	disconnect(dcc_manager, SIGNAL(dccEvent(DccSocket*)),
		this, SLOT(dccEvent(DccSocket*)));
	disconnect(dcc_manager, SIGNAL(socketDestroying(DccSocket*)),
		this, SLOT(socketDestroying(DccSocket*)));
	VoiceChatDialog::destroyAll();
	kdebugf2();
}

void VoiceManager::testGsmEncoding()
{
	kdebugf();
	if (GsmEncodingTestMsgBox != NULL)
		return;
		
	GsmEncodingTestHandle = gsm_create();
	if (GsmEncodingTestHandle == 0)
	{
		MessageBox::wrn(tr("Opening DSP Encoder failed."));
		return;
	}
		
	GsmEncodingTestDevice = sound_manager->openDevice(8000);
	if (GsmEncodingTestDevice == NULL)
	{
		MessageBox::wrn(tr("Opening sound device failed."));
		return;
	}
	sound_manager->setFlushingEnabled(GsmEncodingTestDevice, false);

	GsmEncodingTestSample = new int16_t[160];
	GsmEncodingTestFrames = new gsm_frame[8000 * 3 / 160]; // = 150
	GsmEncodingTestCurrFrame = 0;

	sound_manager->enableThreading(GsmEncodingTestDevice);
	connect(sound_manager, SIGNAL(sampleRecorded(SoundDevice)), this, SLOT(gsmEncodingTestSampleRecorded(SoundDevice)));
	connect(sound_manager, SIGNAL(samplePlayed(SoundDevice)), this, SLOT(gsmEncodingTestSamplePlayed(SoundDevice)));

	GsmEncodingTestMsgBox = new MessageBox(tr("Testing GSM Encoding. Please talk now (3 seconds)."));
	GsmEncodingTestMsgBox->show();

	sound_manager->recordSample(GsmEncodingTestDevice, GsmEncodingTestSample, sizeof(int16_t) * 160);
	kdebugf2();
}

void VoiceManager::gsmEncodingTestSampleRecorded(SoundDevice device)
{
	kdebugf();
	if (device == GsmEncodingTestDevice)
	{
		if (GsmEncodingTestCurrFrame < 150)
		{
			kdebugmf(KDEBUG_INFO, "Encoding gsm frame no %i\n", GsmEncodingTestCurrFrame);
			gsm_encode(GsmEncodingTestHandle, GsmEncodingTestSample, GsmEncodingTestFrames[GsmEncodingTestCurrFrame]);
			GsmEncodingTestCurrFrame++;
			sound_manager->recordSample(GsmEncodingTestDevice, GsmEncodingTestSample, sizeof(int16_t) * 160);
		}
		else
		{
			delete GsmEncodingTestMsgBox;
			GsmEncodingTestMsgBox = new MessageBox(tr("You should hear your recorded sample now."));
			GsmEncodingTestMsgBox->show();
			GsmEncodingTestCurrFrame = 0;
			kdebugmf(KDEBUG_INFO, "Decoding gsm frame no %i\n", GsmEncodingTestCurrFrame);
			gsm_decode(GsmEncodingTestHandle, GsmEncodingTestFrames[GsmEncodingTestCurrFrame], GsmEncodingTestSample);
			sound_manager->playSample(device, GsmEncodingTestSample, sizeof(int16_t) * 160);
			GsmEncodingTestCurrFrame++;
		}
	}
	kdebugf2();
}

void VoiceManager::gsmEncodingTestSamplePlayed(SoundDevice device)
{
	kdebugf();
	if (device == GsmEncodingTestDevice)
	{
		if (GsmEncodingTestCurrFrame < 150)
		{
			kdebugmf(KDEBUG_INFO, "Decoding gsm frame no %i\n", GsmEncodingTestCurrFrame);
			gsm_decode(GsmEncodingTestHandle, GsmEncodingTestFrames[GsmEncodingTestCurrFrame], GsmEncodingTestSample);
			sound_manager->playSample(device, GsmEncodingTestSample, sizeof(int16_t) * 160);
			GsmEncodingTestCurrFrame++;
		}
		else
		{
			disconnect(sound_manager, SIGNAL(sampleRecorded(SoundDevice)), this, SLOT(gsmEncodingTestSampleRecorded(SoundDevice)));
			disconnect(sound_manager, SIGNAL(samplePlayed(SoundDevice)), this, SLOT(gsmEncodingTestSamplePlayed(SoundDevice)));
			sound_manager->closeDevice(device);
			delete[] GsmEncodingTestSample;
			GsmEncodingTestSample = NULL;
			GsmEncodingTestMsgBox->deleteLater();
			GsmEncodingTestMsgBox = NULL;
			delete[] GsmEncodingTestFrames;
			gsm_destroy(GsmEncodingTestHandle);
		}
	}
	kdebugf2();
}

void VoiceManager::setup()
{
	kdebugf();
	if (!pt->running())
	{
		device = sound_manager->openDevice(8000);
		if (device == NULL)
		{
			MessageBox::wrn(tr("Opening sound device failed."));
			return;
		}
		sound_manager->setFlushingEnabled(device, false);
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
			delete [] gsmsample.data;
		}
		pt->mutex.unlock();
		sound_manager->closeDevice(device);
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
		sound_manager->playSample(device, output, outlen);
		if (gsm_decode(voice_dec, (gsm_byte *) pos, output))
		{
			kdebugmf(KDEBUG_ERROR, "gsm_decode() error\n");
			return;
		}
		pos += 32;
		sound_manager->playSample(device, output, outlen);
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
		sound_manager->recordSample(device, input, inlen);
		gsm_encode(voice_enc, input, (gsm_byte *) pos);
		pos += 32;
		sound_manager->recordSample(device, input, inlen);
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

	UserBox *activeUserBox=UserBox::getActiveUserBox();
	UserList users;
	if (activeUserBox==NULL)
		return;
	users = activeUserBox->getSelectedUsers();
	if (users.count() != 1)
		return;
	UserListElement user = (*users.begin());
	makeVoiceChat(user.uin());

	kdebugf2();
}

void VoiceManager::makeVoiceChat(UinType dest)
{
	kdebugf();
	if (config_file.readBoolEntry("Network", "AllowDCC"))
		if (dcc_manager->dccEnabled())
		{
			const UserListElement& user = userlist.byUin(dest);

			DccManager::TryType type = dcc_manager->initDCCConnection(user.ip().ip4Addr(),
				user.port(),
				config_file.readNumEntry("General", "UIN"),
				user.uin(),
				SLOT(dccVoiceChat(uint32_t, uint16_t, UinType, UinType, struct gg_dcc **)),
				GG_SESSION_DCC_VOICE);

			if (type==DccManager::DIRECT)
				direct.push_front(user.uin());
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

void VoiceManager::dccError(DccSocket* socket)
{
	kdebugf();
	if (VoiceChatDialog::bySocket(socket) != NULL)
		socket->setState(DCC_SOCKET_VOICECHAT_DISCARDED);

	UinType peer_uin=socket->ggDccStruct()->peer_uin;
	if (direct.contains(peer_uin))
	{
		direct.remove(peer_uin);
		const UserListElement& user = userlist.byUin(peer_uin);
		dcc_manager->initDCCConnection(user.ip().ip4Addr(),
				user.port(),
				config_file.readNumEntry("General", "UIN"),
				user.uin(),
				SLOT(dccVoiceChat(uint32_t, uint16_t, UinType, UinType, struct gg_dcc **)),
				GG_SESSION_DCC_VOICE, true);
	}
	kdebugf2();
}

void VoiceManager::dccEvent(DccSocket* socket)
{
	UinType peer_uin;
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

			//je¿eli druga strona potwierdzi³a, to znaczy,
			//¿e nie bêdziemy potrzebowali po³±czenia zwrotnego
			peer_uin=socket->ggDccStruct()->peer_uin;
			if (direct.contains(peer_uin))
				direct.remove(peer_uin);

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

	UinType peer_uin=socket->ggDccStruct()->peer_uin;
	if (direct.contains(peer_uin))
		direct.remove(peer_uin);

	kdebugf2();
}

VoiceManager* voice_manager = NULL;
