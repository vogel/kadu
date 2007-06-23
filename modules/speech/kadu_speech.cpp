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

#include "../notify/notify.h"

#include "chat_widget.h"
#include "chat_manager.h"
#include "configuration_window_widgets.h"
#include "debug.h"
#include "kadu_speech.h"
#include "kadu.h"
#include "kadu_parser.h"

/**
 * @ingroup speech
 * @{
 */
extern "C" int speech_init()
{
	kdebugf();

	speech = new Speech();
	MainConfigurationWindow::registerUiFile(dataPath("kadu/modules/configuration/speech.ui"), speech);

	kdebugf2();
	return 0;
}

extern "C" void speech_close()
{
	kdebugf();

	MainConfigurationWindow::unregisterUiFile(dataPath("kadu/modules/configuration/speech.ui"), speech);
	delete speech;
	speech = 0;

	kdebugf2();
}

bool isFemale(QString s)
{
	return s.endsWith("a", false);
}

SpeechConfigurationWidget::SpeechConfigurationWidget(QWidget *parent, char *name)
	: NotifierConfigurationWidget(parent, name), currentNotifyEvent("")
{
	maleLineEdit = new QLineEdit(this);
	femaleLineEdit = new QLineEdit(this);

	QGridLayout *gridLayout = new QGridLayout(this, 0, 0, 0, 5);
	gridLayout->addWidget(new QLabel(tr("Male format") + ":", this), 0, 0, Qt::AlignRight);
	gridLayout->addWidget(maleLineEdit, 0, 1);
	gridLayout->addWidget(new QLabel(tr("Female format") + ":", this), 1, 0, Qt::AlignRight);
	gridLayout->addWidget(femaleLineEdit, 1, 1);
}

SpeechConfigurationWidget::~SpeechConfigurationWidget()
{
}

void SpeechConfigurationWidget::saveNotifyConfigurations()
{
	if (currentNotifyEvent != "")
	{
		maleFormat[currentNotifyEvent] = maleLineEdit->text();
		femaleFormat[currentNotifyEvent] = femaleLineEdit->text();
	}

	CONST_FOREACH(text, maleFormat)
	{
		const QString &eventName = text.key();
		config_file.writeEntry("Speech", eventName + "_Syntax/Male", *text);
	}

	CONST_FOREACH(text, femaleFormat)
	{
		const QString &eventName = text.key();
		config_file.writeEntry("Speech", eventName + "_Syntax/Female", *text);
	}
}

void SpeechConfigurationWidget::switchToEvent(const QString &event)
{
	if (currentNotifyEvent != "")
	{
		maleFormat[currentNotifyEvent] = maleLineEdit->text();
		femaleFormat[currentNotifyEvent] = femaleLineEdit->text();
	}
	currentNotifyEvent = event;

	if (maleFormat.contains(event))
		maleLineEdit->setText(maleFormat[event]);
	else
		maleLineEdit->setText(config_file.readEntry("Speech", event + "_Syntax/Male"));

	if (femaleFormat.contains(event))
		femaleLineEdit->setText(femaleFormat[event]);
	else
		femaleLineEdit->setText(config_file.readEntry("Speech", event + "_Syntax/Female"));
}

Speech::Speech()
	: Notifier(), lastSpeech()
{
	kdebugf();
	srand(time(NULL));
	lastSpeech.start();

	import_0_5_0_Configuration();

	notification_manager->registerNotifier(QT_TRANSLATE_NOOP("@default", "Speech"), this);

	config_file.addVariable("Notify", "NewChat_Speech", true);

	kdebugf2();
}

Speech::~Speech()
{
	kdebugf();
	notification_manager->unregisterNotifier("Speech");

	kdebugf2();
}

void Speech::import_0_5_0_ConfigurationFromTo(const QString &from, const QString &to)
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

void Speech::import_0_5_0_Configuration()
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

	bool arts = config_file.readBoolEntry("Speech", "UseArts", false);
	bool esd = config_file.readBoolEntry("Speech", "UseEsd", false);
	bool dsp = config_file.readBoolEntry("Speech", "UseDsp", false);

	if (arts)
		config_file.writeEntry("Speech", "SoundSystem", "aRts");
	else if (esd)
		config_file.writeEntry("Speech", "SoundSystem", "Eds");
	else if (dsp)
		config_file.writeEntry("Speech", "SoundSystem", "Dsp");

	config_file.removeVariable("Speech", "UseArts");
	config_file.removeVariable("Speech", "UseEsd");
	config_file.removeVariable("Speech", "UseDsp");
}

