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
#include <qprocess.h>
#include <qslider.h>

#include <stdlib.h>
#include <time.h>

#include "chat_widget.h"
#include "chat_manager.h"
// #include "config_dialog.h"
#include "debug.h"
#include "kadu_speech.h"
#include "kadu.h"
#include "kadu_parser.h"
#include "../notify/notify.h"

/**
 * @ingroup speech
 * @{
 */
extern "C" int speech_init()
{
	kdebugf();
	speech_slots=new SpeechSlots(NULL, "speech_slots");
	kdebugf2();
	return 0;
}

extern "C" void speech_close()
{
	kdebugf();
	delete speech_slots;
	speech_slots=NULL;
	kdebugf2();
}

bool isFemale(QString s)
{
	return s.endsWith("a", false);
}

void SpeechSlots::useArts()
{
	kdebugf();
// 	ConfigDialog::getCheckBox("Speech", "Use aRts", "usearts")->setChecked(true);
// 	ConfigDialog::getCheckBox("Speech", "Use Esd", "useesd")->setChecked(false);
// 	ConfigDialog::getCheckBox("Speech", "Use Dsp", "usedsp")->setChecked(false);

// 	ConfigDialog::getCheckBox("Speech", "Klatt synthesizer (requires dsp)")->setChecked(false);
// 	ConfigDialog::getCheckBox("Speech", "Klatt synthesizer (requires dsp)")->setEnabled(false);
// 	ConfigDialog::getLineEdit("Speech", "Dsp device:")->setEnabled(false);
	kdebugf2();
}

void SpeechSlots::useEsd()
{
	kdebugf();
// 	ConfigDialog::getCheckBox("Speech", "Use aRts", "usearts")->setChecked(false);
// 	ConfigDialog::getCheckBox("Speech", "Use Esd", "useesd")->setChecked(true);
// 	ConfigDialog::getCheckBox("Speech", "Use Dsp", "usedsp")->setChecked(false);

// 	ConfigDialog::getCheckBox("Speech", "Klatt synthesizer (requires dsp)")->setChecked(false);
// 	ConfigDialog::getCheckBox("Speech", "Klatt synthesizer (requires dsp)")->setEnabled(false);
// 	ConfigDialog::getLineEdit("Speech", "Dsp device:")->setEnabled(false);
	kdebugf2();
}

void SpeechSlots::useDsp()
{
	kdebugf();
// 	ConfigDialog::getCheckBox("Speech", "Use aRts", "usearts")->setChecked(false);
// 	ConfigDialog::getCheckBox("Speech", "Use Esd", "useesd")->setChecked(false);
// 	ConfigDialog::getCheckBox("Speech", "Use Dsp", "usedsp")->setChecked(true);

// 	ConfigDialog::getCheckBox("Speech", "Klatt synthesizer (requires dsp)")->setEnabled(true);
// 	ConfigDialog::getLineEdit("Speech", "Dsp device:")->setEnabled(true);
	kdebugf2();
}

