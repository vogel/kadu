/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qfiledialog.h>
#include <qregexp.h>
#include <qprocess.h>
#include "kadu_speech.h"
#include "debug.h"
#include "config_dialog.h"
#include "events.h"
#include "chat.h"
#include <stdlib.h>
#include <time.h>

extern "C" int speech_init()
{
	kdebugf();

	speechObj=new SpeechSlots();

	QObject::connect(&event_manager, SIGNAL(chatMsgReceived1(UinsList, const QString&, time_t,bool&)),
		speechObj, SLOT(chat(UinsList, const QString&, time_t,bool&)));
	QObject::connect(&event_manager, SIGNAL(chatMsgReceived2(UinsList, const QString&, time_t)),
		speechObj, SLOT(message(UinsList, const QString&,time_t)));
	QObject::connect(&userlist, SIGNAL(statusModified(UserListElement*)),
		speechObj, SLOT(notify(UserListElement*)));

	ConfigDialog::addTab("Notify");
	ConfigDialog::addVGroupBox("Notify", "Notify", "Notify options");
	ConfigDialog::addCheckBox("Notify", "Notify options",
			QT_TRANSLATE_NOOP("@default","Notify by speech"), "NotifyWithSpeech", true);

	ConfigDialog::addTab(QT_TRANSLATE_NOOP("@default","Speech"));

	ConfigDialog::addCheckBox("Speech", "Speech",
			QT_TRANSLATE_NOOP("@default","Say only when chat window is not active"),
			"SayWhenWinNotActive", true);

	ConfigDialog::addGrid("Speech", "Speech", "freq", 2);
	ConfigDialog::addLabel("Speech", "freq", QT_TRANSLATE_NOOP("@default","Frequency"));
	ConfigDialog::addSlider("Speech", "freq", "slider1", "Frequency", 5000, 22050, 500, 8000);

	ConfigDialog::addGrid("Speech", "Speech", "tempo", 2);
	ConfigDialog::addLabel("Speech", "tempo", QT_TRANSLATE_NOOP("@default","Tempo"));
	ConfigDialog::addSlider("Speech", "tempo", "slider2", "Tempo", 0, 10, 1, 5);

	ConfigDialog::addGrid("Speech", "Speech", "basefreq", 2);
	ConfigDialog::addLabel("Speech", "basefreq", QT_TRANSLATE_NOOP("@default","Base frequency"));
	ConfigDialog::addSlider("Speech", "basefreq", "slider3", "BaseFrequency", 60, 440, 10, 133);

	ConfigDialog::addCheckBox("Speech", "Speech", QT_TRANSLATE_NOOP("@default","Melody"), "Melody", true);
	ConfigDialog::addCheckBox("Speech", "Speech", 
			QT_TRANSLATE_NOOP("@default","Klatt synthesizer (requires dsp)"), "KlattSynt", false);
	ConfigDialog::addCheckBox("Speech", "Speech",
			QT_TRANSLATE_NOOP("@default","Use aRts"), "UseArts", false, "", "usearts");
	ConfigDialog::addCheckBox("Speech", "Speech", 
			QT_TRANSLATE_NOOP("@default","Use Esd"), "UseEsd", false, "", "useesd");
	ConfigDialog::addCheckBox("Speech", "Speech",
			QT_TRANSLATE_NOOP("@default","Use Dsp"), "UseDsp", true, "", "usedsp");
	ConfigDialog::addLineEdit("Speech", "Speech",
			QT_TRANSLATE_NOOP("@default","Dsp device:"), "DspDev","/dev/dsp");

	ConfigDialog::addHGroupBox("Speech", "Speech", QT_TRANSLATE_NOOP("@default","Program"));
	ConfigDialog::addLineEdit("Speech", "Program",
			QT_TRANSLATE_NOOP("@default","Speech program:"), "SpeechProgram","powiedz");
	ConfigDialog::addPushButton("Speech", "Program", "", "OpenFile","","speech_fileopen");
	ConfigDialog::connectSlot("Speech", "", SIGNAL(clicked()), speechObj, SLOT(chooseSpeechProgram()), "speech_fileopen");
	
	ConfigDialog::addLineEdit("Speech", "Speech",
			QT_TRANSLATE_NOOP("@default","Chat format (male):"),
			"ChatFormatMale", SpeechSlots::tr("man %a said %1"));
	ConfigDialog::addLineEdit("Speech", "Speech", 
			QT_TRANSLATE_NOOP("@default","Chat format (female):"),
			"ChatFormatFemale", SpeechSlots::tr("woman %a said %1"));

	ConfigDialog::addLineEdit("Speech", "Speech", 
			QT_TRANSLATE_NOOP("@default","Message format (male):"),
			"MessageFormatMale", SpeechSlots::tr("man %a said %1"));
	ConfigDialog::addLineEdit("Speech", "Speech", 
			QT_TRANSLATE_NOOP("@default","Message format (female):"),
			"MessageFormatFemale", SpeechSlots::tr("woman %a said %1"));

	ConfigDialog::addLineEdit("Speech", "Speech", 
			QT_TRANSLATE_NOOP("@default","Notify format (male):"),
			"NotifyFormatMale", SpeechSlots::tr("man %a changed status to %s %d"));
	ConfigDialog::addLineEdit("Speech", "Speech", 
			QT_TRANSLATE_NOOP("@default","Notify format (female):"),
			"NotifyFormatFemale", SpeechSlots::tr("woman %a changed status to %s %d"));
	
	ConfigDialog::addPushButton("Speech", "Speech",	QT_TRANSLATE_NOOP("@default","Test"), "", "", "testspeech");

	ConfigDialog::connectSlot("Speech", "Test", SIGNAL(clicked()), speechObj, SLOT(testSpeech()), "testspeech");
	ConfigDialog::connectSlot("Speech", "Use aRts", SIGNAL(toggled(bool)), speechObj, SLOT(useArts(bool)), "usearts");
	ConfigDialog::connectSlot("Speech", "Use Esd", SIGNAL(toggled(bool)), speechObj, SLOT(useEsd(bool)), "useesd");
	ConfigDialog::connectSlot("Speech", "Use Dsp", SIGNAL(toggled(bool)), speechObj, SLOT(useDsp(bool)), "usedsp");

	ConfigDialog::registerSlotOnCreate(speechObj, SLOT(onCreateConfigDialog()));

	kdebugf2();
	return 0;
}

