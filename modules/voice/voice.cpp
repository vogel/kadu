/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "voice.h"

#include <qcheckbox.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qmessagebox.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>

#include "userbox.h"
#include "userlist.h"
#include "config_file.h"
#include "debug.h"
#include "config_dialog.h"
#include "kadu.h"
#include "gadu.h"
#include "message_box.h"

/**
 * @ingroup voice
 * @{
 */
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

VoiceChatDialog::VoiceChatDialog(DccSocket *socket)
	: QDialog (NULL, "voice_chat_dialog"), Socket(socket), chatFinished(false)
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

	Dialogs.insert(socket, this);

	if (voice_manager->setup() == -1)
	{
		chatFinished= true;  /* jezeli urzadzenie device jest zajete albo go nie ma
										zrywamy polaczenie oraz zamykamy okienko*/
		socket->setState(DCC_SOCKET_VOICECHAT_DISCARDED);
		delete this;
	}

	kdebugf2();
}

VoiceChatDialog::~VoiceChatDialog()
{
	kdebugf();
	//windziany klient co¶ s³abo reaguje na zamkniêcie
	//gniazdka w przypadku rozmów g³osowych
	//trzeba jeszcze nad tym popracowaæ
//	Socket->setState(DCC_SOCKET_CONNECTION_BROKEN);
	Dialogs.remove(Socket);
	voice_manager->free();
	if (!chatFinished)
		delete Socket;
	kdebugf2();
}

VoiceChatDialog *VoiceChatDialog::bySocket(DccSocket *socket)
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

void VoiceChatDialog::sendDataToAll(char *data, int length)
{
	FOREACH(i, Dialogs)
		gadu->dccVoiceSend(i.key()->ggDccStruct(), data, length);
}

PlayThread::PlayThread() : QObject(0, 0), QThread(), wsem(32), samples(), samplesMutex(), end(false)
{
	wsem += 32; //mo¿e byæ max 32 próbek w kolejce
}

void PlayThread::run()
{
	kdebugf();
	struct gsm_sample gsmsample;
	while (true)
	{
		waitForData();

//		endMutex.lock();
		if (end)
		{
//			endMutex.unlock();
			break;
		}
//		endMutex.unlock();

		samplesMutex.lock();
		if (samples.empty())
		{
			samplesMutex.unlock();
			continue;
		}
		gsmsample = samples.front();
		samples.pop_front();
		samplesMutex.unlock();

		emit playGsmSample(gsmsample.data, gsmsample.length);
		delete [] gsmsample.data;
	}

	samplesMutex.lock();
	while (!samples.empty())
	{
		gsmsample = samples.front();
		samples.pop_front();
		delete [] gsmsample.data;
	}
	samplesMutex.unlock();
	deleteLater();
	kdebugf2();
}

void PlayThread::moreData()
{
	wsem--;
}

void PlayThread::waitForData()
{
	wsem++;
}

void PlayThread::endThread()
{
//	endMutex.lock();
	end = true;
//	endMutex.unlock();
	moreData();
	wait();
}

void PlayThread::addGsmSample(char *data, int length)
{
	kdebugf();
	if (end)
	{
		delete [] data;
		kdebugmf(KDEBUG_FUNCTION_END, "end: thread is going to be deleted!\n");
		return;
	}
	struct gsm_sample gsmsample;
	gsmsample.data = data;
	gsmsample.length = length;
	samplesMutex.lock();
	if (samples.size() >= 3)
	{
		kdebugm(KDEBUG_WARNING, "losing 3 frames\n");
		// jak nie bêdziemy takich rzeczy robiæ, to lag bêdzie siê powiêksza³ :/
		while (!samples.empty())
		{
			delete [] samples[0].data;
			samples.pop_front();
		}
	}
	samples.append(gsmsample);
	samplesMutex.unlock();
	moreData();
	kdebugf2();
}

RecordThread::RecordThread() : QObject(0, 0), QThread(), end(false)
{
}