SpeechSlots::SpeechSlots(QObject *parent, const char *name) : Notifier(parent, name), lastSpeech()
{
	kdebugf();
	srand(time(NULL));
	lastSpeech.start();

	import_0_5_0_Configuration();

// 	ConfigDialog::addTab(QT_TRANSLATE_NOOP("@default","Speech"), "SpeechTab");

// 	ConfigDialog::addCheckBox("Speech", "Speech",
// 			QT_TRANSLATE_NOOP("@default","Say only when chat window is not active"),
// 			"SayWhenWinNotActive", true);

// 	ConfigDialog::addGrid("Speech", "Speech", "freq", 2);
// 	ConfigDialog::addLabel("Speech", "freq", QT_TRANSLATE_NOOP("@default","Frequency"));
// 	ConfigDialog::addSlider("Speech", "freq", "slider1", "Frequency", 5000, 22050, 500, 8000);

// 	ConfigDialog::addGrid("Speech", "Speech", "tempo", 2);
// 	ConfigDialog::addLabel("Speech", "tempo", QT_TRANSLATE_NOOP("@default","Tempo"));
// 	ConfigDialog::addSlider("Speech", "tempo", "slider2", "Tempo", 0, 10, 1, 5);

// 	ConfigDialog::addGrid("Speech", "Speech", "basefreq", 2);
// 	ConfigDialog::addLabel("Speech", "basefreq", QT_TRANSLATE_NOOP("@default","Base frequency"));
// 	ConfigDialog::addSlider("Speech", "basefreq", "slider3", "BaseFrequency", 60, 440, 10, 133);
// 	ConfigDialog::addSpinBox("Speech", "Speech", QT_TRANSLATE_NOOP("@default",
// 		"Maximum number of spoken letters"), "MaxLength", 10, 2001, 1, 200);

// 	ConfigDialog::addCheckBox("Speech", "Speech", QT_TRANSLATE_NOOP("@default","Melody"), "Melody", true);
// 	ConfigDialog::addCheckBox("Speech", "Speech",
// 			QT_TRANSLATE_NOOP("@default","Klatt synthesizer (requires dsp)"), "KlattSynt", false);
// 	ConfigDialog::addCheckBox("Speech", "Speech",
// 			QT_TRANSLATE_NOOP("@default","Use aRts"), "UseArts", false, 0, "usearts");
// 	ConfigDialog::addCheckBox("Speech", "Speech",
// 			QT_TRANSLATE_NOOP("@default","Use Esd"), "UseEsd", false, 0, "useesd");
// 	ConfigDialog::addCheckBox("Speech", "Speech",
// 			QT_TRANSLATE_NOOP("@default","Use Dsp"), "UseDsp", true, 0, "usedsp");
// 	ConfigDialog::addLineEdit("Speech", "Speech",
// 			QT_TRANSLATE_NOOP("@default","Dsp device:"), "DspDev", "/dev/dsp", 0, 0, Advanced);

// 	ConfigDialog::addHGroupBox("Speech", "Speech", QT_TRANSLATE_NOOP("@default","Program"));
// 	ConfigDialog::addLineEdit("Speech", "Program",
// 			QT_TRANSLATE_NOOP("@default","Speech program:"), "SpeechProgram","powiedz");
// 	ConfigDialog::addPushButton("Speech", "Program", 0, "OpenFile", 0, "speech_fileopen");
// 	ConfigDialog::connectSlot("Speech", 0, SIGNAL(clicked()), this, SLOT(chooseSpeechProgram()), "speech_fileopen");

// 	ConfigDialog::addLineEdit("Speech", "Speech",
// 			QT_TRANSLATE_NOOP("@default","Chat format (male):"),
// 			"NewChat_Syntax/Male", SpeechSlots::tr("man %a wrote %1"), Kadu::SyntaxText, 0, Advanced);
// 	ConfigDialog::addLineEdit("Speech", "Speech",
// 			QT_TRANSLATE_NOOP("@default","Chat format (female):"),
// 			"NewChat_Syntax/Female", SpeechSlots::tr("woman %a wrote %1"), Kadu::SyntaxText, 0, Advanced);

// 	ConfigDialog::addLineEdit("Speech", "Speech",
// 			QT_TRANSLATE_NOOP("@default","Message format (male):"),
// 			"NewMessage_Syntax/Male", SpeechSlots::tr("man %a wrote %1"), Kadu::SyntaxText, 0, Advanced);
// 	ConfigDialog::addLineEdit("Speech", "Speech",
// 			QT_TRANSLATE_NOOP("@default","Message format (female):"),
// 			"NewMessage_Syntax/Female", SpeechSlots::tr("woman %a wrote %1"), Kadu::SyntaxText, 0, Advanced);

// 	ConfigDialog::addLineEdit("Speech", "Speech",
// 			QT_TRANSLATE_NOOP("@default","Status changed to online (male):"),
// 			"StatusChanged/ToOnline_Syntax/Male", SpeechSlots::tr("man %a changed status to %s %d"), Kadu::SyntaxText, 0, Advanced);
// 	ConfigDialog::addLineEdit("Speech", "Speech",
// 			QT_TRANSLATE_NOOP("@default","Status changed to online (female):"),
// 			"StatusChanged/ToOnline_Syntax/Female", SpeechSlots::tr("woman %a changed status to %s %d"), Kadu::SyntaxText, 0, Advanced);

// 	ConfigDialog::addLineEdit("Speech", "Speech",
// 			QT_TRANSLATE_NOOP("@default","Status changed to busy (male):"),
// 			"StatusChanged/ToBusy_Syntax/Male", SpeechSlots::tr("man %a changed status to %s %d"), Kadu::SyntaxText, 0, Advanced);
// 	ConfigDialog::addLineEdit("Speech", "Speech",
// 			QT_TRANSLATE_NOOP("@default","Status changed to busy (female):"),
// 			"StatusChanged/ToBusy_Syntax/Female", SpeechSlots::tr("woman %a changed status to %s %d"), Kadu::SyntaxText, 0, Advanced);

// 	ConfigDialog::addLineEdit("Speech", "Speech",
// 			QT_TRANSLATE_NOOP("@default","Status changed to invisible (male):"),
// 			"StatusChanged/ToInvisible_Syntax/Male", SpeechSlots::tr("man %a changed status to %s %d"), Kadu::SyntaxText, 0, Advanced);
// 	ConfigDialog::addLineEdit("Speech", "Speech",
// 			QT_TRANSLATE_NOOP("@default","Status changed to invisible (female):"),
// 			"StatusChanged/ToInvisible_Syntax/Female", SpeechSlots::tr("woman %a changed status to %s %d"), Kadu::SyntaxText, 0, Advanced);

// 	ConfigDialog::addLineEdit("Speech", "Speech",
// 			QT_TRANSLATE_NOOP("@default","Status changed to offline (male):"),
// 			"StatusChanged/ToOffline_Syntax/Male", SpeechSlots::tr("man %a changed status to %s %d"), Kadu::SyntaxText, 0, Advanced);
// 	ConfigDialog::addLineEdit("Speech", "Speech",
// 			QT_TRANSLATE_NOOP("@default","Status changed to offline (female):"),
// 			"StatusChanged/ToOffline_Syntax/Female", SpeechSlots::tr("woman %a changed status to %s %d"), Kadu::SyntaxText, 0, Advanced);

// 	ConfigDialog::addLineEdit("Speech", "Speech",
// 			QT_TRANSLATE_NOOP("@default","Connection error:"),
// 			"ConnectionError_Syntax", SpeechSlots::tr("Connection error - #{error}"), Kadu::SyntaxText, 0, Advanced);

// 	ConfigDialog::addLineEdit("Speech", "Speech",
// 			QT_TRANSLATE_NOOP("@default","Message too long (male):"),
// 			"MsgTooLongMale", SpeechSlots::tr("%a wrote long message"), Kadu::SyntaxText, 0, Advanced);
// 	ConfigDialog::addLineEdit("Speech", "Speech",
// 			QT_TRANSLATE_NOOP("@default","Message too long (female):"),
// 			"MsgTooLongFemale", SpeechSlots::tr("%a wrote long message"), Kadu::SyntaxText, 0, Advanced);

// 	ConfigDialog::addPushButton("Speech", "Speech",	QT_TRANSLATE_NOOP("@default","Test"), QString::null, 0, "testspeech");

// 	ConfigDialog::connectSlot("Speech", "Test", SIGNAL(clicked()), this, SLOT(testSpeech()), "testspeech");
// 	ConfigDialog::connectSlot("Speech", "Use aRts", SIGNAL(clicked()), this, SLOT(useArts()), "usearts");
// 	ConfigDialog::connectSlot("Speech", "Use Esd", SIGNAL(clicked()), this, SLOT(useEsd()), "useesd");
// 	ConfigDialog::connectSlot("Speech", "Use Dsp", SIGNAL(clicked()), this, SLOT(useDsp()), "usedsp");

// 	ConfigDialog::registerSlotOnCreateTab("Speech", this, SLOT(onCreateTabSpeech()));

	notification_manager->registerNotifier(QT_TRANSLATE_NOOP("@default", "Speech"), this);

	config_file.addVariable("Notify", "NewChat_Speech", true);

	kdebugf2();
}