extern "C" void speech_close()
{
	kdebugf();

	ConfigDialog::unregisterSlotOnCreate(speechObj, SLOT(onCreateConfigDialog()));

	ConfigDialog::disconnectSlot("Speech", "", SIGNAL(clicked()), speechObj, SLOT(chooseSpeechProgram()), "speech_fileopen");
	ConfigDialog::disconnectSlot("Speech", "Test", SIGNAL(clicked()), speechObj, SLOT(testSpeech()), "testspeech");
	ConfigDialog::disconnectSlot("Speech", "Use aRts", SIGNAL(toggled(bool)), speechObj, SLOT(useArts(bool)), "usearts");
	ConfigDialog::disconnectSlot("Speech", "Use Esd", SIGNAL(toggled(bool)), speechObj, SLOT(useEsd(bool)), "useesd");
	ConfigDialog::disconnectSlot("Speech", "Use Dsp", SIGNAL(toggled(bool)), speechObj, SLOT(useDsp(bool)), "usedsp");

	QObject::disconnect(&event_manager, SIGNAL(chatMsgReceived1(UinsList, const QString&, time_t,bool&)),
		speechObj, SLOT(chat(UinsList, const QString&, time_t,bool&)));
	QObject::disconnect(&event_manager, SIGNAL(chatMsgReceived2(UinsList, const QString&, time_t)),
		speechObj, SLOT(message(UinsList, const QString&,time_t)));

	QObject::disconnect(&userlist, SIGNAL(statusModified(UserListElement*)),
		speechObj, SLOT(notify(UserListElement*)));

	ConfigDialog::removeControl("Speech", "Test", "testspeech");
	ConfigDialog::removeControl("Speech", "Notify format (female):");
	ConfigDialog::removeControl("Speech", "Notify format (male):");
	ConfigDialog::removeControl("Speech", "Message format (female):");
	ConfigDialog::removeControl("Speech", "Message format (male):");
	ConfigDialog::removeControl("Speech", "Chat format (female):");
	ConfigDialog::removeControl("Speech", "Chat format (male):");
	ConfigDialog::removeControl("Speech", "", "speech_fileopen");
	ConfigDialog::removeControl("Speech", "Speech program:");
	ConfigDialog::removeControl("Speech", "Program");
	ConfigDialog::removeControl("Speech", "Dsp device:");
	ConfigDialog::removeControl("Speech", "Use Dsp", "usedsp");
	ConfigDialog::removeControl("Speech", "Use Esd", "useesd");
	ConfigDialog::removeControl("Speech", "Use aRts", "usearts");
	ConfigDialog::removeControl("Speech", "Klatt synthesizer (requires dsp)");
	ConfigDialog::removeControl("Speech", "Melody");
	ConfigDialog::removeControl("Speech", "slider3");
	ConfigDialog::removeControl("Speech", "Base frequency");
	ConfigDialog::removeControl("Speech", "basefreq");
	ConfigDialog::removeControl("Speech", "slider2");
	ConfigDialog::removeControl("Speech", "Tempo");
	ConfigDialog::removeControl("Speech", "tempo");
	ConfigDialog::removeControl("Speech", "slider1");
	ConfigDialog::removeControl("Speech", "Frequency");
	ConfigDialog::removeControl("Speech", "freq");
	ConfigDialog::removeControl("Speech", "Say only when chat window is not active");
	ConfigDialog::removeControl("Notify", "Notify by speech");
	ConfigDialog::removeTab("Speech");

	delete speechObj;
	speechObj=NULL;
	kdebugf2();
}