void RecordThread::run()
{
	kdebugf();
	char data[GG_DCC_VOICE_FRAME_LENGTH_505];
	int length = GG_DCC_VOICE_FRAME_LENGTH_505;
	while (true)
	{
//		endMutex.lock();
		if (end)
		{
//			endMutex.unlock();
			break;
		}
//		endMutex.unlock();
		emit recordSample(data, length);
	}
	deleteLater();
	kdebugf2();
}

void RecordThread::endThread()
{
//	endMutex.lock();
	end = true;
//	endMutex.unlock();
	wait();
}

int VoiceManager::setup()
{
	kdebugf();
	if (!playThread)
	{
		device = sound_manager->openDevice(PLAY_AND_RECORD, 8000);
		if (device == NULL)
		{
			MessageBox::wrn(tr("Opening sound device failed."));
			return -1;
		}
		sound_manager->setFlushingEnabled(device, false);
		playThread = new PlayThread();
		connect(playThread, SIGNAL(playGsmSample(char *, int)), this, SLOT(playGsmSampleReceived(char *, int)));
		playThread->start();
	}
	if (!recordThread)
	{
		recordThread = new RecordThread();
		connect(recordThread, SIGNAL(recordSample(char *, int)), this, SLOT(recordSampleReceived(char *, int)));
		recordThread->start();
	}
	kdebugf2();
	return 0;
}

void VoiceManager::free()
{
	kdebugf();
	if (recordThread && recordThread->running())
	{
		disconnect(recordThread, SIGNAL(recordSample(char *, int)), this, SLOT(recordSampleReceived(char *, int)));
		recordThread->endThread();
		recordThread = NULL;
	}
	if (playThread && playThread->running())
	{
		disconnect(playThread, SIGNAL(playGsmSample(char *, int)), this, SLOT(playGsmSampleReceived(char *, int)));
		playThread->endThread();
		playThread = NULL;
	}
	if (device)
		sound_manager->closeDevice(device);
	kdebugf2();
}

VoiceManager::VoiceManager(QObject *parent, const char *name) : QObject(parent, name),
	GsmEncodingTestMsgBox(0), GsmEncodingTestDevice(0),	GsmEncodingTestHandle(0),
	GsmEncodingTestSample(0), GsmEncodingTestFrames(0), GsmEncodingTestCurrFrame(0),
	device(0), playThread(0), recordThread(0), voice_enc(0), voice_dec(0), direct()
{
	kdebugf();
	ConfigDialog::addHotKeyEdit("ShortCuts", "Define keys",
			QT_TRANSLATE_NOOP("@default", "Voice chat"), "kadu_voicechat", "F7");
	ConfigDialog::addVGroupBox("Sounds", "Sounds", QT_TRANSLATE_NOOP("@default","Voice chat"));
	ConfigDialog::addPushButton("Sounds", "Voice chat", QT_TRANSLATE_NOOP("@default","Test GSM Encoding"));
	ConfigDialog::addCheckBox("Sounds", "Voice chat", QT_TRANSLATE_NOOP("@default", "Faster compression algorithm (degrades quality)"), "FastGSM", false, 0, 0, Expert);
	ConfigDialog::addCheckBox("Sounds", "Voice chat", QT_TRANSLATE_NOOP("@default", "Cut-off optimization (faster but degrades quality)"), "CutGSM", false, 0, 0, Expert);
	ConfigDialog::connectSlot("Sounds", "Test GSM Encoding", SIGNAL(clicked()), this, SLOT(testGsmEncoding()));

	UserBox::userboxmenu->addItemAtPos(2,"VoiceChat", tr("Voice chat"), this,
		SLOT(makeVoiceChat()), HotKey::shortCutFromFile("ShortCuts", "kadu_voicechat"));
	connect(UserBox::userboxmenu, SIGNAL(popup()),
		this, SLOT(userBoxMenuPopup()));
	connect(kadu, SIGNAL(keyPressed(QKeyEvent*)),
		this, SLOT(mainDialogKeyPressed(QKeyEvent*)));
	connect(dcc_manager, SIGNAL(connectionBroken(DccSocket*)),
		this, SLOT(connectionBroken(DccSocket*)));
	connect(dcc_manager, SIGNAL(dccError(DccSocket*)),
		this, SLOT(dccError(DccSocket*)));
	connect(dcc_manager, SIGNAL(dccEvent(DccSocket*)),
		this, SLOT(dccEvent(DccSocket*)));
	connect(dcc_manager, SIGNAL(socketDestroying(DccSocket*)),
		this, SLOT(socketDestroying(DccSocket*)));
	connect(dcc_manager, SIGNAL(setState(DccSocket*)),
		this, SLOT(setState(DccSocket*)));
	kdebugf2();
}