SpeechSlots::~SpeechSlots()
{
	kdebugf();
	notification_manager->unregisterNotifier("Speech");

// 	ConfigDialog::unregisterSlotOnCreateTab("Speech", this, SLOT(onCreateTabSpeech()));

// 	ConfigDialog::disconnectSlot("Speech", 0, SIGNAL(clicked()), this, SLOT(chooseSpeechProgram()), "speech_fileopen");
// 	ConfigDialog::disconnectSlot("Speech", "Test", SIGNAL(clicked()), this, SLOT(testSpeech()), "testspeech");
// 	ConfigDialog::disconnectSlot("Speech", "Use aRts", SIGNAL(clicked()), this, SLOT(useArts()), "usearts");
// 	ConfigDialog::disconnectSlot("Speech", "Use Esd", SIGNAL(clicked()), this, SLOT(useEsd()), "useesd");
// 	ConfigDialog::disconnectSlot("Speech", "Use Dsp", SIGNAL(clicked()), this, SLOT(useDsp()), "usedsp");

// 	ConfigDialog::removeControl("Speech", "Test", "testspeech");
// 	ConfigDialog::removeControl("Speech", "Connection error:");
// 	ConfigDialog::removeControl("Speech", "Message too long (male):");
// 	ConfigDialog::removeControl("Speech", "Message too long (female):");
// 	ConfigDialog::removeControl("Speech", "Notify format (female):");
// 	ConfigDialog::removeControl("Speech", "Notify format (male):");
// 	ConfigDialog::removeControl("Speech", "Message format (female):");
// 	ConfigDialog::removeControl("Speech", "Message format (male):");
// 	ConfigDialog::removeControl("Speech", "Chat format (female):");
// 	ConfigDialog::removeControl("Speech", "Chat format (male):");
// 	ConfigDialog::removeControl("Speech", 0, "speech_fileopen");
// 	ConfigDialog::removeControl("Speech", "Speech program:");
// 	ConfigDialog::removeControl("Speech", "Program");
// 	ConfigDialog::removeControl("Speech", "Dsp device:");
// 	ConfigDialog::removeControl("Speech", "Use Dsp", "usedsp");
// 	ConfigDialog::removeControl("Speech", "Use Esd", "useesd");
// 	ConfigDialog::removeControl("Speech", "Use aRts", "usearts");
// 	ConfigDialog::removeControl("Speech", "Klatt synthesizer (requires dsp)");
// 	ConfigDialog::removeControl("Speech", "Melody");
// 	ConfigDialog::removeControl("Speech", "Maximum number of spoken letters");
// 	ConfigDialog::removeControl("Speech", "slider3");
// 	ConfigDialog::removeControl("Speech", "Base frequency");
// 	ConfigDialog::removeControl("Speech", "basefreq");
// 	ConfigDialog::removeControl("Speech", "slider2");
// 	ConfigDialog::removeControl("Speech", "Tempo");
// 	ConfigDialog::removeControl("Speech", "tempo");
// 	ConfigDialog::removeControl("Speech", "slider1");
// 	ConfigDialog::removeControl("Speech", "Frequency");
// 	ConfigDialog::removeControl("Speech", "freq");
// 	ConfigDialog::removeControl("Speech", "Say only when chat window is not active");
// 	ConfigDialog::removeTab("Speech");
	kdebugf2();
}