bool isFemale(QString s)
{
#if QT_VERSION >= 0x030200
	return s.endsWith("a", false);
#else
	return s.endsWith("a")||s.endsWith("A");
#endif
}

void SpeechSlots::useArts(bool b)
{
	kdebugf();
	if (b)
	{
		ConfigDialog::getCheckBox("Speech", "Use aRts", "usearts")->setEnabled(false);

		ConfigDialog::getCheckBox("Speech", "Use Esd", "useesd")->setChecked(false);
		ConfigDialog::getCheckBox("Speech", "Use Dsp", "usedsp")->setChecked(false);

		ConfigDialog::getCheckBox("Speech", "Use Esd", "useesd")->setEnabled(true);
		ConfigDialog::getCheckBox("Speech", "Use Dsp", "usedsp")->setEnabled(true);
	}
	kdebugf2();
}

void SpeechSlots::useEsd(bool b)
{
	kdebugf();
	if (b)
	{
		ConfigDialog::getCheckBox("Speech", "Use Esd", "useesd")->setEnabled(false);

		ConfigDialog::getCheckBox("Speech", "Use aRts", "usearts")->setChecked(false);
		ConfigDialog::getCheckBox("Speech", "Use Dsp", "usedsp")->setChecked(false);

		ConfigDialog::getCheckBox("Speech", "Use aRts", "usearts")->setEnabled(true);
		ConfigDialog::getCheckBox("Speech", "Use Dsp", "usedsp")->setEnabled(true);
	}
	kdebugf2();
}

void SpeechSlots::useDsp(bool b)
{
	kdebugf();
	if (b)
	{
		ConfigDialog::getCheckBox("Speech", "Use Dsp", "usedsp")->setEnabled(false);

		ConfigDialog::getCheckBox("Speech", "Use Esd", "useesd")->setChecked(false);
		ConfigDialog::getCheckBox("Speech", "Use aRts", "usearts")->setChecked(false);

		ConfigDialog::getCheckBox("Speech", "Use Esd", "useesd")->setEnabled(true);
		ConfigDialog::getCheckBox("Speech", "Use aRts", "usearts")->setEnabled(true);
	}
	kdebugf2();
}


