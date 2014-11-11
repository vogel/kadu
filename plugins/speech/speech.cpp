/*
 * %kadu copyright begin%
 * Copyright 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010, 2011, 2013, 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011, 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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
#include <QtGui/QTextDocument>

#include "configuration/configuration.h"
#include "configuration/deprecated-configuration-api.h"
#include "core/application.h"
#include "notify/notification-manager.h"
#include "notify/notification/chat-notification.h"
#include "notify/notification/notification.h"
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
		Notifier("Speech", QT_TRANSLATE_NOOP("@default", "Read a text"), KaduIcon()), lastSpeech()
{
	kdebugf();

	NotificationManager::instance()->registerNotifier(this);

	Application::instance()->configuration()->deprecatedApi()->addVariable("Notify", "NewChat_Speech", true);

	kdebugf2();
}

Speech::~Speech()
{
	kdebugf();
	NotificationManager::instance()->unregisterNotifier(this);

	kdebugf2();
}

void Speech::say(const QString &s, const QString &path,
		bool klatt, bool melody,
		const QString &sound_system, const QString &device,
		int freq, int tempo, int basefreq)
{
	kdebugf();

	QString t, dev, soundSystem;
	QStringList list;

	if (path.isEmpty())
	{
		t = Application::instance()->configuration()->deprecatedApi()->readEntry("Speech","SpeechProgram", "powiedz");
		klatt = Application::instance()->configuration()->deprecatedApi()->readBoolEntry("Speech", "KlattSynt");
		melody = Application::instance()->configuration()->deprecatedApi()->readBoolEntry("Speech", "Melody");
		soundSystem = Application::instance()->configuration()->deprecatedApi()->readBoolEntry("Speech", "SoundSystem");
		dev = Application::instance()->configuration()->deprecatedApi()->readEntry("Speech", "DspDev", "/dev/dsp");
		freq = Application::instance()->configuration()->deprecatedApi()->readNumEntry("Speech", "Frequency");
		tempo = Application::instance()->configuration()->deprecatedApi()->readNumEntry("Speech", "Tempo");
		basefreq = Application::instance()->configuration()->deprecatedApi()->readNumEntry("Speech", "BaseFrequency");
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
	p->write(s.toUtf8().constData());
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

	QString syntax = Application::instance()->configuration()->deprecatedApi()->readEntry("Speech", notification->type() + "_Syntax/" + sex, QString());
	if (syntax.isEmpty())
		text = notification->text();
	else
	{
		QString details = notification->details().join(QLatin1String("\n"));
		if (details.length() > Application::instance()->configuration()->deprecatedApi()->readNumEntry("Speech", "MaxLength"))
			syntax = Application::instance()->configuration()->deprecatedApi()->readEntry("Speech", "MsgTooLong" + sex);

		syntax = syntax.arg(details);

		if (chat)
		{
			Contact contact = *chat.contacts().begin();
			text = Parser::parse(syntax, Talkable(contact), notification, ParserEscape::HtmlEscape);
		}
		else
			text= Parser::parse(syntax, notification, ParserEscape::HtmlEscape);
	}

	QTextDocument document;
	document.setHtml(text);
	say(document.toPlainText());
	lastSpeech.restart();

	kdebugf2();
}

NotifierConfigurationWidget * Speech::createConfigurationWidget(QWidget *parent)
{
	return new SpeechConfigurationWidget(parent);
}


/** @} */

#include "moc_speech.cpp"
