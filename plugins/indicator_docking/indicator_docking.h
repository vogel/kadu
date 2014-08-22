/*
 * %kadu copyright begin%
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2011 Marcin Dawidziuk (cinekdawidziuk@gmail.com)
 * Copyright 2011, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011, 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#ifndef INDICATOR_DOCKING_H
#define INDICATOR_DOCKING_H

#include <QtCore/QMultiMap>
#include <QtCore/QPoint>

#include "chat/chat.h"
#include "notify/notifier.h"

#include "plugins/docking/docker.h"

template<typename T> class QList;
class QMouseEvent;

namespace QIndicate
{
	class Indicator;
	class Server;
}

class ChatNotification;
class ChatWidget;

class IndicatorDocking : public Notifier, public Docker
{
	Q_OBJECT

	static IndicatorDocking *Instance;

	typedef QMultiMap<QIndicate::Indicator *, ChatNotification *> IndMMap;

	QIndicate::Server *Server;
	IndMMap IndicatorsMap;

	IndicatorDocking();
	virtual ~IndicatorDocking();

	void createDefaultConfiguration();

	IndMMap::iterator iteratorForChat(const Chat &chat);
	QList<IndMMap::iterator> iteratorsForAggregateChat(const Chat &chat);

	void removeNotification(ChatNotification *chatNotification);

private slots:
	void indicateUnreadMessages();

	void showMainWindow();
	void displayIndicator(QIndicate::Indicator *indicator);

	void notificationClosed(Notification *notification);

	void chatUpdated(const Chat &chat);
	void chatWidgetAdded(ChatWidget *chatWidget);

	void silentModeToggled(bool silentMode);

public:
	static IndicatorDocking * instance();
	static void createInstance();
	static void destroyInstance();

	virtual void notify(Notification *notification);
	virtual NotifierConfigurationWidget * createConfigurationWidget(QWidget *parent = 0) { Q_UNUSED(parent); return 0; }
	virtual CallbackCapacity callbackCapacity() { return CallbackNotSupported; }

	virtual void changeTrayIcon(const KaduIcon &icon) { Q_UNUSED(icon); return; }
	virtual void changeTrayMovie(const QString &moviePath) { Q_UNUSED(moviePath); return; }
	virtual void changeTrayTooltip(const QString &tooltip) { Q_UNUSED(tooltip); return; }
	virtual QPoint trayPosition() { return QPoint(0, 0); }

};

#endif // INDICATOR_DOCKING_H
