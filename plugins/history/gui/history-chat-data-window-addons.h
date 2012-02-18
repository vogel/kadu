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

#ifndef HISTORY_CHAT_DATA_WINDOW_ADDONS_H
#define HISTORY_CHAT_DATA_WINDOW_ADDONS_H

#include <QtCore/QMap>
#include <QtCore/QObject>

#include "configuration/configuration-aware-object.h"
#include "gui/windows/chat-data-window-aware-object.h"

class QCheckBox;

class HistoryChatDataWindowAddons : public QObject, ConfigurationAwareObject, ChatDataWindowAwareObject
{
	Q_OBJECT

	bool StoreHistory;
	QMap<ChatDataWindow *, QCheckBox *> StoreHistoryCheckBoxes;

private slots:
	void save();

protected:
	virtual void configurationUpdated();

	virtual void chatDataWindowCreated(ChatDataWindow *chatDataWindow);
	virtual void chatDataWindowDestroyed(ChatDataWindow *chatDataWindow);

public:
	explicit HistoryChatDataWindowAddons(QObject *parent);
	virtual ~HistoryChatDataWindowAddons();

};

#endif // HISTORY_CHAT_DATA_WINDOW_ADDONS_H
