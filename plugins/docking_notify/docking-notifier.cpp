/*
 * %kadu copyright begin%
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

#include <QtGui/QTextDocument>
#include <QtWidgets/QSystemTrayIcon>

#include "notification/notification-manager.h"
#include "notification/notification.h"

#include "configuration/configuration.h"
#include "configuration/deprecated-configuration-api.h"
#include "core/injected-factory.h"
#include "gui/widgets/chat-widget/chat-widget-manager.h"
#include "gui/windows/message-dialog.h"
#include "parser/parser.h"
#include "debug.h"

#include "plugins/docking/docking-plugin-object.h"
#include "plugins/docking/docking.h"

#include "docking-notifier.h"

/**
 * @ingroup qt4_notify
 * @{
 */

DockingNotifier::DockingNotifier(QObject *parent) :
		QObject{parent},
		Notifier("Tray Icon Balloon", QT_TRANSLATE_NOOP("@default", "Tray Icon Balloon"), KaduIcon("external_modules/qt4notify")),
		configurationWidget{}
{
}

DockingNotifier::~DockingNotifier()
{
}

void DockingNotifier::setChatWidgetManager(ChatWidgetManager *chatWidgetManager)
{
	m_chatWidgetManager = chatWidgetManager;
}

void DockingNotifier::setConfiguration(Configuration *configuration)
{
	m_configuration = configuration;
}

void DockingNotifier::setDocking(Docking *docking)
{
	m_docking = docking;
}

void DockingNotifier::setInjectedFactory(InjectedFactory *injectedFactory)
{
	m_injectedFactory = injectedFactory;
}

void DockingNotifier::setNotificationManager(NotificationManager *notificationManager)
{
	m_notificationManager = notificationManager;
}

void DockingNotifier::setParser(Parser *parser)
{
	m_parser = parser;
}

void DockingNotifier::init()
{
	createDefaultConfiguration();
	connect(m_docking, SIGNAL(messageClicked()), this, SLOT(messageClicked()));
}

QString DockingNotifier::toPlainText(const QString &text)
{
	QTextDocument doc;
	doc.setHtml(text);
	return doc.toPlainText();
}

QString DockingNotifier::parseText(const QString &text, const Notification &notification, const QString &def)
{
	QString ret;

	chat = notification.data()["chat"].value<Chat>();

	if (!text.isEmpty())
	{
		if (chat)
		{
			Contact contact = *chat.contacts().constBegin();
			ret = m_parser->parse(text, Talkable(contact), &notification, ParserEscape::HtmlEscape);
		}
		else
			ret = m_parser->parse(text, &notification, ParserEscape::HtmlEscape);

		ret = ret.replace("%&m", notification.text());
		ret = ret.replace("%&t", notification.title());
		ret = ret.replace("%&d", notification.details().join(QStringLiteral("\n")));
	}
	else
		ret = def;


	return toPlainText(ret);
}

void DockingNotifier::notify(const Notification &notification)
{
	kdebugf();

	auto key = m_notificationManager->notifyConfigurationKey(notification.type());
	unsigned int timeout = m_configuration->deprecatedApi()->readNumEntry("Qt4DockingNotifier", QString("Event_") + key + "_timeout");
	unsigned int icon = m_configuration->deprecatedApi()->readNumEntry("Qt4DockingNotifier", QString("Event_") + key + "_icon");
	QString title = m_configuration->deprecatedApi()->readEntry("Qt4DockingNotifier", QString("Event_") + key + "_title");
	QString syntax = m_configuration->deprecatedApi()->readEntry("Qt4DockingNotifier", QString("Event_") + key + "_syntax");

	m_docking->showMessage(parseText(title, notification, notification.text()),
		parseText(syntax, notification, notification.details().join(QStringLiteral("\n"))),
		(QSystemTrayIcon::MessageIcon)icon, timeout * 1000);

	kdebugf2();
}

void DockingNotifier::messageClicked()
{
	m_chatWidgetManager->openChat(chat, OpenChatActivation::Activate);
}

NotifierConfigurationWidget * DockingNotifier::createConfigurationWidget(QWidget *parent)
{
	return m_injectedFactory->makeInjected<DockingNotifyConfigurationWidget>(parent);
}