void SpeechSlots::import_0_5_0_ConfigurationFromTo(const QString &from, const QString &to)
{
	QString entry = config_file.readEntry("Speech", from + "Female", "");
	if (entry != "")
		config_file.writeEntry("Speech", from + "_Syntax/Female", entry);
	config_file.removeVariable("Speech", from + "Female");

	entry = config_file.readEntry("Speech", to + "Male", "");
	if (entry != "")
		config_file.writeEntry("Speech", to + "_Syntax/Male", entry);
	config_file.removeVariable("Speech", to + "Male");
}

void SpeechSlots::import_0_5_0_Configuration()
{
	QString entry;

	entry = config_file.readEntry("Speech", "ConnectionError", "");
	if (entry != "")
		config_file.writeEntry("Speech", "ConnectionError_Syntax", entry.replace("%1", "#{error}"));
	config_file.removeVariable("Speech", "ConnectionError");

	entry = config_file.readEntry("Speech", "NotifyFormatFemale", "");
	if (entry != "")
	{
		config_file.writeEntry("Speech", "StatusChanged/ToOnline_Syntax/Female", entry);
		config_file.writeEntry("Speech", "StatusChanged/ToBusy_Syntax/Female", entry);
		config_file.writeEntry("Speech", "StatusChanged/ToInvisible_Syntax/Female", entry);
		config_file.writeEntry("Speech", "StatusChanged/ToOffline_Syntax/Female", entry);
	}
	config_file.removeVariable("Speech", "NotifyFormatFemale");

	entry = config_file.readEntry("Speech", "NotifyFormatMale", "");
	if (entry != "")
	{
		config_file.writeEntry("Speech", "StatusChanged/ToOnline_Syntax/Male", entry);
		config_file.writeEntry("Speech", "StatusChanged/ToBusy_Syntax/Male", entry);
		config_file.writeEntry("Speech", "StatusChanged/ToInvisible_Syntax/Male", entry);
		config_file.writeEntry("Speech", "StatusChanged/ToOffline_Syntax/Male", entry);
	}
	config_file.removeVariable("Speech", "NotifyFormatMale");

	import_0_5_0_ConfigurationFromTo("NewChat", "NewChat");
	import_0_5_0_ConfigurationFromTo("NewMessage", "NewMessage");
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
	connect(p, SIGNAL(processExited()), p, SLOT(deleteLater()));
	p->launch(s.local8Bit());
	kdebugf2();
}