SpeechSlots::SpeechSlots()
{
	kdebugf();
	srand(time(NULL));
	lastSpeech.start();
	kdebugf2();
}

void SpeechSlots::say(const QString &s, const QString &path,
						bool klatt, bool melody,
						bool arts, bool esd, bool dsp, const QString &device,
						int freq, int tempo, int basefreq)
{
	kdebugf();
//	kdebugm(KDEBUG_INFO, "%s\n", (const char *)s.local8Bit());
	QString t,dev;
	QStringList list;

	if (path==QString::null)
	{
		t=config_file.readEntry("Speech","SpeechProgram", "powiedz");
		klatt=config_file.readBoolEntry("Speech", "KlattSynt");
		melody=config_file.readBoolEntry("Speech", "Melody");
		arts=config_file.readBoolEntry("Speech", "UseArts");
		esd=config_file.readBoolEntry("Speech", "UseEsd");
		dsp=config_file.readBoolEntry("Speech", "UseDsp");
		dev=config_file.readEntry("Speech", "DspDev", "/dev/dsp");
		freq=config_file.readNumEntry("Speech", "Frequency");
		tempo=config_file.readNumEntry("Speech", "Tempo");
		basefreq=config_file.readNumEntry("Speech", "BaseFrequency");
	}
	else
	{
		t=path;
		dev=device;
	}
	
	list.append(t);
	if (klatt && dsp)
		list.append(" -L");
	if (!melody)
		list.append("-n");
	if (arts)
		list.append("-k");
//	if (esd)
//		t.append(" -");
	if (dsp)
	{
		list.append("-a");
		list.append(dev);
	}
	list.append("-r");
	list.append(QString::number(freq));
	list.append("-t");
	list.append(QString::number(tempo));
	list.append("-f");
	list.append(QString::number(basefreq));

	kdebugm(KDEBUG_INFO, "%s\n", (const char *)list.join(" ").local8Bit());
	QProcess *p=new QProcess(list);
	p->launch(s.local8Bit());
//	delete p;
	kdebugf2();
}

void SpeechSlots::testSpeech()
{
	kdebugf();
	QString program=ConfigDialog::getLineEdit("Speech", "Speech program:")->text();
	QString formatM=ConfigDialog::getLineEdit("Speech", "Chat format (male):")->text();
	QString formatF=ConfigDialog::getLineEdit("Speech", "Chat format (female):")->text();
	QString device=ConfigDialog::getLineEdit("Speech", "Dsp device:")->text();
	bool klatt=ConfigDialog::getCheckBox("Speech", "Klatt synthesizer (requires dsp)")->isChecked();
	bool mel=ConfigDialog::getCheckBox("Speech", "Melody")->isChecked();

	bool arts=ConfigDialog::getCheckBox("Speech", "Use aRts", "usearts")->isChecked();
	bool esd=ConfigDialog::getCheckBox("Speech", "Use Esd", "useesd")->isChecked();
	bool dsp=ConfigDialog::getCheckBox("Speech", "Use Dsp", "usedsp")->isChecked();
	
	kdebugm(KDEBUG_INFO, "flags: %d %d %d %d %d\n", mel, klatt, arts, esd, dsp);
	
	int freq=ConfigDialog::getSlider("Speech", "slider1")->value();
	int tempo=ConfigDialog::getSlider("Speech", "slider2")->value();
	int basefreq=ConfigDialog::getSlider("Speech", "slider3")->value();
	
	int i=int(userlist.count()*float(rand())/RAND_MAX);
	if (i>0) i--;
	UserListElement ule=userlist[i];

	kdebugm(KDEBUG_INFO, "%d %d %d %d\n", freq, tempo, basefreq, i);
	
	if (isFemale(ule.first_name))
		say(parse(formatF, ule).arg("Test"), program, klatt, mel, arts, esd, dsp, device, freq, tempo, basefreq);
	else
		say(parse(formatM, ule).arg("Test"), program, klatt, mel, arts, esd, dsp, device, freq, tempo, basefreq);
	kdebugf2();
}

