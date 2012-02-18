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

#ifndef HISTORY_TALKABLE_DATA_H
#define HISTORY_TALKABLE_DATA_H

#include "storage/module-data.h"

class HistoryTalkableData : public ModuleData
{
	Q_OBJECT

	bool StoreHistory;

protected:
	virtual void load();
	virtual void store();
	virtual bool shouldStore();

public:
	HistoryTalkableData(const QString &moduleName, StorableObject *parent, QObject *qobjectParent);
	virtual ~HistoryTalkableData();

	virtual QString name() const;

	bool storeHistory();
	void setStoreHistory(bool storeHistory);

};

#endif // HISTORY_TALKABLE_DATA_H
