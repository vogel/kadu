/*
 * %kadu copyright begin%
 * Copyright 2008, 2009, 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2008 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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
#include <QtGui/QApplication>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>

#include "buddies/buddy-list.h"
#include "buddies/buddy.h"

#include "chat/chat.h"

#include "configuration/configuration-file.h"

#include "contacts/contact-set.h"

#include "gui/widgets/configuration/notify-group-box.h"
#include "gui/windows/main-configuration-window.h"

#include "misc/misc.h"

#include "notify/account-notification.h"
#include "notify/chat-notification.h"
#include "notify/notification-manager.h"
#include "notify/notification.h"

#include "icons/icons-manager.h"
#include "parser/parser.h"
#include "debug.h"

#include "exec_notify.h"

ExecConfigurationWidget::ExecConfigurationWidget(QWidget *parent)
	: NotifierConfigurationWidget(parent)
{
	commandLineEdit = new QLineEdit(this);
	commandLineEdit->setToolTip(qApp->translate("@default", MainConfigurationWindow::SyntaxTextNotify));

	QHBoxLayout *layout = new QHBoxLayout(this);
	layout->addWidget(commandLineEdit);

	static_cast<NotifyGroupBox *>(parent)->addWidget(this);
}

ExecConfigurationWidget::~ExecConfigurationWidget()
{
}

void ExecConfigurationWidget::saveNotifyConfigurations()
{
	if (!currentNotifyEvent.isEmpty())
		Commands[currentNotifyEvent] = commandLineEdit->text();

	for (QMap<QString, QString>::const_iterator it = Commands.constBegin(), end = Commands.constEnd(); it != end; ++it)
		config_file.writeEntry("Exec Notify", it.key() + "Cmd", it.value());
}

void ExecConfigurationWidget::switchToEvent(const QString &event)
{
	if (!currentNotifyEvent.isEmpty())
		Commands[currentNotifyEvent] = commandLineEdit->text();
	currentNotifyEvent = event;

	if (Commands.contains(event))
		commandLineEdit->setText(Commands[event]);
	else
		commandLineEdit->setText(config_file.readEntry("Exec Notify", event + "Cmd"));
}

ExecNotify::ExecNotify(QObject *parent) :
		Notifier("Exec", QT_TRANSLATE_NOOP("@default", "Run command"), KaduIcon("external_modules/execnotify"), parent)
{
	kdebugf();

	import_0_6_5_configuration();
	createDefaultConfiguration();
	NotificationManager::instance()->registerNotifier(this);

	kdebugf2();
}

ExecNotify::~ExecNotify()
{
	kdebugf();

	NotificationManager::instance()->unregisterNotifier(this);

	kdebugf2();
}

void ExecNotify::import_0_6_5_configuration()
{
    	config_file.addVariable("Exec Notify", "StatusChanged/ToAwayCmd", config_file.readEntry("Exec Notify", "StatusChanged/ToBusyCmd"));
}

void ExecNotify::createDefaultConfiguration()
{
	config_file.addVariable("Exec Notify", "NewChatCmd", "Xdialog --msgbox \"#{protocol} %u %ids #{event}\" 10 100");
	config_file.addVariable("Exec Notify", "NewMessageCmd", "Xdialog --msgbox \"#{protocol} %u %ids #{event}\" 10 100");
	config_file.addVariable("Exec Notify", "ConnectionErrorCmd", "Xdialog --msgbox \"#{protocol} #{event}\" 10 100");
	config_file.addVariable("Exec Notify", "StatusChanged", "Xdialog --msgbox \"#{protocol} %u #{event}\" 10 100");
	config_file.addVariable("Exec Notify", "StatusChanged/ToFreeForChatCmd", "Xdialog --msgbox \"%protocol %u #{event}\" 10 100");
	config_file.addVariable("Exec Notify", "StatusChanged/ToOnlineCmd", "Xdialog --msgbox \"%protocol %u #{event}\" 10 100");
	config_file.addVariable("Exec Notify", "StatusChanged/ToAwayCmd", "Xdialog --msgbox \"#{protocol} %u #{event}\" 10 100");
	config_file.addVariable("Exec Notify", "StatusChanged/ToNotAvailableCmd", "Xdialog --msgbox \"#{protocol} %u #{event}\" 10 100");
	config_file.addVariable("Exec Notify", "StatusChanged/ToDoNotDisturbCmd", "Xdialog --msgbox \"#{protocol} %u #{event}\" 10 100");
	config_file.addVariable("Exec Notify", "StatusChanged/ToOfflineCmd", "Xdialog --msgbox \"#{protocol} %u #{event}\" 10 100");
}

// TODO: merge with HistoryManager version
QStringList mySplit(const QChar &sep, const QString &str)
{
	kdebugf();
	QStringList strlist;
	QString token;
	unsigned int idx = 0, strlength = str.length();
	bool inString = false;

	int pos1, pos2;
	while (idx < strlength)
	{
		const QChar &letter = str[idx];
		if (inString)
		{
			if (letter == '\\')
			{
				switch (str[idx + 1].digitValue())
				{
					case 'n':
						token.append('\n');
						break;
					case '\\':
						token.append('\\');
						break;
					case '\"':
						token.append('"');
						break;
					default:
						token.append('?');
				}
				idx += 2;
			}
			else if (letter == '"')
			{
				strlist.append(token);
				inString = false;
				++idx;
			}
			else
			{
				pos1 = str.indexOf('\\', idx);
				if (pos1 == -1)
					pos1 = strlength;
				pos2 = str.indexOf('"', idx);
				if (pos2 == -1)
					pos2 = strlength;
				if (pos1 < pos2)
				{
					token.append(str.mid(idx, pos1 - idx));
					idx = pos1;
				}
				else
				{
					token.append(str.mid(idx, pos2 - idx));
					idx = pos2;
				}
			}
		}
		else // out of the string
		{
			if (letter == sep)
			{
				if (!token.isEmpty())
					token.clear();
				else
					strlist.append(QString());
			}
			else if (letter == '"')
				inString = true;
			else
			{
				pos1 = str.indexOf(sep, idx);
				if (pos1 == -1)
					pos1 = strlength;
				token.append(str.mid(idx, pos1 - idx));
				strlist.append(token);
				idx = pos1;
				continue;
			}
			++idx;
		}
	}

	kdebugf2();
	return strlist;
}

void ExecNotify::notify(Notification *notification)
{
	QString syntax = config_file.readEntry("Exec Notify", notification->key() + "Cmd");
	if (syntax.isEmpty())
		return;
	QStringList s = mySplit(' ', syntax);
	QStringList result;

	ChatNotification *chatNotification = qobject_cast<ChatNotification *>(notification);
	if (chatNotification)
	{
		ContactSet contacts = chatNotification->chat().contacts();

		QStringList sendersList;
		foreach (const Contact &contact, contacts)
			sendersList.append(contact.id());
		QString sendersString = sendersList.join(",");

		Contact contact = *contacts.constBegin();
		foreach (QString it, s)
			result.append(Parser::parse(it.replace("%ids", sendersString), Talkable(contact), notification));
	}
	else
		foreach (const QString &it, s)
			result.append(Parser::parse(it, notification));

	run(result);
}

void ExecNotify::run(const QStringList &args)
{
	foreach(const QString &arg, args)
	{
		kdebugm(KDEBUG_INFO, "arg: '%s'\n", qPrintable(arg));
	}

	QProcess *p = new QProcess(this);
	connect(p, SIGNAL(finished(int, QProcess::ExitStatus)), p, SLOT(deleteLater()));
	p->start(args.at(0), args.mid(1));
}

NotifierConfigurationWidget *ExecNotify::createConfigurationWidget(QWidget *parent)
{
	return new ExecConfigurationWidget(parent);
}
