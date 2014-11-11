/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2008 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009 Maciej Płaza (plaza.maciej@gmail.com)
 * Copyright 2009, 2010, 2011, 2013, 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtGui/QTextDocument>
#include <QtWidgets/QSystemTrayIcon>

#include "notify/notification-manager.h"
#include "notify/notification/chat-notification.h"
#include "notify/notification/notification.h"

#include "configuration/configuration.h"
#include "configuration/deprecated-configuration-api.h"
#include "core/application.h"
#include "core/core.h"
#include "gui/widgets/chat-widget/chat-widget-manager.h"
#include "gui/windows/message-dialog.h"
#include "parser/parser.h"
#include "debug.h"

#include "plugins/qt4_docking/qt4_docking.h"

#include "qt4-docking-notify.h"

/**
 * @ingroup qt4_notify
 * @{
 */

Qt4Notify::Qt4Notify(QObject *parent) :
		Notifier("Tray Icon Balloon", QT_TRANSLATE_NOOP("@default", "Tray Icon Balloon"), KaduIcon("external_modules/qt4notify"), parent),
		configurationWidget{}
{
	kdebugf();

	createDefaultConfiguration();

	NotificationManager::instance()->registerNotifier(this);

	if (Qt4TrayIcon::instance())
		connect(Qt4TrayIcon::instance(), SIGNAL(messageClicked()), this, SLOT(messageClicked()));

	kdebugf2();
}

Qt4Notify::~Qt4Notify()
{
	kdebugf();
	NotificationManager::instance()->unregisterNotifier(this);

	if (Qt4TrayIcon::instance())
		disconnect(Qt4TrayIcon::instance(), 0, this, 0);

	kdebugf2();
}

QString Qt4Notify::toPlainText(const QString &text)
{
	QTextDocument doc;
	doc.setHtml(text);
	return doc.toPlainText();
}

QString Qt4Notify::parseText(const QString &text, Notification *notification, const QString &def)
{
	QString ret;

	ChatNotification *chatNotification = qobject_cast<ChatNotification *>(notification);
	chat = chatNotification ? chatNotification->chat() : Chat();

	if (!text.isEmpty())
	{
		if (chatNotification)
		{
			Contact contact = *chatNotification->chat().contacts().constBegin();
			ret = Parser::parse(text, Talkable(contact), notification, ParserEscape::HtmlEscape);
		}
		else
			ret = Parser::parse(text, notification, ParserEscape::HtmlEscape);

		ret = ret.replace("%&m", notification->text());
		ret = ret.replace("%&t", notification->title());
		ret = ret.replace("%&d", notification->details().join(QLatin1String("\n")));
	}
	else
		ret = def;


	return toPlainText(ret);
}

void Qt4Notify::notify(Notification *notification)
{
	kdebugf();

	if (Qt4TrayIcon::instance())
	{
		notification->acquire(this);

		unsigned int timeout = Application::instance()->configuration()->deprecatedApi()->readNumEntry("Qt4DockingNotify", QString("Event_") + notification->key() + "_timeout");
		unsigned int icon = Application::instance()->configuration()->deprecatedApi()->readNumEntry("Qt4DockingNotify", QString("Event_") + notification->key() + "_icon");
		QString title = Application::instance()->configuration()->deprecatedApi()->readEntry("Qt4DockingNotify", QString("Event_") + notification->key() + "_title");
		QString syntax = Application::instance()->configuration()->deprecatedApi()->readEntry("Qt4DockingNotify", QString("Event_") + notification->key() + "_syntax");

		Qt4TrayIcon::instance()->showMessage(parseText(title, notification, notification->text()),
			parseText(syntax, notification, notification->details().join(QLatin1String("\n"))),
			(QSystemTrayIcon::MessageIcon)icon, timeout * 1000);

		notification->release(this);
	}

	kdebugf2();
}

void Qt4Notify::messageClicked()
{
	Core::instance()->chatWidgetManager()->openChat(chat, OpenChatActivation::Activate);
}

