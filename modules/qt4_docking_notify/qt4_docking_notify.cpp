/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009 Kermit (plaza.maciej@gmail.com)
 * Copyright 2008 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2009, 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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

#include <QtGui/QSystemTrayIcon>
#include <QtGui/QTextDocument>

#include "notify/chat-notification.h"
#include "notify/notification-manager.h"
#include "notify/notification.h"

#include "gui/widgets/chat-widget-manager.h"
#include "gui/windows/message-dialog.h"
#include "configuration/configuration-file.h"
#include "parser/parser.h"

#include "debug.h"

#include "qt4_docking_notify.h"

/**
 * @ingroup qt4_notify
 * @{
 */
extern "C" KADU_EXPORT int qt4_docking_notify_init(bool firstLoad)
{
	Q_UNUSED(firstLoad)
	kdebugf();

	if (!QSystemTrayIcon::supportsMessages())
		return 1;

	qt4_notify = new Qt4Notify;

	kdebugf2();
	return 0;
}

extern "C" KADU_EXPORT void qt4_docking_notify_close()
{
	kdebugf();

	delete qt4_notify;
	qt4_notify = 0;

	kdebugf2();
}

Qt4Notify::Qt4Notify() :
		Notifier("Tray Icon Balloon", "Tray Icon Balloon", IconsManager::instance()->iconByPath("external_modules/qt4notify"))
{
	kdebugf();

	import_0_6_5_configuration();
	createDefaultConfiguration();

	NotificationManager::instance()->registerNotifier(this);

	if (qt4_tray_icon != 0)
	{
		connect(qt4_tray_icon, SIGNAL(messageClicked()), this, SLOT(messageClicked()));
	}

	kdebugf2();
}

Qt4Notify::~Qt4Notify()
{
	kdebugf();
	NotificationManager::instance()->unregisterNotifier(this);

	if (qt4_tray_icon != 0)
	{
		disconnect(qt4_tray_icon, SIGNAL(messageClicked()), this, SLOT(messageClicked()));
	}

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

	ChatNotification *chatNotification = dynamic_cast<ChatNotification *>(notification);
	chat = chatNotification ? chatNotification->chat() : Chat();

	if (!text.isEmpty())
	{
		if (chatNotification)
		{
			Contact contact = *chatNotification->chat().contacts().begin();
			ret = Parser::parse(text, BuddyOrContact(contact), notification);
		}
		else
			ret = Parser::parse(text, notification);

		ret = ret.replace("%&m", notification->text());
		ret = ret.replace("%&t", notification->title());
		ret = ret.replace("%&d", notification->details());
	}
	else
		ret = def;


	return toPlainText(ret);
}

void Qt4Notify::notify(Notification *notification)
{
	kdebugf();

	if (qt4_tray_icon)
	{
		notification->acquire();

		unsigned int timeout = config_file.readNumEntry("Qt4DockingNotify", QString("Event_") + notification->key() + "_timeout");
		unsigned int icon = config_file.readNumEntry("Qt4DockingNotify", QString("Event_") + notification->key() + "_icon");
		QString title = config_file.readEntry("Qt4DockingNotify", QString("Event_") + notification->key() + "_title");
		QString syntax = config_file.readEntry("Qt4DockingNotify", QString("Event_") + notification->key() + "_syntax");

		qt4_tray_icon->showMessage(parseText(title, notification, notification->text()),
			parseText(syntax, notification, notification->details()),
			(QSystemTrayIcon::MessageIcon)icon, timeout * 1000);

		notification->release();
	}

	kdebugf2();
}

void Qt4Notify::messageClicked()
{
	if (chat)
		ChatWidgetManager::instance()->openPendingMsgs(chat, true);
}

NotifierConfigurationWidget *Qt4Notify::createConfigurationWidget(QWidget *parent)
{
	configurationWidget = new Qt4NotifyConfigurationWidget(parent);
	return configurationWidget;
}

void Qt4Notify::import_0_6_5_configuration()
{
    	config_file.addVariable("Qt4DockingNotify", "Event_StatusChanged/ToAway_timeout",
		    config_file.readEntry("Qt4DockingNotify", "Event_StatusChanged/ToBusy_timeout"));
	config_file.addVariable("Qt4DockingNotify", "Event_StatusChanged/ToAway_syntax",
		    config_file.readEntry("Qt4DockingNotify", "Event_StatusChanged/ToBusy_timeout"));
	config_file.addVariable("Qt4DockingNotify", "Event_StatusChanged/ToAway_timeout",
		    config_file.readEntry("Qt4DockingNotify", "Event_StatusChanged/ToBusy_title"));
	config_file.addVariable("Qt4DockingNotify", "Event_StatusChanged/ToAway_icon",
		    config_file.readEntry("Qt4DockingNotify", "Event_StatusChanged/ToBusy_icon"));
}

