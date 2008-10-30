/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtGui/QLabel>
#include <QtGui/QVBoxLayout>
#include <QtGui/QSystemTrayIcon>
#include <QtGui/QTextDocument>

#include "../notify/notify.h"
#include "../notify/notification.h"

#include "message_box.h"
#include "config_file.h"
#include "debug.h"
#include "userlist.h"
#include "chat_manager.h"
#include "kadu_parser.h"

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

	qt4_notify = new Qt4Notify(0, "qt4_notify");

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

Qt4Notify::Qt4Notify(QObject *parent, const char *name) : Notifier(parent, name)
{
	kdebugf();

	createDefaultConfiguration();
	notification_manager->registerNotifier(QT_TRANSLATE_NOOP("@default", "Tray Icon Baloon"), this);
	
	if (qt4_tray_icon != 0)
	{
		connect(qt4_tray_icon, SIGNAL(messageClicked()), this, SLOT(messageClicked()));
	}

	kdebugf2();
}

Qt4Notify::~Qt4Notify()
{
	kdebugf();
	notification_manager->unregisterNotifier("Tray Icon Baloon");

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

QString Qt4Notify::parseText(QString text, Notification *notification, QString def)
{
	UserListElement ule;
	QString ret;

	if (notification->userListElements().count())
		ule = notification->userListElements()[0];

	if (!text.isEmpty())
	{
		ret = KaduParser::parse(text, ule, notification);
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

		senders = UserListElements(notification->userListElements());

		qt4_tray_icon->showMessage(parseText(title, notification, notification->text()),
			parseText(syntax, notification, notification->details()),
			(QSystemTrayIcon::MessageIcon)icon, timeout * 1000);

		notification->release();
	}

	kdebugf2();
}

void Qt4Notify::messageClicked()
{
	if (!senders.isEmpty())
		chat_manager->openPendingMsgs(senders, true);
}

NotifierConfigurationWidget *Qt4Notify::createConfigurationWidget(QWidget *parent, char *name)
{
	configurationWidget = new Qt4NotifyConfigurationWidget(parent, name);
	return configurationWidget;
}

void Qt4Notify::mainConfigurationWindowCreated(MainConfigurationWindow*)
{
}

void Qt4Notify::configurationUpdated()
{
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

	config_file.addVariable("Qt4DockingNotify", "Event_StatusChanged/ToOnline_timeout", 10);
	config_file.addVariable("Qt4DockingNotify", "Event_StatusChanged/ToOnline_syntax", "%&d");
	config_file.addVariable("Qt4DockingNotify", "Event_StatusChanged/ToOnline_title", "%&m");
	config_file.addVariable("Qt4DockingNotify", "Event_StatusChanged/ToOnline_icon", 0);
	
	config_file.addVariable("Qt4DockingNotify", "Event_StatusChanged/ToBusy_timeout", 10);
	config_file.addVariable("Qt4DockingNotify", "Event_StatusChanged/ToBusy_syntax", "%&d");
	config_file.addVariable("Qt4DockingNotify", "Event_StatusChanged/ToBusy_title", "%&m");
    config_file.addVariable("Qt4DockingNotify", "Event_StatusChanged/ToBusy_icon", 0);

    config_file.addVariable("Qt4DockingNotify", "Event_StatusChanged/ToOffline_timeout", 10);
    config_file.addVariable("Qt4DockingNotify", "Event_StatusChanged/ToOffline_syntax", "%&d");
    config_file.addVariable("Qt4DockingNotify", "Event_StatusChanged/ToOffline_title", "%&m");
    config_file.addVariable("Qt4DockingNotify", "Event_StatusChanged/ToOffline_icon", 0);

    config_file.addVariable("Qt4DockingNotify", "Event_StatusChanged/ToInvisible_timeout", 10);
    config_file.addVariable("Qt4DockingNotify", "Event_StatusChanged/ToInvisible_syntax", "%&d");
    config_file.addVariable("Qt4DockingNotify", "Event_StatusChanged/ToInvisible_title", "%&m");
    config_file.addVariable("Qt4DockingNotify", "Event_StatusChanged/ToInvisible_icon", 0);
	
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