NotifierConfigurationWidget *Qt4Notify::createConfigurationWidget(QWidget *parent)
{
	configurationWidget = new Qt4NotifyConfigurationWidget(parent);
	return configurationWidget;
}

void Qt4Notify::createDefaultConfiguration()
{
	Application::instance()->configuration()->deprecatedApi()->addVariable("Qt4DockingNotify", "Event_ConnectionError_timeout", 10);
	Application::instance()->configuration()->deprecatedApi()->addVariable("Qt4DockingNotify", "Event_ConnectionError_syntax", "%&m");
	Application::instance()->configuration()->deprecatedApi()->addVariable("Qt4DockingNotify", "Event_ConnectionError_title", "%&t");
	Application::instance()->configuration()->deprecatedApi()->addVariable("Qt4DockingNotify", "Event_ConnectionError_icon", 3);

	Application::instance()->configuration()->deprecatedApi()->addVariable("Qt4DockingNotify", "Event_NewChat_timeout", 10);
	Application::instance()->configuration()->deprecatedApi()->addVariable("Qt4DockingNotify", "Event_NewChat_syntax", "%&d");
	Application::instance()->configuration()->deprecatedApi()->addVariable("Qt4DockingNotify", "Event_NewChat_title", "%&m");
	Application::instance()->configuration()->deprecatedApi()->addVariable("Qt4DockingNotify", "Event_NewChat_icon", 1);

	Application::instance()->configuration()->deprecatedApi()->addVariable("Qt4DockingNotify", "Event_NewMessage_timeout", 10);
	Application::instance()->configuration()->deprecatedApi()->addVariable("Qt4DockingNotify", "Event_NewMessage_syntax", "%&d");
	Application::instance()->configuration()->deprecatedApi()->addVariable("Qt4DockingNotify", "Event_NewMessage_title", "%&m");
	Application::instance()->configuration()->deprecatedApi()->addVariable("Qt4DockingNotify", "Event_NewMessage_icon", 1);

	Application::instance()->configuration()->deprecatedApi()->addVariable("Qt4DockingNotify", "Event_StatusChanged_timeout", 10);
	Application::instance()->configuration()->deprecatedApi()->addVariable("Qt4DockingNotify", "Event_StatusChanged_syntax", "%&d");
	Application::instance()->configuration()->deprecatedApi()->addVariable("Qt4DockingNotify", "Event_StatusChanged_title", "%&m");
	Application::instance()->configuration()->deprecatedApi()->addVariable("Qt4DockingNotify", "Event_StatusChangedt_icon", 0);

	Application::instance()->configuration()->deprecatedApi()->addVariable("Qt4DockingNotify", "Event_StatusChanged/ToFreeForChat_timeout", 10);
	Application::instance()->configuration()->deprecatedApi()->addVariable("Qt4DockingNotify", "Event_StatusChanged/ToFreeForChat_syntax", "%&d");
	Application::instance()->configuration()->deprecatedApi()->addVariable("Qt4DockingNotify", "Event_StatusChanged/ToFreeForChat_title", "%&m");
	Application::instance()->configuration()->deprecatedApi()->addVariable("Qt4DockingNotify", "Event_StatusChanged/ToFreeForChat_icon", 0);

	Application::instance()->configuration()->deprecatedApi()->addVariable("Qt4DockingNotify", "Event_StatusChanged/ToOnline_timeout", 10);
	Application::instance()->configuration()->deprecatedApi()->addVariable("Qt4DockingNotify", "Event_StatusChanged/ToOnline_syntax", "%&d");
	Application::instance()->configuration()->deprecatedApi()->addVariable("Qt4DockingNotify", "Event_StatusChanged/ToOnline_title", "%&m");
	Application::instance()->configuration()->deprecatedApi()->addVariable("Qt4DockingNotify", "Event_StatusChanged/ToOnline_icon", 0);

	Application::instance()->configuration()->deprecatedApi()->addVariable("Qt4DockingNotify", "Event_StatusChanged/ToAway_timeout", 10);
	Application::instance()->configuration()->deprecatedApi()->addVariable("Qt4DockingNotify", "Event_StatusChanged/ToAway_syntax", "%&d");
	Application::instance()->configuration()->deprecatedApi()->addVariable("Qt4DockingNotify", "Event_StatusChanged/ToAway_title", "%&m");
	Application::instance()->configuration()->deprecatedApi()->addVariable("Qt4DockingNotify", "Event_StatusChanged/ToAway_icon", 0);

	Application::instance()->configuration()->deprecatedApi()->addVariable("Qt4DockingNotify", "Event_StatusChanged/ToNotAvailable_timeout", 10);
	Application::instance()->configuration()->deprecatedApi()->addVariable("Qt4DockingNotify", "Event_StatusChanged/ToNotAvailable_syntax", "%&d");
	Application::instance()->configuration()->deprecatedApi()->addVariable("Qt4DockingNotify", "Event_StatusChanged/ToNotAvailable_title", "%&m");
	Application::instance()->configuration()->deprecatedApi()->addVariable("Qt4DockingNotify", "Event_StatusChanged/ToNotAvailable_icon", 0);

	Application::instance()->configuration()->deprecatedApi()->addVariable("Qt4DockingNotify", "Event_StatusChanged/ToDoNotDisturb_timeout", 10);
	Application::instance()->configuration()->deprecatedApi()->addVariable("Qt4DockingNotify", "Event_StatusChanged/ToDoNotDisturb_syntax", "%&d");
	Application::instance()->configuration()->deprecatedApi()->addVariable("Qt4DockingNotify", "Event_StatusChanged/ToDoNotDisturb_title", "%&m");
	Application::instance()->configuration()->deprecatedApi()->addVariable("Qt4DockingNotify", "Event_StatusChanged/ToDoNotDisturb_icon", 0);

	Application::instance()->configuration()->deprecatedApi()->addVariable("Qt4DockingNotify", "Event_StatusChanged/ToOffline_timeout", 10);
	Application::instance()->configuration()->deprecatedApi()->addVariable("Qt4DockingNotify", "Event_StatusChanged/ToOffline_syntax", "%&d");
	Application::instance()->configuration()->deprecatedApi()->addVariable("Qt4DockingNotify", "Event_StatusChanged/ToOffline_title", "%&m");
	Application::instance()->configuration()->deprecatedApi()->addVariable("Qt4DockingNotify", "Event_StatusChanged/ToOffline_icon", 0);

	Application::instance()->configuration()->deprecatedApi()->addVariable("Qt4DockingNotify", "Event_FileTransfer/Finished_timeout", 10);
	Application::instance()->configuration()->deprecatedApi()->addVariable("Qt4DockingNotify", "Event_FileTransfer/Finished_syntax", "%&m");
	Application::instance()->configuration()->deprecatedApi()->addVariable("Qt4DockingNotify", "Event_FileTransfer/Finished_title", "%&t");
	Application::instance()->configuration()->deprecatedApi()->addVariable("Qt4DockingNotify", "Event_FileTransfer/Finished_icon", 2);

	Application::instance()->configuration()->deprecatedApi()->addVariable("Qt4DockingNotify", "Event_FileTransfer/IncomingFile_timeout", 10);
	Application::instance()->configuration()->deprecatedApi()->addVariable("Qt4DockingNotify", "Event_FileTransfer/IncomingFile_syntax", "%&m");
	Application::instance()->configuration()->deprecatedApi()->addVariable("Qt4DockingNotify", "Event_FileTransfer/IncomingFile_title", "%&t");
	Application::instance()->configuration()->deprecatedApi()->addVariable("Qt4DockingNotify", "Event_FileTransfer/IncomingFile_icon", 2);
	Application::instance()->configuration()->deprecatedApi()->addVariable("Qt4DockingNotify", "Qt4DockingNotifyEventConfiguration_Geometry", "50, 50, 615, 290");
}

/** @} */


#include "moc_qt4-docking-notify.cpp"