void Speech::say(const QString &s, const QString &path,
		bool klatt, bool melody,
		const QString &sound_system, const QString &device,
		int freq, int tempo, int basefreq)
{
	kdebugf();

	QString t, dev, soundSystem;
	QStringList list;

	if (path.isNull())
	{
		t = config_file.readEntry("Speech","SpeechProgram", "powiedz");
		klatt = config_file.readBoolEntry("Speech", "KlattSynt");
		melody = config_file.readBoolEntry("Speech", "Melody");
		soundSystem = config_file.readBoolEntry("Speech", "SoundSystem");
		dev = config_file.readEntry("Speech", "DspDev", "/dev/dsp");
		freq = config_file.readNumEntry("Speech", "Frequency");
		tempo = config_file.readNumEntry("Speech", "Tempo");
		basefreq = config_file.readNumEntry("Speech", "BaseFrequency");
	}
	else
	{
		t = path;
		dev = device;
		soundSystem = sound_system;
	}

	list.append(t);
	if (klatt && soundSystem == "Dsp")
		list.append(" -L");
	if (!melody)
		list.append("-n");
	if (soundSystem == "aRts")
		list.append("-k");
// TODO: dlaczego tak?
//	if (esd)
//		t.append(" -");
	if (soundSystem == "Dsp")
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

	QProcess *p  =new QProcess(list);
	connect(p, SIGNAL(processExited()), p, SLOT(deleteLater()));
	p->launch(s.local8Bit());

	kdebugf2();
}

void Speech::testSpeech()
{
	kdebugf();

	QString program = programSelectFile->file();
	// TODO: mo¿e u¿ywaæ jakiego¶ normalnego tekstu ?
	QString formatM = config_file.readEntry("Speech", "NewChat_Syntax/Male");
	QString formatF = config_file.readEntry("Speech", "NewChat_Syntax/Female");
	QString device = dspDeviceLineEdit->text();
	bool klatt = klattSyntCheckBox->isChecked();
	bool mel = melodyCheckBox->isChecked();

	QString sound_system = soundSystemComboBox->currentItemValue();
	kdebugm(KDEBUG_INFO, "flags: %d %d %s\n", mel, klatt, sound_system.data());

	int frequency = frequencySlider->value();
	int tempo = tempoSlider->value();
	int baseFrequency = baseFrequencySlider->value();

	int i = int(userlist->count() * float(rand())/RAND_MAX);
	if (i > 0)
		--i;
	UserList::const_iterator it = userlist->constBegin();
	while (i-- > 0)
		++it;
	UserListElement ule = *it;

	kdebugm(KDEBUG_INFO, "%d %d %d %d\n", frequency, tempo, baseFrequency, i);

	if (isFemale(ule.firstName()))
		say(KaduParser::parse(formatF, ule).arg("Test"), program, klatt, mel, sound_system, device, frequency, tempo, baseFrequency);
	else
		say(KaduParser::parse(formatM, ule).arg("Test"), program, klatt, mel, sound_system, device, frequency, tempo, baseFrequency);

	kdebugf2();
}

void Speech::notify(Notification *notification)
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

void Speech::mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow)
{
	frequencySlider = dynamic_cast<QSlider *>(mainConfigurationWindow->widgetById("speech/frequency"));
	tempoSlider = dynamic_cast<QSlider *>(mainConfigurationWindow->widgetById("speech/tempo"));
	baseFrequencySlider = dynamic_cast<QSlider *>(mainConfigurationWindow->widgetById("speech/baseFrequency"));
	melodyCheckBox = dynamic_cast<QCheckBox *>(mainConfigurationWindow->widgetById("spech/melody"));

	programSelectFile = dynamic_cast<SelectFile *>(mainConfigurationWindow->widgetById("speech/program"));;

	soundSystemComboBox = dynamic_cast<ConfigComboBox *>(mainConfigurationWindow->widgetById("speech/soundSystem"));
	dspDeviceLineEdit = dynamic_cast<QLineEdit *>(mainConfigurationWindow->widgetById("speech/dspDevice"));
	klattSyntCheckBox = dynamic_cast<QCheckBox *>(mainConfigurationWindow->widgetById("speech/klattSynt"));

	connect(soundSystemComboBox, SIGNAL(activated(int)), this, SLOT(soundSystemChanged(int)));
	connect(mainConfigurationWindow->widgetById("speech/test"), SIGNAL(clicked()), this, SLOT(testSpeech()));
}

NotifierConfigurationWidget * Speech::createConfigurationWidget(QWidget *parent, char *name)
{
	return new SpeechConfigurationWidget(parent, name);
}

void Speech::soundSystemChanged(int index)
{
	bool dsp = soundSystemComboBox->currentItemValue() == "Dsp";

	dspDeviceLineEdit->setEnabled(dsp);
	klattSyntCheckBox->setEnabled(dsp);
}

Speech *speech;

/** @} */