VoiceManager::~VoiceManager()
{
	kdebugf();
	ConfigDialog::disconnectSlot("Sounds", "Test GSM Encoding", SIGNAL(clicked()), this, SLOT(testGsmEncoding()));
	ConfigDialog::removeControl("Sounds", "Cut-off optimization (faster but degrades quality)");
	ConfigDialog::removeControl("Sounds", "Faster compression algorithm (degrades quality)");
	ConfigDialog::removeControl("Sounds", "Test GSM Encoding");
	ConfigDialog::removeControl("Sounds", "Voice chat");
	ConfigDialog::removeControl("ShortCuts", "Voice chat");
	int voice_chat_item = UserBox::userboxmenu->getItem(tr("Voice chat"));
	UserBox::userboxmenu->removeItem(voice_chat_item);
	disconnect(UserBox::userboxmenu,SIGNAL(popup()),
		this, SLOT(userBoxMenuPopup()));
	disconnect(kadu, SIGNAL(keyPressed(QKeyEvent*)),
		this, SLOT(mainDialogKeyPressed(QKeyEvent*)));
	disconnect(dcc_manager, SIGNAL(setState(DccSocket*)),
		this, SLOT(setState(DccSocket*)));
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
	int value = 1;
	gsm_option(GsmEncodingTestHandle, GSM_OPT_VERBOSE, &value);
	if (ConfigDialog::getCheckBox("Sounds", "Faster compression algorithm (degrades quality)")->isChecked())
		gsm_option(GsmEncodingTestHandle, GSM_OPT_FAST, &value);
	if (ConfigDialog::getCheckBox("Sounds", "Cut-off optimization (faster but degrades quality)")->isChecked())
		gsm_option(GsmEncodingTestHandle, GSM_OPT_LTP_CUT, &value);

	GsmEncodingTestDevice = sound_manager->openDevice(PLAY_AND_RECORD, 8000);
	if (GsmEncodingTestDevice == NULL)
	{
		MessageBox::wrn(tr("Opening sound device failed."));
		return;
	}
	sound_manager->enableThreading(GsmEncodingTestDevice);
	sound_manager->setFlushingEnabled(GsmEncodingTestDevice, false);

	GsmEncodingTestSample = new int16_t[160 * 10];
	GsmEncodingTestFrames = new gsm_frame[8000 * 3 / 160]; // = 150
	GsmEncodingTestCurrFrame = 0;

	connect(sound_manager, SIGNAL(sampleRecorded(SoundDevice)), this, SLOT(gsmEncodingTestSampleRecorded(SoundDevice)));
	connect(sound_manager, SIGNAL(samplePlayed(SoundDevice)), this, SLOT(gsmEncodingTestSamplePlayed(SoundDevice)));

	GsmEncodingTestMsgBox = new MessageBox(tr("Testing GSM Encoding. Please talk now (3 seconds)."));
	GsmEncodingTestMsgBox->show();

	sound_manager->recordSample(GsmEncodingTestDevice, GsmEncodingTestSample, sizeof(int16_t) * 160 * 10);
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
			for(int i = 0; i < 10; ++i)
				gsm_encode(GsmEncodingTestHandle, GsmEncodingTestSample + i * 160, GsmEncodingTestFrames[GsmEncodingTestCurrFrame++]);
			sound_manager->recordSample(GsmEncodingTestDevice, GsmEncodingTestSample, sizeof(int16_t) * 160 * 10);
		}
		else
		{
			delete GsmEncodingTestMsgBox;
			GsmEncodingTestMsgBox = new MessageBox(tr("You should hear your recorded sample now."));
			GsmEncodingTestMsgBox->show();
			GsmEncodingTestCurrFrame = 0;
			kdebugmf(KDEBUG_INFO, "Decoding gsm frame no %i\n", GsmEncodingTestCurrFrame);
			for (int i = 0; i < 10; ++i)
				if (gsm_decode(GsmEncodingTestHandle, GsmEncodingTestFrames[GsmEncodingTestCurrFrame++], GsmEncodingTestSample + i * 160))
					kdebugm(KDEBUG_ERROR, "we've got problem, decoding failed %d\n", i);
			sound_manager->playSample(device, GsmEncodingTestSample, sizeof(int16_t) * 160 * 10);
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
			for (int i = 0; i < 10; ++i)
				if (gsm_decode(GsmEncodingTestHandle, GsmEncodingTestFrames[GsmEncodingTestCurrFrame++], GsmEncodingTestSample + i * 160))
					kdebugm(KDEBUG_ERROR, "we've got problem, decoding failed %d\n", i);
			sound_manager->playSample(device, GsmEncodingTestSample, sizeof(int16_t) * 160 * 10);
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
	gsm_option(voice_enc, GSM_OPT_WAV49, &value);
	gsm_option(voice_enc, GSM_OPT_VERBOSE, &value);
	if (config_file.readBoolEntry("Sounds", "FastGSM"))
		gsm_option(voice_enc, GSM_OPT_FAST, &value);
	if (config_file.readBoolEntry("Sounds", "CutGSM"))
		gsm_option(voice_enc, GSM_OPT_LTP_CUT, &value);

	kdebugf2();
}