void DockingNotifier::createDefaultConfiguration()
{
	m_configuration->deprecatedApi()->addVariable("Qt4DockingNotifier", "Event_ConnectionError_timeout", 10);
	m_configuration->deprecatedApi()->addVariable("Qt4DockingNotifier", "Event_ConnectionError_syntax", "%&m");
	m_configuration->deprecatedApi()->addVariable("Qt4DockingNotifier", "Event_ConnectionError_title", "%&t");
	m_configuration->deprecatedApi()->addVariable("Qt4DockingNotifier", "Event_ConnectionError_icon", 3);

	m_configuration->deprecatedApi()->addVariable("Qt4DockingNotifier", "Event_NewChat_timeout", 10);
	m_configuration->deprecatedApi()->addVariable("Qt4DockingNotifier", "Event_NewChat_syntax", "%&d");
	m_configuration->deprecatedApi()->addVariable("Qt4DockingNotifier", "Event_NewChat_title", "%&m");
	m_configuration->deprecatedApi()->addVariable("Qt4DockingNotifier", "Event_NewChat_icon", 1);

	m_configuration->deprecatedApi()->addVariable("Qt4DockingNotifier", "Event_NewMessage_timeout", 10);
	m_configuration->deprecatedApi()->addVariable("Qt4DockingNotifier", "Event_NewMessage_syntax", "%&d");
	m_configuration->deprecatedApi()->addVariable("Qt4DockingNotifier", "Event_NewMessage_title", "%&m");
	m_configuration->deprecatedApi()->addVariable("Qt4DockingNotifier", "Event_NewMessage_icon", 1);

	m_configuration->deprecatedApi()->addVariable("Qt4DockingNotifier", "Event_StatusChanged_timeout", 10);
	m_configuration->deprecatedApi()->addVariable("Qt4DockingNotifier", "Event_StatusChanged_syntax", "%&d");
	m_configuration->deprecatedApi()->addVariable("Qt4DockingNotifier", "Event_StatusChanged_title", "%&m");
	m_configuration->deprecatedApi()->addVariable("Qt4DockingNotifier", "Event_StatusChangedt_icon", 0);

	m_configuration->deprecatedApi()->addVariable("Qt4DockingNotifier", "Event_StatusChanged/ToFreeForChat_timeout", 10);
	m_configuration->deprecatedApi()->addVariable("Qt4DockingNotifier", "Event_StatusChanged/ToFreeForChat_syntax", "%&d");
	m_configuration->deprecatedApi()->addVariable("Qt4DockingNotifier", "Event_StatusChanged/ToFreeForChat_title", "%&m");
	m_configuration->deprecatedApi()->addVariable("Qt4DockingNotifier", "Event_StatusChanged/ToFreeForChat_icon", 0);

	m_configuration->deprecatedApi()->addVariable("Qt4DockingNotifier", "Event_StatusChanged/ToOnline_timeout", 10);
	m_configuration->deprecatedApi()->addVariable("Qt4DockingNotifier", "Event_StatusChanged/ToOnline_syntax", "%&d");
	m_configuration->deprecatedApi()->addVariable("Qt4DockingNotifier", "Event_StatusChanged/ToOnline_title", "%&m");
	m_configuration->deprecatedApi()->addVariable("Qt4DockingNotifier", "Event_StatusChanged/ToOnline_icon", 0);

	m_configuration->deprecatedApi()->addVariable("Qt4DockingNotifier", "Event_StatusChanged/ToAway_timeout", 10);
	m_configuration->deprecatedApi()->addVariable("Qt4DockingNotifier", "Event_StatusChanged/ToAway_syntax", "%&d");
	m_configuration->deprecatedApi()->addVariable("Qt4DockingNotifier", "Event_StatusChanged/ToAway_title", "%&m");
	m_configuration->deprecatedApi()->addVariable("Qt4DockingNotifier", "Event_StatusChanged/ToAway_icon", 0);

	m_configuration->deprecatedApi()->addVariable("Qt4DockingNotifier", "Event_StatusChanged/ToNotAvailable_timeout", 10);
	m_configuration->deprecatedApi()->addVariable("Qt4DockingNotifier", "Event_StatusChanged/ToNotAvailable_syntax", "%&d");
	m_configuration->deprecatedApi()->addVariable("Qt4DockingNotifier", "Event_StatusChanged/ToNotAvailable_title", "%&m");
	m_configuration->deprecatedApi()->addVariable("Qt4DockingNotifier", "Event_StatusChanged/ToNotAvailable_icon", 0);

	m_configuration->deprecatedApi()->addVariable("Qt4DockingNotifier", "Event_StatusChanged/ToDoNotDisturb_timeout", 10);
	m_configuration->deprecatedApi()->addVariable("Qt4DockingNotifier", "Event_StatusChanged/ToDoNotDisturb_syntax", "%&d");
	m_configuration->deprecatedApi()->addVariable("Qt4DockingNotifier", "Event_StatusChanged/ToDoNotDisturb_title", "%&m");
	m_configuration->deprecatedApi()->addVariable("Qt4DockingNotifier", "Event_StatusChanged/ToDoNotDisturb_icon", 0);

	m_configuration->deprecatedApi()->addVariable("Qt4DockingNotifier", "Event_StatusChanged/ToOffline_timeout", 10);
	m_configuration->deprecatedApi()->addVariable("Qt4DockingNotifier", "Event_StatusChanged/ToOffline_syntax", "%&d");
	m_configuration->deprecatedApi()->addVariable("Qt4DockingNotifier", "Event_StatusChanged/ToOffline_title", "%&m");
	m_configuration->deprecatedApi()->addVariable("Qt4DockingNotifier", "Event_StatusChanged/ToOffline_icon", 0);

	m_configuration->deprecatedApi()->addVariable("Qt4DockingNotifier", "Event_FileTransfer/Finished_timeout", 10);
	m_configuration->deprecatedApi()->addVariable("Qt4DockingNotifier", "Event_FileTransfer/Finished_syntax", "%&m");
	m_configuration->deprecatedApi()->addVariable("Qt4DockingNotifier", "Event_FileTransfer/Finished_title", "%&t");
	m_configuration->deprecatedApi()->addVariable("Qt4DockingNotifier", "Event_FileTransfer/Finished_icon", 2);

	m_configuration->deprecatedApi()->addVariable("Qt4DockingNotifier", "Event_FileTransfer/IncomingFile_timeout", 10);
	m_configuration->deprecatedApi()->addVariable("Qt4DockingNotifier", "Event_FileTransfer/IncomingFile_syntax", "%&m");
	m_configuration->deprecatedApi()->addVariable("Qt4DockingNotifier", "Event_FileTransfer/IncomingFile_title", "%&t");
	m_configuration->deprecatedApi()->addVariable("Qt4DockingNotifier", "Event_FileTransfer/IncomingFile_icon", 2);
	m_configuration->deprecatedApi()->addVariable("Qt4DockingNotifier", "Qt4DockingNotificationEventConfiguration_Geometry", "50, 50, 615, 290");
}

/** @} */


#include "moc_docking-notifier.cpp"
