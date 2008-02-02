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

#include "../dcc/dcc.h"
#include "../dcc/dcc_socket.h"

#include "chat_widget.h"
#include "chat_manager.h"
#include "config_file.h"
#include "debug.h"
#include "gadu.h"
#include "hot_key.h"
#include "kadu.h"
#include "message_box.h"
#include "userbox.h"
#include "userlist.h"

/**
 * @ingroup voice
 * @{
 */
extern "C" int voice_init()
{
	voice_manager = new VoiceManager();
	MainConfigurationWindow::registerUiFile(dataPath("kadu/modules/configuration/voice.ui"), voice_manager);

	return 0;
}

extern "C" void voice_close()
{
	MainConfigurationWindow::unregisterUiFile(dataPath("kadu/modules/configuration/voice.ui"), voice_manager);

	delete voice_manager;
	voice_manager = 0;
}

VoiceChatDialog::VoiceChatDialog()
	: QDialog (NULL, "voice_chat_dialog"), Socket(0), chatFinished(false)
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

	VoiceChats.append(this);

	kdebugf2();
}

VoiceChatDialog::~VoiceChatDialog()
{
	kdebugf();

	if (!Socket)
		return;

	delete Socket;
	Socket = 0;

	VoiceChats.remove(this);
	voice_manager->free();

	kdebugf2();
}

bool VoiceChatDialog::addSocket(DccSocket *socket)
{
	kdebugf();

	Socket = socket;
	if (Socket)
	{
		if (voice_manager->setup() == -1)
		{
			chatFinished = true;  /* jezeli urzadzenie device jest zajete albo go nie ma
											zrywamy polaczenie oraz zamykamy okienko*/
			Socket->reject();
			delete Socket;
			return false;
		}

		return true;
	}

	return false;
}

void VoiceChatDialog::removeSocket(DccSocket *socket)
{
	kdebugf();

	if (Socket == socket)
	{
		Socket = 0;
		delete this;
	}

	kdebugf2();
}

bool VoiceChatDialog::socketEvent(DccSocket *socket, bool &lock)
{
	kdebugf();

	switch (socket->ggDccEvent()->type)
	{
		case GG_EVENT_DCC_VOICE_DATA:
		{
			kdebugmf(KDEBUG_INFO, "GG_EVENT_DCC_VOICE_DATA\n");
			int len = socket->ggDccEvent()->event.dcc_voice_data.length;

			//zostawiamy zapas gdyby to siê kiedy¶ zmieni³o
			if (len > 1630) // 1630 == 5 * 326
			{
				socket->reject();
				delete socket;
				return true;
			}

			char *voice_buf = new char[len];
			memcpy(voice_buf, socket->ggDccEvent()->event.dcc_voice_data.data, len);
			voice_manager->addGsmSample(voice_buf, len);
			return true;
		}
		default:
			return false;
	}
}

void VoiceChatDialog::destroyAll()
{
	kdebugf();

	while (!VoiceChats.empty())
		delete *(VoiceChats.begin());

	kdebugf2();
}

void VoiceChatDialog::sendData(char *data, int length)
{
	kdebugf();

	if (Socket)
		Socket->sendVoiceData(data, length);
}

void VoiceChatDialog::sendDataToAll(char *data, int length)
{
	kdebugf();

	FOREACH(i, VoiceChats)
		(*i)->sendData(data, length);
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

void VoiceManager::mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow)
{
	QWidget *allowDCC = mainConfigurationWindow->widgetById("dcc/AllowDCC");
	QWidget *voice = mainConfigurationWindow->widgetById("voice/voice");

	connect(allowDCC, SIGNAL(toggled(bool)), voice, SLOT(setEnabled(bool)));

	connect(mainConfigurationWindow->widgetById("voice/test"), SIGNAL(clicked()), this, SLOT(testGsmEncoding()));

	testFast = dynamic_cast<QCheckBox *>(mainConfigurationWindow->widgetById("voice/testFast"));
	testCut = dynamic_cast<QCheckBox *>(mainConfigurationWindow->widgetById("voice/testCut"));
}

