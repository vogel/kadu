/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2011, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#ifndef BUDDY_CONFIGURATION_WIDGET_TAB_ADAPTER_H
#define BUDDY_CONFIGURATION_WIDGET_TAB_ADAPTER_H

#include <QtCore/QObject>

#include "exports.h"

class QTab;
class QTabWidget;

class BuddyConfigurationWidget;
class BuddyDataWindow;

class KADUAPI BuddyConfigurationWidgetTabAdapter : public QObject
{
	Q_OBJECT

	BuddyDataWindow *MyBuddyDataWindow;
	QTabWidget *MyTabWidget;

private slots:
	void widgetAdded(BuddyConfigurationWidget *widget);

public:
	explicit BuddyConfigurationWidgetTabAdapter(BuddyDataWindow *buddyDataWindow, QTabWidget *tabWidget, QObject *parent = 0);
	virtual ~BuddyConfigurationWidgetTabAdapter();

};

#endif // BUDDY_CONFIGURATION_WIDGET_TAB_ADAPTER_H