void Qt4Notify::createDefaultConfiguration()
{
	config_file.addVariable("Qt4DockingNotify", "Event_ConnectionError_timeout", 10);
	config_file.addVariable("Qt4DockingNotify", "Event_ConnectionError_syntax", "%&m");
	config_file.addVariable("Qt4DockingNotify", "Event_ConnectionError_title", "%&t");
	config_file.addVariable("Qt4DockingNotify", "Event_ConnectionError_icon", 3);

	config_file.addVariable("Qt4DockingNotify", "Event_NewChat_timeout", 10);
	config_file.addVariable("Qt4DockingNotify", "Event_NewChat_syntax", "%&d");
	config_file.addVariable("Qt4DockingNotify", "Event_NewChat_title", "%&m");
	config_file.addVariable("Qt4DockingNotify", "Event_NewChat_icon", 1);

	config_file.addVariable("Qt4DockingNotify", "Event_NewMessage_timeout", 10);
	config_file.addVariable("Qt4DockingNotify", "Event_NewMessage_syntax", "%&d");
	config_file.addVariable("Qt4DockingNotify", "Event_NewMessage_title", "%&m");
	config_file.addVariable("Qt4DockingNotify", "Event_NewMessage_icon", 1);

	config_file.addVariable("Qt4DockingNotify", "Event_StatusChanged_timeout", 10);
	config_file.addVariable("Qt4DockingNotify", "Event_StatusChanged_syntax", "%&d");
	config_file.addVariable("Qt4DockingNotify", "Event_StatusChanged_title", "%&m");
	config_file.addVariable("Qt4DockingNotify", "Event_StatusChangedt_icon", 0);

	config_file.addVariable("Qt4DockingNotify", "Event_StatusChanged/ToFreeForChat_timeout", 10);
	config_file.addVariable("Qt4DockingNotify", "Event_StatusChanged/ToFreeForChat_syntax", "%&d");
	config_file.addVariable("Qt4DockingNotify", "Event_StatusChanged/ToFreeForChat_title", "%&m");
	config_file.addVariable("Qt4DockingNotify", "Event_StatusChanged/ToFreeForChat_icon", 0);

	config_file.addVariable("Qt4DockingNotify", "Event_StatusChanged/ToOnline_timeout", 10);
	config_file.addVariable("Qt4DockingNotify", "Event_StatusChanged/ToOnline_syntax", "%&d");
	config_file.addVariable("Qt4DockingNotify", "Event_StatusChanged/ToOnline_title", "%&m");
	config_file.addVariable("Qt4DockingNotify", "Event_StatusChanged/ToOnline_icon", 0);

	config_file.addVariable("Qt4DockingNotify", "Event_StatusChanged/ToAway_timeout", 10);
	config_file.addVariable("Qt4DockingNotify", "Event_StatusChanged/ToAway_syntax", "%&d");
	config_file.addVariable("Qt4DockingNotify", "Event_StatusChanged/ToAway_title", "%&m");
	config_file.addVariable("Qt4DockingNotify", "Event_StatusChanged/ToAway_icon", 0);

	config_file.addVariable("Qt4DockingNotify", "Event_StatusChanged/ToNotAvailable_timeout", 10);
	config_file.addVariable("Qt4DockingNotify", "Event_StatusChanged/ToNotAvailable_syntax", "%&d");
	config_file.addVariable("Qt4DockingNotify", "Event_StatusChanged/ToNotAvailable_title", "%&m");
	config_file.addVariable("Qt4DockingNotify", "Event_StatusChanged/ToNotAvailable_icon", 0);

	config_file.addVariable("Qt4DockingNotify", "Event_StatusChanged/ToDoNotDisturb_timeout", 10);
	config_file.addVariable("Qt4DockingNotify", "Event_StatusChanged/ToDoNotDisturb_syntax", "%&d");
	config_file.addVariable("Qt4DockingNotify", "Event_StatusChanged/ToDoNotDisturb_title", "%&m");
	config_file.addVariable("Qt4DockingNotify", "Event_StatusChanged/ToDoNotDisturb_icon", 0);

	config_file.addVariable("Qt4DockingNotify", "Event_StatusChanged/ToOffline_timeout", 10);
	config_file.addVariable("Qt4DockingNotify", "Event_StatusChanged/ToOffline_syntax", "%&d");
	config_file.addVariable("Qt4DockingNotify", "Event_StatusChanged/ToOffline_title", "%&m");
	config_file.addVariable("Qt4DockingNotify", "Event_StatusChanged/ToOffline_icon", 0);

	config_file.addVariable("Qt4DockingNotify", "Event_FileTransfer/Finished_timeout", 10);
	config_file.addVariable("Qt4DockingNotify", "Event_FileTransfer/Finished_syntax", "%&m");
	config_file.addVariable("Qt4DockingNotify", "Event_FileTransfer/Finished_title", "%&t");
	config_file.addVariable("Qt4DockingNotify", "Event_FileTransfer/Finished_icon", 2);

	config_file.addVariable("Qt4DockingNotify", "Event_FileTransfer/IncomingFile_timeout", 10);
	config_file.addVariable("Qt4DockingNotify", "Event_FileTransfer/IncomingFile_syntax", "%&m");
	config_file.addVariable("Qt4DockingNotify", "Event_FileTransfer/IncomingFile_title", "%&t");
	config_file.addVariable("Qt4DockingNotify", "Event_FileTransfer/IncomingFile_icon", 2);
	config_file.addVariable("Qt4DockingNotify", "Qt4DockingNotifyEventConfiguration_Geometry", "50, 50, 615, 290");
}

Qt4Notify *qt4_notify = 0;

/** @} */

