/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Tomasz Rostanski (rozteck@interia.pl)
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2008, 2010, 2010, 2010 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2010, 2011, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "growl-notify-configuration-widget.h"
#include "growl-notify.h"
#include "growlnotifier.h"

#include <QtGui/QMessageBox>
#include <QtGui/QTextDocument>

#include "avatars/avatar.h"
#include "configuration/configuration-file.h"
#include "gui/windows/message-dialog.h"
#include "icons/icons-manager.h"
#include "notify/notification-manager.h"
#include "notify/notification/account-notification.h"
#include "notify/notification/chat-notification.h"
#include "notify/notification/notification.h"
#include "parser/parser.h"
#include "debug.h"

GrowlNotify *GrowlNotify::Instance = NULL;

GrowlNotify::GrowlNotify(QObject *parent) : Notifier("Growl", "Growl", KaduIcon("kadu_icons/notify-hints"), parent)
{
	kdebugf();

	Instance = this;

	// Initialize GrowlNotifier
	QStringList notifications;
	notifications << "Kadu Notification";
	growlNotifier = new GrowlNotifier(notifications, notifications, "Kadu");

	kdebugf2();
}

GrowlNotify::~GrowlNotify()
{
	kdebugf();
	growlNotifier->cleanupAfterGrowl();
	kdebugf2();
}

bool GrowlNotify::init(bool firstLoad)
{
	Q_UNUSED(firstLoad)

	if (!grow_is_installed())
	{
		QMessageBox::information(NULL, tr("Error"), tr("Growl is not installed in your system"));
		return false;
	}

	NotificationManager::instance()->registerNotifier(this);
	createDefaultConfiguration();

	return true;
}

void GrowlNotify::done()
{
	NotificationManager::instance()->unregisterNotifier(this);
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
			chatNotification->chat().contacts().toContact().avatar(true);
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

void GrowlNotify::mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow)
{
	Q_UNUSED(mainConfigurationWindow);
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

Q_EXPORT_PLUGIN2(growl_notify, GrowlNotify)

#include "moc_growl-notify.cpp"
