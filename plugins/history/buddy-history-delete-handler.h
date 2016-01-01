/*
 * %kadu copyright begin%
 * Copyright 2011 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#pragma once

#include "buddies/buddy-additional-data-delete-handler.h"

#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

class History;

class BuddyHistoryDeleteHandler : public QObject, public BuddyAdditionalDataDeleteHandler
{
	Q_OBJECT

public:
	Q_INVOKABLE explicit BuddyHistoryDeleteHandler(QObject *parent = nullptr);
	virtual ~BuddyHistoryDeleteHandler();

	virtual QString name();
	virtual QString displayName();
	virtual void deleteBuddyAdditionalData(Buddy buddy);

private:
	QPointer<History> m_history;

private slots:
	INJEQT_SETTER void setHistory(History *history);

};
