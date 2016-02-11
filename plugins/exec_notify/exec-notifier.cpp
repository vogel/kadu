/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2011, 2012, 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "exec-notifier.h"

#include "exec-configuration-widget.h"

#include "configuration/configuration.h"
#include "configuration/deprecated-configuration-api.h"
#include "core/injected-factory.h"
#include "notification/notifier-repository.h"
#include "notification/notification-manager.h"
#include "notification/notification.h"
#include "parser/parser.h"

#include <QtCore/QProcess>

ExecNotifier::ExecNotifier(QObject *parent) :
		QObject{parent},
		Notifier("Exec", QT_TRANSLATE_NOOP("@default", "Run command"), KaduIcon("external_modules/execnotify"))
{
}

ExecNotifier::~ExecNotifier()
{
}

void ExecNotifier::setConfiguration(Configuration *configuration)
{
	m_configuration = configuration;
}

void ExecNotifier::setInjectedFactory(InjectedFactory *injectedFactory)
{
	m_injectedFactory = injectedFactory;
}

void ExecNotifier::setNotificationManager(NotificationManager *notificationManager)
{
	m_notificationManager = notificationManager;
}

void ExecNotifier::setNotifierRepository(NotifierRepository *notifierRepository)
{
	m_notifierRepository = notifierRepository;
}

void ExecNotifier::setParser(Parser *parser)
{
	m_parser = parser;
}

void ExecNotifier::init()
{
	createDefaultConfiguration();
}

void ExecNotifier::createDefaultConfiguration()
{
	m_configuration->deprecatedApi()->addVariable("Exec Notify", "NewChatCmd", "Xdialog --msgbox \"#{protocol} %u %ids #{event}\" 10 100");
	m_configuration->deprecatedApi()->addVariable("Exec Notify", "NewMessageCmd", "Xdialog --msgbox \"#{protocol} %u %ids #{event}\" 10 100");
	m_configuration->deprecatedApi()->addVariable("Exec Notify", "ConnectionErrorCmd", "Xdialog --msgbox \"#{protocol} #{event}\" 10 100");
	m_configuration->deprecatedApi()->addVariable("Exec Notify", "StatusChanged", "Xdialog --msgbox \"#{protocol} %u #{event}\" 10 100");
	m_configuration->deprecatedApi()->addVariable("Exec Notify", "StatusChanged/ToFreeForChatCmd", "Xdialog --msgbox \"%protocol %u #{event}\" 10 100");
	m_configuration->deprecatedApi()->addVariable("Exec Notify", "StatusChanged/ToOnlineCmd", "Xdialog --msgbox \"%protocol %u #{event}\" 10 100");
	m_configuration->deprecatedApi()->addVariable("Exec Notify", "StatusChanged/ToAwayCmd", "Xdialog --msgbox \"#{protocol} %u #{event}\" 10 100");
	m_configuration->deprecatedApi()->addVariable("Exec Notify", "StatusChanged/ToNotAvailableCmd", "Xdialog --msgbox \"#{protocol} %u #{event}\" 10 100");
	m_configuration->deprecatedApi()->addVariable("Exec Notify", "StatusChanged/ToDoNotDisturbCmd", "Xdialog --msgbox \"#{protocol} %u #{event}\" 10 100");
	m_configuration->deprecatedApi()->addVariable("Exec Notify", "StatusChanged/ToOfflineCmd", "Xdialog --msgbox \"#{protocol} %u #{event}\" 10 100");
}

// TODO: merge with HistoryManager version
QStringList mySplit(const QChar &sep, const QString &str)
{
	auto strlist = QStringList{};
	auto token = QString{};
	auto idx = 0;
	auto strlength = str.length();
	auto inString = false;

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
				auto pos1 = str.indexOf('\\', idx);
				if (pos1 == -1)
					pos1 = strlength;
				auto pos2 = str.indexOf('"', idx);
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
				auto pos1 = str.indexOf(sep, idx);
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

	return strlist;
}

void ExecNotifier::notify(const Notification &notification)
{
	auto key = m_notificationManager->notifyConfigurationKey(notification.type());
	auto syntax = m_configuration->deprecatedApi()->readEntry("Exec Notify", key + "Cmd");
	if (syntax.isEmpty())
		return;
	auto s = mySplit(' ', syntax);
	auto result = QStringList{};

	auto chat = notification.data()["chat"].value<Chat>();
	if (chat)
	{
		auto contacts = chat.contacts();

		auto sendersList = QStringList{};
		for (auto contact : contacts)
			sendersList.append(m_parser->escape(contact.id()));
		QString sendersString = sendersList.join(",");

		Contact contact = *contacts.constBegin();
		foreach (QString it, s)
			result.append(m_parser->parse(it.replace("%ids", sendersString), Talkable(contact), &notification, ParserEscape::HtmlEscape));
	}
	else
		foreach (const QString &it, s)
			result.append(m_parser->parse(it, &notification, ParserEscape::HtmlEscape));

	run(result);
}

void ExecNotifier::run(const QStringList &args)
{
	auto p = new QProcess{};
	QProcess::connect(p, SIGNAL(finished(int, QProcess::ExitStatus)), p, SLOT(deleteLater()));
	p->start(args.at(0), args.mid(1));
}

NotifierConfigurationWidget * ExecNotifier::createConfigurationWidget(QWidget *parent)
{
	return m_injectedFactory->makeInjected<ExecConfigurationWidget>(parent);
}

#include "moc_exec-notifier.cpp"
