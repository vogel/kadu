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

void Qt4Notify::notify(Notification *notification)
{
	kdebugf();

	if (qt4_tray_icon != 0)
	{
		notification->acquire();

		unsigned int timeout = config_file.readNumEntry("Qt4DockingNotify", QString("Event_") + notification->type() + "_timeout");
		unsigned int icon = config_file.readNumEntry("Qt4DockingNotify", QString("Event_") + notification->type() + "_icon");

		senders = UserListElements(notification->userListElements());
		qt4_tray_icon->showMessage(toPlainText(notification->text()), toPlainText(notification->details()), 
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
}

Qt4Notify *qt4_notify = 0;

/** @} */