void VoiceManager::resetDecoder()
{
	kdebugf();
	int value = 1;
	if (voice_dec)
		gsm_destroy(voice_dec);
	voice_dec = gsm_create();
	gsm_option(voice_dec, GSM_OPT_WAV49, &value);
	gsm_option(voice_dec, GSM_OPT_VERBOSE, &value);
	if (config_file.readBoolEntry("Sounds", "FastGSM"))
		gsm_option(voice_dec, GSM_OPT_FAST, &value);
	if (config_file.readBoolEntry("Sounds", "CutGSM"))
		gsm_option(voice_dec, GSM_OPT_LTP_CUT, &value);
	kdebugf2();
}

void VoiceManager::playGsmSampleReceived(char *data, int length)
{
	kdebugf();
	char *pos = data;
	int outlen = 320;
	gsm_signal output[160 * 10], *output2;
	output2 = output;

	resetDecoder();
	++data;
	++pos;
	--length;
	while (pos <= (data + length - 65))
	{
		if (gsm_decode(voice_dec, (gsm_byte *) pos, output2))
		{
			kdebugmf(KDEBUG_ERROR, "gsm_decode() error\n");
			return;
		}
		pos += 33;
		output2 += 160;
		if (gsm_decode(voice_dec, (gsm_byte *) pos, output2))
		{
			kdebugmf(KDEBUG_ERROR, "gsm_decode() error\n");
			return;
		}
		pos += 32;
		output2 += 160;
	}
	sound_manager->playSample(device, output, outlen * 10);
	kdebugf2();
}