void SpeechSlots::chat(UinsList senders,const QString& msg,time_t time, bool& grab)
{
	kdebugf();
	if (lastSpeech.elapsed()>1500)
	{
		Chat* chatWin= chat_manager->findChatByUins(senders);
		if (config_file.readBoolEntry("Speech","SayWhenWinNotActive") && chatWin)
			if (chatWin->isActiveWindow())
				return;
		UserListElement ule=userlist.byUin(senders.first());
		if (isFemale(ule.first_name))
			say(parse(config_file.readEntry("Speech", "ChatFormatFemale"), ule).arg(msg));
		else
			say(parse(config_file.readEntry("Speech", "ChatFormatMale"), ule).arg(msg));
		lastSpeech.restart();
	}
	kdebugf2();
}

void SpeechSlots::message(UinsList senders,const QString& msg,time_t time)
{
	kdebugf();
	if (lastSpeech.elapsed()>1500)
	{
		UserListElement ule=userlist.byUin(senders.first());
		if (isFemale(ule.first_name))
			say(parse(config_file.readEntry("Speech", "MessageFormatFemale"), ule).arg(msg));
		else
			say(parse(config_file.readEntry("Speech", "MessageFormatMale"), ule).arg(msg));
		lastSpeech.restart();
	}
}

void SpeechSlots::notify(UserListElement *ule)
{
	kdebugf();
	if (!config_file.readBoolEntry("Notify","NotifyStatusChange"))
		return;

	if (userlist.containsUin(ule->uin)) {
		if (!ule->notify && !config_file.readBoolEntry("Notify","NotifyAboutAll"))
			return;
		}
	else
		if (!config_file.readBoolEntry("Notify","NotifyAboutAll"))
			return;
	int status=ule->status;
	if (config_file.readBoolEntry("Notify","NotifyStatusChange") && (status == GG_STATUS_AVAIL ||
		status == GG_STATUS_AVAIL_DESCR || status == GG_STATUS_BUSY || status == GG_STATUS_BUSY_DESCR
		|| status == GG_STATUS_BLOCKED)/* &&
		(oldstatus == GG_STATUS_NOT_AVAIL || oldstatus == GG_STATUS_NOT_AVAIL_DESCR || oldstatus == GG_STATUS_INVISIBLE ||
		oldstatus == GG_STATUS_INVISIBLE_DESCR || oldstatus == GG_STATUS_INVISIBLE2)*/) 
		if (config_file.readBoolEntry("Notify","NotifyWithSpeech"))
			if (lastSpeech.elapsed()>1500)
			{
				QString t;
				if (isFemale(ule->first_name))
					t=parse(config_file.readEntry("Speech", "NotifyFormatFemale"), *ule);
				else
					t=parse(config_file.readEntry("Speech", "NotifyFormatMale"), *ule);
				t.replace(QRegExp("&nbsp;"), " ");
				t.replace(QRegExp("&lt;"), "<");
				t.replace(QRegExp("&gt;"), ">");
				t.replace(QRegExp("&amp;"), "&");
				say(t);
				lastSpeech.restart();
			}
	kdebugf2();
}

void SpeechSlots::onCreateConfigDialog()
{
	kdebugf();
	QCheckBox *b=ConfigDialog::getCheckBox("Speech", "Use aRts", "usearts");
	if (b->isChecked())
		b->setEnabled(false);
	b=ConfigDialog::getCheckBox("Speech", "Use Dsp", "usedsp");
	if (b->isChecked())
		b->setEnabled(false);
	b=ConfigDialog::getCheckBox("Speech", "Use Esd", "useesd");
	if (b->isChecked())
		b->setEnabled(false);
	kdebugf2();
}

void SpeechSlots::chooseSpeechProgram()
{
	kdebugf();
	QLineEdit *e_speechprog= ConfigDialog::getLineEdit("Speech", "Speech program:");

	QString s(QFileDialog::getOpenFileName( e_speechprog->text(), "All Files (*)"));
	if (s.length())
		e_speechprog->setText(s);
	kdebugf2();
}

SpeechSlots *speechObj;

