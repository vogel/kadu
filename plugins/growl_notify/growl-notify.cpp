/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011 Tomasz Rostanski (rozteck@interia.pl)
 * Copyright 2008, 2010 Tomasz Rostański (rozteck@interia.pl)
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

/* This program uses the GrowlNotifier, located in growlnotifier.h and
 * growlnotifier.cpp file, which are the part of PSI Jabber Client.
 */

#include "growlnotifier.h"
#include "growl-notify.h"
#include "growl-notify-configuration-widget.h"

#include <QtGui/QTextDocument>

#include "avatars/avatar.h"
#include "configuration/configuration-file.h"
#include "gui/windows/message-dialog.h"
#include "notify/account-notification.h"
#include "notify/chat-notification.h"
#include "notify/notification-manager.h"
#include "notify/notification.h"
#include "parser/parser.h"
#include "icons/icons-manager.h"
#include "debug.h"

GrowlNotify::GrowlNotify(QObject *parent) : Notifier("Growl", "Growl", KaduIcon("kadu_icons/notify-hints"), parent)
{
	kdebugf();

	createDefaultConfiguration();
	NotificationManager::instance()->registerNotifier(this);

	// Initialize GrowlNotifier
	QStringList notifications;
	notifications << "Kadu Notification";
	growlNotifier = new GrowlNotifier(notifications, notifications, "Kadu");

	kdebugf2();
}

GrowlNotify::~GrowlNotify()
{
	kdebugf();
	NotificationManager::instance()->unregisterNotifier(this);
	growlNotifier->cleanupAfterGrowl();
	kdebugf2();
}

QString GrowlNotify::toPlainText(const QString &text)
{
	QTextDocument doc;
	doc.setHtml(text);
	return doc.toPlainText();
}

QString GrowlNotify::parseText(const QString &text, Notification *notification, const QString &def)
{
	QString ret;
	if (!text.isEmpty())
	{
		ret = Parser::parse(text, notification);
		ret = ret.replace("%&m", notification->text());
		ret = ret.replace("%&t", notification->title());
		ret = ret.replace("%&d", notification->details());
	}
	else
		ret = def;

	return toPlainText(ret);
}


void GrowlNotify::notify(Notification *notification)
{
	kdebugf();

	if (growlNotifier->isNotifying())
		return;

	QPixmap pixmap;
	QString title = config_file.readEntry("GrowlNotify", QString("Event_") + notification->type() + "_title");
	QString syntax = config_file.readEntry("GrowlNotify", QString("Event_") + notification->type() + "_syntax");
//	bool showAvatar = config_file.readBoolEntry("GrowlNotify", QString("Event_") + notification->type() + "_avatar");

	notification->acquire();

//	if (showAvatar)
	{
		ChatNotification *chatNotification = qobject_cast<ChatNotification *>(notification);
		if (chatNotification)
		{
			Avatar avatar = //chatNotification->account().accountContact().contactAvatar();
			chatNotification->chat().contacts().toContact().contactAvatar();
			if (!avatar.isEmpty())
				pixmap = avatar.pixmap();
		}
	}
	if (pixmap.isNull())
		pixmap = notification->icon().icon().pixmap(128,128);

	growlNotifier->notify("Kadu Notification",
		parseText(title, notification, notification->text()),
		parseText(syntax, notification, notification->details()),
		pixmap, false, notification, SLOT(callbackAccept()));

	//notification->release(); //released withing notification_timeout/notification_clicked

	kdebugf2();
}

void GrowlNotify::createDefaultConfiguration()
{
	config_file.addVariable("GrowlNotify", "Event_ConnectionError_syntax", "%&m");
	config_file.addVariable("GrowlNotify", "Event_ConnectionError_title", "%&t");

	config_file.addVariable("GrowlNotify", "Event_NewChat_syntax", "%&d");
	config_file.addVariable("GrowlNotify", "Event_NewChat_title", "%&m");

	config_file.addVariable("GrowlNotify", "Event_NewMessage_syntax", "%&d");
	config_file.addVariable("GrowlNotify", "Event_NewMessage_title", "%&m");

	config_file.addVariable("GrowlNotify", "Event_StatusChanged/ToOnline_syntax", "%&d");
	config_file.addVariable("GrowlNotify", "Event_StatusChanged/ToOnline_title", "%&m");

	config_file.addVariable("GrowlNotify", "Event_StatusChanged/ToBusy_syntax", "%&d");
	config_file.addVariable("GrowlNotify", "Event_StatusChanged/ToBusy_title", "%&m");

	config_file.addVariable("GrowlNotify", "Event_StatusChanged/ToOffline_syntax", "%&d");
	config_file.addVariable("GrowlNotify", "Event_StatusChanged/ToOffline_title", "%&m");

	config_file.addVariable("GrowlNotify", "Event_StatusChanged/ToInvisible_syntax", "%&d");
	config_file.addVariable("GrowlNotify", "Event_StatusChanged/ToInvisible_title", "%&m");

	config_file.addVariable("GrowlNotify", "Event_FileTransfer/Finished_syntax", "%&m");
	config_file.addVariable("GrowlNotify", "Event_FileTransfer/Finished_title", "%&t");

	config_file.addVariable("GrowlNotify", "Event_FileTransfer/IncomingFile_syntax", "%&m");
	config_file.addVariable("GrowlNotify", "Event_FileTransfer/IncomingFile_title", "%&t");
}

NotifierConfigurationWidget *GrowlNotify::createConfigurationWidget(QWidget *parent)
{
	configurationWidget = new GrowlNotifyConfigurationWidget(parent);
	return configurationWidget;
}