void VoiceManager::recordSampleReceived(char *data, int length)
{
	kdebugf();
	char *pos = data;
//	int inlen = 320;
	gsm_signal input[160 * 10], *input2;
	input2 = input;

	resetCoder();
	*data = 0;
	++data;
	++pos;
	--length;
	sound_manager->recordSample(device, input, 160 * 10 * 2);

	int silence = 0;
	for (int i = 0; i < 160 * 10; ++i)
		if (abs(input[i]) < 256) // 256 ustalone do¶wiadczalnie
			++silence;
	kdebugm(KDEBUG_INFO, "silence: %d / %d\n", silence, 160 * 10);
/*	if (silence == 0)
	{
		for (int i = 0; i < 160 * 10; ++i)
			fprintf(stderr, "%hd ", input[i]);
		fprintf(stderr, "\n");
	}*/
	while (pos <= (data + length - 65))
	{
		gsm_encode(voice_enc, input2, (gsm_byte *) pos);
		pos += 32;
		input2 += 160;
		gsm_encode(voice_enc, input2, (gsm_byte *) pos);
		pos += 33;
		input2 += 160;
	}
	/* celowo sprawdzane jest to PO kodowaniu, bo koder musi
	   znaæ ca³o¶æ sygna³u ¿eby poprawnie pracowaæ */
	if (silence == 160 * 10)
	{
		kdebugm(KDEBUG_INFO, "silence! not sending packet\n");
		return;
	}
	VoiceChatDialog::sendDataToAll(data - 1, length + 1);
	kdebugf2();
}

void VoiceManager::addGsmSample(char *data, int length)
{
	kdebugf();
	playThread->addGsmSample(data, length);
	kdebugf2();
}

void VoiceManager::makeVoiceChat()
{
	kdebugf();

	UserBox *activeUserBox = UserBox::activeUserBox();
	if (activeUserBox == NULL)
		return;

	UserListElements users = activeUserBox->selectedUsers();
	if (users.count() != 1)
		return;

	makeVoiceChat(users[0].ID("Gadu").toUInt());

	kdebugf2();
}

void VoiceManager::makeVoiceChat(UinType dest)
{
	kdebugf();
	if (config_file.readBoolEntry("Network", "AllowDCC"))
		if (dcc_manager->dccEnabled())
		{
			const UserListElement& user = userlist->byID("Gadu", QString::number(dest));

			DccManager::TryType type = dcc_manager->initDCCConnection(user.IP("Gadu").ip4Addr(),
				user.port("Gadu"),
				config_file.readNumEntry("General", "UIN"),
				user.ID("Gadu").toUInt(),
				SLOT(dccVoiceChat(uint32_t, uint16_t, UinType, UinType, struct gg_dcc **)),
				GG_SESSION_DCC_VOICE);

			if (type==DccManager::DIRECT)
				direct.push_front(user.ID("Gadu").toUInt());
		}
	kdebugf2();
}