void SpeechSlots::testSpeech()
{
	kdebugf();
// 	QString program=ConfigDialog::getLineEdit("Speech", "Speech program:")->text();
// 	QString formatM=ConfigDialog::getLineEdit("Speech", "Chat format (male):")->text();
// 	QString formatF=ConfigDialog::getLineEdit("Speech", "Chat format (female):")->text();
// 	QString device=ConfigDialog::getLineEdit("Speech", "Dsp device:")->text();
// 	bool klatt=ConfigDialog::getCheckBox("Speech", "Klatt synthesizer (requires dsp)")->isChecked();
// 	bool mel=ConfigDialog::getCheckBox("Speech", "Melody")->isChecked();

// 	bool arts=ConfigDialog::getCheckBox("Speech", "Use aRts", "usearts")->isChecked();
// 	bool esd=ConfigDialog::getCheckBox("Speech", "Use Esd", "useesd")->isChecked();
// 	bool dsp=ConfigDialog::getCheckBox("Speech", "Use Dsp", "usedsp")->isChecked();

// 	kdebugm(KDEBUG_INFO, "flags: %d %d %d %d %d\n", mel, klatt, arts, esd, dsp);

// 	int freq=ConfigDialog::getSlider("Speech", "slider1")->value();
// 	int tempo=ConfigDialog::getSlider("Speech", "slider2")->value();
// 	int basefreq=ConfigDialog::getSlider("Speech", "slider3")->value();

// 	int i = int(userlist->count()*float(rand())/RAND_MAX);
// 	if (i > 0)
// 		--i;
// 	UserList::const_iterator it = userlist->constBegin();
// 	while (i-- > 0)
// 		++it;
// 	UserListElement ule = *it;

// 	kdebugm(KDEBUG_INFO, "%d %d %d %d\n", freq, tempo, basefreq, i);
//
// 	if (isFemale(ule.firstName()))
// 		say(KaduParser::parse(formatF, ule).arg("Test"), program, klatt, mel, arts, esd, dsp, device, freq, tempo, basefreq);
// 	else
// 		say(KaduParser::parse(formatM, ule).arg("Test"), program, klatt, mel, arts, esd, dsp, device, freq, tempo, basefreq);
	kdebugf2();
}

void SpeechSlots::notify(Notification *notification)
{
	kdebugf();

	if (lastSpeech.elapsed() < 1500)
	{
		kdebugf2();
		return;
	}

	QString text;
	QString sex;

	UserListElement ule;
	if (notification->userListElements().count())
	{
		ule = notification->userListElements()[0];
		if (isFemale(ule.firstName()))
			sex = "/Female";
		else
			sex = "/Male";
	}

	QString syntax = config_file.readEntry("Speech", notification->type() + "_Syntax" + sex, "");
	if (syntax == "")
		text = notification->text();
	else
	{
		QString details = notification->details();
		if (details.length() > config_file.readUnsignedNumEntry("Speech", "MaxLength"))
			syntax = config_file.readEntry("Speech", "MsgTooLong" + sex);

		text = KaduParser::parse(syntax, ule, notification).arg(details);
	}

	text.replace("&nbsp;", " ");
	text.replace("&lt;", "<");
	text.replace("&gt;", ">");
	text.replace("&amp;", "&");

	say(text);
	lastSpeech.restart();

	kdebugf2();
}

void SpeechSlots::onCreateTabSpeech()
{
	kdebugf();
	if (config_file.readBoolEntry("Speech", "UseArts"))
		useArts();
	else if (config_file.readBoolEntry("Speech", "UseEsd"))
		useEsd();
	else if (config_file.readBoolEntry("Speech", "UseDsp"))
		useDsp();
	kdebugf2();
}

void SpeechSlots::chooseSpeechProgram()
{
	kdebugf();
// 	QLineEdit *e_speechprog= ConfigDialog::getLineEdit("Speech", "Speech program:");

// 	QString s(QFileDialog::getOpenFileName( e_speechprog->text(), "All Files (*)", ConfigDialog::configdialog));
// 	if (!s.isEmpty())
// 		e_speechprog->setText(s);
	kdebugf2();
}

SpeechSlots *speech_slots;

/** @} */

