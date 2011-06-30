/*
 * %kadu copyright begin%
 * Copyright 2011 Marcin Dawidziuk (cinekdawidziuk@gmail.com)
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

#ifndef indicator_docking_H
#define indicator_docking_H

#include <QtCore/QObject>
#include <QDebug>
#include <QImage>
#include <QMap>
#include <libindicate-qt/qindicateserver.h>
#include <libindicate-qt/qindicateindicator.h>
#include "plugins/docking/docking.h"
#include "notify/notifier.h"
#include "notify/notification-manager.h"
#include "notify/notification.h"
#include "notify/chat-notification.h"
#include "gui/widgets/chat-widget-manager.h"
#include "plugins/docking/docking.h"
#include "plugins/docking/docker.h"
#include "avatars/avatar.h"
#include "misc/path-conversion.h"
#include "chat/chat-manager.h"
#include "core/core.h"
#include "debug.h"
#include "exports.h"

class IndicatorDocking : public Notifier, public Docker
{
	Q_OBJECT

	static IndicatorDocking *Instance;
	QIndicate::Server* server;
	QMap<QString, QIndicate::Indicator*> indicatorsMap;
	QMap<QString, bool> indicatorsVisible; 
	QMap<QString, Chat> chatsMap;
	QMap<QString, ContactSet> contactsMap;
	QMap<QString, QImage> avatarsMap;

	explicit IndicatorDocking(QObject *parent = 0);
	virtual ~IndicatorDocking();
public:
	static IndicatorDocking * instance();
	static void createInstance();
	static void destroyInstance();

	virtual void notify(Notification *notification);
	virtual NotifierConfigurationWidget *createConfigurationWidget(QWidget *parent = 0) { Q_UNUSED(parent); return 0; }
	virtual CallbackCapacity callbackCapacity() { return CallbackNotSupported; }

	virtual void changeTrayIcon(const KaduIcon &icon) { Q_UNUSED(icon); return; }
	virtual void changeTrayMovie(const QString &moviePath) { Q_UNUSED(moviePath); return; }
	virtual void changeTrayTooltip(const QString &tooltip) { Q_UNUSED(tooltip); return; }
	virtual QPoint trayPosition() { return QPoint(0, 0); }
private:
	bool isThereAnyIndicatorNamed(QString indicatorName);
	bool isThereAnyChatNamed(QString name);
	void deleteIndicatorAndChat(QString name);
	bool isIndicatorVisible(QString name);
	QString getContactsFromChat(Chat chat);
private slots:
	void showMainWindow();
	void displayIndicator(QIndicate::Indicator* indicator);
	void notificationClosed(Notification* notification);
	void chatWidgetActivated(ChatWidget *);
};


#endif