void VoiceManager::askAcceptVoiceChat(DccSocket *socket)
{
	kdebugf();
	QString text = tr("User %1 wants to talk with you. Do you accept it?");
	if (userlist->contains("Gadu", QString::number(socket->ggDccStruct()->peer_uin)) &&
		!userlist->byID("Gadu", QString::number(socket->ggDccStruct()->peer_uin)).isAnonymous())
		text = text.arg(userlist->byID("Gadu", QString::number(socket->ggDccStruct()->peer_uin)).altNick());
	else
		text = text.arg(socket->ggDccStruct()->peer_uin);

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

void VoiceManager::mainDialogKeyPressed(QKeyEvent *e)
{
	if (HotKey::shortCut(e,"ShortCuts", "kadu_voicechat"))
		makeVoiceChat();
}

void VoiceManager::userBoxMenuPopup()
{
	kdebugf();
	UserBox *activeUserBox = UserBox::activeUserBox();
	if (activeUserBox == NULL) //to siê zdarza...
		return;
	UserListElements users = activeUserBox->selectedUsers();
	UserListElement user = users[0];

	bool containsOurUin = users.contains("Gadu", QString::number(config_file.readNumEntry("General", "UIN")));
	int voicechat = UserBox::userboxmenu->getItem(tr("Voice chat"));

	bool enable = (DccSocket::count() < 8 &&
		users.count() == 1 &&
		user.usesProtocol("Gadu") &&
		!containsOurUin &&
		config_file.readBoolEntry("Network", "AllowDCC") &&
		(user.status("Gadu").isOnline() || user.status("Gadu").isBusy()));

	UserBox::userboxmenu->setItemEnabled(voicechat, enable);
	kdebugf2();
}

void VoiceManager::connectionBroken(DccSocket *socket)
{
	kdebugf();
	VoiceChatDialog *dialog = VoiceChatDialog::bySocket(socket);
	if (dialog != NULL)
		socket->setState(DCC_SOCKET_VOICECHAT_DISCARDED);
	else
		kdebugm(KDEBUG_INFO, "not my socket\n");
	kdebugf2();
}

void VoiceManager::dccError(DccSocket *socket)
{
	kdebugf();
	VoiceChatDialog *dialog = VoiceChatDialog::bySocket(socket);
	if (dialog != NULL)
	{
		UinType peer_uin = socket->ggDccStruct()->peer_uin;

		socket->setState(DCC_SOCKET_VOICECHAT_DISCARDED);

		if (direct.contains(peer_uin))
		{
			direct.remove(peer_uin);
			UserListElement user = userlist->byID("Gadu", QString::number(peer_uin));
			dcc_manager->initDCCConnection(user.IP("Gadu").ip4Addr(),
					user.port("Gadu"),
					config_file.readNumEntry("General", "UIN"),
					user.ID("Gadu").toUInt(),
					SLOT(dccVoiceChat(uint32_t, uint16_t, UinType, UinType, struct gg_dcc **)),
					GG_SESSION_DCC_VOICE, true);
		}
	}
	else
		kdebugm(KDEBUG_INFO, "not my socket\n");
	kdebugf2();
}

void VoiceManager::dccEvent(DccSocket *socket)
{
	int len;
	UinType peer_uin;
	char *voice_buf;
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
			peer_uin = socket->ggDccStruct()->peer_uin;
			if (direct.contains(peer_uin))
				direct.remove(peer_uin);

			break;
		case GG_EVENT_DCC_VOICE_DATA:
			kdebugmf(KDEBUG_INFO, "GG_EVENT_DCC_VOICE_DATA\n");
			len = socket->ggDccEvent()->event.dcc_voice_data.length;

			//zostawiamy zapas gdyby to siê kiedy¶ zmieni³o
			if (len > 1630) // 1630 == 5 * 326
			{
				socket->setState(DCC_SOCKET_VOICECHAT_DISCARDED);
				break;
			}

			voice_buf = new char[len];
			memcpy(voice_buf, socket->ggDccEvent()->event.dcc_voice_data.data, len);
			voice_manager->addGsmSample(voice_buf, len);
			break;
	}
}

void VoiceManager::socketDestroying(DccSocket *socket)
{
	kdebugf();

	VoiceChatDialog *dialog = VoiceChatDialog::bySocket(socket);
	if (dialog)
	{
		UinType peer_uin = socket->ggDccStruct()->peer_uin;
		if (direct.contains(peer_uin))
			direct.remove(peer_uin);
		delete dialog;
	}
	else
		kdebugm(KDEBUG_INFO, "not my socket\n");

	kdebugf2();
}

void VoiceManager::setState(DccSocket *socket)
{
	kdebugf();

	VoiceChatDialog *dialog = VoiceChatDialog::bySocket(socket);
	if (dialog != NULL)
		dialog->chatFinished = true;
	else
		kdebugm(KDEBUG_INFO, "not my socket\n");

	kdebugf2();
}

QMap<DccSocket *, VoiceChatDialog *> VoiceChatDialog::Dialogs;
VoiceManager *voice_manager = NULL;

/** @} */

