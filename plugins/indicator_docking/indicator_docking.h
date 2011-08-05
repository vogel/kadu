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

#ifndef INDICATOR_DOCKING_H
#define INDICATOR_DOCKING_H

#include <QtCore/QObject>
#include <QtCore/QDebug>
#include <QtCore/QMap>
#include <QtGui/QImage>

#include "notify/notifier.h"
#include "gui/widgets/chat-widget-manager.h"
#include "plugins/docking/docker.h"
#include "misc/path-conversion.h"
#include "core/core.h"
#include "debug.h"
#include "exports.h"

#include <libindicate-qt/qindicateserver.h>
#include <libindicate-qt/qindicateindicator.h>


class IndicatorDocking : public Notifier, public Docker
{
	Q_OBJECT

	static IndicatorDocking *Instance;
	QIndicate::Server* Server;
	QMap<QString, QIndicate::Indicator*> IndicatorsMap;
	QMap<QString, bool> IndicatorsVisible; 
	QMap<QString, Chat> ChatsMap;
	QMap<QString, ContactSet> ContactsMap;
	QMap<QString, QImage> AvatarsMap;
	QScopedPointer<QMouseEvent> EventForShowMainWindow;

	explicit IndicatorDocking(QObject *parent = 0);
	virtual ~IndicatorDocking();	
	
	void createDefaultConfiguration();
	
	void deleteAllIndicators();

	void deleteIndicator(const QString name);

private slots:
	void showMainWindow();

	void displayIndicator(QIndicate::Indicator* indicator);

	void notificationClosed(Notification* notification);

	void chatWidgetActivated(ChatWidget *);

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

};

#endif
