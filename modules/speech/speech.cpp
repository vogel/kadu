/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * %kadu copyright end%
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <QtCore/QProcess>

#include "configuration/configuration-file.h"
#include "notify/chat-notification.h"
#include "notify/notification-manager.h"
#include "notify/notification.h"
#include "parser/parser.h"
#include "debug.h"
#include "speech-configuration-widget.h"

#include "speech.h"

/**
 * @ingroup speech
 * @{
 */

Speech * Speech::Instance = 0;

void Speech::createInstance()
{
	if (!Instance)
		Instance = new Speech();
}

void Speech::destroyInstance()
{
	delete Instance;
	Instance = 0;
}

Speech * Speech::instance()
{
	return Instance;
}

bool isFemale(const QString &s)
{
	return s.endsWith('a', Qt::CaseInsensitive);
}

Speech::Speech() :
		Notifier("Speech", QT_TRANSLATE_NOOP("@default", "Read a text"), QString()), lastSpeech()
{
	kdebugf();

	import_0_5_0_Configuration();
	import_0_6_5_configuration();

	NotificationManager::instance()->registerNotifier(this);

	config_file.addVariable("Notify", "NewChat_Speech", true);

	kdebugf2();
}

Speech::~Speech()
{
	kdebugf();
	NotificationManager::instance()->unregisterNotifier(this);

	kdebugf2();
}

void Speech::import_0_5_0_ConfigurationFromTo(const QString &from, const QString &to)
{
	QString entry = config_file.readEntry("Speech", from + "Female", QString());
	if (!entry.isEmpty())
		config_file.writeEntry("Speech", from + "_Syntax/Female", entry);
	config_file.removeVariable("Speech", from + "Female");

	entry = config_file.readEntry("Speech", to + "Male", QString());
	if (!entry.isEmpty())
		config_file.writeEntry("Speech", to + "_Syntax/Male", entry);
	config_file.removeVariable("Speech", to + "Male");
}

void Speech::import_0_5_0_Configuration()
{
	QString entry;

	entry = config_file.readEntry("Speech", "ConnectionError", QString());
	if (!entry.isEmpty())
		config_file.writeEntry("Speech", "ConnectionError_Syntax", entry.replace("%1", "(#{errorServer}) #{error}"));
	config_file.removeVariable("Speech", "ConnectionError");

	entry = config_file.readEntry("Speech", "NotifyFormatFemale", QString());
	if (!entry.isEmpty())
	{
		config_file.writeEntry("Speech", "StatusChanged/ToOnline_Syntax/Female", entry);
		config_file.writeEntry("Speech", "StatusChanged/ToBusy_Syntax/Female", entry);
		config_file.writeEntry("Speech", "StatusChanged/ToInvisible_Syntax/Female", entry);
		config_file.writeEntry("Speech", "StatusChanged/ToOffline_Syntax/Female", entry);
		config_file.writeEntry("Speech", "StatusChanged/ToTalkWithMe_Syntax/Female", entry);
		config_file.writeEntry("Speech", "StatusChanged/ToDoNotDisturb_Syntax/Female", entry);
	}
	config_file.removeVariable("Speech", "NotifyFormatFemale");

	entry = config_file.readEntry("Speech", "NotifyFormatMale", QString());
	if (!entry.isEmpty())
	{
		config_file.writeEntry("Speech", "StatusChanged/ToOnline_Syntax/Male", entry);
		config_file.writeEntry("Speech", "StatusChanged/ToBusy_Syntax/Male", entry);
		config_file.writeEntry("Speech", "StatusChanged/ToInvisible_Syntax/Male", entry);
		config_file.writeEntry("Speech", "StatusChanged/ToOffline_Syntax/Male", entry);
		config_file.writeEntry("Speech", "StatusChanged/ToTalkWithMe_Syntax/Male", entry);
		config_file.writeEntry("Speech", "StatusChanged/ToDoNotDisturb_Syntax/Male", entry);
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

void Speech::import_0_6_5_configuration()
{

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

	kdebugm(KDEBUG_INFO, "text: %s command: %s %s\n", qPrintable(s), qPrintable(t), qPrintable(list.join(" ")));

	QProcess *p = new QProcess();
	connect(p, SIGNAL(finished(int, QProcess::ExitStatus)), p, SLOT(deleteLater()));
	p->start(t, list);
	p->write(qPrintable(s));
	p->closeWriteChannel();

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
	QString sex = "Male";

	ChatNotification *chatNotification = qobject_cast<ChatNotification *>(notification);
	Chat chat = chatNotification ? chatNotification->chat() : Chat::null;

	// TODO:
	if (chat)
	{
		if (isFemale((*chat.contacts().begin()).ownerBuddy().firstName()))
			sex = "Female";
	}

	QString syntax = config_file.readEntry("Speech", notification->type() + "_Syntax/" + sex, QString());
	if (syntax.isEmpty())
		text = notification->text();
	else
	{
		QString details = notification->details();
		if (details.length() > config_file.readNumEntry("Speech", "MaxLength"))
			syntax = config_file.readEntry("Speech", "MsgTooLong" + sex);

		syntax = syntax.arg(details);

		if (chat)
		{
			Contact contact = *chat.contacts().begin();
			text = Parser::parse(syntax, BuddyOrContact(contact), notification);
		}
		else
			text= Parser::parse(syntax, notification);
	}

	text.replace("&nbsp;", " ");
	text.replace("&lt;", "<");
	text.replace("&gt;", ">");
	text.replace("&amp;", "&");

	say(text);
	lastSpeech.restart();

	kdebugf2();
}

NotifierConfigurationWidget * Speech::createConfigurationWidget(QWidget *parent)
{
	return new SpeechConfigurationWidget(parent);
}


/** @} */