int VoiceManager::setup()
{
	kdebugf();
	if (!playThread)
	{
		device = sound_manager->openDevice(PLAY_AND_RECORD, 8000);
		if (device == NULL)
		{
			MessageBox::msg(tr("Opening sound device failed."), false, "Warning");
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

VoiceManager::VoiceManager()
	: GsmEncodingTestMsgBox(0), GsmEncodingTestDevice(0),	GsmEncodingTestHandle(0),
		GsmEncodingTestSample(0), GsmEncodingTestFrames(0), GsmEncodingTestCurrFrame(0),
		device(0), playThread(0), recordThread(0), voice_enc(0), voice_dec(0)
{
	kdebugf();

	createDefaultConfiguration();

	UserBox::userboxmenu->addItemAtPos(2,"VoiceChat", tr("Voice chat"), this,
		SLOT(makeVoiceChat()), HotKey::shortCutFromFile("ShortCuts", "kadu_voicechat"));
	connect(UserBox::userboxmenu, SIGNAL(popup()),
		this, SLOT(userBoxMenuPopup()));
	connect(kadu, SIGNAL(keyPressed(QKeyEvent*)),
		this, SLOT(mainDialogKeyPressed(QKeyEvent*)));

	connect(chat_manager, SIGNAL(chatWidgetCreated(ChatWidget *)), this, SLOT(chatCreated(ChatWidget *)));
	connect(chat_manager, SIGNAL(chatWidgetDestroying(ChatWidget *)), this, SLOT(chatDestroying(ChatWidget*)));

	FOREACH(it, chat_manager->chats())
		chatCreated(*it);

	dcc_manager->addHandler(this);

	kdebugf2();
}

VoiceManager::~VoiceManager()
{
	kdebugf();

	dcc_manager->removeHandler(this);

	int voice_chat_item = UserBox::userboxmenu->getItem(tr("Voice chat"));
	UserBox::userboxmenu->removeItem(voice_chat_item);

	disconnect(chat_manager, SIGNAL(chatWidgetCreated(ChatWidget *)), this, SLOT(chatCreated(ChatWidget *)));
	disconnect(chat_manager, SIGNAL(chatWidgetDestroying(ChatWidget *)), this, SLOT(chatDestroying(ChatWidget*)));

	FOREACH(it, chat_manager->chats())
		chatDestroying(*it);

	disconnect(UserBox::userboxmenu,SIGNAL(popup()),
		this, SLOT(userBoxMenuPopup()));
	disconnect(kadu, SIGNAL(keyPressed(QKeyEvent*)),
		this, SLOT(mainDialogKeyPressed(QKeyEvent*)));
	VoiceChatDialog::destroyAll();

	kdebugf2();
}

void VoiceManager::chatCreated(ChatWidget *chat)
{
	connect(chat, SIGNAL(keyPressed(QKeyEvent *, ChatWidget *, bool &)),
		this, SLOT(chatKeyPressed(QKeyEvent *, ChatWidget *, bool &)));
}

void VoiceManager::chatDestroying(ChatWidget *chat)
{
	disconnect(chat, SIGNAL(keyPressed(QKeyEvent *, ChatWidget *, bool &)),
		this, SLOT(chatKeyPressed(QKeyEvent *, ChatWidget *, bool &)));
}

void VoiceManager::testGsmEncoding()
{
	kdebugf();
	if (GsmEncodingTestMsgBox != NULL)
		return;

	GsmEncodingTestHandle = gsm_create();
	if (GsmEncodingTestHandle == 0)
	{
		MessageBox::msg(tr("Opening DSP Encoder failed."), false, "Warning");
		return;
	}
	int value = 1;
	gsm_option(GsmEncodingTestHandle, GSM_OPT_VERBOSE, &value);

	if (testFast->isChecked())
		gsm_option(GsmEncodingTestHandle, GSM_OPT_FAST, &value);
	if (testCut->isChecked())
		gsm_option(GsmEncodingTestHandle, GSM_OPT_LTP_CUT, &value);

	GsmEncodingTestDevice = sound_manager->openDevice(PLAY_AND_RECORD, 8000);
	if (GsmEncodingTestDevice == NULL)
	{
		MessageBox::msg(tr("Opening sound device failed."), false, "Warning");
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
			const UserListElement &user = userlist->byID("Gadu", QString::number(dest));
			dcc_manager->getVoiceSocket(user.IP("Gadu").ip4Addr(), user.port("Gadu"), config_file.readNumEntry("General", "UIN"), user.ID("Gadu").toUInt(), this);
		}
	kdebugf2();
}

bool VoiceManager::askAcceptVoiceChat(DccSocket *socket)
{
	kdebugf();
	QString text = tr("User %1 wants to talk with you. Do you accept it?");
	if (userlist->contains("Gadu", QString::number(socket->peerUin())) &&
		!userlist->byID("Gadu", QString::number(socket->peerUin())).isAnonymous())
		text = text.arg(userlist->byID("Gadu", QString::number(socket->peerUin())).altNick());
	else
		text = text.arg(socket->peerUin());

	return MessageBox::ask(text, QString::null, kadu);
}

void VoiceManager::mainDialogKeyPressed(QKeyEvent *e)
{
	if (HotKey::shortCut(e,"ShortCuts", "kadu_voicechat"))
		makeVoiceChat();
}

void VoiceManager::chatKeyPressed(QKeyEvent *e, ChatWidget *chatWidget, bool &handled)
{
	if (HotKey::shortCut(e,"ShortCuts", "kadu_voicechat"))
	{
		UserListElements users = chatWidget->users()->toUserListElements();
		if (users.size() == 1)
			makeVoiceChat(users[0].ID("Gadu").toUInt());
		handled = true;
	}
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

	bool enable =
		(users.count() == 1) &&
		user.usesProtocol("Gadu") &&
		!containsOurUin &&
		config_file.readBoolEntry("Network", "AllowDCC") &&
		(user.status("Gadu").isOnline() || user.status("Gadu").isBusy());

	UserBox::userboxmenu->setItemVisible(voicechat, enable);
	kdebugf2();
}

bool VoiceManager::socketEvent(DccSocket *socket, bool &lock)
{
	kdebugf();

	switch (socket->ggDccEvent()->type)
	{
		case GG_EVENT_DCC_NEED_VOICE_ACK:
			kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "GG_EVENT_DCC_NEED_VOICE_ACK! uin:%d peer_uin:%d\n",
				socket->uin(), socket->peerUin());
			if (askAcceptVoiceChat(socket))
			{
				VoiceChatDialog *voiceChatDialog = new VoiceChatDialog();
				socket->setHandler(voiceChatDialog);
			}
			else
			{
				socket->reject();
				delete socket;
			}
			return true;

		case GG_EVENT_DCC_ACK:
			kdebugmf(KDEBUG_INFO, "GG_EVENT_DCC_ACK\n");
			if (socket->type() == GG_SESSION_DCC_VOICE)
			{
				VoiceChatDialog *voiceChatDialog = new VoiceChatDialog();
				socket->setHandler(voiceChatDialog);
			}
			return true;

		default:
			return false;
	}
}

void VoiceManager::createDefaultConfiguration()
{
	config_file.addVariable("ShortCuts", "kadu_voicechat", "");
}

QValueList<VoiceChatDialog *> VoiceChatDialog::VoiceChats;
VoiceManager *voice_manager = NULL;

/** @} */

