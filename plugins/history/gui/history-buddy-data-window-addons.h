/*
 * %kadu copyright begin%
 * Copyright 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef HISTORY_BUDDY_DATA_WINDOW_ADDONS_H
#define HISTORY_BUDDY_DATA_WINDOW_ADDONS_H

#include <QtCore/QMap>
#include <QtCore/QObject>

#include "configuration/configuration-aware-object.h"
#include "gui/windows/buddy-data-window-aware-object.h"

class QCheckBox;

class HistoryBuddyDataWindowAddons : public QObject, ConfigurationAwareObject, BuddyDataWindowAwareObject
{
	Q_OBJECT

	bool StoreHistory;
	QMap<BuddyDataWindow *, QCheckBox *> StoreHistoryCheckBoxes;

private slots:
	void save();

protected:
	virtual void configurationUpdated();

	virtual void buddyDataWindowCreated(BuddyDataWindow *buddyDataWindow);
	virtual void buddyDataWindowDestroyed(BuddyDataWindow *buddyDataWindow);

public:
	explicit HistoryBuddyDataWindowAddons(QObject *parent);
	virtual ~HistoryBuddyDataWindowAddons();

};

#endif // HISTORY_BUDDY_DATA_WINDOW_ADDONS_H
