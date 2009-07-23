/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtGui/QSystemTrayIcon>
#include <QtGui/QTextDocument>

#include "notify/account-notification.h"
#include "notify/notification-manager.h"
#include "notify/notification.h"

#include "chat/chat.h"
#include "gui/widgets/chat-widget-manager.h"
#include "gui/windows/message-box.h"
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

Qt4Notify::Qt4Notify()
	: Notifier("Tray Icon Baloon", "Tray Icon Baloon", IconsManager::instance()->loadIcon("Qt4Notify")), chat(0)
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

	if (!text.isEmpty())
	{
		AccountNotification *accountNotification = dynamic_cast<AccountNotification *>(notification);
		if (accountNotification && accountNotification->chat() && accountNotification->chat()->contacts().count())
			ret = Parser::parse(text, accountNotification->account(), *accountNotification->chat()->contacts().begin(), notification);

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

	if (qt4_tray_icon != 0)
	{
		notification->acquire();

		unsigned int timeout = config_file.readNumEntry("Qt4DockingNotify", QString("Event_") + notification->type() + "_timeout");
		unsigned int icon = config_file.readNumEntry("Qt4DockingNotify", QString("Event_") + notification->type() + "_icon");
		QString title = config_file.readEntry("Qt4DockingNotify", QString("Event_") + notification->type() + "_title");
		QString syntax = config_file.readEntry("Qt4DockingNotify", QString("Event_") + notification->type() + "_syntax");

		AccountNotification *accountNotification = dynamic_cast<AccountNotification *>(notification);
		if (accountNotification)
			chat = notification->chat();
		else
			chat = 0;

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
}

Qt4Notify *qt4_notify = 0;

/** @} */

